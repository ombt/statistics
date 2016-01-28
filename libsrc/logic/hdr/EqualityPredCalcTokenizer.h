//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_EQUALITY_PREDICATE_CALCULUS_TOKENIZER
#define __OMBT_EQUALITY_PREDICATE_CALCULUS_TOKENIZER

// predicate calculus wth equality tokenizer

// os headers
#include <string>

// headers
#include "atomic/BaseObject.h"
#include "logic/IncludeFiles.h"

namespace ombt {

// tokenizer class
class EqualityPredCalcTokenizer: public BaseObject
{
public:
    // token enumerations
    enum Type 
    {
        Unknown = 0,      // unused character
        WhiteSpace,       // [ \t]+
        Pound,            // #[^\n]*\n
        NewLine,          // \n
        Therefore,        // |-
        Query,            // ?-
        Biconditional,    // <-->
        Implication,      // -->
        Or,               // ||
        And,              // &&
        Negation,         // ~ or !
        LeftParenthesis,  // (
        RightParenthesis, // )
        Comma,            // ,
        Semicolon,        // ;
        Universal,        // forall, ignore case
        LeftUniversal,    // <
        RightUniversal,   // >
        Existential,      // forsome, ignore case
        LeftExistential,  // [
        RightExistential, // ]
        LeftBrace,        // {
        RightBrace,       // }
        Equal,            // =
        NotEqual,         // !=
        Argument,         // argument
        Quit,             // quit
        True,             // true, ignore case
        False,            // false, ignore case
        SetOfSupport,     // sos, ignore case
        Options,          // options, ignore case
        Number,           // [0-9]+
        NegativeNumber,   // -[0-9]+
        String,           // "[^"]*"
        Identifier,       // [a-z][a-zA-Z0-9_]*
        PIdentifier,      // [A-Z][a-zA-Z0-9_]*
        EndOfProgram,     // end-of-file or end-of-string
        IncludeFile,      // #include "filename"
        Last
    };

    struct Reporter
    {
        Reporter(const bool &dbg, const EqualityPredCalcTokenizer &pct): 
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
        const EqualityPredCalcTokenizer &pct_;
    };

public:
    // ctors and dtor
    EqualityPredCalcTokenizer(bool skip_newline = true);
    EqualityPredCalcTokenizer(const EqualityPredCalcTokenizer &src);
    ~EqualityPredCalcTokenizer();

    // assignment
    EqualityPredCalcTokenizer &operator=(const EqualityPredCalcTokenizer &rhs);

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

    // misc
    inline void addIncludeDirectory(const std::string &directory)
        { includes_.addDirectory(directory); }
    inline bool debug() const
        { return debug_; }
    inline void debug(bool dbg)
        { debug_ = dbg; }
    inline bool skip_newline() const
        { return skip_newline_; }
    inline void skip_newline(bool skip_nl)
        { skip_newline_ = skip_nl; }

protected:
    // utility functions
    void skip_white_space();
    void accept_token();
    void advance_end_to(char end_char);
    bool check_and_advance_end_to(const std::string &chars);
    bool nc_check_and_advance_end_to(const std::string &chars);
    bool identifier_and_advance_end_to();
    bool pidentifier_and_advance_end_to();
    bool number_and_advance_end_to();
    void accept_as_is_or_with_suffix(const std::string &suffix,
                                     Type as_is_type,
                                     Type with_suffix_type);
    bool process_include_file();

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
    bool skip_newline_;
    mutable Type token_type_;
    long program_length_;
    std::string program_;
    std::string filename_;
    IncludeFiles includes_;
};

}

#endif
