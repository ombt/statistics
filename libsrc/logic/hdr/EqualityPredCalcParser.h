//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// -------------------------------------------------------------------
// 
// this is the original yacc EBNF:
//
// start:
// 	/* empty */
// 	| start argument sosconclusionlist rbrace
// 	| start argument expressionlist rbrace
// 	| start argument expressionlist sosconclusionlist rbrace
// 	| start error RBRACE
// 	| start QUIT linesynch
// 	| start options RBRACE
// 	| start options optionslist RBRACE
// 
// options:
// 	OPTION LBRACE;
// 
// optionslist:
// 	IDENTIFIER EQUAL STRING linesynch
// 	| IDENTIFIER EQUAL NUMBER linesynch
// 	| IDENTIFIER EQUAL NEGNUMBER linesynch
// 	| optionslist IDENTIFIER EQUAL STRING linesynch
// 	| optionslist IDENTIFIER EQUAL NUMBER linesynch
// 	| optionslist IDENTIFIER EQUAL NEGNUMBER linesynch
// 	;
// 
// rbrace:	RBRACE
// 
// argument:
// 	ARGUMENT LBRACE
// 
// sosconclusionlist:
// 	sos_option conclusiontype linesynch
// 
// expressionlist:
// 	separator_expressionlist linesynch
// 	| expressionlist separator_expressionlist linesynch
// 
// separator_expressionlist:
// 	sos_option expression
// 	| separator_expressionlist separator sos_option expression
// 
// sos_option:
// 	/* empty */
// 	| SOS
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
// 	| UNIVERSAL LPAREN IDENTIFIER RPAREN
// 
// existential:
// 	LEFTEXISTENTIAL IDENTIFIER RIGHTEXISTENTIAL
// 	| EXISTENTIAL LPAREN IDENTIFIER RPAREN
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
// 	| term EQUAL term
// 	| term NOTEQUAL term
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
// conclusiontype:
// 	THEREFORE expression
// 	| QUERY expression
// 
// linesynch:
// 	SEMICOLON 
// 
// -------------------------------------------------------------------
// 
// this is the yacc ebnf converted so an LL(1) parser can handle it
//
// start ==>> 
//     /* empty */ |
//     argument (/* empty */ | expressionlist) 
//              (/* empty */ | sosconclusionlist) RBRACE start |
//     QUIT linesynch |
//     options optionslist RBRACE start
//
// options ==>> 
//     OPTION LBRACE
//
// optionslist ==>> 
//     /* empty */ | 
//     IDENTIFIER EQUAL option_types linesynch optionslist
//
// option_types ==>> 
//    STRING | 
//    NUMBER | 
//    NEGNUMBER 
//
// argument ==>> 
//     ARGUMENT LBRACE
// 
// sosconclusionlist ==>> 
//     /*empty*/ | 
//     sos_option conclusiontype linesynch sosconclusionlist 
// 
// expressionlist ==>> 
//     separator_expressionlist linesynch |
//     separator_expressionlist linesynch expressionlist_tail
// 
// expressionlist_tail ==>> 
//     separator_expressionlist linesynch |
//     separator_expressionlist linesynch expressionlist_tail
// 
// separator_expressionlist ==>> 
//     sos_option expression |
//     sos_option expression separator_expressionlist_tail
// 
// separator_expressionlist_tail ==>> 
//     separator sos_option expression |
//     separator sos_option expression separator_expressionlist_tail
// 
// sos_option ==>> 
//     /* empty */ | SOS
// 
// expression ==>> 
//     biconditional
// 
// biconditional ==>> 
//     implication |
//     implication biconditional_tail

