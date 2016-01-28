//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// rational class functions

// local definitions
#if 0
#define zero Rational<T>(0)
#define plusone Rational<T>(1)
#define minusone Rational<T>(-1)
#endif

// constructors and destructor
template <class T>
Rational<T>::Rational():
	n_(0), d_(1)
{
}

template <class T>
Rational<T>::Rational(T n):
	n_(n), d_(1)
{
}

template <class T>
Rational<T>::Rational(T n, T d):
	n_(n), d_(d)
{
	MustBeTrue(d_ != 0);
	normalize(*this);
}

template <class T>
Rational<T>::Rational(const Rational<T> &r): 
	n_(r.n_), d_(r.d_)
{
	MustBeTrue(d_ != 0);
	normalize(*this);
}

template <class T>
Rational<T>::Rational(const typename Rational<T>::ValueType &r):
	n_(r.n_), d_(r.d_)
{
	MustBeTrue(d_ != 0);
	normalize(*this);
}

template <class T>
Rational<T>::~Rational() 
{
}

// return copy of internal data
template <class T>
void 
Rational<T>::internal(typename Rational<T>::ValueType &raw) const
{
	raw.n_ = n_;
	raw.d_ = d_;
	return;
}

template <class T>
Rational<T> &
Rational<T>::operator=(const typename Rational<T>::ValueType &rhs)
{
	n_ = rhs.n_;
	d_ = rhs.d_;
	MustBeTrue(d_ != 0);
	return(normalize(*this));
}

// arithmetic operators
template <class T>
Rational<T> &
Rational<T>::operator=(const Rational<T> &r)
{
	n_ = r.n_;
	d_ = r.d_;
	MustBeTrue(d_ != 0);
	return(normalize(*this));
}

template <class T>
Rational<T> &
Rational<T>::operator+=(const Rational<T> &r)
{
	T n = n_*r.d_ + r.n_*d_;
        T d = d_*r.d_;
	d_ = d;
	n_ = n;
	MustBeTrue(d_ != 0);
	return(normalize(*this));
}

template <class T>
Rational<T> &
Rational<T>::operator-=(const Rational<T> &r)
{
	T n = n_*r.d_ - r.n_*d_;
        T d = d_*r.d_;
	d_ = d;
	n_ = n;
	MustBeTrue(d_ != 0);
	return(normalize(*this));
}

template <class T>
Rational<T> &
Rational<T>::operator*=(const Rational<T> &r)
{
	d_ *= r.d_;
	n_ *= r.n_;
	MustBeTrue(d_ != 0);
	return(normalize(*this));
}

template <class T>
Rational<T> &
Rational<T>::operator/=(const Rational<T> &r)
{
	T d = d_*r.n_;
	T n = n_*r.d_;
	d_ = d;
	n_ = n;
	MustBeTrue(d_ != 0);
	return(normalize(*this));
}

template <class T>
Rational<T>
Rational<T>::operator+(const Rational<T> &r) const
{
	return(Rational<T>(*this) += r);
}

template <class T>
Rational<T>
Rational<T>::operator-(const Rational<T> &r) const
{
	return(Rational<T>(*this) -= r);
}

template <class T>
Rational<T>
Rational<T>::operator*(const Rational<T> &r) const
{
	return(Rational<T>(*this) *= r);
}

template <class T>
Rational<T>
Rational<T>::operator/(const Rational<T> &r) const
{
	return(Rational<T>(*this) /= r);
}

// logical operators
template <class T>
bool
Rational<T>::operator==(const Rational<T> &r) const
{
	return(d_*r.n_ == n_*r.d_);
}

template <class T>
bool
Rational<T>::operator!=(const Rational<T> &r) const
{
	return(!(*this == r));
}

template <class T>
bool
Rational<T>::operator<(const Rational<T> &r) const
{
	return (n_*r.d_ < r.n_*d_);
}

template <class T>
bool
Rational<T>::operator>(const Rational<T> &r) const
{
	return (n_*r.d_ > r.n_*d_);
}

template <class T>
bool
Rational<T>::operator<=(const Rational<T> &r) const
{
	return(!(*this > r));
}

template <class T>
bool
Rational<T>::operator>=(const Rational<T> &r) const
{
	return(!(*this < r));
}

// arithmetic operators
template <class T>
Rational<T> &
Rational<T>::operator=(T r)
{
	n_ = r;
	d_ = 1;
	return(*this);
}

template <class T>
Rational<T> &
Rational<T>::operator+=(T r)
{
	n_ += r*d_;
	MustBeTrue(d_ != 0);
	return(normalize(*this));
}

template <class T>
Rational<T> &
Rational<T>::operator-=(T r)
{
	n_ -= r*d_;
	MustBeTrue(d_ != 0);
	return(normalize(*this));
}

template <class T>
Rational<T> &
Rational<T>::operator*=(T r)
{
	n_ *= r;
	MustBeTrue(d_ != 0);
	return(normalize(*this));
}

template <class T>
Rational<T> &
Rational<T>::operator/=(T r)
{
	d_ *= r;
	MustBeTrue(d_ != 0);
	return(normalize(*this));
}

