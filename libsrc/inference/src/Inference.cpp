//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// inference engine for logic problems

// system includes
#include <string>
#include <list>

// local includes
#include "system/Debug.h"
#include "errors/Errors.h"
#include "hdr/Inference.h"

namespace ombt {

// allocate 
Inference *
Inference::allocate(const Program &program)
{
    return new Inference(program);
}

// ctors and dtor
Inference::Inference():
    error_(),
    program_(),
    BaseObject()
{
}

Inference::Inference(const Program &program): 
    error_(),
    program_(program),
    BaseObject()
{
}

Inference::Inference(const Inference &src): 
    error_(src.error_),
    program_(src.program_),
    BaseObject(src)
{
}

Inference::~Inference()
{
}

// assignment
Inference &
Inference::operator=(const Program &program)
{
    program_ = program;
    return(*this);
}

Inference &
Inference::operator=(const Inference &rhs)
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
operator<<(std::ostream &os, const Inference &p)
{
    return os;
}

}

