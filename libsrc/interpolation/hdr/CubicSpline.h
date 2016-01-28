//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __CUBIC_SPLINE_H
#define __CUBIC_SPLINE_H

// cubic spline interpolation

// headers
#include <vector>
#include <limits>
#include "system/Returns.h"
#include "system/Debug.h"

namespace ombt {

// forward declarations
template <class DT> class CubicSpline;
template <class DT> std::ostream &operator<<(std::ostream &, const CubicSpline<DT> &);

// cubic spline class
template <class DT>
class CubicSpline {
public:
    // types
    typedef std::vector<DT> Coordinates;

    // ctors and dtor
    CubicSpline();
    CubicSpline(int npoints, const DT x[], const DT y[], 
                DT yp1 = DT(0), DT ypn = DT(0));
    CubicSpline(const Coordinates &xs, const Coordinates &ys, 
                DT yp1 = DT(0), DT ypn = DT(0));
    CubicSpline(const CubicSpline &cs);
    ~CubicSpline();

    // assignment
    CubicSpline &operator=(const CubicSpline &cs);

    // calulate spline parameters
    int calculate(int npoints, const DT x[], const DT y[],
                  DT yp1 = DT(0), DT ypn = DT(0));
    int calculate(const Coordinates &xs, const Coordinates &ys,
                  DT yp1 = DT(0), DT ypn = DT(0));

    // interpolate for given value(s) of x
    DT operator()(DT x) const;
    DT interpolate(DT x) const;
    void interpolate(DT x, DT &y) const;
    void interpolate(int npoints, const DT xs[], DT ys[]) const;
    void interpolate(const Coordinates &xs, Coordinates &ys) const;

    // output
    friend std::ostream &operator<<<>(std::ostream &, const CubicSpline<DT> &);
 
private:
    // does actual calculations
    int calculate();

    // data
    typedef std::vector<DT> Values;
    typedef std::vector<DT> SecondDerivatives;

    int npoints_;
    DT yp1_;
    DT ypn_;
    Coordinates xs_;
    Coordinates ys_;
    SecondDerivatives ypps_;
};

#include "interpolation/CubicSpline.i"

}
#endif
