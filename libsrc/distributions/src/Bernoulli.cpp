//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// bernoulli distribution
//
// F(X) = if (X<=p) return 1, else return 0;
//

// headers
#include "hdr/Bernoulli.h"

namespace ombt {

// ctors and dtor
Bernoulli::Bernoulli(): 
    BaseObject(false), p_(0), rng_()
{
    setOk(false);
}

Bernoulli::Bernoulli(double p, const Random &rng): 
    BaseObject(false), p_(p), rng_(rng)
{
    setOk(true);
}

Bernoulli::Bernoulli(const Bernoulli &src): 
    BaseObject(src), p_(src.p_), rng_(src.rng_)
{
    // nothing to do
}

Bernoulli::~Bernoulli()
{
    setOk(false);
}

// assignment
Bernoulli &
Bernoulli::operator=(const Bernoulli &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        p_ = rhs.p_;
        rng_ = rhs.rng_;
    }
    return(*this);
}

// get value
double
Bernoulli::nextValue()
{
    double u = rng_.random0to1();
    return((u <= p_) ? 1 : 0);
}

}
