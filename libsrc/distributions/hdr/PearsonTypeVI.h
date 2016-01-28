//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_PEARSON_TYPE_VI_H
#define __OMBT_PEARSON_TYPE_VI_H

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

// pearson type VI distribution
class PearsonTypeVI: public BaseObject
{
public:
    // ctors and dtor
    PearsonTypeVI();
    PearsonTypeVI(double alpha1, double alpha2, double beta, const Random &rng);
    PearsonTypeVI(const PearsonTypeVI &src);
    ~PearsonTypeVI();

    // assignment
    PearsonTypeVI &operator=(const PearsonTypeVI &rhs);

    // get a random value
    double nextValue();

protected:
    // data
    double alpha1_;
    double alpha2_;
    double beta_;
    ExtUseCntPtr<Gamma> pgrng1_;
    ExtUseCntPtr<Gamma> pgrng2_;
};

}

#endif
