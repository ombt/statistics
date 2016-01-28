//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// complex class functions

// local definitions
#define plusi Complex<T>(0.0, 1.0)
#define minusi Complex<T>(0.0, -1.0)
#define plusone Complex<T>(1.0, 0.0)
#define minusone Complex<T>(-1.0, 0.0)

// constructors and destructor
template <class T>
Complex<T>::Complex():
    x(0), y(0)
{
    // do nothing
}

template <class T>
Complex<T>::Complex(T srcx):
    x(srcx), y(0)
{
    // do nothing
}

template <class T>
Complex<T>::Complex(T srcx, T srcy):
    x(srcx), y(srcy)
{
    // do nothing
}

template <class T>
Complex<T>::Complex(const Complex<T> &src): 
    x(src.x), y(src.y)
{
    // do nothing
}

template <class T>
Complex<T>::~Complex() 
{
    // do nothing
}

// arithmetic operators
template <class T>
Complex<T> &
Complex<T>::operator=(const Complex<T> &c)
{
    x = c.x;
    y = c.y;
    return(*this);
}

template <class T>
Complex<T> &
Complex<T>::operator+=(const Complex<T> &c)
{
    x += c.x;
    y += c.y;
    return(*this);
}

template <class T>
Complex<T> &
Complex<T>::operator-=(const Complex<T> &c)
{
    x -= c.x;
    y -= c.y;
    return(*this);
}

template <class T>
Complex<T> &
Complex<T>::operator*=(const Complex<T> &c)
{
    T tmpx = x*c.x - y*c.y;
    T tmpy = x*c.y + y*c.x;
    x = tmpx;
    y = tmpy;
    return(*this);
}

template <class T>
Complex<T> &
Complex<T>::operator/=(const Complex<T> &c)
{
    MustBeTrue(c.x != 0.0 || c.y != 0.0);
    T tmpm = c.x*c.x + c.y*c.y;
    T tmpx = (x*c.x + y*c.y)/tmpm;
    T tmpy = (y*c.x - x*c.y)/tmpm;
    x = tmpx;
    y = tmpy;
    return(*this);
}

template <class T>
Complex<T>
Complex<T>::operator+(const Complex<T> &c) const
{
    return(Complex<T>(*this) += c);
}

template <class T>
Complex<T>
Complex<T>::operator-(const Complex<T> &c) const
{
    return(Complex<T>(*this) -= c);
}

template <class T>
Complex<T>
Complex<T>::operator*(const Complex<T> &c) const
{
    return(Complex<T>(*this) *= c);
}

template <class T>
Complex<T>
Complex<T>::operator/(const Complex<T> &c) const
{
    return(Complex<T>(*this) /= c);
}

// logical operators
template <class T>
int
Complex<T>::operator==(const Complex<T> &c) const
{
    // return((x == c.x) && (y == c.y));
    return((::abs(x-c.x) <= std::numeric_limits<T>::epsilon()) && 
           (::abs(y-c.y) <= std::numeric_limits<T>::epsilon()));
}

template <class T>
int
Complex<T>::operator!=(const Complex<T> &c) const
{
    // return((x != c.x) || (y != c.y));
    return(!(*this == c));
}

template <class T>
int
Complex<T>::operator<(const Complex<T> &c) const
{
    return(abs(*this) < abs(c));
}

template <class T>
int
Complex<T>::operator>(const Complex<T> &c) const
{
    return(abs(*this) > abs(c));
}

template <class T>
int
Complex<T>::operator<=(const Complex<T> &c) const
{
    // return(abs(*this) <= abs(c));
    return(!(*this > c));
}

template <class T>
int
Complex<T>::operator>=(const Complex<T> &c) const
{
    // return(abs(*this) >= abs(c));
    return(!(*this < c));
}

template <class T>
int
Complex<T>::operator||(const Complex<T> &c) const
{
    return(abs(*this) || abs(c));
}

template <class T>
int
Complex<T>::operator&&(const Complex<T> &c) const
{
    return(abs(*this) && abs(c));
}

