//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_UNIFORM_H
#define __OMBT_UNIFORM_H

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "random/Random.h"

namespace ombt {

// uniform distribution
class Uniform: public BaseObject
{
public:
    // ctors and dtor
    Uniform();
    Uniform(double a, double b, const Random &rng);
    Uniform(const Uniform &src);
    ~Uniform();

    // assignment
    Uniform &operator=(const Uniform &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    double a_;
    double b_;
    Random rng_;
};

}

#endif
