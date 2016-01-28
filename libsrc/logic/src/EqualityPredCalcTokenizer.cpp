//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// predicate calculus tokenizer

// system includes
#include <ctype.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// local includes
#include "system/Debug.h"
#include "hdr/EqualityPredCalcTokenizer.h"

namespace ombt {

// ctors and dtor
EqualityPredCalcTokenizer::EqualityPredCalcTokenizer(bool skip_newline): 
    debug_(false),
    line_no_(1),
    line_start_pos_(0),
    token_start_pos_(-1),
    token_end_pos_(-1), 
    token_type_(Unknown), 
    skip_newline_(skip_newline),
    program_length_(0),
    program_(""),
    BaseObject()
{
}

EqualityPredCalcTokenizer::EqualityPredCalcTokenizer(
    const EqualityPredCalcTokenizer &src): 
        debug_(src.debug_),
        line_no_(src.line_no_),
        line_start_pos_(src.line_start_pos_),
        token_start_pos_(src.token_start_pos_),
        token_end_pos_(src.token_end_pos_), 
        token_type_(src.token_type_), 
        skip_newline_(src.skip_newline_), 
        program_length_(src.program_length_),
        program_(src.program_),
        BaseObject(src)
{
}

EqualityPredCalcTokenizer::~EqualityPredCalcTokenizer()
{
}

// assignment
EqualityPredCalcTokenizer &
EqualityPredCalcTokenizer::operator=(const EqualityPredCalcTokenizer &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        debug_ = rhs.debug_;
        line_no_ = rhs.line_no_;
        line_start_pos_ = rhs.line_start_pos_;
        token_start_pos_ = rhs.token_start_pos_;
        token_end_pos_ = rhs.token_end_pos_;
        token_type_ = rhs.token_type_;
        skip_newline_ = rhs.skip_newline_;
        program_length_ = rhs.program_length_;
        program_ = rhs.program_;
    }
    return(*this);
}

// tokenizing functions
void
EqualityPredCalcTokenizer::resetProgram()
{
    line_no_ = 1;
    line_start_pos_ = 0;
    token_start_pos_ = -1;
    token_end_pos_ = -1;
    token_type_ = Unknown;
    program_length_ = program_.size();
}

void
EqualityPredCalcTokenizer::setFile(const std::string &filename)
{
    line_no_ = 1;
    line_start_pos_ = 0;
    token_start_pos_ = -1;
    token_end_pos_ = -1;
    token_type_ = Unknown;
    filename_ = filename;

    std::ifstream in(filename.c_str());
    std::stringstream buffer;
    buffer << in.rdbuf();

    program_ = buffer.str();
    program_length_ = program_.size();
    std::cerr << "File   : " << filename_ << std::endl;
    std::cerr << "File Sz: " << program_length_ << std::endl;
}

void
EqualityPredCalcTokenizer::setProgram(const std::string &program)
{
    line_no_ = 1;
    line_start_pos_ = 0;
    token_start_pos_ = -1;
    token_end_pos_ = -1;
    token_type_ = Unknown;
    program_length_ = program.size();
    program_ = program;
}

