//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_RATIONAL_H
#define __OMBT_RATIONAL_H
// rational number class, ie, fraction

// headers
#include <stdio.h>
#include <iostream>
#include "system/Returns.h"
#include "system/Debug.h"

namespace ombt {

// forward declarations 
template <class T> class Rational;
template <class T> Rational<T> operator+(T, const Rational<T> &);
template <class T> Rational<T> operator-(T, const Rational<T> &);
template <class T> Rational<T> operator*(T, const Rational<T> &);
template <class T> Rational<T> operator/(T, const Rational<T> &);
template <class T> bool operator==(T, const Rational<T> &);
template <class T> bool operator!=(T, const Rational<T> &);
template <class T> bool operator<(T, const Rational<T> &);
template <class T> bool operator>(T, const Rational<T> &);
template <class T> bool operator<=(T, const Rational<T> &);
template <class T> bool operator>=(T, const Rational<T> &);
template <class T> std::ostream &operator<<(std::ostream &, const Rational<T> &);
template <class T> Rational<T> abs(const Rational<T> &);
template <class T> Rational<T> operator-(const Rational<T> &);
template <class T> Rational<T> operator+(const Rational<T> &);

// rational number class definition
template <class T> class Rational
{
public:
	// data types
	struct Value {
		T n_;	// numerator
		T d_;	// denominator
	};
	typedef struct Value ValueType;

	// constructors and destructor
	Rational();
	Rational(T);
	Rational(T, T);
	Rational(const Rational &);
	Rational(const ValueType &);
	~Rational();

	// return copy of internal data
	void internal(ValueType &) const;
	Rational &operator=(const ValueType &);

	// arithmetic operations
	Rational &operator=(const Rational &);
	Rational &operator+=(const Rational &);
	Rational &operator-=(const Rational &);
	Rational &operator*=(const Rational &);
	Rational &operator/=(const Rational &);
	Rational operator+(const Rational &) const;
	Rational operator-(const Rational &) const;
	Rational operator*(const Rational &) const;
	Rational operator/(const Rational &) const;

	// logical operators 
	bool operator==(const Rational &) const;
	bool operator!=(const Rational &) const;
	bool operator<(const Rational &) const;
	bool operator>(const Rational &) const;
	bool operator<=(const Rational &) const;
	bool operator>=(const Rational &) const;

	// arithmetic operations
	Rational &operator=(T);
	Rational &operator+=(T);
	Rational &operator-=(T);
	Rational &operator*=(T);
	Rational &operator/=(T);
	Rational operator+(T) const;
	Rational operator-(T) const;
	Rational operator*(T) const;
	Rational operator/(T) const;
	template<typename TT> friend Rational<TT> operator+(TT, const Rational<TT> &);
	template<typename TT> friend Rational<TT> operator-(TT, const Rational<TT> &);
	template<typename TT> friend Rational<TT> operator*(TT, const Rational<TT> &);
	template<typename TT> friend Rational<TT> operator/(TT, const Rational<TT> &);

	// logical operators 
	bool operator==(T) const;
	bool operator!=(T) const;
	bool operator<(T) const;
	bool operator>(T) const;
	bool operator<=(T) const;
	bool operator>=(T) const;
	template<typename TT> friend bool operator==(TT, const Rational<TT> &);
	template<typename TT> friend bool operator!=(TT, const Rational<TT> &);
	template<typename TT> friend bool operator<(TT, const Rational<TT> &);
	template<typename TT> friend bool operator>(TT, const Rational<TT> &);
	template<typename TT> friend bool operator<=(TT, const Rational<TT> &);
	template<typename TT> friend bool operator>=(TT, const Rational<TT> &);

	// mathematical functions
	// friend Rational<T> abs<>(const Rational<T> &);
	// friend Rational<T> operator-<>(const Rational<T> &);
	template <typename TT> friend Rational<TT> abs(const Rational<TT> &);
	template <typename TT> friend Rational<TT> operator-(const Rational<TT> &);
	template <typename TT> friend Rational<TT> operator+(const Rational<TT> &);

	// number theory
	static T gcd(T u, T v);
	static T euclid_div_gcd(T u, T v);
	static T euclid_sub_gcd(T u, T v);
	static T euclid_recur_gcd(T u, T v);

	// miscellaneous
	friend std::ostream &operator<<<>(std::ostream &, const Rational<T> &);
	void print(FILE *, const char *, const char *) const;

	// normalize
	static Rational<T> &normalize(Rational<T> &r) {
#ifdef RATIONAL_USE_DIV_GCD
		T GCD = euclid_div_gcd(r.n_, r.d_);
#else
		T GCD = gcd(r.n_, r.d_);
#endif
		r.d_ /= GCD;
		r.n_ /= GCD;
		if (r.d_ < 0)
		{
			r.n_ = -r.n_;
			r.d_ = -r.d_;
		}
		else if (r.n_ == 0)
		{
			r.d_ = 1;
		}
		return(r);
	}

protected:
	// internal data = n_/d_
	T n_;	// numerator
	T d_;	// denominator
};

#include "numerics/Rational.i"

}

#endif
