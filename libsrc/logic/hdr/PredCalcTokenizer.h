//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_PREDICATE_CALCULUS_TOKENIZER
#define __OMBT_PREDICATE_CALCULUS_TOKENIZER

// predicate calculus tokenizer

// os headers
#include <string>

// headers
#include "atomic/BaseObject.h"

namespace ombt {

// tokenizer class
class PredCalcTokenizer: public BaseObject
{
public:
    // token enumerations
    enum Type 
    {
        Unknown = 0,          // unused character
        WhiteSpace,       // [ \t]+
        Pound,            // #[^\n]*\n
        NewLine,          // \n
        Therefore,        // |-
        Biconditional,    // <-->
        Implication,      // -->
        Or,               // ||
        And,              // &&
        Negation,         // ~
        LeftParenthesis,  // (
        RightParenthesis, // )
        Comma,            // ,
        Semicolon,        // ;
        LeftUniversal,    // <
        RightUniversal,   // >
        LeftExistential,  // [
        RightExistential, // ]
        Quit,             // quit
        True,             // True
        False,            // False
        Number,           // [0-9]+
        String,           // "[^"]*"
        Identifier,       // [a-z][a-zA-Z0-9_]*
        PIdentifier,      // [A-Z][a-zA-Z0-9_]*
        EndOfProgram      // end-of-file or end-of-string
    };

    struct Reporter
    {
        Reporter(const bool &dbg, const PredCalcTokenizer &pct): 
                     debug_(dbg), pct_(pct) { dump("Entry: "); }
        ~Reporter() { dump("Return: "); }

        void dump(const std::string &msg) const
        {
            if (debug_)
            {
                std::cerr << msg
                          << pct_.currentValue()
                          << ","
                          << pct_.currentTypeName()
                          << std::endl;
            }
        }

        const bool &debug_;
        const PredCalcTokenizer &pct_;
    };

public:
    // ctors and dtor
    PredCalcTokenizer();
    PredCalcTokenizer(const PredCalcTokenizer &src);
    ~PredCalcTokenizer();

    // assignment
    PredCalcTokenizer &operator=(const PredCalcTokenizer &rhs);

    // tokenizing operations
    void setFile(const std::string &filename);
    void setProgram(const std::string &program);
    void resetProgram();
    bool advanceToNextToken();
    Type currentType() const;
    const std::string currentValue() const;
    const std::string currentTypeName() const;
    bool is_end_of_program() const;
    void report_syntax_error();
    void debug(bool dbg)
        { debug_ = dbg; }
    bool debug() const
        { return debug_; }

protected:
    // utility functions
    void skip_white_space();
    void accept_token();
    void advance_end_to(char end_char);
    bool check_and_advance_end_to(const std::string &chars);
    bool identifier_and_advance_end_to();
    bool pidentifier_and_advance_end_to();
    bool number_and_advance_end_to();
    void accept_as_is_or_with_suffix(const std::string &suffix,
                                     Type as_is_type,
                                     Type with_suffix_type);

    // inline utility functions
    inline bool is_a_to_z(char c) const
        { return ('a' <= c && c <= 'z'); }
    inline bool is_A_to_Z(char c) const
        { return ('A' <= c && c <= 'Z'); }
    inline bool is_0_to_9(char c) const
        { return ('0' <= c && c <= '9'); }

protected:
    // data
    bool debug_;
    long line_no_;
    long line_start_pos_;
    long token_start_pos_;
    long token_end_pos_;
    mutable Type token_type_;
    long program_length_;
    std::string program_;
    std::string filename_;
};

}

#endif
