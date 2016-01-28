//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_PROGRAM
#define __OMBT_PROGRAM

// list of semantic records representing a program

// os headers
#include <string>
#include <list>
#include <map>
#include <iostream>

// headers
#include "atomic/BaseObject.h"
#include "atomic/ExtUseCntPtr.h"
#include "logic/Semantic.h"

namespace ombt {

// list semantic records for logic-base languages
class Program: public BaseObject
{
public:
    // data types
    enum FormType
    {
        FT_Unknown = -1,
        FT_ParsedForm = 0,
        FT_ConjunctiveNormalForm,
        FT_DisjunctiveNormalForm,
        FT_ClauseForm,
    };

    enum LogicType
    {
        LT_Unknown = -1,
        LT_Propositional = 0,
        LT_Predicate,
        LT_PredicateWithEquality,
    };

    typedef std::list<ExtUseCntPtr<Semantic> > Type;
    typedef Type::iterator TypeIter;
    typedef Type::const_iterator TypeCIter;

    typedef std::map<std::string, ExtUseCntPtr<Semantic> > OptionsType;
    typedef OptionsType::iterator OptionsTypeIter;
    typedef OptionsType::const_iterator OptionsTypeCIter;

public:
    // ctors and dtor
    Program();
    Program(const Program &src);
    virtual ~Program();

    // assignment
    Program &operator=(const Program &rhs);

    // access functions
    inline Type &program()
        { return program_; }
    inline const Type &program() const
        { return program_; }
    inline OptionsType &options()
        { return options_; }
    inline const OptionsType &options() const
        { return options_; }
    inline Type &goals()
        { return goals_; }
    inline const Type &goals() const
        { return goals_; }
    inline Type &queries()
        { return queries_; }
    inline const Type &queries() const
        { return queries_; }

    inline void insert(ExtUseCntPtr<Semantic> &expr)
    {
        program_.push_back(expr);
        if (expr->type() == Semantic::Option)
        {
            options_.insert(OptionsType::value_type(expr->name(), expr));
        }
        else if (expr->conclusion())
        {
            goals_.push_back(expr);
        }
        else if (expr->query())
        {
            queries_.push_back(expr);
        }
    }

    inline int size() const
        { return program_.size(); }
    inline int options_size() const
        { return options_.size(); }
    inline int goals_size() const
        { return goals_.size(); }
    inline int queries_size() const
        { return queries_.size(); }

    inline FormType form_type() const
        { return form_type_; }
    inline void form_type(FormType ftype)
        { form_type_ = ftype; }
    inline LogicType logic_type() const
        { return logic_type_; }
    inline void logic_type(LogicType ltype)
        { logic_type_ = ltype; }

    inline void clear_all()
    {
        program_.clear();
        options_.clear();
        goals_.clear();
        queries_.clear();
    }

    // print data
    virtual void reconstruct(std::ostream &) const;
    friend std::ostream &operator<<(std::ostream &, const Program &);

protected:
    // internal data
    FormType form_type_;
    LogicType logic_type_;
    Type program_;
    OptionsType options_;
    Type goals_;
    Type queries_;
};

}

#endif

