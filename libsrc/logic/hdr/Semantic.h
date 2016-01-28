//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_SEMANTIC
#define __OMBT_SEMANTIC

// semantic records representing logic-based languages

// os headers
#include <string>
#include <list>
#include <iostream>

// headers
#include "atomic/BaseObject.h"
#include "atomic/ExtUseCntPtr.h"
#include "logic/Symbol.h"

namespace ombt {

// semantic record for logic-base languages
class Semantic: public BaseObject
{
public:
    // types of records
    enum Type
    {
        And, 
        StartArgument, 
        EndArgument, 
        Biconditional, 
        Conditional, 
        Constant, 
        Equal, 
        Existential, 
        Expression, 
        Function, 
        LogicalConstant, 
        Negation, 
        Number, 
        Option, 
        Or, 
        Predicate, 
        PredicateConstant, 
        PredicateFunction, 
        QuotedString, 
        Term, 
        Universal, 
        Unknown,
        Variable, 
        Last
    };

    typedef ExtUseCntPtr<Semantic> ArgType;
    typedef std::list<ArgType> ArgListType;
    typedef ArgListType::iterator ArgListTypeIter;
    typedef ArgListType::const_iterator ArgListTypeCIter;

    typedef std::map<std::string, ArgType> SkolemMapType;
    typedef SkolemMapType::iterator SkolemMapTypeIter;
    typedef SkolemMapType::const_iterator SkolemMapTypeCIter;

    typedef std::map<std::string, std::string> RenameMapType;
    typedef RenameMapType::iterator RenameMapTypeIter;
    typedef RenameMapType::const_iterator RenameMapTypeCIter;

public:
    // ctors and dtor
    Semantic();
    Semantic(const Semantic &src);
    Semantic(Type);
    Semantic(Type, const std::string &); 
    Semantic(Type, const std::string &, const std::string &); 
    Semantic(Type, ExtUseCntPtr<Semantic> &);
    Semantic(Type, ExtUseCntPtr<Semantic> &, ExtUseCntPtr<Semantic> &);
    Semantic(Type, const std::string &, ExtUseCntPtr<Semantic> &);
    ~Semantic();

    // assignment
    Semantic &operator=(const Semantic &rhs);

    // typing info
    inline bool isPredicate() const
        { return type_ == Predicate; }
    inline bool isExpression() const
        { return type_ == Expression; }
    inline bool isTerm() const
        { return type_ == Term; }

    // get/set functions
    inline bool conclusion() const
        { return conclusion_; }
    inline void conclusion(bool c)
        { conclusion_ = c; }

    inline bool set_of_support() const
        { return set_of_support_; }
    inline void set_of_support(bool sos)
        { set_of_support_ = sos; }

    inline bool query() const
        { return query_; }
    inline void query(bool q)
        { query_ = q; }

    inline int number_of_arguments() const
        { return number_of_arguments_; }
    inline void number_of_arguments(int na)
        { number_of_arguments_ = na; }

    inline Type type() const
        { return type_; }
    inline void type(Type t)
        { type_ = t; }

    inline std::string name() const
        { return name_; }
    inline void name(const std::string &n)
        { name_ = n; }

    inline std::string value() const
        { return value_; }
    inline void value(const std::string &n)
        { value_ = n; }

    inline ExtUseCntPtr<Semantic> &left() 
        { return left_; }

    inline ExtUseCntPtr<Semantic> &right() 
        { return right_; }

    inline ArgListType &arguments()
        { return arguments_; }

    // functions to convert to CNF
    int removeBiconditionals();
    int removeConditionals();
    int removeExtraNots();
    int demorgans();
    int renameVariables();
    int skolemize();
    int removeUniversals();
    int distributionOrsOverAnds();
    int distributionAndsOverOrs();
    int removeAnds(std::list<ExtUseCntPtr<Semantic> > &);
    int renameAgain();
    int removeOrs(std::list<ExtUseCntPtr<Semantic> > &);

    // print data
    void reconstruct(std::string &) const;
    friend std::ostream &operator<<(std::ostream &, const Semantic &);
    static const std::string type_name(Type);

protected:
    // helper functions
    int renameVariables(Symbol::StackType &);
    int skolemize(Symbol::StackType &, SkolemMapType &);
    int distributionOrsOverAnds(bool &);
    int distributionAndsOverOrs(bool &);
    int removeAnds(std::list<ExtUseCntPtr<Semantic> > &, bool, bool, bool);
    int renameAgain(RenameMapType &);
    int removeOrs(std::list<ExtUseCntPtr<Semantic> > &, bool, bool, bool);
    void dumpData(std::ostream &) const;

    static std::string uniqueName(const std::string &);

protected:
    // internal data
    bool conclusion_;
    bool set_of_support_;
    bool query_;
    int number_of_arguments_;
    Type type_;
    std::string name_;
    std::string value_;
    ExtUseCntPtr<Semantic> left_;
    ExtUseCntPtr<Semantic> right_;
    ArgListType arguments_;
};

}

#endif

