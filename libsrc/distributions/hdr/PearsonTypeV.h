//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_PEARSON_TYPE_V_H
#define __OMBT_PEARSON_TYPE_V_H

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

// pearson type V distribution
class PearsonTypeV: public BaseObject
{
public:
    // ctors and dtor
    PearsonTypeV();
    PearsonTypeV(double alpha, double beta, const Random &rng);
    PearsonTypeV(const PearsonTypeV &src);
    ~PearsonTypeV();

    // assignment
    PearsonTypeV &operator=(const PearsonTypeV &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    double alpha_;
    double beta_;
    ExtUseCntPtr<Gamma> pgrng_;
};

}

#endif