// returns false if end-of-program, else true for everything else.
bool 
EqualityPredCalcTokenizer::advanceToNextToken()
{
    Reporter rpt(debug_, *this);

    // scan characters until a token is identified or EOF
    while (!is_end_of_program())
    {
        // skip current token or start of the first time
        // and skip any leading white space.
        accept_token();
        if (is_end_of_program()) return false;
        skip_white_space();
        if (is_end_of_program()) return false;

        // start looking at lead characters and 
        // try to determine the token, if any.
        long &tsp = token_start_pos_;
        long &tep = token_end_pos_;
        long &lsp = line_start_pos_;
        switch (program_[tep])
        {
        case '#':
        {
            advance_end_to('\n');
            std::cerr << "Line " 
                      << line_no_ << ": "
                      << program_.substr(lsp, (tep-lsp))
                      << std::endl;
            line_no_ += 1;
            bool ok = process_include_file();
            if ((token_type_ == IncludeFile) && (!ok))
            {
                return true;
            }
            lsp = tep+1;
            continue;
        }
        case '\n':
            token_type_ = NewLine;
            std::cerr << "Line " 
                      << line_no_ << ": "
                      << program_.substr(lsp, (tep-lsp))
                      << std::endl;
            line_no_ += 1;
            lsp = tep+1;
            if (skip_newline_)
                continue;
            else
                return true;

        case '|':
            if (++tep < program_length_)
            {
                switch (program_[tep])
                {
                case '-':
                    token_type_ = Therefore;
                    break;

                case '|':
                    token_type_ = Or;
                    break;

                default:
                    tep = tsp;
                    token_type_ = Unknown;
                    break;
                }
            }
            else
            {
                tep = tsp;
                token_type_ = Unknown;
            }
            return true;

        case '?':
            if (++tep < program_length_)
            {
                switch (program_[tep])
                {
                case '-':
                    token_type_ = Query;
                    break;

                default:
                    tep = tsp;
                    token_type_ = Unknown;
                    break;
                }
            }
            else
            {
                tep = tsp;
                token_type_ = Unknown;
            }
            return true;

        case '!':
            if (++tep < program_length_)
            {
                switch (program_[tep])
                {
                case '=':
                    token_type_ = NotEqual;
                    break;

                default:
                    tep = tsp;
                    token_type_ = Negation;
                    break;
                }
            }
            else
            {
                tep = tsp;
                token_type_ = Negation;
            }
            return true;

        case '<':
            accept_as_is_or_with_suffix(
                "-->", LeftUniversal, Biconditional);
            return true;

        case '-':
            if (++tep < program_length_)
            {
                if (check_and_advance_end_to("->"))
                {
                    token_type_ = Implication;
                }
                else if (number_and_advance_end_to())
                {
                    token_type_ = NegativeNumber;
                }
                else
                {
                    tep = tsp;
                    token_type_ = Negation;
                }
            }
            else
            {
                tep = tsp;
                token_type_ = Unknown;
            }
            return true;

        case '&':
            accept_as_is_or_with_suffix("&", Unknown, And);
            return true;

        case '~':
            token_type_ = Negation;
            return true;

        case '=':
            token_type_ = Equal;
            return true;

        case '(':
            token_type_ = LeftParenthesis;
            return true;

        case ')':
            token_type_ = RightParenthesis;
            return true;

        case ',':
            token_type_ = Comma;
            return true;

        case ';':
            token_type_ = Semicolon;
            return true;

        case '>':
            token_type_ = RightUniversal;
            return true;

        case '[':
            token_type_ = LeftExistential;
            return true;

        case ']':
            token_type_ = RightExistential;
            return true;

        case '{':
            token_type_ = LeftBrace;
            return true;

        case '}':
            token_type_ = RightBrace;
            return true;

        case '"':
            advance_end_to('"');
            if (is_end_of_program())
            {
                return false;
            }
            token_type_ = String;
            return true;

        default:
            if (nc_check_and_advance_end_to("quit"))
            {
                token_type_ = Quit;
            }
            else if (nc_check_and_advance_end_to("true"))
            {
                token_type_ = True;
            }
            else if (nc_check_and_advance_end_to("false"))
            {
                token_type_ = False;
            }
            else if (nc_check_and_advance_end_to("argument"))
            {
                token_type_ = Argument;
            }
            else if (nc_check_and_advance_end_to("forall"))
            {
                token_type_ = Universal;
            }
            else if (nc_check_and_advance_end_to("forsome"))
            {
                token_type_ = Existential;
            }
            else if (nc_check_and_advance_end_to("sos"))
            {
                token_type_ = SetOfSupport;
            }
            else if (nc_check_and_advance_end_to("options"))
            {
                token_type_ = Options;
            }
            else if (identifier_and_advance_end_to())
            {
                token_type_ = Identifier;
            }
            else if (pidentifier_and_advance_end_to())
            {
                token_type_ = PIdentifier;
            }
            else if (number_and_advance_end_to())
            {
                token_type_ = Number;
            }
            else
            {
                tep = tsp;
                token_type_ = Unknown;
            }
            return true;
        }
    }

    // ran out of characters
    token_type_ = EndOfProgram;
    return false;
}

EqualityPredCalcTokenizer::Type 
EqualityPredCalcTokenizer::currentType() const
{
    return token_type_;
}

#undef ENUMSTRINGIFY
#define ENUMSTRINGIFY(ENUM) { ENUM, #ENUM }

