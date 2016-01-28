//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// predicate calculus parser

// system includes
#include <string>

// local includes
#include "system/Debug.h"
#include "hdr/EqualityPredCalcParser.h"
#include "hdr/Program.h"
#include "hdr/ParseFrame.h"
#include "hdr/Symbol.h"
#include "conversions/Generic.h"

namespace ombt {

#define OFFSETMULTIPLIER 2

int EqualityPredCalcParser::ioffset_ = 0;

#define ERRORRETURN(RETVAL) { \
    if (debug_) { \
        std::cerr << std::string(OFFSETMULTIPLIER*ioffset_, ' '); \
    } \
    std::cerr << __FUNCTION__ \
              << "'" \
              << __LINE__ \
              << ": " \
              << return_type((RETVAL)) \
              << ", " \
              << tokenizer.currentValue() \
              << ", " \
              << tokenizer.currentTypeName() \
              << std::endl; \
    ioffset_--; \
    return(RETVAL); \
}

#define DEBUGENTRY() { \
    ioffset_++; \
    if (debug_) { \
        std::cerr << std::string(OFFSETMULTIPLIER*ioffset_, ' ') \
                  << __FUNCTION__ \
                  << "'" \
                  << __LINE__ \
                  << ": Entry, " \
                  << tokenizer.currentValue() \
                  << ", " \
                  << tokenizer.currentTypeName() \
                  << std::endl; \
    } \
}

#define DEBUGRETURN(RETVAL) { \
    if (debug_) { \
        std::cerr << std::string(OFFSETMULTIPLIER*ioffset_, ' ') \
                  << __FUNCTION__ \
                  << "'" \
                  << __LINE__ \
                  << ": " \
                  << return_type((RETVAL)) \
                  << ", " \
                  << tokenizer.currentValue() \
                  << ", " \
                  << tokenizer.currentTypeName() \
                  << std::endl; \
    } \
    ioffset_--; \
    return(RETVAL); \
}

// global frame for parser
ParseFrame global_frame;

// ctors and dtor
EqualityPredCalcParser::EqualityPredCalcParser(bool allow_empty_arglist): 
    last_status_(Unknown),
    last_error_line_no_(-1),
    last_error_char_no_(-1),
    last_error_msg_(""),
    allow_empty_arglist_(allow_empty_arglist),
    enable_semantics_(true),
    quit_(false),
    debug_(false),
    program_(),
    BaseObject()
{
    status(OK);
}

EqualityPredCalcParser::EqualityPredCalcParser(
    const EqualityPredCalcParser &src): 
        last_status_(src.last_status_),
        last_error_msg_(src.last_error_msg_),
        last_error_line_no_(src.last_error_line_no_),
        last_error_char_no_(src.last_error_char_no_),
        allow_empty_arglist_(src.allow_empty_arglist_),
        enable_semantics_(src.enable_semantics_),
        quit_(src.quit_),
        debug_(src.debug_),
        program_(src.program_),
        BaseObject(src)
{
    status(OK);
}

EqualityPredCalcParser::~EqualityPredCalcParser()
{
    status(NOTOK);
}

// assignment
EqualityPredCalcParser &
EqualityPredCalcParser::operator=(const EqualityPredCalcParser &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        last_status_ = rhs.last_status_;
        last_error_line_no_ = rhs.last_error_line_no_;
        last_error_char_no_ = rhs.last_error_char_no_;
        last_error_msg_ = rhs.last_error_msg_;
        allow_empty_arglist_ = rhs.allow_empty_arglist_;
        enable_semantics_ = rhs.enable_semantics_;
        quit_ = rhs.quit_;
        debug_ = rhs.debug_;
        program_ = rhs.program_;
        status(OK);
    }
    return(*this);
}

// parsing functions
EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse(EqualityPredCalcTokenizer &tokenizer,
                              Program &program)
{ DEBUGENTRY();
    ReturnType retval;

    // clear all caches 
    status(NOTOK);
    program.program().clear();
    program_.program().clear();
    last_error_msg_ = "";
    Symbol::clear_table();
    Symbol::clear_scope();

    if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    ParseFrame frame;

    ParseFrame::Data &global = frame.global();
    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    program_.form_type(Program::FT_Unknown);
    program_.logic_type(Program::LT_Unknown);

    do {
        quit_ = false;
        retval = parse_start(tokenizer);
    } while ((retval == Match) && (!quit_));

    if (!quit_)
    {
        tokenizer.report_syntax_error();
        report_semantic_error();
        ERRORRETURN(retval);
    }
    else
    {
        // successfully parsed the program. return the 
        // semantic details for further processing or
        // interpretation.
        program_.form_type(Program::FT_ParsedForm);
        if (program_.logic_type() == Program::LT_Unknown)
        {
            program_.logic_type(Program::LT_Propositional);
        }
        program = program_;
        status(OK);
        DEBUGRETURN(Match);
    }
}

