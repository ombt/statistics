//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_QPLACE_ARBITRARY_DISCRETE_H
#define __OMBT_QPLACE_ARBITRARY_DISCRETE_H

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

// arbitrary discrete distribution - single vector implementation
class QPlaceArbitraryDiscrete: public BaseObject
{
public:
    // ctors and dtor
    QPlaceArbitraryDiscrete();
    QPlaceArbitraryDiscrete(unsigned int qdigits, const std::vector<double> &mass, const Random &rng);
    QPlaceArbitraryDiscrete(const QPlaceArbitraryDiscrete &src);
    ~QPlaceArbitraryDiscrete();

    // assignment
    QPlaceArbitraryDiscrete &operator=(const QPlaceArbitraryDiscrete &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    unsigned int qdigits_;
    unsigned int scale_;
    std::vector<double> dist_;
    Random rng_;
};

}

#endif
