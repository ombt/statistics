//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// lognormal distribution
//
// 1) calculate mu, sigma2 from the lognormal
// muln, sigmaln2 as follows:
// mu = log(muln**2/sqrt(sigmaln2+muln**2))
// sigma2 = log((sigmaln2+muln**2)/muln**2)
// 2) generate y ~ N(mu, sigma2).
// 3) return x = exp(y).

// headers
#include "hdr/LogNormal.h"

namespace ombt {

// ctors and dtor
LogNormal::LogNormal(): 
    BaseObject(false),
    muln_(0), sigmaln2_(0), pgrng_(NULL)
{
    setOk(false);
}

LogNormal::LogNormal(double muln, double sigmaln2, const Random &rng): 
    BaseObject(false), 
    muln_(muln), sigmaln2_(sigmaln2)
{
    pgrng_ = new Gaussian(log(muln*muln/sqrt(sigmaln2+muln*muln)),
                          log((sigmaln2+muln*muln)/(muln*muln)), 
                          rng);
    setOk(true);
}

LogNormal::LogNormal(const LogNormal &src): 
    BaseObject(src), 
    muln_(src.muln_), sigmaln2_(src.sigmaln2_), pgrng_(src.pgrng_)
{
    // nothing to do
}

LogNormal::~LogNormal()
{
    setOk(false);
}

// assignment
LogNormal &
LogNormal::operator=(const LogNormal &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        muln_ = rhs.muln_;
        sigmaln2_ = rhs.sigmaln2_;
        pgrng_ = rhs.pgrng_;
    }
    return(*this);
}

// get value
double
LogNormal::nextValue()
{
    double y = pgrng_->nextValue();
    return(exp(y));
}

}
