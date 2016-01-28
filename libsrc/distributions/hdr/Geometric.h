//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_GEOMETRIC_H
#define __OMBT_GEOMETRIC_H

// system headers
#include <math.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "random/Random.h"

namespace ombt {

// geometric distribution
class Geometric: public BaseObject
{
public:
    // ctors and dtor
    Geometric();
    Geometric(double p, const Random &rng);
    Geometric(const Geometric &src);
    ~Geometric();

    // assignment
    Geometric &operator=(const Geometric &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    double p_;
    Random rng_;
};

}

#endif
