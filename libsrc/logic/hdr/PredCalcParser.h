//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_PREDICATE_CALCULUS_PARSER
#define __OMBT_PREDICATE_CALCULUS_PARSER

//
// predicate calculus parser
//
// 1) original rules:
// 
// start:
// 	/* empty */
// 	| start conclusion linesynch
// 	| start expressionlist conclusion linesynch
// 	| start newline
// 	| start error newline
// 	| start QUIT linesynch
// 	| start QUIT newline
// 
// expressionlist:
// 	separator_expressionlist linesynch
// 	| expressionlist separator_expressionlist linesynch
// 
// separator_expressionlist:
// 	expression
// 	| separator_expressionlist separator expression
// 
// expression:
// 	biconditional
// 
// biconditional:
// 	implication
// 	| biconditional BICONDITIONAL implication
// 
// implication:
// 	or
// 	| implication IMPLICATION or
// 
// or:
// 	and
// 	| or OR and
// 
// and:
// 	unary
// 	| and AND unary
// 
// unary:
// 	atom
// 	| NEGATION unary
// 	| universal unary
// 	| existential unary
// 
// universal:
// 	LEFTUNIVERSAL IDENTIFIER RIGHTUNIVERSAL
// 
// existential:
// 	LEFTEXISTENTIAL IDENTIFIER RIGHTEXISTENTIAL
// 
// atom:
// 	predicate
// 	| TRUE
// 	| FALSE
// 	| LPAREN expression RPAREN
// 
// predicate:
// 	PIDENTIFIER
// 	| PIDENTIFIER LPAREN arglist RPAREN
// 
// arglist:
// 	arg
// 	| arglist separator arg
// 
// arg:
// 	term
// 
// term:
// 	constant
// 	| variable
// 	| function
// 
// constant:
// 	STRING
// 	| NUMBER
// 
// variable:
// 	IDENTIFIER
// 
// function:
// 	IDENTIFIER LPAREN arglist RPAREN
// 	
// separator:
// 	COMMA 
// 
// conclusion:
// 	THEREFORE expression
// 
// linesynch:
// 	SEMICOLON newline
// 
// newline:
// 	NEWLINE
// 
// =========================================================================
// 
// 2) slightly rewritten ruls.
// 
// start ==>>  /* empty */
// start ==>>  start conclusion linesynch
// start ==>>  start expressionlist conclusion linesynch
// start ==>>  start newline
// start ==>>  start error newline
// start ==>>  start QUIT linesynch
// start ==>>  start QUIT newline
// 
// expressionlist ==>> separator_expressionlist linesynch
// expressionlist ==>> expressionlist separator_expressionlist linesynch
// 
// separator_expressionlist ==>> expression
// separator_expressionlist ==>> separator_expressionlist separator expression
// 
// expression ==>> biconditional
// 
// biconditional ==>> implication
// biconditional ==>> biconditional BICONDITIONAL implication
// 
// implication ==>> or
// implication ==>> implication IMPLICATION or
// 
// or ==>> and
// or ==>> or OR and
// 
// and ==>> unary
// and ==>> and AND unary
// 
// unary ==>> atom
// unary ==>> NEGATION unary
// unary ==>> universal unary
// unary ==>> existential unary
// 
// universal ==>> LEFTUNIVERSAL IDENTIFIER RIGHTUNIVERSAL
// 
// existential ==>> LEFTEXISTENTIAL IDENTIFIER RIGHTEXISTENTIAL
// 
// atom ==>> predicate
// atom ==>> TRUE
// atom ==>> FALSE
// atom ==>> LPAREN expression RPAREN
// 
// predicate ==>> PIDENTIFIER
// predicate ==>> PIDENTIFIER LPAREN arglist RPAREN
// 
// arglist ==>> arg
// arglist ==>> arglist separator arg
// 
// arg ==>> term
// 
// term ==>> constant
// term ==>> variable
// term ==>> function
// 
// constant ==>> STRING
// constant ==>> NUMBER
// 
// variable ==>> IDENTIFIER
// 
// function ==>> IDENTIFIER LPAREN arglist RPAREN
// 	
// separator ==>> COMMA 
// 
// conclusion ==>> THEREFORE expression
// 
// linesynch ==>> SEMICOLON newline
// 
// newline ==>> NEWLINE
// 
// =========================================================================
// 
// 3) start converting to LL(1).
// 
// ORIGINAL: start ==>>  /* empty */
// ORIGINAL: start ==>>  start conclusion linesynch
// ORIGINAL: start ==>>  start expressionlist conclusion linesynch
// ORIGINAL: start ==>>  start newline
// ORIGINAL: start ==>>  start error newline
// ORIGINAL: start ==>>  start QUIT linesynch
// ORIGINAL: start ==>>  start QUIT newline
// 
// start ==>>  /* empty */
// start ==>>  /* empty */ start_tail
// 
// start_tail ==>>  conclusion linesynch
// start_tail ==>>  conclusion linesynch start_tail
// start_tail ==>>  expressionlist conclusion linesynch
// start_tail ==>>  expressionlist conclusion linesynch start_tail
// start_tail ==>>  newline
// start_tail ==>>  newline start_tail
// start_tail ==>>  error newline
// start_tail ==>>  error newline start_tail
// start_tail ==>>  QUIT linesynch
// start_tail ==>>  QUIT linesynch start_tail
// start_tail ==>>  QUIT newline
// start_tail ==>>  QUIT newline start_tail
// 
// -------------------------------------------------------------------
// 
// ORIGINAL: expressionlist ==>> separator_expressionlist linesynch
// ORIGINAL: expressionlist ==>> expressionlist separator_expressionlist linesynch
// 
// expressionlist ==>> separator_expressionlist linesynch
// expressionlist ==>> separator_expressionlist linesynch expressionlist_tail
// 
// expressionlist_tail ==>> separator_expressionlist linesynch
// expressionlist_tail ==>> separator_expressionlist linesynch expressionlist_tail
// 
// -------------------------------------------------------------------
// 
// ORIGINAL: separator_expressionlist ==>> expression
// ORIGINAL: separator_expressionlist ==>> separator_expressionlist separator expression
// 
// separator_expressionlist ==>> expression
// separator_expressionlist ==>> expression separator_expressionlist_tail
// 
// separator_expressionlist_tail ==>> separator expression
// separator_expressionlist_tail ==>> separator expression separator_expressionlist_tail
// 
// -------------------------------------------------------------------
// 
// // expression ==>> biconditional
// 
// -------------------------------------------------------------------
// 
// ORIGINAL: biconditional ==>> implication
// ORIGINAL: biconditional ==>> biconditional BICONDITIONAL implication
// 
// 
// biconditional ==>> implication
// biconditional ==>> implication biconditional_tail
// 
// biconditional_tail ==>> BICONDITIONAL implication
// biconditional_tail ==>> BICONDITIONAL implication biconditional_tail
// 
// -------------------------------------------------------------------
// 
// ORIGINAL: implication ==>> or
// ORIGINAL: implication ==>> implication IMPLICATION or
// 
// implication ==>> or
// implication ==>> or implication_tail
// 
// implication_tail ==>> IMPLICATION or
// implication_tail ==>> IMPLICATION or implication_tail
// 
// -------------------------------------------------------------------
// 
// ORIGINAL: or ==>> and
// ORIGINAL: or ==>> or OR and
// 
// or ==>> and
// or ==>> and or_tail
// 
// or_tail ==>> OR and
// or_tail ==>> OR and or_tail
// 
// -------------------------------------------------------------------
// 
// ORIGINAL: and ==>> unary
// ORIGINAL: and ==>> and AND unary
// 
// and ==>> unary
// and ==>> unary and_tail
// 
// and_tail ==>> AND unary
// and_tail ==>> AND unary and_tail
// 
// -------------------------------------------------------------------
// 
// unary ==>> atom
// unary ==>> NEGATION unary
// unary ==>> universal unary
// unary ==>> existential unary
// 
// -------------------------------------------------------------------
// 
// universal ==>> LEFTUNIVERSAL IDENTIFIER RIGHTUNIVERSAL
// 
// -------------------------------------------------------------------
// 
// existential ==>> LEFTEXISTENTIAL IDENTIFIER RIGHTEXISTENTIAL
// 
// -------------------------------------------------------------------
// 
// atom ==>> predicate
// atom ==>> TRUE
// atom ==>> FALSE
// atom ==>> LPAREN expression RPAREN
// 
// -------------------------------------------------------------------
// 
// predicate ==>> PIDENTIFIER
// predicate ==>> PIDENTIFIER LPAREN arglist RPAREN
// 
// -------------------------------------------------------------------
// 
// ORIGINAL: arglist ==>> arg
// ORIGINAL: arglist ==>> arglist separator arg
// 
// arglist ==>> arg
// arglist ==>> arg arglist_tail
// 
// arglist_tail ==>> separator arg
// arglist_tail ==>> separator arg arglist_tail
// 
// -------------------------------------------------------------------
// 
// arg ==>> term
// 
// -------------------------------------------------------------------
// 
// term ==>> constant
// term ==>> variable
// term ==>> function
// 
// -------------------------------------------------------------------
// 
// constant ==>> STRING
// constant ==>> NUMBER
// 
// -------------------------------------------------------------------
// 
// variable ==>> IDENTIFIER
// 
// -------------------------------------------------------------------
// 
// function ==>> IDENTIFIER LPAREN arglist RPAREN
// 
// -------------------------------------------------------------------
// 	
// separator ==>> COMMA 
// 
// -------------------------------------------------------------------
// 
// conclusion ==>> THEREFORE expression
// 
// -------------------------------------------------------------------
// 
// linesynch ==>> SEMICOLON newline
// 
// -------------------------------------------------------------------
// 
// newline ==>> NEWLINE
// 
// -------------------------------------------------------------------
//
// 4) left-recursion and common prefixes are removed.
//
// start ==>>  conclusion linesynch
// start ==>>  expressionlist conclusion linesynch
// start ==>>  newline
// start ==>>  error newline
// start ==>>  QUIT start_quit_tail
// start_quit_tail==>>  newline | linesynch
//
// expressionlist ==>> separator_expressionlist linesynch
// expressionlist ==>> separator_expressionlist linesynch expressionlist_tail
// expressionlist_tail ==>> separator_expressionlist linesynch
// expressionlist_tail ==>> separator_expressionlist linesynch expressionlist_tail
//
// separator_expressionlist ==>> expression
// separator_expressionlist ==>> expression separator_expressionlist_tail
// separator_expressionlist_tail ==>> separator expression
// separator_expressionlist_tail ==>> separator expression separator_expressionlist_tail
//
// expression ==>> biconditional
//
// biconditional ==>> implication
// biconditional ==>> implication biconditional_tail
// biconditional_tail ==>> BICONDITIONAL implication
// biconditional_tail ==>> BICONDITIONAL implication biconditional_tail
//
// implication ==>> or
// implication ==>> or implication_tail
// implication_tail ==>> IMPLICATION or
// implication_tail ==>> IMPLICATION or implication_tail
//
// or ==>> and
// or ==>> and or_tail
// or_tail ==>> OR and
// or_tail ==>> OR and or_tail
//
// and ==>> unary
// and ==>> unary and_tail
// and_tail ==>> AND unary
// and_tail ==>> AND unary and_tail
//
// unary ==>> atom
// unary ==>> NEGATION unary
// unary ==>> universal unary
// unary ==>> existential unary
//
// universal ==>> LEFTUNIVERSAL IDENTIFIER RIGHTUNIVERSAL
//
// existential ==>> LEFTEXISTENTIAL IDENTIFIER RIGHTEXISTENTIAL
//
// atom ==>> predicate
// atom ==>> TRUE
// atom ==>> FALSE
// atom ==>> LPAREN expression RPAREN
//
// predicate ==>> PIDENTIFIER predicate_suffix
// predicate_suffix ==>> /* empty */ | LPAREN arglist RPAREN
//
// arglist ==>> arg
// arglist ==>> arg arglist_tail
// arglist_tail ==>> separator arg
// arglist_tail ==>> separator arg arglist_tail
//
// arg ==>> term
//
// term ==>> constant
// term ==>> variable
// term ==>> function
//
// constant ==>> STRING
// constant ==>> NUMBER
//
// ORIGINAL: variable ==>> IDENTIFIER
// ORIGINAL: function ==>> IDENTIFIER LPAREN arglist RPAREN
//
// variable_function ==>> IDENTIFIER variable_function_tail
// variable_function_tail ==>> /* empty */ | LPAREN arglist RPAREN
//
// separator ==>> COMMA 
//
// conclusion ==>> THEREFORE expression
//
// linesynch ==>> SEMICOLON newline
//
// newline ==>> NEWLINE
//
// -------------------------------------------------------------------
//

