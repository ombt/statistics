//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_BINOMIAL_H
#define __OMBT_BINOMIAL_H

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "random/Random.h"

namespace ombt {

// binomial distribution
class Binomial: public BaseObject
{
public:
    // ctors and dtor
    Binomial();
    Binomial(double p, unsigned long n, const Random &rng);
    Binomial(const Binomial &src);
    ~Binomial();

    // assignment
    Binomial &operator=(const Binomial &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    double p_;
    unsigned long n_;
    Random rng_;
};

}

#endif
