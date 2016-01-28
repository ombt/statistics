//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_MERLANG_H
#define __OMBT_MERLANG_H

// system headers
#include <stdio.h>
#include <math.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "random/Random.h"

namespace ombt {

// exponential distribution
class mErlang: public BaseObject
{
public:
    // ctors and dtor
    mErlang();
    mErlang(double beta, unsigned long m, const Random &rng);
    mErlang(const mErlang &src);
    ~mErlang();

    // assignment
    mErlang &operator=(const mErlang &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    double beta_;
    unsigned long m_;
    Random rng_;
};

}

#endif
