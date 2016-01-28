//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_CNF_PROGRAM
#define __OMBT_CNF_PROGRAM

// list of semantic records representing a conjunctive normal form program

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
class CNFProgram: public Program
{
public:
    // ctors and dtor
    CNFProgram();
    CNFProgram(const CNFProgram &src);
    CNFProgram(const Program &src);
    ~CNFProgram();

    // assignment
    CNFProgram &operator=(const Program &rhs);
    CNFProgram &operator=(const CNFProgram &rhs);

    // print data
    friend std::ostream &operator<<(std::ostream &, const CNFProgram &);
};

}

#endif

