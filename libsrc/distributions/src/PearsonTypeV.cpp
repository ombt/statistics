//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// pearson type V distribution
//
// 1) y ~ gamma(alpha, 1/beta)
// 2) return x = 1/y.

// headers
#include "hdr/PearsonTypeV.h"

namespace ombt {

// ctors and dtor
PearsonTypeV::PearsonTypeV(): 
    BaseObject(false),
    alpha_(0), beta_(0), pgrng_(NULL)
{
    setOk(false);
}

PearsonTypeV::PearsonTypeV(double alpha, double beta, const Random &rng): 
    BaseObject(false), 
    alpha_(alpha), beta_(beta), pgrng_(NULL)
{
    pgrng_ = new Gamma(alpha_, 1/beta_, rng);
    setOk(true);
}

PearsonTypeV::PearsonTypeV(const PearsonTypeV &src): 
    BaseObject(src), 
    alpha_(src.alpha_), beta_(src.beta_), pgrng_(src.pgrng_)
{
    // nothing to do
}

PearsonTypeV::~PearsonTypeV()
{
    setOk(false);
}

// assignment
PearsonTypeV &
PearsonTypeV::operator=(const PearsonTypeV &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        alpha_ = rhs.alpha_;
        beta_ = rhs.beta_;
        pgrng_ = rhs.pgrng_;
    }
    return(*this);
}

// get value
double
PearsonTypeV::nextValue()
{
    double y = pgrng_->nextValue();
    return(1.0/y);
}

}
