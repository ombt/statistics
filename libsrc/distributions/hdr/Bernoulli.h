//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_BERNOULLI_H
#define __OMBT_BERNOULLI_H

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "random/Random.h"

namespace ombt {

// uniform distribution
class Bernoulli: public BaseObject
{
public:
    // ctors and dtor
    Bernoulli();
    Bernoulli(double p, const Random &rng);
    Bernoulli(const Bernoulli &src);
    ~Bernoulli();

    // assignment
    Bernoulli &operator=(const Bernoulli &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    double p_;
    Random rng_;
};

}

#endif
