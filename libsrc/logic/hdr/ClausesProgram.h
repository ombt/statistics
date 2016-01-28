//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_CLAUSES_PROGRAM
#define __OMBT_CLAUSES_PROGRAM

// list of semantic records representing a conjunctive normal form program

// os headers
#include <string>
#include <list>
#include <iostream>

// headers
#include "atomic/BaseObject.h"
#include "atomic/ExtUseCntPtr.h"
#include "logic/Semantic.h"
#include "logic/CNFProgram.h"
#include "logic/Clause.h"

namespace ombt {

// list semantic records for logic-base languages
class ClausesProgram: public CNFProgram
{
public:
    // data types
    typedef std::list<Clause> Type;
    typedef Type::iterator TypeIter;
    typedef Type::const_iterator TypeCIter;

public:
    // ctors and dtor
    ClausesProgram();
    ClausesProgram(const CNFProgram &src);
    ClausesProgram(const ClausesProgram &src);
    ~ClausesProgram();

    // assignment
    ClausesProgram &operator=(const CNFProgram &rhs);
    ClausesProgram &operator=(const ClausesProgram &rhs);

    // access data
    inline void insert(Clause &c)
        { clauses_.push_back(c); }

    // print data
    virtual void reconstruct(std::ostream &) const;
    friend std::ostream &operator<<(std::ostream &, const ClausesProgram &);

protected:
    // internal data
    Type clauses_;
};

}

#endif

