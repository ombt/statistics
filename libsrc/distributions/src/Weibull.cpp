//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// weibull distribution
//
// F(x) = alpha*(beta**-alpha)*(x**(n-1))*exp(-(x/beta)**alpha)
// u ~ U(0,1)
// x = beta*(-log(1-u))**(1/alpha)

// headers
#include "hdr/Weibull.h"

namespace ombt {

// ctors and dtor
Weibull::Weibull(): 
    BaseObject(false), alpha_(0), beta_(0), rng_()
{
    setOk(false);
}

Weibull::Weibull(double alpha, double beta, const Random &rng): 
    BaseObject(false), 
    alpha_(alpha), beta_(beta), rng_(rng)
{
    setOk(true);
}

Weibull::Weibull(const Weibull &src): 
    BaseObject(src), 
    alpha_(src.alpha_), beta_(src.beta_), rng_(src.rng_)
{
    // nothing to do
}

Weibull::~Weibull()
{
    setOk(false);
}

// assignment
Weibull &
Weibull::operator=(const Weibull &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        alpha_ = rhs.alpha_;
        beta_ = rhs.beta_;
        rng_ = rhs.rng_;
    }
    return(*this);
}

// get value
double
Weibull::nextValue()
{
    double u = rng_.random0to1();
    return(beta_*pow(-log(u), 1/alpha_));
}

}
