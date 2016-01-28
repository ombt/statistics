//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_EXPONENTIAL_H
#define __OMBT_EXPONENTIAL_H

// system headers
#include <stdio.h>
#include <math.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "random/Random.h"

namespace ombt {

// exponential distribution
class Exponential: public BaseObject
{
public:
    // ctors and dtor
    Exponential();
    Exponential(double beta, const Random &rng);
    Exponential(const Exponential &src);
    ~Exponential();

    // assignment
    Exponential &operator=(const Exponential &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    double beta_;
    Random rng_;
};

}

#endif
