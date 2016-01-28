//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// poisson distribution
//

// headers
#include "hdr/Poisson.h"

namespace ombt {

// ctors and dtor
Poisson::Poisson(): 
    BaseObject(false), lambda_(0), a_(0), rng_()
{
    setOk(false);
}

Poisson::Poisson(double lambda, const Random &rng): 
    BaseObject(false), 
    lambda_(lambda), a_(0), rng_(rng)
{
    a_ = exp(-lambda);
    setOk(true);
}

Poisson::Poisson(const Poisson &src): 
    BaseObject(src), 
    lambda_(src.lambda_), a_(src.a_), rng_(src.rng_)
{
    // nothing to do
}

Poisson::~Poisson()
{
    setOk(false);
}

// assignment
Poisson &
Poisson::operator=(const Poisson &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        lambda_ = rhs.lambda_;
        a_ = rhs.a_;
        rng_ = rhs.rng_;
    }
    return(*this);
}

// get value
double
Poisson::nextValue()
{
    double b = 1;
    for (unsigned long i=0; ; ++i)
    {
        double U = rng_.random0to1();
        b = b*U;
        if (b < a_) return(i);
    }
}

}
