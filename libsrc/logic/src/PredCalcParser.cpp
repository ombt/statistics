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
#include "hdr/PredCalcParser.h"

namespace ombt {

#define ERRORRETURN(RETVAL) { \
    std::cerr << __FILE__ \
    << ":" \
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
    return(RETVAL); \
}

#define DEBUGENTRY() { \
    if (debug_) { \
        std::cerr << __FILE__ \
        << ":" \
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
        std::cerr << __FILE__ \
        << ":" \
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
    return(RETVAL); \
}

// ctors and dtor
PredCalcParser::PredCalcParser(): 
    last_status_(Unknown),
    last_error_line_no_(-1),
    last_error_char_no_(-1),
    last_error_msg_(""),
    allow_empty_arglist_(true),
    allow_empty_file_(true),
    quit_(false),
    debug_(false),
    BaseObject()
{
    status(OK);
}

PredCalcParser::PredCalcParser(
    const PredCalcParser &src): 
        last_status_(src.last_status_),
        last_error_msg_(src.last_error_msg_),
        last_error_line_no_(src.last_error_line_no_),
        last_error_char_no_(src.last_error_char_no_),
        allow_empty_arglist_(src.allow_empty_arglist_),
        allow_empty_file_(src.allow_empty_file_),
        quit_(src.quit_),
        debug_(src.debug_),
        BaseObject(src)
{
    status(OK);
}

PredCalcParser::~PredCalcParser()
{
}

// assignment
PredCalcParser &
PredCalcParser::operator=(const PredCalcParser &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        last_status_ = rhs.last_status_;
        last_error_line_no_ = rhs.last_error_line_no_;
        last_error_char_no_ = rhs.last_error_char_no_;
        last_error_msg_ = rhs.last_error_msg_;
        allow_empty_arglist_ = rhs.allow_empty_arglist_;
        allow_empty_file_ = rhs.allow_empty_file_;
        quit_ = rhs.quit_;
        debug_ = rhs.debug_;
        status(OK);
    }
    return(*this);
}

// parsing functions

PredCalcParser::ReturnType
PredCalcParser::parse(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    ReturnType retval;

    status(NOTOK);
    if (!tokenizer.advanceToNextToken())
    {
        if (allow_empty_file_)
        {
            status(OK);
            DEBUGRETURN(Match);
        }
        else
        {
            ERRORRETURN(EndOfProgram);
        }
    }

    do {
        quit_ = false;
        retval = parse_start(tokenizer);
    } while ((retval == Match) && (!quit_));

    if (!quit_)
    {
        tokenizer.report_syntax_error();
        ERRORRETURN(retval);
    }
    else
    {
        status(NOTOK);
        DEBUGRETURN(Match);
    }
}

