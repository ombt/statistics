//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// binomial distribution
//

// headers
#include "hdr/Binomial.h"

namespace ombt {

// ctors and dtor
Binomial::Binomial(): 
    BaseObject(false), p_(0), n_(0), rng_()
{
    setOk(false);
}

Binomial::Binomial(double p, unsigned long n, const Random &rng): 
    BaseObject(false), p_(p), n_(n), rng_(rng)
{
    setOk(true);
}

Binomial::Binomial(const Binomial &src): 
    BaseObject(src), p_(src.p_), n_(src.n_), rng_(src.rng_)
{
    // nothing to do
}

Binomial::~Binomial()
{
    setOk(false);
}

// assignment
Binomial &
Binomial::operator=(const Binomial &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        p_ = rhs.p_;
        n_ = rhs.n_;
        rng_ = rhs.rng_;
    }
    return(*this);
}

// get value
double
Binomial::nextValue()
{
    double X = 0;
    for (int i=1; i<=n_; ++i)
    {
        double u = rng_.random0to1();
        if (u<=p_) X += 1;
    }
    return(X);
}

}
