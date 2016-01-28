//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_DISCRETE_UNIFORM_H
#define __OMBT_DISCRETE_UNIFORM_H

// system headers
#include <stdio.h>
#include <math.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "random/Random.h"

namespace ombt {

// discrete uniform distribution
class DiscreteUniform: public BaseObject
{
public:
    // ctors and dtor
    DiscreteUniform();
    DiscreteUniform(double start, double end, const Random &rng);
    DiscreteUniform(const DiscreteUniform &src);
    ~DiscreteUniform();

    // assignment
    DiscreteUniform &operator=(const DiscreteUniform &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    double start_;
    double end_;
    Random rng_;
};

}

#endif
