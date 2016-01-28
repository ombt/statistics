//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_BETA_H
#define __OMBT_BETA_H

// system headers
#include <stdio.h>
#include <string.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "atomic/ExtUseCntPtr.h"
#include "distributions/Gamma.h"

namespace ombt {

// beta distribution
class Beta: public BaseObject
{
public:
    // ctors and dtor
    Beta();
    Beta(double alpha1, double alpha2, const Random &rng);
    Beta(const Beta &src);
    ~Beta();

    // assignment
    Beta &operator=(const Beta &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    double alpha1_;
    double alpha2_;
    ExtUseCntPtr<Gamma> pgrng1_;
    ExtUseCntPtr<Gamma> pgrng2_;
};

}

#endif
