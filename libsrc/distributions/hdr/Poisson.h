//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_POISSON_H
#define __OMBT_POISSON_H

// system headers
#include <math.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "random/Random.h"

namespace ombt {

// poisson distribution
class Poisson: public BaseObject
{
public:
    // ctors and dtor
    Poisson();
    Poisson(double lambda, const Random &rng);
    Poisson(const Poisson &src);
    ~Poisson();

    // assignment
    Poisson &operator=(const Poisson &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    double lambda_;
    double a_;
    Random rng_;
};

}

#endif