// utility functions
EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_start(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::Quit)
    {
        quit_ = true;

        if (!tokenizer.advanceToNextToken())
        {
            // this is questionable. who really cares?
            ERRORRETURN(EndOfProgram);
        }
    
        ReturnType retval = parse_linesynch(tokenizer);
        switch (retval)
        {
        case NoMatch:
            // this is questionable. who really cares?
            ERRORRETURN(SyntaxError);
        case Match:
            DEBUGRETURN(Match);
        default:
            ERRORRETURN(retval);
        }
    }

    // start a new program
    ReturnType retval = parse_argument(tokenizer);
    switch (retval)
    {
    case NoMatch:
        break;
    case Match:
        if (enable_semantics_)
        {
            // place a marker to indicate start of program
            current.record_ = new Semantic(Semantic::StartArgument);
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
            program_.insert(current.record_);
        }
        retval = parse_expressionlist(tokenizer);
        switch (retval)
        {
        case NoMatch:
            break;
        case Match:
            break;
        default:
            ERRORRETURN(retval);
        }

        retval = parse_sosconclusionlist(tokenizer);
        switch (retval)
        {
        case NoMatch:
            break;
        case Match:
            break;
        default:
            ERRORRETURN(retval);
        }

        if (tokenizer.is_end_of_program())
        {
            ERRORRETURN(EndOfProgram);
        }
        else if (tokenizer.currentType() != EqualityPredCalcTokenizer::RightBrace)
        {
            ERRORRETURN(SyntaxError);
        }

        if (enable_semantics_)
        {
            // place a marker to indicate end of program
            current.record_ = new Semantic(Semantic::EndArgument);
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
            program_.insert(current.record_);
        }

        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }

        retval = parse_start(tokenizer);
        switch (retval)
        {
        case NoMatch:
            DEBUGRETURN(Match);
        case Match:
            DEBUGRETURN(Match);
        default:
            ERRORRETURN(retval);
        }

    default:
        ERRORRETURN(retval);
    }

    retval = parse_options(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        // we found options 
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_optionslist(tokenizer);
    switch (retval)
    {
    case NoMatch:
        break;
    case Match:
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::RightBrace)
    {
        ERRORRETURN(SyntaxError);
    }
    else
    {
        tokenizer.advanceToNextToken();
    }

    retval = parse_start(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_options(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Options)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::LeftBrace)
    {
        ERRORRETURN(SyntaxError);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }
    else
    {
        DEBUGRETURN(Match);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_optionslist(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Identifier)
    {
        DEBUGRETURN(NoMatch);
    }

    if (enable_semantics_)
    {
        current.name_ = tokenizer.currentValue();
    }

    if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Equal)
    {
        ERRORRETURN(SyntaxError);
    }

    if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_option_types(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        if (enable_semantics_)
        {
            current.value_ = returned.value_;
            current.record_ = new Semantic(Semantic::Option,
                                           current.name_,
                                           current.value_);
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
            program_.insert(current.record_);
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_linesynch(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_optionslist_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_optionslist_tail(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Identifier)
    {
        DEBUGRETURN(NoMatch);
    }

    if (enable_semantics_)
    {
        current.name_ = tokenizer.currentValue();
    }

    if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Equal)
    {
        ERRORRETURN(SyntaxError);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_option_types(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        if (enable_semantics_)
        {
            current.value_ = returned.value_;
            current.record_ = new Semantic(Semantic::Option,
                                           current.name_,
                                           current.value_);
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
            program_.insert(current.record_);
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_linesynch(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_optionslist(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);

    case Match:
        DEBUGRETURN(Match);

    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_option_types(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::String)
    {
        if (enable_semantics_)
        {
            current.type_ = ParseFrame::Data::String;
            current.value_ = tokenizer.currentValue();
        }
        tokenizer.advanceToNextToken();
        DEBUGRETURN(Match);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::Number)
    {
        if (enable_semantics_)
        {
            current.type_ = ParseFrame::Data::Number;
            current.value_ = tokenizer.currentValue();
        }
        tokenizer.advanceToNextToken();
        DEBUGRETURN(Match);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::NegativeNumber)
    {
        if (enable_semantics_)
        {
            current.type_ = ParseFrame::Data::NegativeNumber;
            current.value_ = tokenizer.currentValue();
        }
        tokenizer.advanceToNextToken();
        DEBUGRETURN(Match);
    }
    else
    {
        DEBUGRETURN(NoMatch);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_argument(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Argument)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::LeftBrace)
    {
        DEBUGRETURN(SyntaxError);
    }
    tokenizer.advanceToNextToken();
    return Match;
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_sosconclusionlist(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_sos_option(tokenizer);
    switch (retval)
    {
    case NoMatch:
        if (enable_semantics_)
        {
            current.type_ = returned.type_;
        }
        break;
    case Match:
        if (enable_semantics_)
        {
            current.type_ = returned.type_;
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_conclusiontype(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
            current.record_->set_of_support(
                (current.type_ == ParseFrame::Data::SetOfSupport));
            program_.insert(current.record_);
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_linesynch(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_sosconclusionlist(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_expressionlist(EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_separator_expressionlist(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_linesynch(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_expressionlist_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_expressionlist_tail(EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_separator_expressionlist(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_linesynch(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_expressionlist_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_separator_expressionlist(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_expression(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
            program_.insert(current.record_);
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_separator_expressionlist_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_separator_expressionlist_tail(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_separator(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_expression(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
            program_.insert(current.record_);
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_separator_expressionlist_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_sos_option(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::SetOfSupport)
    {
        if (enable_semantics_)
        {
            current.type_ = ParseFrame::Data::Unknown;
        }
        DEBUGRETURN(NoMatch);
    }

    if (enable_semantics_)
    {
        current.type_ = ParseFrame::Data::SetOfSupport;
    }

    tokenizer.advanceToNextToken();

    DEBUGRETURN(Match);
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_expression(EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_biconditional(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_biconditional(EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_implication(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    retval = parse_biconditional_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = new Semantic(Semantic::Biconditional, 
                                           current.record_,
                                           returned.record_);
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_biconditional_tail(EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Biconditional)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_implication(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_biconditional_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = new Semantic(Semantic::Biconditional, 
                                           current.record_,
                                           returned.record_);
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_implication(EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_or(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    retval = parse_implication_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = new Semantic(Semantic::Conditional, 
                                           current.record_,
                                           returned.record_);
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_implication_tail(EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Implication)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_or(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    retval = parse_implication_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = new Semantic(Semantic::Conditional, 
                                           current.record_,
                                           returned.record_);
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_or(EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_and(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_or_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = new Semantic(Semantic::Or, 
                                           current.record_,
                                           returned.record_);
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_or_tail(EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Or)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_and(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_or_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = new Semantic(Semantic::Or, 
                                           current.record_,
                                           returned.record_);
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_and(EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_unary(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_and_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = new Semantic(Semantic::And, 
                                           current.record_,
                                           returned.record_);
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_and_tail(EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::And)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_unary(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    retval = parse_and_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(Match);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = new Semantic(Semantic::And, 
                                           current.record_,
                                           returned.record_);
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_unary(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_atom(tokenizer);
    switch (retval)
    {
    case NoMatch:
        break;
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.currentType() == EqualityPredCalcTokenizer::Negation)
    {
        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }

        retval = parse_unary(tokenizer);
        switch (retval)
        {
        case NoMatch:
            ERRORRETURN(SyntaxError);
        case Match:
            if (enable_semantics_)
            {
                current.record_ = new Semantic(Semantic::Negation, 
                                               returned.record_);
                if (!semantic_checks_ok(current.record_))
                {
                    ERRORRETURN(SemanticError);
                }
            }
            DEBUGRETURN(Match);
        default:
            ERRORRETURN(retval);
        }
    }

    retval = parse_universal(tokenizer);
    switch (retval)
    {
    case NoMatch:
        break;
    case Match:
        if (enable_semantics_)
        {
            current.record_ = new Semantic(Semantic::Universal, 
                                           returned.name_);
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        retval = parse_unary(tokenizer);
        switch (retval)
        {
        case NoMatch:
            ERRORRETURN(SyntaxError);
        case Match:
            if (enable_semantics_)
            {
                current.record_->right() = returned.record_;
                if (!semantic_checks_ok(current.record_))
                {
                    ERRORRETURN(SemanticError);
                }
            }
            return Match;
        default:
            ERRORRETURN(retval);
        }
    default:
        ERRORRETURN(retval);
    }

    retval = parse_existential(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = new Semantic(Semantic::Existential, 
                                           returned.name_);
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        retval = parse_unary(tokenizer);
        switch (retval)
        {
        case NoMatch:
            ERRORRETURN(SyntaxError);
        case Match:
            if (enable_semantics_)
            {
                current.record_->right() = returned.record_;
                if (!semantic_checks_ok(current.record_))
                {
                    ERRORRETURN(SemanticError);
                }
            }
            DEBUGRETURN(Match);
        default:
            ERRORRETURN(retval);
        }
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_universal(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::LeftUniversal)
    {
        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }
        else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Identifier)
        {
            ERRORRETURN(SyntaxError);
        }

        if (enable_semantics_)
        {
            current.name_ = tokenizer.currentValue();
        }

        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }
        else if (tokenizer.currentType() != EqualityPredCalcTokenizer::RightUniversal)
        {
            ERRORRETURN(SyntaxError);
        }
        tokenizer.advanceToNextToken();

        if (enable_semantics_)
        {
            switch (program_.logic_type())
            {
            case Program::LT_Unknown:
            case Program::LT_Propositional:
                program_.logic_type(Program::LT_Predicate);
                break;
            case Program::LT_Predicate:
            case Program::LT_PredicateWithEquality:
                break;
            default:
                MustBeTrue(0);
            }
        }

        DEBUGRETURN(Match);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::Universal)
    {
        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }
        else if (tokenizer.currentType() != EqualityPredCalcTokenizer::LeftParenthesis)
        {
            ERRORRETURN(SyntaxError);
        }
        else if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }
        else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Identifier)
        {
            ERRORRETURN(SyntaxError);
        }

        if (enable_semantics_)
        {
            current.name_ = tokenizer.currentValue();
        }

        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }
        else if (tokenizer.currentType() != EqualityPredCalcTokenizer::RightParenthesis)
        {
            ERRORRETURN(SyntaxError);
        }
        tokenizer.advanceToNextToken();

        if (enable_semantics_)
        {
            switch (program_.logic_type())
            {
            case Program::LT_Unknown:
            case Program::LT_Propositional:
                program_.logic_type(Program::LT_Predicate);
                break;
            case Program::LT_Predicate:
            case Program::LT_PredicateWithEquality:
                break;
            default:
                MustBeTrue(0);
            }
        }

        DEBUGRETURN(Match);
    }
    else
    {
        DEBUGRETURN(NoMatch);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_existential(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::LeftExistential)
    {
        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }
        else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Identifier)
        {
            ERRORRETURN(SyntaxError);
        }

        if (enable_semantics_)
        {
            current.name_ = tokenizer.currentValue();
        }
        
        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }
        else if (tokenizer.currentType() != EqualityPredCalcTokenizer::RightExistential)
        {
            ERRORRETURN(SyntaxError);
        }
        tokenizer.advanceToNextToken();

        if (enable_semantics_)
        {
            switch (program_.logic_type())
            {
            case Program::LT_Unknown:
            case Program::LT_Propositional:
                program_.logic_type(Program::LT_Predicate);
                break;
            case Program::LT_Predicate:
            case Program::LT_PredicateWithEquality:
                break;
            default:
                MustBeTrue(0);
            }
        }

        DEBUGRETURN(Match);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::Existential)
    {
        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }
        else if (tokenizer.currentType() != EqualityPredCalcTokenizer::LeftParenthesis)
        {
            ERRORRETURN(SyntaxError);
        }
        else if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }
        else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Identifier)
        {
            ERRORRETURN(SyntaxError);
        }

        if (enable_semantics_)
        {
            current.name_ = tokenizer.currentValue();
        }

        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }
        else if (tokenizer.currentType() != EqualityPredCalcTokenizer::RightParenthesis)
        {
            ERRORRETURN(SyntaxError);
        }
        tokenizer.advanceToNextToken();

        if (enable_semantics_)
        {
            switch (program_.logic_type())
            {
            case Program::LT_Unknown:
            case Program::LT_Propositional:
                program_.logic_type(Program::LT_Predicate);
                break;
            case Program::LT_Predicate:
            case Program::LT_PredicateWithEquality:
                break;
            default:
                MustBeTrue(0);
            }
        }

        DEBUGRETURN(Match);
    }
    else
    {
        DEBUGRETURN(NoMatch);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_atom(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_predicate(tokenizer);
    switch (retval)
    {
    case NoMatch:
        break;
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::True)
    {
        if (enable_semantics_)
        {
            current.record_ = new Semantic(Semantic::LogicalConstant, "true");
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        tokenizer.advanceToNextToken();
        DEBUGRETURN(Match);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::False)
    {
        if (enable_semantics_)
        {
            current.record_ = new Semantic(Semantic::LogicalConstant, "false");
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        tokenizer.advanceToNextToken();
        DEBUGRETURN(Match);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::LeftParenthesis)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    retval = parse_expression(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::RightParenthesis)
    {
        ERRORRETURN(SyntaxError);
    }

    tokenizer.advanceToNextToken();
    DEBUGRETURN(Match);
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_predicate(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::PIdentifier)
    {
        if (enable_semantics_)
        {
            // for now assume it is a constant
            current.record_ = new Semantic(Semantic::PredicateConstant, 
                                           tokenizer.currentValue());
        }

        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }

        ReturnType retval = parse_predicate_identifier_tail(tokenizer);
        switch (retval)
        {
        case NoMatch:
            // semantic check: we assumed the identifier was a 
            // constant above. now check the symbol table and 
            // verify that any previous use or mention of the 
            // variable is not contradicted by the present use.
            if (enable_semantics_)
            {
                if (!semantic_checks_ok(current.record_))
                {
                    ERRORRETURN(SemanticError);
                }
            }
            DEBUGRETURN(Match);
        case Match:
            // we have a predicate function
            if (enable_semantics_)
            {
                // semantic check: we assumed the identifier was a 
                // constant above, but now we know it's a function.
                // now check the symbol table and verify that any 
                // previous use or mention of the identifier is not 
                // contradicted by the present use.
                current.record_->type(Semantic::PredicateFunction);
                current.record_->arguments().clear();
                current.record_->arguments().insert(
                    current.record_->arguments().begin(),
                    returned.arglist_.begin(),
                    returned.arglist_.end());
                if (!semantic_checks_ok(current.record_))
                {
                    ERRORRETURN(SemanticError);
                }
            }
            DEBUGRETURN(Match);
        default:
            ERRORRETURN(retval);
        }
    }
    else
    {
        ReturnType retval = parse_term(tokenizer);
        switch (retval)
        {
        case NoMatch:
            DEBUGRETURN(NoMatch);
        case Match:
            if (enable_semantics_)
            {
                current.record_ = returned.record_;
            }
            break;
        default:
            ERRORRETURN(retval);
        }

        if (tokenizer.is_end_of_program())
        {
            ERRORRETURN(EndOfProgram);
        }

        retval = parse_predicate_term_tail(tokenizer);
        switch (retval)
        {
        case NoMatch:
            ERRORRETURN(SyntaxError);
        case Match:
            if (enable_semantics_)
            {
                switch (returned.type_)
                {
                case ParseFrame::Data::Equal:
                    current.record_ = new Semantic(Semantic::Equal, 
                                                   current.record_,
                                                   returned.record_);
                    if (!semantic_checks_ok(current.record_))
                    {
                        ERRORRETURN(SemanticError);
                    }
                    DEBUGRETURN(Match);
                case ParseFrame::Data::NotEqual:
                    current.record_ = new Semantic(Semantic::Equal, 
                                                   current.record_,
                                                   returned.record_);
                    if (!semantic_checks_ok(current.record_))
                    {
                        ERRORRETURN(SemanticError);
                    }
                    current.record_ = new Semantic(Semantic::Negation, 
                                                   current.record_);
                    if (!semantic_checks_ok(current.record_))
                    {
                        ERRORRETURN(SemanticError);
                    }
                    DEBUGRETURN(Match);
                default:
                    ERRORRETURN(SyntaxError);
                }
            }
            DEBUGRETURN(Match);
        default:
            ERRORRETURN(retval);
        }
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_predicate_identifier_tail(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::LeftParenthesis)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_arglist(tokenizer);
    switch (retval)
    {
    case NoMatch:
        if (allow_empty_arglist_)
        {
            if (enable_semantics_)
            {
                current.arglist_.clear();
            }
            break; // this allows calls like P(), no arguments.
        }
        else
        {
            ERRORRETURN(SyntaxError);
        }
    case Match:
        if (enable_semantics_)
        {
            current.arglist_.clear();
            current.arglist_.insert(current.arglist_.end(),
                                    returned.arglist_.begin(),
                                    returned.arglist_.end());
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::RightParenthesis)
    {
        ERRORRETURN(SyntaxError);
    }
    tokenizer.advanceToNextToken();
    DEBUGRETURN(Match);
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_predicate_term_tail(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::Equal)
    {
        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }

        ReturnType retval = parse_term(tokenizer);
        switch (retval)
        {
        case NoMatch:
            ERRORRETURN(SyntaxError);
        case Match:
            if (enable_semantics_)
            {
                current.type_ = ParseFrame::Data::Equal;
                current.record_ = returned.record_;
                program_.logic_type(Program::LT_PredicateWithEquality);
            }
            DEBUGRETURN(Match);
        default:
            ERRORRETURN(retval);
        }
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::NotEqual)
    {
        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }

        ReturnType retval = parse_term(tokenizer);
        switch (retval)
        {
        case NoMatch:
            ERRORRETURN(SyntaxError);
        case Match:
            if (enable_semantics_)
            {
                current.type_ = ParseFrame::Data::NotEqual;
                current.record_ = returned.record_;
                program_.logic_type(Program::LT_PredicateWithEquality);
            }
            DEBUGRETURN(Match);
        default:
            ERRORRETURN(retval);
        }
    }
    else
    {
        DEBUGRETURN(NoMatch);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_arglist(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_arg(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        if (enable_semantics_)
        {
            current.arglist_.clear();
            current.arglist_.push_back(returned.record_);
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_arglist_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        if (enable_semantics_)
        {
            current.arglist_.insert(current.arglist_.end(),
                                    returned.arglist_.begin(),
                                    returned.arglist_.end());
        }
        DEBUGRETURN(Match);
    case Match:
        if (enable_semantics_)
        {
            current.arglist_.insert(current.arglist_.end(),
                                    returned.arglist_.begin(),
                                    returned.arglist_.end());
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_arglist_tail(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_separator(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    retval = parse_arg(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        if (enable_semantics_)
        {
            current.arglist_.clear();
            current.arglist_.push_back(returned.record_);
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    retval = parse_arglist_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        if (enable_semantics_)
        {
            current.arglist_.insert(current.arglist_.end(),
                                    returned.arglist_.begin(),
                                    returned.arglist_.end());
        }
        DEBUGRETURN(Match);
    case Match:
        if (enable_semantics_)
        {
            current.arglist_.insert(current.arglist_.end(),
                                    returned.arglist_.begin(),
                                    returned.arglist_.end());
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_arg(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_term(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_term(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_constant(tokenizer);
    switch (retval)
    {
    case NoMatch:
        break;
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }

    retval = parse_variable_function(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        if (enable_semantics_)
        {
            current.record_ = returned.record_;
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_constant(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::String)
    {
        if (enable_semantics_)
        {
            current.record_ = new Semantic(Semantic::QuotedString, 
                                           tokenizer.currentValue());
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        tokenizer.advanceToNextToken();
        DEBUGRETURN(Match);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::Number)
    {
        if (enable_semantics_)
        {
            current.record_ = new Semantic(Semantic::Number, 
                                           tokenizer.currentValue());
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        tokenizer.advanceToNextToken();
        DEBUGRETURN(Match);
    }
    else
    {
        DEBUGRETURN(NoMatch);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_variable_function(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Identifier)
    {
        DEBUGRETURN(NoMatch);
    }

    if (enable_semantics_)
    {
        current.record_ = new Semantic(Semantic::Variable,
                                       tokenizer.currentValue());
#if 0
        if (!semantic_checks_ok(current.record_))
        {
            ERRORRETURN(SemanticError);
        }
#endif
    }

    if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_variable_function_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        if (enable_semantics_)
        {
            // semantic check: we assumed the identifier was a 
            // variable above. now check the symbol table and 
            // verify that any previous use or mention of the 
            // variable is not contradicted by the present use.
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        DEBUGRETURN(Match);
    case Match:
        if (enable_semantics_)
        {
            // semantic check: we assumed the identifier was a 
            // constant above, but now we know it's a function.
            // now check the symbol table and verify that any 
            // previous use or mention of the identifier is not 
            // contradicted by the present use.
            current.record_->type(Semantic::Function);
            current.record_->arguments().clear();
            current.record_->arguments().insert(
                current.record_->arguments().begin(),
                returned.arglist_.begin(),
                returned.arglist_.end());
            if (!semantic_checks_ok(current.record_))
            {
                ERRORRETURN(SemanticError);
            }
        }
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_variable_function_tail(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::LeftParenthesis)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_arglist(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        if (enable_semantics_)
        {
            current.arglist_.clear();
            current.arglist_.insert(current.arglist_.end(),
                                    returned.arglist_.begin(),
                                    returned.arglist_.end());
        }
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::RightParenthesis)
    {
        ERRORRETURN(SyntaxError);
    }
    tokenizer.advanceToNextToken();
    DEBUGRETURN(Match);
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_separator(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Comma)
    {
        DEBUGRETURN(NoMatch);
    }
    tokenizer.advanceToNextToken();
    DEBUGRETURN(Match);
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_conclusiontype(
    EqualityPredCalcTokenizer &tokenizer)
{
    ParseFrame frame;

    ParseFrame::Data &current = frame.current();
    ParseFrame::Data &returned = frame.returned();

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::Therefore)
    {
        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }

        ReturnType retval = parse_expression(tokenizer);
        switch (retval)
        {
        case NoMatch:
            ERRORRETURN(SyntaxError);
        case Match:
            // we have a conclusion, turn off query
            if (enable_semantics_)
            {
                returned.record_->conclusion(true);
                returned.record_->query(false);
                current.record_ = returned.record_;
            }
            DEBUGRETURN(Match);
        default:
            ERRORRETURN(retval);
        }
    }
    else if (tokenizer.currentType() == EqualityPredCalcTokenizer::Query)
    {
        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }

        ReturnType retval = parse_expression(tokenizer);
        switch (retval)
        {
        case NoMatch:
            ERRORRETURN(SyntaxError);
        case Match:
            // we have a query, turn off conclusion
            if (enable_semantics_)
            {
                returned.record_->query(true);
                returned.record_->conclusion(false);
                current.record_ = returned.record_;
            }
            DEBUGRETURN(Match);
        default:
            ERRORRETURN(retval);
        }
    }
    else
    {
        DEBUGRETURN(NoMatch);
    }
}

EqualityPredCalcParser::ReturnType
EqualityPredCalcParser::parse_linesynch(
    EqualityPredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != EqualityPredCalcTokenizer::Semicolon)
    {
        DEBUGRETURN(NoMatch);
    }
    tokenizer.advanceToNextToken();
    DEBUGRETURN(Match);
}

// semantic checks
bool
EqualityPredCalcParser::semantic_checks_ok(ExtUseCntPtr<Semantic> &prec)
{
    if (prec == NULL)
    {
        last_error_msg_= "NULL Semantic record passed in.";
        return false;
    }

    switch (prec->type())
    {
    case Semantic::And:
    {
        // Left && Right
        if (prec->left() == NULL || prec->right() == NULL)
        {
            last_error_msg_= "And: either left or right "
                             "Semantic record is NULL.";
            return false;
        }
        return true;
    } 
    case Semantic::StartArgument:
    {
        // clear all other data
        prec->conclusion(false);
        prec->set_of_support(false);
        prec->query(false);
        prec->number_of_arguments(0);
        prec->name("");
        prec->value("");
        prec->left() = NULL;
        prec->right() = NULL;
        prec->arguments().clear();
        return true;
    } 
    case Semantic::EndArgument:
    {
        // clear all other data
        prec->conclusion(false);
        prec->set_of_support(false);
        prec->query(false);
        prec->number_of_arguments(0);
        prec->name("");
        prec->value("");
        prec->left() = NULL;
        prec->right() = NULL;
        prec->arguments().clear();
        return true;
    } 
    case Semantic::Biconditional:
    {
        // Left <--> Right 
        if (prec->left() == NULL || prec->right() == NULL)
        {
            last_error_msg_= "Biconditional: either left or right "
                             "Semantic record is NULL.";
            return false;
        }
        return true;
    } 
    case Semantic::Conditional:
    {
        // Left --> Right 
        if (prec->left() == NULL || prec->right() == NULL)
        {
            last_error_msg_= "Conditional: either left or right "
                             "Semantic record is NULL.";
            return false;
        }
        return true;
    } 
    case Semantic::Constant:
    {
        return true;
    } 
    case Semantic::Equal:
    {
        // Left = Right 
        if (prec->left() == NULL || prec->right() == NULL)
        {
            last_error_msg_= "Equality: either left or right "
                             "Semantic record is NULL.";
            return false;
        }
        return true;
    } 
    case Semantic::Existential:
    {
        // [var] expression 
        //        or
        // forsome(var) expression
        if (prec->right() == NULL)
        {
            // start of existential, check if variable is already in use
            // and if it is known.
            Symbol sym(prec->name(), Symbol::ExistentialVariable);
            if (Symbol::retrieve(sym))
            {
                // variable is known. check type.
                switch (sym.type())
                {
                case Symbol::Variable:
                case Symbol::UniversalVariable:
                    // reset to existential variable type
                    sym.type(Symbol::ExistentialVariable);
                case Symbol::ExistentialVariable:
                    // ok, it's been seen as a variable
                    break;

                default:
                    // oops, not a variable. can't change type.
                    last_error_msg_= 
                        "Existential: variable " + prec->name() +
                        " previously used but NOT as a variable";
                    return false;
                }
            }
            else if (!Symbol::insert(sym))
            {
                // insert failed
                last_error_msg_= 
                    "Existential: symbol insert for " + prec->name() +
                    " failed.";
                return false;
            }

            // check if variable is already in use.
            if (Symbol::find(sym))
            {
                // variable name already in use
                last_error_msg_= 
                    "Existential: variable " + prec->name() +
                    " already in use.";
                return false;
            }
            else if (!Symbol::push(sym))
            {
                // push failed
                last_error_msg_= 
                    "Existential: symbol push for " + prec->name() +
                    " failed.";
                return false;
            }
        }
        else
        {
            // end of existential: pop top symbol and check
            Symbol sym;
            if (!Symbol::top(sym))
            {
                // top failed
                last_error_msg_= 
                    "Existential: symbol top for " + prec->name() +
                    " failed.";
                return false;
            }
            else if (sym.name() != prec->name())
            {
                // expected name does not match found name.
                last_error_msg_= 
                    "Existential: expected variable name " + prec->name() +
                    " does NOT amatch found name " + sym.name() + ".";
                return false;
            }
            else if (sym.type() != Symbol::ExistentialVariable)
            {
                // not an existential variable
                last_error_msg_= 
                    "Existential: symbol " + sym.name() + " is NOT an " +
                    "existential variable type.";
                return false;
            }
            else if (sym.is_not_used())
            {
                // symbol is not used in quantifier.
                last_error_msg_= 
                    "Existential: variable " + prec->name() + " not used.";
                return false;
            }
            else if (!Symbol::pop())
            {
                // pop failed
                last_error_msg_= 
                    "Existential: symbol pop for " + prec->name() +
                    " failed.";
                return false;
            }
        }
        return true;
    } 
    case Semantic::Expression:
    {
        return true;
    } 
    case Semantic::Function:
    {
        Symbol sym(prec->name(), 
                   Symbol::Function,
                   prec->number_of_arguments());
        if (Symbol::retrieve(sym))
        {
            // identifier is known. must be a function.
            if (sym.type() != Symbol::Function)
            {
                // oops, not a constant. can't change type.
                last_error_msg_= 
                    "Function: identifier " + prec->name() +
                    " previously used but NOT as a function";
                return false;
            }
            else if (prec->number_of_arguments() != sym.number_of_arguments())
            {
                last_error_msg_= 
                    "Function: function " + prec->name() +
                    " number of arguments changed.";
                return false;
            }
        }
        else if (!Symbol::insert(sym))
        {
            // insert failed
            last_error_msg_= 
                "Function: symbol insert for " + prec->name() +
                " failed.";
            return false;
        }
        return true;
    } 
    case Semantic::LogicalConstant:
    {
        // "true" or "false"
        if (prec->value() != "true" && prec->value() != "false")
        {
            last_error_msg_= "LogicalConstant: not "
                             "set to 'true' or 'false'.";
            return false;
        }
        return true;
    } 
    case Semantic::Negation:
    {
        // ~ Right
        if (prec->left() != NULL)
        {
            last_error_msg_= "Negation: is a unary operation.";
            return false;
        }
        else if (prec->right() == NULL)
        {
            last_error_msg_= "Negation: right Semantic record is NULL.";
            return false;
        }
        return true;
    } 
    case Semantic::Number:
    {
        return true;
    } 
    case Semantic::Option:
    {
        // name = value
        if (prec->name() == "" || prec->value() == "")
        {
            last_error_msg_= "Option: either name or value is NULL";
            return false;
        }
        return true;
    } 
    case Semantic::Or:
    {
        // Left || Right
        if (prec->left() == NULL || prec->right() == NULL)
        {
            last_error_msg_= "Or: either left or right "
                             "Semantic record is NULL.";
            return false;
        }
        return true;
    } 
    case Semantic::Predicate:
    {
        return true;
    } 
    case Semantic::PredicateConstant:
    {
        Symbol sym(prec->name(), Symbol::PredicateConstant);
        if (Symbol::retrieve(sym))
        {
            // identifier is known. must be a constant.
            if (sym.type() != Symbol::PredicateConstant)
            {
                // oops, not a constant. can't change type.
                last_error_msg_= 
                    "PredicateConstant: variable " + prec->name() +
                    " previously used but NOT as a predicate constant";
                return false;
            }
        }
        else if (!Symbol::insert(sym))
        {
            // insert failed
            last_error_msg_= 
                "Universal: symbol insert for " + prec->name() +
                " failed.";
            return false;
        }
        return true;
    } 
    case Semantic::PredicateFunction:
    {
        Symbol sym(prec->name(), 
                   Symbol::PredicateFunction,
                   prec->number_of_arguments());
        if (Symbol::retrieve(sym))
        {
            // identifier is known. must be a function.
            if (sym.type() != Symbol::PredicateFunction)
            {
                // oops, not a constant. can't change type.
                last_error_msg_= 
                    "PredicateFunction: identifier " + prec->name() +
                    " previously used but NOT as a predicate function";
                return false;
            }
            else if (prec->number_of_arguments() != sym.number_of_arguments())
            {
                last_error_msg_= 
                    "PredicateFunction: function " + prec->name() +
                    " number of arguments changed.";
                return false;
            }
        }
        else if (!Symbol::insert(sym))
        {
            // insert failed
            last_error_msg_= 
                "PredicateFunction: symbol insert for " + prec->name() +
                " failed.";
            return false;
        }
        return true;
    } 
    case Semantic::QuotedString:
    {
        return true;
    } 
    case Semantic::Term:
    {
        return true;
    } 
    case Semantic::Universal:
    {
        // <var> expression 
        //        or
        // forall(var) expression
        if (prec->right() == NULL)
        {
            // start of universal, check if variable is already in use
            // and if it is known.
            Symbol sym(prec->name(), Symbol::UniversalVariable);
            if (Symbol::retrieve(sym))
            {
                // variable is known. check type.
                switch (sym.type())
                {
                case Symbol::Variable:
                case Symbol::ExistentialVariable:
                    // reset to universal variable type
                    sym.type(Symbol::UniversalVariable);
                case Symbol::UniversalVariable:
                    // ok, it's been seen as a variable
                    break;

                default:
                    // oops, not a variable. can't change type.
                    last_error_msg_= 
                        "Universal: variable " + prec->name() +
                        " previously used but NOT as a variable";
                    return false;
                }
            }
            else if (!Symbol::insert(sym))
            {
                // insert failed
                last_error_msg_= 
                    "Universal: symbol insert for " + prec->name() +
                    " failed.";
                return false;
            }

            // check if variable is already in use.
            if (Symbol::find(sym))
            {
                // variable name already in use
                last_error_msg_= 
                    "Universal: variable " + prec->name() +
                    " already in use.";
                return false;
            }
            else if (!Symbol::push(sym))
            {
                // push failed
                last_error_msg_= 
                    "Universal: symbol push for " + prec->name() +
                    " failed.";
                return false;
            }
        }
        else
        {
            // end of Universal: pop top symbol and check
            Symbol sym;
            if (!Symbol::top(sym))
            {
                // top failed
                last_error_msg_= 
                    "Universal: symbol top for " + prec->name() +
                    " failed.";
                return false;
            }
            else if (sym.name() != prec->name())
            {
                // expected name does not match found name.
                last_error_msg_= 
                    "Universal: expected variable name " + prec->name() +
                    " does NOT amatch found name " + sym.name() + ".";
                return false;
            }
            else if (sym.type() != Symbol::UniversalVariable)
            {
                // not an Universal variable
                last_error_msg_= 
                    "Universal: symbol " + sym.name() + " is NOT an " +
                    "universal variable type.";
                return false;
            }
            else if (sym.is_not_used())
            {
                // symbol is not used in quantifier.
                last_error_msg_= 
                    "Universal: variable " + prec->name() + " not used.";
                return false;
            }
            else if (!Symbol::pop())
            {
                // pop failed
                last_error_msg_= 
                    "Universal: symbol pop for " + prec->name() +
                    " failed.";
                return false;
            }
        }
        return true;
    } 
    case Semantic::Unknown:
    {
        return true;
    }
    case Semantic::Variable:
    {
        // check if variable is in scope
        Symbol scope_sym(prec->name());
        if (Symbol::find(scope_sym))
        {
            // in scope, so it must be a variable of some type.
            // get info from symbol table and verify consistency.
            Symbol sym(prec->name(), Symbol::Variable);
            if (Symbol::retrieve(sym))
            {
                // variable is known. check type.
                switch (sym.type())
                {
                case Symbol::ExistentialVariable:
                case Symbol::UniversalVariable:
                    // ok, it's been seen as a variable
                    break;
    
                default:
                    // oops, not a variable. can't change type.
                    last_error_msg_= 
                        "Variable: variable " + prec->name() +
                        " previously used but NOT as a variable";
                    return false;
                }

                // increment use count
                Symbol::increment(sym);
            }
            else
            {
                // in scope, but is not the symbol table?
                last_error_msg_= 
                    "Variable: unknown variable " + prec->name();
                return false;
            }
        }
        else
        {
            // not in scope, interprete as a constant.
            Symbol sym(prec->name(), Symbol::Constant);
            if (Symbol::retrieve(sym))
            {
                // we've seen it before, must be a constant.
                if (sym.type() != Symbol::Constant)
                {
                    last_error_msg_= 
                        "Constant: identifier " + prec->name() +
                        " changed type.";
                    return false;
                }
            }
            else if (!Symbol::insert(sym))
            {
                // insert failed
                last_error_msg_= 
                    "Constant: symbol insert for " + prec->name() +
                    " failed.";
                return false;
            }

            // reset type to constant
            prec->type(Semantic::Constant);
        }
        return true;
    } 
    case Semantic::Last:
    {
        return true;
    }
    default:
        return false;
    }
}

bool
EqualityPredCalcParser::is_type_ok(const std::string &name,
                                   Symbol::Type expected_type)
{
    Symbol sym(name, expected_type);
    if (Symbol::retrieve(sym))
    {
        if (sym.type() != expected_type)
        {
            last_error_msg_= 
                "For symbol: " + name + ", expected: " +
                Symbol::type_name(expected_type) + ", found: " +
                Symbol::type_name(sym.type());
            return false;
        }
    }
    else if ( ! Symbol::insert(sym))
    {
        last_error_msg_ = "For symbol, " + name + ", insert failed.";
        return false;
    }
    return true;
}

bool
EqualityPredCalcParser::is_type_ok(const std::string &name,
                                   Symbol::Type expected_type,
                                   int expected_nargs)
{
    Symbol sym(name, expected_type, expected_nargs);
    if (Symbol::retrieve(sym))
    {
        if (sym.type() != expected_type)
        {
            last_error_msg_= 
                "For symbol: " + name + ", expected: " +
                Symbol::type_name(expected_type) + ", found: " +
                Symbol::type_name(sym.type());
            return false;
        }
        else if (sym.number_of_arguments() != expected_nargs)
        {
            
            last_error_msg_= 
                "For symbol: " + name + ", expected nargs: " +
                to_string(sym.number_of_arguments()) + ", found nargs: " +
                to_string(expected_nargs);
            return false;
        }
    }
    else if ( ! Symbol::insert(sym))
    {
        last_error_msg_ = "For symbol, " + name + ", insert failed.";
        return false;
    }
    return true;
}

void
EqualityPredCalcParser::report_semantic_error()
{
    if (last_error_msg_ != "")
    {
        std::cerr << "ERROR ==>> " << last_error_msg_ << std::endl;
    }
}

#undef ENUMSTRINGIFY
#define ENUMSTRINGIFY(ENUM) { ENUM, #ENUM }

const std::string 
EqualityPredCalcParser::return_type(ReturnType retval) const
{
    static struct {
        ReturnType type_;
        const char *name_;
    } type_names[] = {
        ENUMSTRINGIFY(Unknown),
        ENUMSTRINGIFY(Match),
        ENUMSTRINGIFY(NoMatch),
        ENUMSTRINGIFY(SyntaxError),
        ENUMSTRINGIFY(EndOfProgram),
        ENUMSTRINGIFY(SemanticError),
        ENUMSTRINGIFY(Last)
    };
    return type_names[retval].name_;
}

#undef ENUMSTRINGIFY
}