// biconditional_tail ==>> 
//     BICONDITIONAL implication |
//     BICONDITIONAL implication biconditional_tail
// 
// implication ==>> 
//     or |
//     or implication_tail
//
// implication_tail ==>> 
//     IMPLICATION or |
//     IMPLICATION or implication_tail
// 
// or ==>> 
//     and |
//     and or_tail
//
// or_tail==>> 
//     OR and |
//     OR and or_tail
// 
// and ==>> 
//     unary |
//     unary and_tail
//
// and_tail ==>> 
//     AND unary |
//     AND unary and_tail
// 
// unary ==>> 
//     atom |
//     NEGATION unary |
//     universal unary |
//     existential unary
// 
// universal ==>> 
//     LEFTUNIVERSAL IDENTIFIER RIGHTUNIVERSAL |
//     UNIVERSAL LPAREN IDENTIFIER RPAREN
// 
// existential ==>> 
//     LEFTEXISTENTIAL IDENTIFIER RIGHTEXISTENTIAL |
//     EXISTENTIAL LPAREN IDENTIFIER RPAREN
// 
// atom ==>> 
//     predicate |
//     TRUE |
//     FALSE |
//     LPAREN expression RPAREN
// 
// predicate ==>> 
//     PIDENTIFIER predicate_identifier_tail |
//     term predicate_term_tail
//
// predicate_identifier_tail ==>> 
//     /* empty */ | 
//     LPAREN arglist RPAREN
//
// predicate_term_tail ==>> 
//     EQUAL term | 
//     NOTEQUAL term
// 
// arglist ==>> 
//     arg |
//     arg arglist_tail
// 
// arglist_tail ==>> 
//     separator arg |
//     separator arg arglist_tail
// 
// arg ==>> 
//     term
// 
// term ==>> 
//     constant |
//     variable_function
// 
// constant ==>> 
//     STRING |
//     NUMBER
// 
// variable_function ==>> 
//     IDENTIFIER variable_function_tail
//
// variable_function_tail ==>> 
//     /* empty */ | 
//     LPAREN arglist RPAREN
// 
// separator ==>> 
//     COMMA 
// 
// conclusiontype ==>> 
//     THEREFORE expression |
//     QUERY expression
// 
// linesynch ==>> 
//     SEMICOLON 
//
// -------------------------------------------------------------------
//

#ifndef __OMBT_EQUALITY_PREDICATE_CALCULUS_PARSER
#define __OMBT_EQUALITY_PREDICATE_CALCULUS_PARSER

// os headers
#include <string>

// headers
#include "atomic/BaseObject.h"
#include "logic/EqualityPredCalcTokenizer.h"
#include "logic/Semantic.h"
#include "logic/Program.h"
#include "logic/Symbol.h"

namespace ombt {

// forward declarations
class Program;

// parser class
class EqualityPredCalcParser: public BaseObject
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
        SemanticError,
        Last
    };

public:
    // ctors and dtor
    EqualityPredCalcParser(bool allow_empty_arglist = true);
    EqualityPredCalcParser(const EqualityPredCalcParser &src);
    ~EqualityPredCalcParser();

    // assignment
    EqualityPredCalcParser &operator=(const EqualityPredCalcParser &rhs);

    // parsing operations
    ReturnType parse(EqualityPredCalcTokenizer &tokenizer, 
                     Program &program);

    // misc
    inline bool debug() const
        { return debug_; }
    inline void debug(bool value)
        { debug_ = value; }
    inline bool enable_semantics() const
        { return enable_semantics_; }
    inline void enable_semantics(bool b)
        { enable_semantics_ = b; }

    // semantics checks
    bool is_type_ok(const std::string &, Symbol::Type);
    bool is_type_ok(const std::string &, Symbol::Type, int);
    bool semantic_checks_ok(ExtUseCntPtr<Semantic> &precord);
    void report_semantic_error();

