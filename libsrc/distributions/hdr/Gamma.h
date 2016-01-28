//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_GAMMA_H
#define __OMBT_GAMMA_H

// system headers
#include <stdio.h>
#include <math.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "random/Random.h"

namespace ombt {

// gamma distribution
class Gamma: public BaseObject
{
public:
    // ctors and dtor
    Gamma();
    Gamma(double beta, double alpha, const Random &rng);
    Gamma(const Gamma &src);
    ~Gamma();

    // assignment
    Gamma &operator=(const Gamma &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    double alpha_;
    double beta_;
    Random rng_;
};

}

#endif