template <class T>
Rational<T>
Rational<T>::operator+(T r) const
{
	return(Rational<T>(*this) += r);
}

template <class T>
Rational<T>
Rational<T>::operator-(T r) const
{
	return(Rational<T>(*this) -= r);
}

template <class T>
Rational<T>
Rational<T>::operator*(T r) const
{
	return(Rational<T>(*this) *= r);
}

template <class T>
Rational<T>
Rational<T>::operator/(T r) const
{
	return(Rational<T>(*this) /= r);
}

template <class T>
Rational<T>
operator+(T n, const Rational<T> &r)
{
	return(Rational<T>(n) += r);
}

template <class T>
Rational<T>
operator-(T n, const Rational<T> &r)
{
	return(Rational<T>(n) -= r);
}

template <class T>
Rational<T>
operator*(T n, const Rational<T> &r)
{
	return(Rational<T>(n) *= r);
}

template <class T>
Rational<T>
operator/(T n, const Rational<T> &r)
{
	return(Rational<T>(n) /= r);
}

// logical operators
template <class T>
bool
Rational<T>::operator==(T r) const
{
	return(*this == Rational<T>(r));
}

template <class T>
bool
Rational<T>::operator!=(T r) const
{
	return(!(*this == r));
}

template <class T>
bool
Rational<T>::operator<(T r) const
{
	return(*this < Rational<T>(r));
}

template <class T>
bool
Rational<T>::operator>(T r) const
{
	return(*this > Rational<T>(r));
}

template <class T>
bool
Rational<T>::operator<=(T r) const
{
	return(!(*this > r));
}

template <class T>
bool
Rational<T>::operator>=(T r) const
{
	return(!(*this < r));
}

template <class T>
bool
operator==(T n, const Rational<T> &r)
{
	return(r == n);
}

template <class T>
bool
operator!=(T n, const Rational<T> &r)
{
	return(!(r == n));
}

template <class T>
bool
operator<(T n, const Rational<T> &r)
{
	return(!(r >= n));
}

template <class T>
bool
operator>(T n, const Rational<T> &r)
{
	return(!(r <= n));
}

template <class T>
bool
operator<=(T n, const Rational<T> &r)
{
	return(!(r > n));
}

template <class T>
bool
operator>=(T n, const Rational<T> &r)
{
	return(!(r < n));
}

// misc
template <class T>
Rational<T>
abs(const Rational<T> &r)
{
	if (r < T(0))
		return(-r);
	else
		return(r);
}

template <class T>
Rational<T>
operator-(const Rational<T> &r)
{
	return(r*T(-1));
}

template <class T>
Rational<T>
operator+(const Rational<T> &r)
{
	return(r);
}

/* print complex number */
template <class T>
std::ostream &
operator<<(std::ostream &os, const Rational<T> &r)
{
	if (r.n_ == T(0) || r.d_ == T(1))
		os << r.n_;
	else 
		os << r.n_ << "/" << r.d_;
	return(os);
}

template <class T>
void
Rational<T>::print(FILE *os, const char *format, const char *newline) const
{
	if (n_ == T(0) || d_ == T(1))
	{
		fprintf(os, format, n_);
	}
	else 
	{
		fprintf(os, format, n_);
		fprintf(os, "/");
		fprintf(os, format, d_);
	}
	fprintf(os, "%s", newline);
}

// find gcd for two numbers
template <class T>
T
Rational<T>::euclid_div_gcd(T a, T b)
{
	while (b != T(0))
	{
		T t = b;
		b = a % b;
		a = t;
	}
	return(a);
}

template <class T>
T
Rational<T>::euclid_sub_gcd(T a, T b)
{
	if (a == T(0)) return(b);

	while (b != T(0))
	{
		if (a > b)
			a -= b;
		else
			b -= a;
	}
	return(a);
}

template <class T>
T
Rational<T>::euclid_recur_gcd(T a, T b)
{
	if (b == T(0))
		return(a);
	else
		return(euclid_recur_gcd(b, a%b));
}

template <class T>
T
Rational<T>::gcd(T u, T v)
{
	int shift;

	// must be non-negative
	if (u < 0) u = -u;
	if (v < 0) v = -v;
 
	/* GCD(0,x) := x */
	if (u == 0 || v == 0)
		return u | v;
 
	/* Let shift := lg K, where K is the greatest power of 2
           dividing both u and v. */
	for (shift = 0; ((u | v) & 1) == 0; ++shift)
	{
		u >>= 1;
		v >>= 1;
	}
 
	while ((u & 1) == 0)
	{
		u >>= 1;
	}
 
	/* From here on, u is always odd. */
	do {
		while ((v & 1) == 0)  /* Loop X */
		{
			v >>= 1;
		}
 
		/* Now u and v are both odd, so diff(u, v) is even.
		   Let u = min(u, v), v = diff(u, v)/2. */
		if (u < v)
		{
			v -= u;
		}
		else
		{
			T diff = u - v;
			u = v;
			v = diff;
		}
		v >>= 1;
	} while (v != 0);
 
	return u << shift;
}