// os headers
#include <string>

// headers
#include "atomic/BaseObject.h"
#include "logic/PredCalcTokenizer.h"

namespace ombt {

// parser class
class PredCalcParser: public BaseObject
{
public:
    // data types
    enum ReturnType
    {
        Unknown = 0,
        Match,
        NoMatch,
        SyntaxError,
        EndOfProgram,
        Last
    };

public:
    // ctors and dtor
    PredCalcParser();
    PredCalcParser(const PredCalcParser &src);
    ~PredCalcParser();

    // assignment
    PredCalcParser &operator=(const PredCalcParser &rhs);

    // parsing operations
    ReturnType parse(PredCalcTokenizer &tokenizer);

    // misc
    inline void allow_empty_arglist(bool value) {
        allow_empty_arglist_ = value;
    }
    inline void allow_empty_file(bool value) {
        allow_empty_file_ = value;
    }
    inline void debug(bool value) {
       debug_ = value;
    }

protected:
    // utility functions
    ReturnType parse_start(PredCalcTokenizer &tokenizer);
    ReturnType parse_start_quit_tail(PredCalcTokenizer &tokenizer);
    ReturnType parse_expressionlist(PredCalcTokenizer &tokenizer);
    ReturnType parse_expressionlist_tail(PredCalcTokenizer &tokenizer);
    ReturnType parse_separator_expressionlist(PredCalcTokenizer &tokenizer);
    ReturnType parse_separator_expressionlist_tail(PredCalcTokenizer &tokenizer);
    ReturnType parse_expression(PredCalcTokenizer &tokenizer);
    ReturnType parse_biconditional(PredCalcTokenizer &tokenizer);
    ReturnType parse_biconditional_tail(PredCalcTokenizer &tokenizer);
    ReturnType parse_implication(PredCalcTokenizer &tokenizer);
    ReturnType parse_implication_tail(PredCalcTokenizer &tokenizer);
    ReturnType parse_or(PredCalcTokenizer &tokenizer);
    ReturnType parse_or_tail(PredCalcTokenizer &tokenizer);
    ReturnType parse_and(PredCalcTokenizer &tokenizer);
    ReturnType parse_and_tail(PredCalcTokenizer &tokenizer);
    ReturnType parse_unary(PredCalcTokenizer &tokenizer);
    ReturnType parse_universal(PredCalcTokenizer &tokenizer);
    ReturnType parse_existential(PredCalcTokenizer &tokenizer);
    ReturnType parse_atom(PredCalcTokenizer &tokenizer);
    ReturnType parse_predicate(PredCalcTokenizer &tokenizer);
    ReturnType parse_predicate_suffix(PredCalcTokenizer &tokenizer);
    ReturnType parse_arglist(PredCalcTokenizer &tokenizer);
    ReturnType parse_arglist_tail(PredCalcTokenizer &tokenizer);
    ReturnType parse_arg(PredCalcTokenizer &tokenizer);
    ReturnType parse_term(PredCalcTokenizer &tokenizer);
    ReturnType parse_constant(PredCalcTokenizer &tokenizer);
    ReturnType parse_variable(PredCalcTokenizer &tokenizer);
    ReturnType parse_function(PredCalcTokenizer &tokenizer);
    ReturnType parse_variable_function(PredCalcTokenizer &tokenizer);
    ReturnType parse_variable_function_tail(PredCalcTokenizer &tokenizer);
    ReturnType parse_separator(PredCalcTokenizer &tokenizer);
    ReturnType parse_conclusion(PredCalcTokenizer &tokenizer);
    ReturnType parse_linesynch(PredCalcTokenizer &tokenizer);
    ReturnType parse_newline(PredCalcTokenizer &tokenizer);

    const std::string return_type(ReturnType retval) const;

protected:
    // data
    ReturnType last_status_;
    long last_error_line_no_;
    long last_error_char_no_;
    std::string last_error_msg_;
    bool allow_empty_arglist_;
    bool allow_empty_file_;
    bool quit_;
    bool debug_;
};

}

#endif

