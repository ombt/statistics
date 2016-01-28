//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// arbitrary discrete distribution
//
// p(0), p(1), p(2), ..., p(n)
// choose U in (0,1), then return X=I where:
// sum from j=0 to j=I-1 of p(j) <= U < sum from j=0 to I of p(j)

// headers
#include "hdr/NaiveArbitraryDiscrete.h"

namespace ombt {

// ctors and dtor
NaiveArbitraryDiscrete::NaiveArbitraryDiscrete(): 
    BaseObject(false), dist_(), rng_()
{
    setOk(false);
}

NaiveArbitraryDiscrete::NaiveArbitraryDiscrete(
    const std::vector<double> &mass, const Random &rng): 
        BaseObject(false), dist_(), rng_(rng)
{
    long distsize = mass.size() + 1;
    dist_.resize(distsize);
    dist_[0] = 0.0;
    for (int i=1; i<distsize; ++i)
    {
        dist_[i] = dist_[i-1] + mass[i-1];
    }
    setOk(true);
}

NaiveArbitraryDiscrete::NaiveArbitraryDiscrete(
    const NaiveArbitraryDiscrete &src): 
        BaseObject(src), dist_(src.dist_), rng_(src.rng_)
{
    // nothing to do
}

NaiveArbitraryDiscrete::~NaiveArbitraryDiscrete()
{
    setOk(false);
}

// assignment
NaiveArbitraryDiscrete &
NaiveArbitraryDiscrete::operator=(const NaiveArbitraryDiscrete &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        dist_ = rhs.dist_;
        rng_ = rhs.rng_;
    }
    return(*this);
}

// get value
double
NaiveArbitraryDiscrete::nextValue()
{
    // very naive way 
    double U = rng_.random0to1();
    long I;
    for (I=dist_.size()-1; I>=0 && U<=dist_[I]; --I) ;
    return(I+1);
}

}
