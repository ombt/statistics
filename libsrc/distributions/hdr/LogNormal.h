//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_LOGNORMAL_H
#define __OMBT_LOGNORMAL_H

// system headers
#include <stdio.h>
#include <string.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "atomic/ExtUseCntPtr.h"
#include "distributions/Gaussian.h"

namespace ombt {

// lognormal distribution
class LogNormal: public BaseObject
{
public:
    // ctors and dtor
    LogNormal();
    LogNormal(double muln, double sigmaln2, const Random &rng);
    LogNormal(const LogNormal &src);
    ~LogNormal();

    // assignment
    LogNormal &operator=(const LogNormal &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    double muln_;
    double sigmaln2_;
    ExtUseCntPtr<Gaussian> pgrng_;
};

}

#endif
