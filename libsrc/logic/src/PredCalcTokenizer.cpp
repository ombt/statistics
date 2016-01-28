//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// predicate calculus tokenizer

// system includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// local includes
#include "system/Debug.h"
#include "hdr/PredCalcTokenizer.h"

namespace ombt {

// ctors and dtor
PredCalcTokenizer::PredCalcTokenizer(): 
    debug_(false),
    line_no_(1),
    line_start_pos_(0),
    token_start_pos_(-1),
    token_end_pos_(-1), 
    token_type_(Unknown), 
    program_length_(0),
    program_(""),
    BaseObject()
{
}

PredCalcTokenizer::PredCalcTokenizer(
    const PredCalcTokenizer &src): 
        debug_(src.debug_),
        line_no_(src.line_no_),
        line_start_pos_(src.line_start_pos_),
        token_start_pos_(src.token_start_pos_),
        token_end_pos_(src.token_end_pos_), 
        token_type_(src.token_type_), 
        program_length_(src.program_length_),
        program_(src.program_),
        BaseObject(src)
{
}

PredCalcTokenizer::~PredCalcTokenizer()
{
}

// assignment
PredCalcTokenizer &
PredCalcTokenizer::operator=(const PredCalcTokenizer &rhs)
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
        program_length_ = rhs.program_length_;
        program_ = rhs.program_;
    }
    return(*this);
}

// tokenizing functions
void
PredCalcTokenizer::resetProgram()
{
    line_no_ = 1;
    line_start_pos_ = 0;
    token_start_pos_ = -1;
    token_end_pos_ = -1;
    token_type_ = Unknown;
    program_length_ = program_.size();
}

void
PredCalcTokenizer::setFile(const std::string &filename)
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
}

void
PredCalcTokenizer::setProgram(const std::string &program)
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
PredCalcTokenizer::advanceToNextToken()
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
            advance_end_to('\n');
            std::cerr << "Line " 
                      << line_no_ << ": "
                      << program_.substr(lsp, (tep-lsp))
                      << std::endl;
            line_no_ += 1;
            lsp = tep+1;
            continue;

        case '\n':
            token_type_ = NewLine;
            std::cerr << "Line " 
                      << line_no_ << ": "
                      << program_.substr(lsp, (tep-lsp))
                      << std::endl;
            line_no_ += 1;
            lsp = tep+1;
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

        case '<':
            accept_as_is_or_with_suffix(
                "-->", LeftUniversal, Biconditional);
            return true;

        case '-':
            accept_as_is_or_with_suffix("->", Unknown, Implication);
            return true;

        case '&':
            accept_as_is_or_with_suffix("&", Unknown, And);
            return true;

        case '~':
            token_type_ = Negation;
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

        case '"':
            advance_end_to('"');
            if (is_end_of_program())
            {
                return false;
            }
            token_type_ = String;
            return true;

        default:
            if (check_and_advance_end_to("quit"))
            {
                token_type_ = Quit;
            }
            else if (check_and_advance_end_to("True"))
            {
                token_type_ = True;
            }
            else if (check_and_advance_end_to("False"))
            {
                token_type_ = False;
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

PredCalcTokenizer::Type 
PredCalcTokenizer::currentType() const
{
    return token_type_;
}

#undef ENUMSTRINGIFY
#define ENUMSTRINGIFY(ENUM) { ENUM, #ENUM }

const std::string 
PredCalcTokenizer::currentTypeName() const
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
        ENUMSTRINGIFY(Biconditional),
        ENUMSTRINGIFY(Implication),
        ENUMSTRINGIFY(Or),
        ENUMSTRINGIFY(And),
        ENUMSTRINGIFY(Negation),
        ENUMSTRINGIFY(LeftParenthesis),
        ENUMSTRINGIFY(RightParenthesis),
        ENUMSTRINGIFY(Comma),
        ENUMSTRINGIFY(Semicolon),
        ENUMSTRINGIFY(LeftUniversal),
        ENUMSTRINGIFY(RightUniversal),
        ENUMSTRINGIFY(LeftExistential),
        ENUMSTRINGIFY(RightExistential),
        ENUMSTRINGIFY(Quit),
        ENUMSTRINGIFY(True),
        ENUMSTRINGIFY(False),
        ENUMSTRINGIFY(Number),
        ENUMSTRINGIFY(String),
        ENUMSTRINGIFY(Identifier),
        ENUMSTRINGIFY(PIdentifier),
        ENUMSTRINGIFY(EndOfProgram),
    };
    return type_names[token_type_].name_;
}

#undef ENUMSTRINGIFY

const std::string
PredCalcTokenizer::currentValue() const
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
PredCalcTokenizer::skip_white_space()
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
PredCalcTokenizer::is_end_of_program() const
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
PredCalcTokenizer::accept_token()
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
PredCalcTokenizer::advance_end_to(char end_char)
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
PredCalcTokenizer::check_and_advance_end_to(const std::string &chars)
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

bool
PredCalcTokenizer::identifier_and_advance_end_to()
{
    long &tep = token_end_pos_;

    if (tep >= program_length_)
    {
        token_type_ = EndOfProgram;
        return false;
    }
    else if (is_a_to_z(program_[tep]))
    {
        token_type_ = Identifier;
        while (++tep < program_length_)
        {
            char c = program_[tep];
            if (!is_a_to_z(c) && !is_A_to_Z(c) && !is_0_to_9(c))
            {
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
PredCalcTokenizer::pidentifier_and_advance_end_to()
{
    long &tep = token_end_pos_;

    if (tep >= program_length_)
    {
        token_type_ = EndOfProgram;
        return false;
    }
    else if (is_A_to_Z(program_[tep]))
    {
        token_type_ = PIdentifier;
        while (++tep < program_length_)
        {
            char c = program_[tep];
            if (!is_a_to_z(c) && !is_A_to_Z(c) && !is_0_to_9(c))
            {
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
PredCalcTokenizer::number_and_advance_end_to()
{
    long &tep = token_end_pos_;

    if (tep >= program_length_)
    {
        token_type_ = EndOfProgram;
        return false;
    }
    else if (is_0_to_9(program_[tep]))
    {
        token_type_ = Number;
        while (++tep < program_length_)
        {
            char c = program_[tep];
            if (!is_0_to_9(c))
            {
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
PredCalcTokenizer::accept_as_is_or_with_suffix(
    const std::string &suffix, 
    PredCalcTokenizer::Type as_is_type,
    PredCalcTokenizer::Type with_suffix_type)
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
PredCalcTokenizer::report_syntax_error()
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

}