// arithmetic operators
template <class T>
Complex<T> &
Complex<T>::operator=(T r)
{
    x = r;
    y = 0.0;
    return(*this);
}

template <class T>
Complex<T> &
Complex<T>::operator+=(T r)
{
    x += r;
    return(*this);
}

template <class T>
Complex<T> &
Complex<T>::operator-=(T r)
{
    x -= r;
    return(*this);
}

template <class T>
Complex<T> &
Complex<T>::operator*=(T r)
{
    x *= r;
    y *= r;
    return(*this);
}

template <class T>
Complex<T> &
Complex<T>::operator/=(T r)
{
    MustBeTrue(r != 0.0);
    x /= r;
    y /= r;
    return(*this);
}

template <class T>
Complex<T>
Complex<T>::operator+(T r) const
{
    return(Complex<T>(*this) += r);
}

template <class T>
Complex<T>
Complex<T>::operator-(T r) const
{
    return(Complex<T>(*this) -= r);
}

template <class T>
Complex<T>
Complex<T>::operator*(T r) const
{
    return(Complex<T>(*this) *= r);
}

template <class T>
Complex<T>
Complex<T>::operator/(T r) const
{
    return(Complex<T>(*this) /= r);
}

template <class T>
Complex<T>
operator+(T r, const Complex<T> &c)
{
    return(Complex<T>(r) += c);
}

template <class T>
Complex<T>
operator-(T r, const Complex<T> &c)
{
    return(Complex<T>(r) -= c);
}

template <class T>
Complex<T>
operator*(T r, const Complex<T> &c)
{
    return(Complex<T>(r) *= c);
}

template <class T>
Complex<T>
operator/(T r, const Complex<T> &c)
{
    return(Complex<T>(r) /= c);
}

// logical operators
template <class T>
int
Complex<T>::operator==(T r) const
{
    return((x == r) && (y == 0.0));
}

template <class T>
int
Complex<T>::operator!=(T r) const
{
    return((x != r) || (y != 0.0));
}

template <class T>
int
Complex<T>::operator<(T r) const
{
    return(abs(*this) < rabs(r));
}

template <class T>
int
Complex<T>::operator>(T r) const
{
    return(abs(*this) > rabs(r));
}

template <class T>
int
Complex<T>::operator<=(T r) const
{
    return(abs(*this) <= rabs(r));
}

template <class T>
int
Complex<T>::operator>=(T r) const
{
    return(abs(*this) >= rabs(r));
}

template <class T>
int
operator==(T r, const Complex<T> &c)
{
    return((c.x == r) && (c.y == 0.0));
}

template <class T>
int
operator!=(T r, const Complex<T> &c)
{
    return((c.x != r) || (c.y != 0.0));
}

template <class T>
int
operator<(T r, const Complex<T> &c)
{
    return(rabs(r) < abs(c));
}

template <class T>
int
operator>(T r, const Complex<T> &c)
{
    return(rabs(r) > abs(c));
}

template <class T>
int
operator<=(T r, const Complex<T> &c)
{
    return(rabs(r) <= abs(c));
}

template <class T>
int
operator>=(T r, const Complex<T> &c)
{
    return(rabs(r) >= abs(c));
}

// mathematical functions
static double exp(double x) { return(::exp(x)); }

template <class T>
Complex<T>
exp(const Complex<T> &p)
{
    T expx = exp(p.x);
    return(Complex<T>(expx*::cos(p.y), expx*::sin(p.y)));
}

static double log(double x) { return(::log(x)); }

template <class T>
Complex<T>
log(const Complex<T> &c)
{
    MustBeTrue((c.x != 0.0) || (c.y != 0.0));
    return(Complex<T>(log(abs(c)), arg(c)));
}

template <class T>
Complex<T>
log10(const Complex<T> &c)
{
    return(log(c)/Complex<T>(::log(10.0)));
}

template <class T>
Complex<T>
pow(const Complex<T> &b, const Complex<T> &p)
{
    if (b == 0.0)
        return(Complex<T>(0.0));
    else
        return(exp(p*log(b)));
}

