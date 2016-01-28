//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_GAUSSIAN_H
#define __OMBT_GAUSSIAN_H

// system headers
#include <stdio.h>
#include <math.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "random/Random.h"

namespace ombt {

// gaussian distribution
class Gaussian: public BaseObject
{
public:
    // ctors and dtor
    Gaussian();
    Gaussian(double mu, double sigma2, const Random &rng);
    Gaussian(const Gaussian &src);
    ~Gaussian();

    // assignment
    Gaussian &operator=(const Gaussian &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    int xindex_;
    double mu_;
    double sigma_;
    double sigma2_;
    Random rng_;
};

}

#endif