// utility functions
PredCalcParser::ReturnType
PredCalcParser::parse_start(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_conclusion(tokenizer);
    switch (retval)
    {
    case NoMatch:
        break;

    case Match:
        retval = parse_linesynch(tokenizer);
        switch (retval)
        {
        case NoMatch:
            ERRORRETURN(SyntaxError);

        case Match:
            DEBUGRETURN(Match);

        default:
            ERRORRETURN(retval);
        }
    default:
        ERRORRETURN(retval);
    }

    retval = parse_expressionlist(tokenizer);
    switch (retval)
    {
    case NoMatch:
        break;

    case Match:
        retval = parse_conclusion(tokenizer);
        switch (retval)
        {
        case NoMatch:
            ERRORRETURN(SyntaxError);
        case Match:
            retval = parse_linesynch(tokenizer);
            switch (retval)
            {
            case NoMatch:
                ERRORRETURN(SyntaxError);

            case Match:
                DEBUGRETURN(Match);

            default:
                ERRORRETURN(retval);
            }

        default:
            ERRORRETURN(retval);
        }
    default:
        ERRORRETURN(retval);
    }

    retval = parse_newline(tokenizer);
    switch (retval)
    {
    case NoMatch:
        break;

    case Match:
        DEBUGRETURN(Match);

    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.currentType() != PredCalcTokenizer::Quit)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    quit_ = true;

    retval = parse_start_quit_tail(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);

    case Match:
        DEBUGRETURN(Match);

    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType 
PredCalcParser::parse_start_quit_tail(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_newline(tokenizer);
    switch (retval)
    {
    case NoMatch:
        break;
    case Match:
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }

    retval = parse_linesynch(tokenizer);
    switch (retval)
    {
    case NoMatch:
        DEBUGRETURN(NoMatch);
    case Match:
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType
PredCalcParser::parse_expressionlist(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
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

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
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

PredCalcParser::ReturnType
PredCalcParser::parse_expressionlist_tail(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
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

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
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

PredCalcParser::ReturnType
PredCalcParser::parse_separator_expressionlist(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
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

PredCalcParser::ReturnType
PredCalcParser::parse_separator_expressionlist_tail(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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

    retval = parse_expression(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
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

PredCalcParser::ReturnType
PredCalcParser::parse_expression(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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
        break;
    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType
PredCalcParser::parse_biconditional(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType
PredCalcParser::parse_biconditional_tail(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::Biconditional)
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
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType
PredCalcParser::parse_implication(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType
PredCalcParser::parse_implication_tail(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::Implication)
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
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType
PredCalcParser::parse_or(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    retval = parse_or_tail(tokenizer);
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

PredCalcParser::ReturnType
PredCalcParser::parse_or_tail(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::Or)
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
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }

    retval = parse_or_tail(tokenizer);
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

PredCalcParser::ReturnType
PredCalcParser::parse_and(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType
PredCalcParser::parse_and_tail(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::And)
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
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType
PredCalcParser::parse_unary(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.currentType() == PredCalcTokenizer::Negation)
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
        retval = parse_unary(tokenizer);
        switch (retval)
        {
        case NoMatch:
            ERRORRETURN(SyntaxError);
        case Match:
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
        retval = parse_unary(tokenizer);
        switch (retval)
        {
        case NoMatch:
            ERRORRETURN(SyntaxError);
        case Match:
            DEBUGRETURN(Match);
        default:
            ERRORRETURN(retval);
        }
    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType
PredCalcParser::parse_universal(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::LeftUniversal)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::Identifier)
    {
        ERRORRETURN(SyntaxError);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::RightUniversal)
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

PredCalcParser::ReturnType
PredCalcParser::parse_existential(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::LeftExistential)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::Identifier)
    {
        ERRORRETURN(SyntaxError);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::RightExistential)
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

PredCalcParser::ReturnType
PredCalcParser::parse_atom(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() == PredCalcTokenizer::True)
    {
        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }
        else
        {
            DEBUGRETURN(Match);
        }
    }
    else if (tokenizer.currentType() == PredCalcTokenizer::False)
    {
        if (!tokenizer.advanceToNextToken())
        {
            ERRORRETURN(EndOfProgram);
        }
        else
        {
            DEBUGRETURN(Match);
        }
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::LeftParenthesis)
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
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::RightParenthesis)
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

PredCalcParser::ReturnType
PredCalcParser::parse_predicate(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::PIdentifier)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_predicate_suffix(tokenizer);
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

PredCalcParser::ReturnType 
PredCalcParser::parse_predicate_suffix(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::LeftParenthesis)
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
            break; // this allows calls like P(), no arguments.
        else
            ERRORRETURN(SyntaxError);
    case Match:
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::RightParenthesis)
    {
        ERRORRETURN(SyntaxError);
    }
    tokenizer.advanceToNextToken();
    DEBUGRETURN(Match);
}

PredCalcParser::ReturnType
PredCalcParser::parse_arglist(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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
        break;
    default:
        ERRORRETURN(retval);
    }

    retval = parse_arglist_tail(tokenizer);
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

PredCalcParser::ReturnType
PredCalcParser::parse_arglist_tail(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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
        DEBUGRETURN(Match);
    case Match:
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType
PredCalcParser::parse_arg(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType
PredCalcParser::parse_term(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
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
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType
PredCalcParser::parse_constant(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() == PredCalcTokenizer::String)
    {
        tokenizer.advanceToNextToken();
        DEBUGRETURN(Match);
    }
    else if (tokenizer.currentType() == PredCalcTokenizer::Number)
    {
        tokenizer.advanceToNextToken();
        DEBUGRETURN(Match);
    }
    DEBUGRETURN(NoMatch);
}

PredCalcParser::ReturnType
PredCalcParser::parse_variable_function(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::Identifier)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_variable_function_tail(tokenizer);
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

PredCalcParser::ReturnType
PredCalcParser::parse_variable_function_tail(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::LeftParenthesis)
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
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::RightParenthesis)
    {
        ERRORRETURN(SyntaxError);
    }
    tokenizer.advanceToNextToken();
    DEBUGRETURN(Match);
}

PredCalcParser::ReturnType
PredCalcParser::parse_variable(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::Identifier)
    {
        DEBUGRETURN(NoMatch);
    }
    tokenizer.advanceToNextToken();
    DEBUGRETURN(Match);
}

PredCalcParser::ReturnType
PredCalcParser::parse_function(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::Identifier)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    if (tokenizer.currentType() != PredCalcTokenizer::LeftParenthesis)
    {
        ERRORRETURN(SyntaxError);
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
        break;
    default:
        ERRORRETURN(retval);
    }

    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::RightParenthesis)
    {
        ERRORRETURN(SyntaxError);
    }
    tokenizer.advanceToNextToken();
    DEBUGRETURN(Match);
}

PredCalcParser::ReturnType
PredCalcParser::parse_separator(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::Comma)
    {
        DEBUGRETURN(NoMatch);
    }
    tokenizer.advanceToNextToken();
    DEBUGRETURN(Match);
}

PredCalcParser::ReturnType
PredCalcParser::parse_conclusion(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::Therefore)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_expression(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType
PredCalcParser::parse_linesynch(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::Semicolon)
    {
        DEBUGRETURN(NoMatch);
    }
    else if (!tokenizer.advanceToNextToken())
    {
        ERRORRETURN(EndOfProgram);
    }

    ReturnType retval = parse_newline(tokenizer);
    switch (retval)
    {
    case NoMatch:
        ERRORRETURN(SyntaxError);
    case Match:
        DEBUGRETURN(Match);
    default:
        ERRORRETURN(retval);
    }
}

PredCalcParser::ReturnType
PredCalcParser::parse_newline(PredCalcTokenizer &tokenizer)
{ DEBUGENTRY();
    if (tokenizer.is_end_of_program())
    {
        ERRORRETURN(EndOfProgram);
    }
    else if (tokenizer.currentType() != PredCalcTokenizer::NewLine)
    {
        DEBUGRETURN(NoMatch);
    }
    tokenizer.advanceToNextToken();
    DEBUGRETURN(Match);
}

#undef ENUMSTRINGIFY
#define ENUMSTRINGIFY(ENUM) { ENUM, #ENUM }

const std::string 
PredCalcParser::return_type(ReturnType retval) const
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
        ENUMSTRINGIFY(Last)
    };
    return type_names[retval].name_;
}

#undef ENUMSTRINGIFY
}
