//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_SYMBOL
#define __OMBT_SYMBOL

// symbol table for logic-based languages

// os headers
#include <string>
#include <list>
#include <map>
#include <iostream>

// headers
#include "system/Debug.h"
#include "atomic/BaseObject.h"

namespace ombt {

// symbol table logic-base languages
class Symbol: public BaseObject
{
public:
    // types of symbol
    enum Type
    {
        PredicateFunction,
        PredicateConstant,
        Function,
        Variable,
        Constant,
        UniversalVariable,
        ExistentialVariable,
        Unknown,
        Last
    };

    // data types
    typedef std::map<std::string, Symbol> TableType;
    typedef TableType::iterator TableTypeIter;
    typedef TableType::const_iterator TableTypeCIter;

    typedef std::list<Symbol> StackType;
    typedef StackType::iterator StackTypeIter;
    typedef StackType::const_iterator StackTypeCIter;

public:
    // constructors and destructor
    Symbol();
    Symbol(const Symbol &);
    Symbol(const std::string &);
    Symbol(const std::string &, Type, int = 0);
    Symbol(const std::string &, const std::string &);
    Symbol(const std::string &, const std::string &, Type, int = 0);
    ~Symbol();

    // assignment operator
    inline Symbol &operator=(const Symbol &rhs)
    {
        if (this != &rhs)
        {
            name_ = rhs.name_;
            unique_name_ = rhs.unique_name_;
            type_ = rhs.type_;
            number_of_arguments_ = rhs.number_of_arguments_;
            used_ = rhs.used_;
        }
        return *this;
    }

    // comparison operators
    inline bool operator==(const Symbol &rhs) const
        { return name_ == rhs.name_; }
    inline bool operator!=(const Symbol &rhs) const
        { return name_ != rhs.name_; }
    inline bool operator<(const Symbol &rhs) const
        { return name_ < rhs.name_; }
    inline bool operator<=(const Symbol &rhs) const
        { return name_ <= rhs.name_; }
    inline bool operator>(const Symbol &rhs) const
        { return name_ > rhs.name_; }
    inline bool operator>=(const Symbol &rhs) const
        { return name_ >= rhs.name_; }

    // increment operators
    inline Symbol &operator++()
        { ++used_; return *this; }
    inline Symbol &operator++(int)
        { ++used_; return *this; }

    // access operators
    inline Type type() const
        { return(type_); }
    inline void type(Type t)
        { type_ = t; }
    inline const std::string &name() const
        { return(name_); };
    inline const std::string &unique_name() const
        { return(unique_name_); };
    inline int number_of_arguments() const
        { return(number_of_arguments_); };
    inline int used() const
        { return(used_); };
    inline bool is_used() const
        { return (used_ > 0); }
    inline bool is_not_used() const
        { return (used_ <= 0); }

    // print data
    friend std::ostream &operator<<(std::ostream &, const Symbol &);
    static const std::string type_name(Type);

    // stack and table operations
    static bool retrieve(Symbol &);
    static bool update(const Symbol &);
    static bool insert(const Symbol &);
    static bool remove(const Symbol &);
    static void clear_table();

    // stack and table operations
    static bool push(const Symbol &);
    static bool top(Symbol &);
    static bool pop();
    static bool find(Symbol &);
    static void clear_scope();
    static bool increment(const Symbol &);

protected:
    // internal data
    std::string name_;
    std::string unique_name_;
    Type type_;
    int number_of_arguments_;
    int used_;

    // symbol table and stack for checking scope.
    static StackType scope_;
    static TableType table_;
};

}

#endif

