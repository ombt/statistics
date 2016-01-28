//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// exponential distribution
//
// F(x) = 1-exp(x/beta)
// X = -beta*log(1-U), U in (0,1).
// or, 
// X = -beta*log(U), U in (0,1).

// headers
#include "hdr/Exponential.h"

namespace ombt {

// ctors and dtor
Exponential::Exponential(): 
    BaseObject(false), beta_(0), rng_()
{
    setOk(false);
}

Exponential::Exponential(double beta, const Random &rng): 
    BaseObject(false), beta_(beta), rng_(rng)
{
    setOk(true);
}

Exponential::Exponential(const Exponential &src): 
    BaseObject(src), beta_(src.beta_), rng_(src.rng_)
{
    // nothing to do
}

Exponential::~Exponential()
{
    setOk(false);
}

// assignment
Exponential &
Exponential::operator=(const Exponential &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        beta_ = rhs.beta_;
        rng_ = rhs.rng_;
    }
    return(*this);
}

// get value
double
Exponential::nextValue()
{
    double u = rng_.random0to1();
    return(-beta_*log(u));
}

}
