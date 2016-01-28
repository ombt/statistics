//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// cubic spline class

// constructors and destructor
template <class DT>
CubicSpline<DT>::CubicSpline():
    npoints_(0), yp1_(0), ypn_(0), xs_(), ys_(), ypps_()
{
    // do nothing
}

template <class DT>
CubicSpline<DT>::CubicSpline(int npoints, 
    const DT x[], const DT y[], DT yp1, DT ypn):
    npoints_(npoints), yp1_(yp1), ypn_(ypn), xs_(), ys_(), ypps_()
{
    MustBeTrue(npoints_ > 0);
    xs_.resize(npoints_+1);
    ys_.resize(npoints_+1);
    ypps_.resize(npoints_+1);
    for (int i=0; i<npoints_; ++i)
    {
        xs_[i] = x[i];
        ys_[i] = y[i];
    }
    calculate();
}

template <class DT>
CubicSpline<DT>::CubicSpline(
    const Coordinates &xs, const Coordinates &ys, DT yp1, DT ypn): 
    npoints_(0), yp1_(yp1), ypn_(ypn), xs_(xs), ys_(ys), ypps_()
{
    MustBeTrue(xs_.size() == ys_.size());
    MustBeTrue((npoints_= xs_.size()) > 0);
    ypps_.resize(npoints_+1);
    calculate();
}

template <class DT>
CubicSpline<DT>::CubicSpline(const CubicSpline<DT> &cs): 
    npoints_(cs.npoints_), yp1_(cs.yp1_), ypn_(cs.ypn_),
    xs_(cs.xs_), ys_(cs.ys_), ypps_(cs.ypps_)
{
    // do nothing
}

template <class DT>
CubicSpline<DT>::~CubicSpline() 
{
    // do nothing
}

// assignments
template <class DT>
CubicSpline<DT> &
CubicSpline<DT>::operator=(const CubicSpline<DT> &cs)
{
    if (this != &cs)
    {
        npoints_ = cs.npoints_;
        yp1_ = cs.yp1_;
        ypn_ = cs.ypn_;
        xs_ = cs.xs_;
        ys_ = cs.ys_;
        ypps_ = cs.ypps_;
    }
    return(*this);
}

// calculates spline parameters
template <class DT>
int
CubicSpline<DT>::calculate()
{
    Values u(npoints_);
    if (yp1_ == DT(0))
    {
        ypps_[1] = DT(0);
        u[1] = DT(0);
    }
    else
    {
        ypps_[1] = DT(-0.5);
        u[1] = (DT(3.0)/(xs_[2]-xs_[1]))*((ys_[2]-ys_[1])/(xs_[2]-xs_[1])-yp1_);
    }

    for (int i=2; i<=(npoints_-1);++i)
    {
        DT sig = (xs_[i]-xs_[i-1])/(xs_[i+1]-xs_[i-1]);
        DT p = sig*ypps_[i-1] + DT(2.0);
        ypps_[i] = (sig-DT(1.0))/p;
        u[i] = (ys_[i+1]-ys_[i])/(xs_[i+1]-xs_[i])-(ys_[i]-ys_[i-1])/(xs_[i]-xs_[i-1]);
        u[i] = (DT(6.0)*u[i]/(xs_[i+1]-xs_[i-1])-sig*u[i-1])/p;
    }

    DT qn;
    DT un;
    if (ypn_ == DT(0))
    {
        qn = DT(0);
        un = DT(0);
    }
    else
    {
        qn = DT(0.5);
        un = (DT(3.0)/(xs_[npoints_]-xs_[npoints_-1]))*(ypn_-(ys_[npoints_]-ys_[npoints_-1])/(xs_[npoints_]-xs_[npoints_-1]));
    }

    ypps_[npoints_]=(un-qn*u[npoints_-1])/(qn*ypps_[npoints_-1]+DT(1.0));
    for (int k=npoints_-1; k>=1; --k)
    {
        ypps_[k] = ypps_[k]*ypps_[k+1]+u[k];
    }

    return(0);
}

template <class DT>
int
CubicSpline<DT>::calculate(int npoints, const DT x[], const DT y[],
                           DT yp1, DT ypn)
{
    MustBeTrue((npoints_ = npoints) > 0);
    xs_.resize(npoints_+1);
    ys_.resize(npoints_+1);
    ypps_.resize(npoints_+1);
    for (int i=0; i<npoints_; ++i)
    {
        xs_[i] = x[i];
        ys_[i] = y[i];
    }
    yp1_ = yp1;
    ypn_ = ypn;
    calculate();
    return(0);
}

template <class DT>
int
CubicSpline<DT>::calculate(const Coordinates &xs, const Coordinates &ys,
                           DT yp1, DT ypn)
{
    MustBeTrue(xs.size() == ys.size());
    MustBeTrue((npoints_= xs.size()) > 0);
    xs_ = xs;
    ys_ = ys;
    yp1_ = yp1;
    ypn_ = ypn;
    ypps_.resize(npoints_+1);
    calculate();
    return(0);
}


template <class DT>
DT
CubicSpline<DT>::interpolate(DT x) const
{
    if (x <= xs_[0])
        return(ys_[0]);
    else if (x >= xs_[npoints_-1])
        return(ys_[npoints_-1]);

    int klo = 1;
    int khi = npoints_;
    while ((khi-klo) > 1)
    {
        int k = (khi+klo) >> 1;
        if (xs_[k] > x)
            khi = k;
        else
            klo = k;
    }
    DT h = xs_[khi]-xs_[klo];
    MustBeTrue (h != DT(0));
    DT a = (xs_[khi]-x)/h;
    DT b = (x-xs_[klo])/h;
    return(a*ys_[klo]+b*ys_[khi]+((a*a*a-a)*ypps_[klo]+(b*b*b-b)*ypps_[khi])*(h*h)/DT(6.0));
}

template <class DT>
DT
CubicSpline<DT>::operator()(DT x) const
{
    return(interpolate(x));
}

template <class DT>
void
CubicSpline<DT>::interpolate(DT x, DT &y) const
{
    y = interpolate(x);
}

template <class DT>
void
CubicSpline<DT>::interpolate(int npoints, const DT x[], DT y[]) const
{
    for (int i=0; i<npoints; ++i)
    {
        y[i] = interpolate(x[i]);
    }
}

template <class DT>
std::ostream &
operator<<(std::ostream &os, const CubicSpline<DT> &c)
{
    os << "Cubic Spline Second Derivatives:" << std::endl;
    for (int i=0; i<c.ypps_.size(); ++i)
    {
        os << "ypps_[" << i << "] = " << c.ypps_[i] << ")" << std::endl;
    }
    return(os);
}
