//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// discrete uniform distribution
//
// P(x) = i/(j-i+1) for x in {i,i+1,...,j}
// X = i+floor((j-i+1)*U), U in (0,1).

// headers
#include "hdr/DiscreteUniform.h"

namespace ombt {

// ctors and dtor
DiscreteUniform::DiscreteUniform(): 
    BaseObject(false), start_(0), end_(0), rng_()
{
    setOk(false);
}

DiscreteUniform::DiscreteUniform(
    double start, double end, const Random &rng): 
    BaseObject(false), start_(start), end_(end), rng_(rng)
{
    setOk(true);
}

DiscreteUniform::DiscreteUniform(const DiscreteUniform &src): 
    BaseObject(src), 
    start_(src.start_), end_(src.end_), rng_(src.rng_)
{
    // nothing to do
}

DiscreteUniform::~DiscreteUniform()
{
    setOk(false);
}

// assignment
DiscreteUniform &
DiscreteUniform::operator=(const DiscreteUniform &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        start_ = rhs.start_;
        end_ = rhs.end_;
        rng_ = rhs.rng_;
    }
    return(*this);
}

// get value
double
DiscreteUniform::nextValue()
{
    double u = rng_.random0to1();
    return(start_+floor((end_-start_+1)*u));
}

}
