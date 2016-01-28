//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// uniform distribution
//
// F(x) = (x-a)/(b-a)
// X = a+U*(b-a), U in (0,1).

// headers
#include "hdr/Uniform.h"

namespace ombt {

// ctors and dtor
Uniform::Uniform(): 
    BaseObject(false), a_(0), b_(0), rng_()
{
    setOk(false);
}

Uniform::Uniform(double a, double b, const Random &rng): 
    BaseObject(false), a_(a), b_(b), rng_(rng)
{
    setOk(true);
}

Uniform::Uniform(const Uniform &src): 
    BaseObject(src), a_(src.a_), b_(src.b_), rng_(src.rng_)
{
    // nothing to do
}

Uniform::~Uniform()
{
    setOk(false);
}

// assignment
Uniform &
Uniform::operator=(const Uniform &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        a_ = rhs.a_;
        b_ = rhs.b_;
        rng_ = rhs.rng_;
    }
    return(*this);
}

// get value
double
Uniform::nextValue()
{
    double u = rng_.random0to1();
    return(a_+u*(b_-a_));
}

}
