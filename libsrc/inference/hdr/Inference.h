//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_INFERENCE_H
#define __OMBT_INFERENCE_H

// inference engine for logic problems.

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

// logic program inference engine
class Inference: public BaseObject
{
public:
    // allocate 
    static Inference *allocate(const Program &);

    // ctors and dtor
    Inference();
    Inference(const Program &program);
    Inference(const Inference &src);
    virtual ~Inference();

    // assignment
    Inference &operator=(const Program &program);
    Inference &operator=(const Inference &rhs);

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
    friend std::ostream &operator<<(std::ostream &, const Inference &);

protected:
    // internal data
    Errors error_;
    Program program_;
};

}

#endif

