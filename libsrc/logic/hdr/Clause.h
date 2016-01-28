//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_CLAUSE
#define __OMBT_CLAUSE

// list of semantic records representing a clause

// os headers
#include <string>
#include <list>
#include <iostream>

// headers
#include "atomic/BaseObject.h"
#include "atomic/ExtUseCntPtr.h"
#include "logic/Semantic.h"

namespace ombt {

// list semantic records for logic-base languages
class Clause: public BaseObject
{
public:
    // data types
    typedef std::list<ExtUseCntPtr<Semantic> > Type;
    typedef Type::iterator TypeIter;
    typedef Type::const_iterator TypeCIter;

public:
    // ctors and dtor
    Clause();
    Clause(std::list<ExtUseCntPtr<Semantic> > &);
    Clause(const Clause &src);
    virtual ~Clause();

    // assignment
    Clause &operator=(const Clause &rhs);

    // access functions
    inline Type &clause()
        { return clause_; }
    inline const Type &clause() const
        { return clause_; }
    inline void insert(ExtUseCntPtr<Semantic> &expr)
        { clause_.push_back(expr); }
    inline int size() const
        { return clause_.size(); }

    // print data
    virtual void reconstruct(std::ostream &) const;
    friend std::ostream &operator<<(std::ostream &, const Clause &);

protected:
    // internal data
    Type clause_;
};

}

#endif

