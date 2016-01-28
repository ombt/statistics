//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_DNF_PROGRAM
#define __OMBT_DNF_PROGRAM

// list of semantic records representing a disjunctive normal form program

// os headers
#include <string>
#include <list>
#include <iostream>

// headers
#include "atomic/BaseObject.h"
#include "atomic/ExtUseCntPtr.h"
#include "logic/Semantic.h"
#include "logic/Program.h"

namespace ombt {

// list semantic records for logic-base languages
class DNFProgram: public Program
{
public:
    // ctors and dtor
    DNFProgram();
    DNFProgram(const DNFProgram &src);
    DNFProgram(const Program &src);
    ~DNFProgram();

    // assignment
    DNFProgram &operator=(const Program &rhs);
    DNFProgram &operator=(const DNFProgram &rhs);

    // print data
    friend std::ostream &operator<<(std::ostream &, const DNFProgram &);
};

}

#endif