protected:
    // utility functions
    ReturnType parse_start(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_options(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_optionslist(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_optionslist_tail(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_option_types(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_argument(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_sosconclusionlist(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_expressionlist(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_expressionlist_tail(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_separator_expressionlist(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_separator_expressionlist_tail(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_sos_option(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_expression(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_biconditional(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_biconditional_tail(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_implication(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_implication_tail(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_or(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_or_tail(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_and(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_and_tail(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_unary(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_universal(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_existential(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_atom(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_predicate(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_predicate_identifier_tail(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_predicate_term_tail(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_arglist(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_arglist_tail(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_arg(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_term(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_constant(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_variable_function(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_variable_function_tail(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_separator(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_conclusiontype(
        EqualityPredCalcTokenizer &tokenizer);
    ReturnType parse_linesynch(
        EqualityPredCalcTokenizer &tokenizer);

    const std::string return_type(ReturnType retval) const;

protected:
    // data
    static int ioffset_;
    ReturnType last_status_;
    long last_error_line_no_;
    long last_error_char_no_;
    std::string last_error_msg_;
    bool allow_empty_arglist_;
    bool enable_semantics_;
    bool quit_;
    bool debug_;
    Program program_;
};

}

#endif

//
// predicate calculus with equality parser
//
// 1) original set of rules.
// 
// start:
// 	/* empty */
// 	| start argument sosconclusionlist rbrace
// 	| start argument expressionlist rbrace
// 	| start argument expressionlist sosconclusionlist rbrace
// 	| start error RBRACE
// 	| start QUIT linesynch
// 	| start options RBRACE
// 	| start options optionslist RBRACE
// 
// options:
// 	OPTION LBRACE;
// 
// optionslist:
// 	IDENTIFIER EQUAL STRING linesynch
// 	| IDENTIFIER EQUAL NUMBER linesynch
// 	| IDENTIFIER EQUAL NEGNUMBER linesynch
// 	| optionslist IDENTIFIER EQUAL STRING linesynch
// 	| optionslist IDENTIFIER EQUAL NUMBER linesynch
// 	| optionslist IDENTIFIER EQUAL NEGNUMBER linesynch
// 	;
// 
// rbrace:	RBRACE
// 
// argument:
// 	ARGUMENT LBRACE
// 
// sosconclusionlist:
// 	sos_option conclusiontype linesynch
// 
// expressionlist:
// 	separator_expressionlist linesynch
// 	| expressionlist separator_expressionlist linesynch
// 
// separator_expressionlist:
// 	sos_option expression
// 	| separator_expressionlist separator sos_option expression
// 
// sos_option:
// 	/* empty */
// 	| SOS
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
// 	| UNIVERSAL LPAREN IDENTIFIER RPAREN
// 
// existential:
// 	LEFTEXISTENTIAL IDENTIFIER RIGHTEXISTENTIAL
// 	| EXISTENTIAL LPAREN IDENTIFIER RPAREN
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
// 	| term EQUAL term
// 	| term NOTEQUAL term
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
// conclusiontype:
// 	THEREFORE expression
// 	| QUERY expression
// 
// linesynch:
// 	SEMICOLON 
// 
// ==========================================================================
// 
// 2) slightly rewritten rules.
// 
// start ==>> /* empty */
// start ==>> start argument sosconclusionlist rbrace
// start ==>> start argument expressionlist rbrace
// start ==>> start argument expressionlist sosconclusionlist rbrace
// start ==>> start error RBRACE
// start ==>> start QUIT linesynch
// start ==>> start options RBRACE
// start ==>> start options optionslist RBRACE
// 
// options ==>> OPTION LBRACE;
// 
// optionslist ==>> IDENTIFIER EQUAL STRING linesynch
// optionslist ==>> IDENTIFIER EQUAL NUMBER linesynch
// optionslist ==>> IDENTIFIER EQUAL NEGNUMBER linesynch
// optionslist ==>> optionslist IDENTIFIER EQUAL STRING linesynch
// optionslist ==>> optionslist IDENTIFIER EQUAL NUMBER linesynch
// optionslist ==>> optionslist IDENTIFIER EQUAL NEGNUMBER linesynch
// 
// rbrace ==>> RBRACE
// 
// argument ==>> ARGUMENT LBRACE
// 
// sosconclusionlist ==>> sos_option conclusiontype linesynch
// 
// expressionlist ==>> separator_expressionlist linesynch
// expressionlist ==>> expressionlist separator_expressionlist linesynch
// 
// separator_expressionlist ==>> sos_option expression
// separator_expressionlist ==>> separator_expressionlist separator sos_option expression
// 
// sos_option ==>> /* empty */
// sos_option ==>> SOS
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
// universal ==>> UNIVERSAL LPAREN IDENTIFIER RPAREN
// 
// existential ==>> LEFTEXISTENTIAL IDENTIFIER RIGHTEXISTENTIAL
// existential ==>> EXISTENTIAL LPAREN IDENTIFIER RPAREN
// 
// atom ==>> predicate
// atom ==>> TRUE
// atom ==>> FALSE
// atom ==>> LPAREN expression RPAREN
// 
// predicate ==>> PIDENTIFIER
// predicate ==>> PIDENTIFIER LPAREN arglist RPAREN
// predicate ==>> term EQUAL term
// predicate ==>> term NOTEQUAL term
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
// conclusiontype ==>> THEREFORE expression
// conclusiontype ==>> QUERY expression
// 
// linesynch ==>> SEMICOLON 
// 
// ==========================================================================
// 
// 3) start converting to LL(1)
// 
// start ==>> /* empty */
// start ==>> start argument sosconclusionlist rbrace
// start ==>> start argument expressionlist rbrace
// start ==>> start argument expressionlist sosconclusionlist rbrace
// start ==>> start error RBRACE
// start ==>> start QUIT linesynch
// start ==>> start options RBRACE
// start ==>> start options optionslist RBRACE
// 
// options ==>> OPTION LBRACE;
// 
// optionslist ==>> IDENTIFIER EQUAL optionslist_types linesynch
// optionslist ==>> optionslist IDENTIFIER EQUAL optionslist_types linesynch
// 
// optionslist_types ==>> STRING | NUMBER | NEGNUMBER 
// 
// rbrace ==>> RBRACE
// 
// argument ==>> ARGUMENT LBRACE
// 
// sosconclusionlist ==>> sos_option conclusiontype linesynch
// 
// expressionlist ==>> separator_expressionlist linesynch
// expressionlist ==>> expressionlist separator_expressionlist linesynch
// 
// separator_expressionlist ==>> sos_option expression
// separator_expressionlist ==>> separator_expressionlist separator sos_option expression
// 
// sos_option ==>> /* empty */
// sos_option ==>> SOS
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
// universal ==>> UNIVERSAL LPAREN IDENTIFIER RPAREN
// 
// existential ==>> LEFTEXISTENTIAL IDENTIFIER RIGHTEXISTENTIAL
// existential ==>> EXISTENTIAL LPAREN IDENTIFIER RPAREN
// 
// atom ==>> predicate
// atom ==>> TRUE
// atom ==>> FALSE
// atom ==>> LPAREN expression RPAREN
// 
// predicate ==>> PIDENTIFIER predicate_identifier_tail
// predicate ==>> term predicate_term_tail
// predicate_identifier_tail ==>> /* empty */ | LPAREN arglist RPAREN
// predicate_term_tail ==>> EQUAL term | NOTEQUAL term
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
// variable_function ==>> IDENTIFIER variable_function_tail
// function_function_tail ==>> /* empty */ | LPAREN arglist RPAREN
// 
// separator ==>> COMMA 
// 
// conclusiontype ==>> THEREFORE expression
// conclusiontype ==>> QUERY expression
// 
// linesynch ==>> SEMICOLON 
// 
// ==========================================================================
// 
// 4) left-recursion and common prefixes removed.
// 
// start ==>> argument start_argument_tail
// 
// start_argument_tail ==>> sosconclusionlist rbrace
// start_argument_tail ==>> expressionlist start_argument_tail_expressionlist_tail
// start_argument_tail_expressionlist_tail ==>> rbrace | sosconclusionlist rbrace
// start ==>> error RBRACE
// start ==>> QUIT linesynch
// start ==>> options start_options_tail
// start_options_tail ==>> RBRACE | optionslist RBRACE
// 
// options ==>> OPTION LBRACE;
// 
// ORIGINAL: optionslist ==>> IDENTIFIER EQUAL optionslist_types linesynch
// ORIGINAL: optionslist ==>> optionslist IDENTIFIER EQUAL optionslist_types linesynch
// 
// optionslist ==>> IDENTIFIER EQUAL optionslist_types linesynch
// optionslist ==>> IDENTIFIER EQUAL optionslist_types linesynch optionslist_tail
// optionslist_tail ==>> IDENTIFIER EQUAL optionslist_types linesynch
// optionslist_tail ==>> IDENTIFIER EQUAL optionslist_types linesynch optionslist_tail
// 
// optionslist_types ==>> STRING | NUMBER | NEGNUMBER 
// 
// rbrace ==>> RBRACE
// 
// argument ==>> ARGUMENT LBRACE
// 
// sosconclusionlist ==>> /*empty*/ | sos_option conclusiontype linesynch sosconclusionlist 
// 
// ORIGINAL: expressionlist ==>> separator_expressionlist linesynch
// ORIGINAL: expressionlist ==>> expressionlist separator_expressionlist linesynch
// 
// expressionlist ==>> separator_expressionlist linesynch
// expressionlist ==>> separator_expressionlist linesynch expressionlist_tail
// expressionlist_tail==>> separator_expressionlist linesynch
// expressionlist_tail==>> separator_expressionlist linesynch expressionlist_tail
// 
// ORIGINAL: separator_expressionlist ==>> sos_option expression
// ORIGINAL: separator_expressionlist ==>> separator_expressionlist separator sos_option expression
// 
// SEMDONE: separator_expressionlist ==>> sos_option expression
// SEMDONE: separator_expressionlist ==>> sos_option expression separator_expressionlist_tail
// SEMDONE: separator_expressionlist_tail ==>> separator sos_option expression
// SEMDONE: separator_expressionlist_tail ==>> separator sos_option expression separator_expressionlist_tail
// 
// SEMDONE: sos_option ==>> /* empty */
// SEMDONE: sos_option ==>> SOS
// 
// SEMDONE: expression ==>> biconditional
// 
// ORIGINAL: biconditional ==>> implication
// ORIGINAL: biconditional ==>> biconditional BICONDITIONAL implication
// 
// SEMDONE: biconditional ==>> implication
// SEMDONE: biconditional ==>> implication biconditional_tail
// SEMDONE: biconditional_tail ==>> BICONDITIONAL implication
// SEMDONE: biconditional_tail ==>> BICONDITIONAL implication biconditional_tail
// 
// ORIGINAL: implication ==>> or
// ORIGINAL: implication ==>> implication IMPLICATION or
// 
// SEMDONE: implication ==>> or
// SEMDONE: implication ==>> or implication_tail
// SEMDONE: implication_tail ==>> IMPLICATION or
// SEMDONE: implication_tail ==>> IMPLICATION or implication_tail
// 
// ORIGINAL: or ==>> and
// ORIGINAL: or ==>> or OR and
// 
// SEMDONE: or ==>> and
// SEMDONE: or ==>> and or_tail
// SEMDONE: or_tail==>> OR and
// SEMDONE: or_tail==>> OR and or_tail
// 
// ORIGINAL: and ==>> unary
// ORIGINAL: and ==>> and AND unary
// 
// SEMDONE: and ==>> unary
// SEMDONE: and ==>> unary and_tail
// SEMDONE: and_tail ==>> AND unary
// SEMDONE: and_tail ==>> AND unary and_tail
// 
// SEMDONE: unary ==>> atom
// SEMDONE: unary ==>> NEGATION unary
// SEMDONE: unary ==>> universal unary
// SEMDONE: unary ==>> existential unary
// 
// SEMDONE: universal ==>> LEFTUNIVERSAL IDENTIFIER RIGHTUNIVERSAL
// SEMDONE: universal ==>> UNIVERSAL LPAREN IDENTIFIER RPAREN
// 
// SEMDONE: existential ==>> LEFTEXISTENTIAL IDENTIFIER RIGHTEXISTENTIAL
// SEMDONE: existential ==>> EXISTENTIAL LPAREN IDENTIFIER RPAREN
// 
// SEMDONE: atom ==>> predicate
// SEMDONE: atom ==>> TRUE
// SEMDONE: atom ==>> FALSE
// SEMDONE: atom ==>> LPAREN expression RPAREN
// 
// SEMDONE: predicate ==>> PIDENTIFIER predicate_identifier_tail
// SEMDONE: predicate ==>> term predicate_term_tail
// SEMDONE: predicate_identifier_tail ==>> /* empty */ | LPAREN arglist RPAREN
// SEMDONE: predicate_term_tail ==>> EQUAL term | NOTEQUAL term
// 
// ORIGINAL: arglist ==>> arg
// ORIGINAL: arglist ==>> arglist separator arg
// 
// SEMDONE: arglist ==>> arg
// SEMDONE: arglist ==>> arg arglist_tail
// SEMDONE: arglist_tail ==>> separator arg
// SEMDONE: arglist_tail ==>> separator arg arglist_tail
// 
// SEMDONE: arg ==>> term
// 
// SEMDONE: term ==>> constant
// SEMDONE: term ==>> variable_function
// 
// SEMDONE: constant ==>> STRING
// SEMDONE: constant ==>> NUMBER
// 
// SEMDONE: variable_function ==>> IDENTIFIER variable_function_tail
// SEMDONE: variable_function_tail ==>> /* empty */ | LPAREN arglist RPAREN
// 
// SEMDONE: separator ==>> COMMA 
// 
// SEMDONE: conclusiontype ==>> THEREFORE expression
// SEMDONE: conclusiontype ==>> QUERY expression
// 
// SEMDONE: linesynch ==>> SEMICOLON 
//
// ==========================================================================
// 
// 5) rewrite the start again ... 
//
// ORIGINAL: start ==>> /* empty */
// ORIGINAL: start ==>> start argument sosconclusionlist rbrace
// ORIGINAL: start ==>> start argument expressionlist rbrace
// ORIGINAL: start ==>> start argument expressionlist sosconclusionlist rbrace
// ORIGINAL: start ==>> start error RBRACE
// ORIGINAL: start ==>> start QUIT linesynch
// ORIGINAL: start ==>> start options RBRACE
// ORIGINAL: start ==>> start options optionslist RBRACE
// 
// ORIGINAL: options ==>> OPTION LBRACE;
// 
// ORIGINAL: optionslist ==>> IDENTIFIER EQUAL STRING linesynch
// ORIGINAL: optionslist ==>> IDENTIFIER EQUAL NUMBER linesynch
// ORIGINAL: optionslist ==>> IDENTIFIER EQUAL NEGNUMBER linesynch
// ORIGINAL: optionslist ==>> optionslist IDENTIFIER EQUAL STRING linesynch
// ORIGINAL: optionslist ==>> optionslist IDENTIFIER EQUAL NUMBER linesynch
// ORIGINAL: optionslist ==>> optionslist IDENTIFIER EQUAL NEGNUMBER linesynch
//
// 1st: optionslist ==>> IDENTIFIER EQUAL option_types linesynch
// 1st: optionslist ==>> optionslist IDENTIFIER EQUAL option_types linesynch
//
// 2nd: optionslist ==>> optionslist IDENTIFIER EQUAL option_types linesynch
//
// A -> A a1 | ... | A an | B1 | ... |Bm
//
// rewrite to:
//
// A -> B1 A' | ... | Bm A'
// A' -> /* empty */ | a1 A' | ... | an A'
//
// 3rd: optionslist ==>> /* empty */ | IDENTIFIER EQUAL option_types linesynch optionslist
//
// option_types ==>> STRING | NUMBER | NEGNUMBER 
//
// 1st: start ==>> /* empty */
// 1st: start ==>> argument sosconclusionlist rbrace start
// 1st: start ==>> argument expressionlist rbrace start
// 1st: start ==>> argument expressionlist sosconclusionlist rbrace start
// 1st: start ==>> QUIT linesynch start
// 1st: start ==>> options optionslist RBRACE start
// 
// 1st: options ==>> OPTION LBRACE
//
// final rewrite - i hope:
//
// start ==>> /* empty */
// start ==>> argument 
//           (/* empty */ | expressionlist) 
//           (/* empty */ | sosconclusionlist} RBRACE start
// start ==>> QUIT linesynch
// SEMDONE: start ==>> options optionslist RBRACE start
// SEMDONE: options ==>> OPTION LBRACE
// SEMDONE: optionslist ==>> /* empty */ | IDENTIFIER EQUAL option_types linesynch optionslist
// SEMDONE: option_types ==>> STRING | NUMBER | NEGNUMBER 