template <class T>
Complex<T>
sqrt(const Complex<T> &c)
{
    if (c == 0.0)
        return(Complex<T>(0.0));
    else
        return(exp(Complex<T>(0.5)*log(c)));
}

static double cosh(double x) { return(::cosh(x)); }
static double sinh(double x) { return(::sinh(x)); }
static double cos(double x) { return(::cos(x)); }
static double sin(double x) { return(::sin(x)); }

template <class T>
Complex<T>
sin(const Complex<T> &c)
{
    return(Complex<T>(sin(c.x)*cosh(c.y), cos(c.x)*sinh(c.y)));
}

template <class T>
Complex<T>
cos(const Complex<T> &c)
{
    return(Complex<T>(cos(c.x)*cosh(c.y), -sin(c.x)*sinh(c.y)));
}

template <class T>
Complex<T>
tan(const Complex<T> &c)
{
    return(sin(c)/cos(c));
}

template <class T>
Complex<T>
asin(const Complex<T> &c)
{
    return(minusi*log(plusi*c+sqrt(plusone-c*c)));
}

template <class T>
Complex<T>
acos(const Complex<T> &c)
{
    return(minusi*log(c+plusi*sqrt(plusone-c*c)));
}

template <class T>
Complex<T>
atan(const Complex<T> &c)
{
    return(Complex<T>(0.5)*plusi*log((plusi+c)/(plusi-c)));
}

template <class T>
Complex<T>
sinh(const Complex<T> &c)
{
    return(Complex<T>(sinh(c.x)*cos(c.y), cosh(c.x)*sin(c.y)));
}

template <class T>
Complex<T>
cosh(const Complex<T> &c)
{
    return(Complex<T>(cosh(c.x)*cos(c.y), sinh(c.x)*sin(c.y)));
}

template <class T>
Complex<T>
tanh(const Complex<T> &c)
{
    return(sinh(c)/cosh(c));
}

template <class T>
Complex<T>
asinh(const Complex<T> &c)
{
    return(log(c+sqrt(c*c+plusone)));
}

template <class T>
Complex<T>
acosh(const Complex<T> &c)
{
    return(log(c+sqrt(c*c-plusone)));
}

template <class T>
Complex<T>
atanh(const Complex<T> &c)
{
    return(Complex<T>(0.5)*log((plusone+c)/(plusone-c)));
}

template <class T>
Complex<T>
conj(const Complex<T> &c)
{
    return(Complex<T>(c.x, -1.0*c.y));
}

static double sqrt(double x) { return(::sqrt(x)); }

template <class T>
T
abs(const Complex<T> &c)
{
    return(sqrt(c.x*c.x+c.y*c.y));
}

template <class T>
T
norm(const Complex<T> &c)
{
    return(sqrt(c.x*c.x+c.y*c.y));
}

static double atan(double x) { return(::atan(x)); }

template <class T>
T
arg(const Complex<T> &c)
{
    if (c.x > 0.0)
    {
        if (c.y > 0.0)
        {
            return(atan(c.y/c.x));
        }
        else if (c.y < 0.0)
        {
            return(-atan(rabs(c.y/c.x)));
        }
        else
        {
            return(0.0);
        }
    }
    else if (c.x < 0.0)
    {
        if (c.y > 0.0)
        {
            return(M_PI-atan(rabs(c.y/c.x)));
        }
        else if (c.y < 0.0)
        {
            return(-M_PI+atan(rabs(c.y/c.x)));
        }
        else
        {
            return(M_PI);
        }
    }
    else
    {
        if (c.y > 0.0)
        {
            return(M_PI/2.0);
        }
        else if (c.y < 0.0)
        {
            return(-M_PI/2.0);
        }
        else
        {
            return(0.0);
        }
    }
}

template <class T>
T
real(const Complex<T> &c)
{
    return(c.x);
}

template <class T>
T
imag(const Complex<T> &c)
{
    return(c.y);
}

template <class T>
T
rabs(T r)
{
    return((r < 0.0) ? -r : r);
}

/* print complex number */
template <class T>
std::ostream &
operator<<(std::ostream &os, const Complex<T> &c)
{
    os << "(" << c.x << "," << c.y << ")";
    return(os);
}
