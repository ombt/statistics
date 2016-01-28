//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// negative binomial distribution
//

// headers
#include "hdr/NegativeBinomial.h"

namespace ombt {

// ctors and dtor
NegativeBinomial::NegativeBinomial(): 
    BaseObject(false), n_(0), grng_()
{
    setOk(false);
}

NegativeBinomial::NegativeBinomial(double p, unsigned long n, const Random &rng): 
    BaseObject(false), n_(n), grng_(p, rng)
{
    setOk(true);
}

NegativeBinomial::NegativeBinomial(const NegativeBinomial &src): 
    BaseObject(src), n_(src.n_), grng_(src.grng_)
{
    // nothing to do
}

NegativeBinomial::~NegativeBinomial()
{
    setOk(false);
}

// assignment
NegativeBinomial &
NegativeBinomial::operator=(const NegativeBinomial &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        n_ = rhs.n_;
        grng_ = rhs.grng_;
    }
    return(*this);
}

// get value
double
NegativeBinomial::nextValue()
{
    double X = 0;
    for (int i=1; i<=n_; ++i)
    {
        X += grng_.nextValue();
    }
    return(X);
}

}
