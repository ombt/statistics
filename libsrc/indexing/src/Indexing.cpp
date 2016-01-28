//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// index for logic problems

// system includes
#include <string>
#include <list>

// local includes
#include "system/Debug.h"
#include "errors/Errors.h"
#include "hdr/Indexing.h"

namespace ombt {

// allocate 
Indexing *
Indexing::allocate(const Program &program)
{
    return new Indexing(program);
}

// ctors and dtor
Indexing::Indexing():
    error_(),
    program_(),
    BaseObject()
{
}

Indexing::Indexing(const Program &program): 
    error_(),
    program_(program),
    BaseObject()
{
}

Indexing::Indexing(const Indexing &src): 
    error_(src.error_),
    program_(src.program_),
    BaseObject(src)
{
}

Indexing::~Indexing()
{
}

// assignment
Indexing &
Indexing::operator=(const Program &program)
{
    program_ = program;
    return(*this);
}

Indexing &
Indexing::operator=(const Indexing &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        error_ = rhs.error_;
        program_ = rhs.program_;
    }
    return(*this);
}

// output operators
std::ostream &
operator<<(std::ostream &os, const Indexing &p)
{
    return os;
}

}