const std::string 
EqualityPredCalcTokenizer::currentTypeName() const
{
    static struct {
        Type type_;
        const char *name_;
    } type_names[] = {
        ENUMSTRINGIFY(Unknown),
        ENUMSTRINGIFY(WhiteSpace),
        ENUMSTRINGIFY(Pound),
        ENUMSTRINGIFY(NewLine),
        ENUMSTRINGIFY(Therefore),
        ENUMSTRINGIFY(Query),
        ENUMSTRINGIFY(Biconditional),
        ENUMSTRINGIFY(Implication),
        ENUMSTRINGIFY(Or),
        ENUMSTRINGIFY(And),
        ENUMSTRINGIFY(Negation),
        ENUMSTRINGIFY(LeftParenthesis),
        ENUMSTRINGIFY(RightParenthesis),
        ENUMSTRINGIFY(Comma),
        ENUMSTRINGIFY(Semicolon),
        ENUMSTRINGIFY(Universal),
        ENUMSTRINGIFY(LeftUniversal),
        ENUMSTRINGIFY(RightUniversal),
        ENUMSTRINGIFY(Existential),
        ENUMSTRINGIFY(LeftExistential),
        ENUMSTRINGIFY(RightExistential),
        ENUMSTRINGIFY(LeftBrace),
        ENUMSTRINGIFY(RightBrace),
        ENUMSTRINGIFY(Equal),
        ENUMSTRINGIFY(NotEqual),
        ENUMSTRINGIFY(Argument),
        ENUMSTRINGIFY(Quit),
        ENUMSTRINGIFY(True),
        ENUMSTRINGIFY(False),
        ENUMSTRINGIFY(SetOfSupport),
        ENUMSTRINGIFY(Options),
        ENUMSTRINGIFY(Number),
        ENUMSTRINGIFY(NegativeNumber),
        ENUMSTRINGIFY(String),
        ENUMSTRINGIFY(Identifier),
        ENUMSTRINGIFY(PIdentifier),
        ENUMSTRINGIFY(EndOfProgram),
        ENUMSTRINGIFY(IncludeFile),
        ENUMSTRINGIFY(Last),
    };
    return type_names[token_type_].name_;
}

#undef ENUMSTRINGIFY

const std::string
EqualityPredCalcTokenizer::currentValue() const
{
    const long &tsp = token_start_pos_;
    const long &tep = token_end_pos_;
    if (0<= tsp && tsp <= tep && tep < program_length_)
        return program_.substr(tsp, tep-tsp+1);
    else
        return "Token Out-Of-Range";
}

// utility functions
void
EqualityPredCalcTokenizer::skip_white_space()
{
    for ( ; (token_end_pos_ < program_length_) &&
            ((program_[token_end_pos_] == ' ') || 
             (program_[token_end_pos_] == '\t')); ++token_end_pos_) ;
    if (token_end_pos_ >= program_length_)
    {
        token_type_ = EndOfProgram;
    }
    token_start_pos_ = token_end_pos_;
}

bool
EqualityPredCalcTokenizer::is_end_of_program() const
{
    if (token_type_ == EndOfProgram)
    {
        return true;
    }
    else if (token_end_pos_ >= program_length_)
    {
        token_type_ = EndOfProgram;
        return true;
    }
    else
        return false;
}

void
EqualityPredCalcTokenizer::accept_token()
{
    if (!is_end_of_program())
    {
        // advance to next position, if first time, then -1+1=0 and 
        // it works.
        if ((token_start_pos_ = token_end_pos_ + 1) >= program_length_)
        {
            token_type_ = EndOfProgram;
        }
        token_end_pos_ = token_start_pos_;
    }
}

void
EqualityPredCalcTokenizer::advance_end_to(char end_char)
{
    for ( ; ((token_end_pos_ < program_length_) &&
             (program_[token_end_pos_] != end_char)); 
           ++token_end_pos_) ;
    if (token_end_pos_ >= program_length_)
    {
        token_type_ = EndOfProgram;
    }
}

bool
EqualityPredCalcTokenizer::check_and_advance_end_to(const std::string &chars)
{
    long &tep = token_end_pos_;

    for (long p=0; p<chars.size(); ++p)
    {
        if ((tep+p) < program_length_)
        {
            if (program_[tep+p] != chars[p])
            {
                return false;
            }
        }
        else
        {
           return false;
        }
    }
    tep += (chars.size() - 1);
    return true;
}

// ignore case or no case in doing comparison
bool
EqualityPredCalcTokenizer::nc_check_and_advance_end_to(const std::string &chars)
{
    long &tep = token_end_pos_;

    for (long p=0; p<chars.size(); ++p)
    {
        if ((tep+p) < program_length_)
        {
            // convert to lowercase for comparison
            if (tolower(program_[tep+p]) != tolower(chars[p]))
            {
                return false;
            }
        }
        else
        {
           return false;
        }
    }
    tep += (chars.size() - 1);
    return true;
}

