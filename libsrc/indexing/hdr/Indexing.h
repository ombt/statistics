//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_INDEXING_H
#define __OMBT_INDEXING_H

// index for logic problems.

// os headers
#include <string>
#include <list>
#include <iostream>

// headers
#include "atomic/BaseObject.h"
#include "atomic/ExtUseCntPtr.h"
#include "errors/Errors.h"
#include "logic/Program.h"

namespace ombt {

// logic program index
class Indexing: public BaseObject
{
public:
    // allocate 
    static Indexing *allocate(const Program &);

    // ctors and dtor
    Indexing();
    Indexing(const Program &program);
    Indexing(const Indexing &src);
    virtual ~Indexing();

    // assignment
    Indexing &operator=(const Program &program);
    Indexing &operator=(const Indexing &rhs);

    // misc
    inline void program(const Program &program)
        { program_ = program; }
    inline const Program &program() const
        { return program_; }
    inline bool operator!() const
        { return !error_; }
    inline const Errors &error() const
        { return error_; }

    // print data
    friend std::ostream &operator<<(std::ostream &, const Indexing &);

protected:
    // internal data
    Errors error_;
    Program program_;
};

}

#endif

