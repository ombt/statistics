//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_WEIBULL_H
#define __OMBT_WEIBULL_H

// system headers
#include <stdio.h>
#include <math.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "random/Random.h"

namespace ombt {

// weibull distribution
class Weibull: public BaseObject
{
public:
    // ctors and dtor
    Weibull();
    Weibull(double alpha, double beta, const Random &rng);
    Weibull(const Weibull &src);
    ~Weibull();

    // assignment
    Weibull &operator=(const Weibull &rhs);

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