bool
EqualityPredCalcTokenizer::identifier_and_advance_end_to()
{
    long &tep = token_end_pos_;

    if (tep >= program_length_)
    {
        token_type_ = EndOfProgram;
        return false;
    }
    else if (is_a_to_z(program_[tep]))
    {
        // we have an identifier.
        token_type_ = Identifier;
        while (++tep < program_length_)
        {
            char c = program_[tep];
            if (!is_a_to_z(c) && !is_A_to_Z(c) && !is_0_to_9(c))
            {
                // search for identifier end
                break;
            }
        }
        tep -= 1;
        return true;
    }
    else
    {
        return false;
    }
}

bool
EqualityPredCalcTokenizer::pidentifier_and_advance_end_to()
{
    long &tep = token_end_pos_;

    if (tep >= program_length_)
    {
        token_type_ = EndOfProgram;
        return false;
    }
    else if (is_A_to_Z(program_[tep]))
    {
        // we have a predicate identifier
        token_type_ = PIdentifier;
        while (++tep < program_length_)
        {
            char c = program_[tep];
            if (!is_a_to_z(c) && !is_A_to_Z(c) && !is_0_to_9(c))
            {
                // search for end of predicate identifier
                break;
            }
        }
        tep -= 1;
        return true;
    }
    else
    {
        return false;
    }
}

bool
EqualityPredCalcTokenizer::number_and_advance_end_to()
{
    long &tep = token_end_pos_;

    if (tep >= program_length_)
    {
        token_type_ = EndOfProgram;
        return false;
    }
    else if (is_0_to_9(program_[tep]))
    {
        // we have a number
        token_type_ = Number;
        while (++tep < program_length_)
        {
            char c = program_[tep];
            if (!is_0_to_9(c))
            {
                // search for end of number
                break;
            }
        }
        tep -= 1;
        return true;
    }
    else
    {
        return false;
    }
}

void
EqualityPredCalcTokenizer::accept_as_is_or_with_suffix(
    const std::string &suffix, 
    EqualityPredCalcTokenizer::Type as_is_type,
    EqualityPredCalcTokenizer::Type with_suffix_type)
{
    long &tsp = token_start_pos_;
    long &tep = token_end_pos_;

    if ((++tep < program_length_) &&
        (check_and_advance_end_to(suffix)))
    {
        token_type_ = with_suffix_type;
    }
    else
    {
        tep = tsp;
        token_type_ = as_is_type;
    }
}

void
EqualityPredCalcTokenizer::report_syntax_error()
{
    long &tsp = token_start_pos_;
    long &tep = token_end_pos_;
    long &lsp = line_start_pos_;

    std::cerr << "ERROR line number: " 
              << line_no_ 
              << std::endl;
    if ((lsp < program_length_) && 
        (tep < program_length_) &&
        (lsp <= tep))
    {
        std::cerr << "ERROR line: " 
                  << program_.substr(lsp, (tep-lsp+1))
                  << std::endl;
    }
    if ((tsp <= tep) && (tep < program_length_))
    {
        std::cerr << "ERROR token: " 
                  << program_.substr(tsp, (tep-tsp+1))
                  << std::endl;
    }
}

bool
EqualityPredCalcTokenizer::process_include_file()
{
    static const std::string include("#include");

    token_type_ = Unknown;

    long &tsp = token_start_pos_;
    long &tep = token_end_pos_;
    long &lsp = line_start_pos_;

    if (!((tsp <= tep) && (tep < program_length_)))
    {
        return true;
    }

    std::string line = program_.substr(tsp, (tep-tsp+1));
    std::string::size_type include_pos = line.find(include);
    if (include_pos == std::string::npos)
    {
        return true;
    }

    token_type_ = IncludeFile;
        
    std::string::size_type first_quote_pos = 
        line.find_first_of("\"", include_pos);
    if (first_quote_pos == std::string::npos)
    {
        return false;
    }
    std::string::size_type second_quote_pos = 
        line.find_first_of("\"", first_quote_pos+1);
    if (second_quote_pos == std::string::npos)
    {
        return false;
    }

    first_quote_pos += 1;
    second_quote_pos -= 1;
    std::string filename = 
        line.substr(first_quote_pos, second_quote_pos-first_quote_pos+1);

    std::string content;
    if (!includes_.readFile(filename, content))
    {
        return false;
    }
    std::string new_program = program_.substr(0,tep+1) +
                              content +
                              program_.substr(tep+1);
    program_ = new_program;
    program_length_ = new_program.size();
    return true;
}

}

