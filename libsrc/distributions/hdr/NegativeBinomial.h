//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_NEGATIVE_BINOMIAL_H
#define __OMBT_NEGATIVE_BINOMIAL_H

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "random/Random.h"
#include "distributions/Geometric.h"

namespace ombt {

// negative binomial distribution
class NegativeBinomial: public BaseObject
{
public:
    // ctors and dtor
    NegativeBinomial();
    NegativeBinomial(double p, unsigned long n, const Random &rng);
    NegativeBinomial(const NegativeBinomial &src);
    ~NegativeBinomial();

    // assignment
    NegativeBinomial &operator=(const NegativeBinomial &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    unsigned long n_;
    Geometric grng_;
};

}

#endif
