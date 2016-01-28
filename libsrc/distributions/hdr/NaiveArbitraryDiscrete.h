//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_NAIVE_ARBITRARY_DISCRETE_H
#define __OMBT_NAIVE_ARBITRARY_DISCRETE_H

// system headers
#include <stdio.h>
#include <math.h>
#include <vector>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"
#include "random/Random.h"

namespace ombt {

// arbitrary discrete distribution - naive implementation
class NaiveArbitraryDiscrete: public BaseObject
{
public:
    // ctors and dtor
    NaiveArbitraryDiscrete();
    NaiveArbitraryDiscrete(const std::vector<double> &mass, const Random &rng);
    NaiveArbitraryDiscrete(const NaiveArbitraryDiscrete &src);
    ~NaiveArbitraryDiscrete();

    // assignment
    NaiveArbitraryDiscrete &operator=(const NaiveArbitraryDiscrete &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    std::vector<double> dist_;
    Random rng_;
};

}

#endif
