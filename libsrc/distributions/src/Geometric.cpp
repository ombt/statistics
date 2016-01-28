//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// geometric distribution
//
// U in U(0,1)
// X = ceil(log(U)/log(1-p))
//

// headers
#include "hdr/Geometric.h"

namespace ombt {

// ctors and dtor
Geometric::Geometric(): 
    BaseObject(false), p_(0), rng_()
{
    setOk(false);
}

Geometric::Geometric(double p, const Random &rng): 
    BaseObject(false), p_(p), rng_(rng)
{
    setOk(true);
}

Geometric::Geometric(const Geometric &src): 
    BaseObject(src), p_(src.p_), rng_(src.rng_)
{
    // nothing to do
}

Geometric::~Geometric()
{
    setOk(false);
}

// assignment
Geometric &
Geometric::operator=(const Geometric &rhs)
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
Geometric::nextValue()
{
    double U = rng_.random0to1();
    double X = ceil(log(U)/log(1-p_));
    return(X);
}

}
