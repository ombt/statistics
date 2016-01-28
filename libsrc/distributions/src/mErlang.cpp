//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// m-erlang distribution
//
// X = (-beta/m)*log(u1*u2*...*um)

// headers
#include "hdr/mErlang.h"

namespace ombt {

// ctors and dtor
mErlang::mErlang(): 
    BaseObject(false), beta_(0), m_(0), rng_()
{
    setOk(false);
}

mErlang::mErlang(double beta, unsigned long m, const Random &rng): 
    BaseObject(false), beta_(beta), m_(m), rng_(rng)
{
    setOk(true);
}

mErlang::mErlang(const mErlang &src): 
    BaseObject(src), beta_(src.beta_), m_(src.m_), rng_(src.rng_)
{
    // nothing to do
}

mErlang::~mErlang()
{
    setOk(false);
}

// assignment
mErlang &
mErlang::operator=(const mErlang &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        beta_ = rhs.beta_;
        m_ = rhs.m_;
        rng_ = rhs.rng_;
    }
    return(*this);
}

// get value
double
mErlang::nextValue()
{
    double product = 1;
    for (int i=1; i<=m_; ++i)
    {
        product *= rng_.random0to1();
    }
    return(-beta_*log(product)/m_);
}

}
