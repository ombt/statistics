//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_SIGNED_INTEGER_H
#define __OMBT_SIGNED_INTEGER_H
// extended precision signed integer

// headers
#include <iostream>
#include <string>
#include <vector>
#include "system/Returns.h"
#include "system/Debug.h"
#include "numerics/UnsignedInteger.h"

namespace ombt {

// forward declarations 
template <int MD, class DT, class NT> class SignedInteger;

template <int MD, class DT, class NT> 
SignedInteger<MD, DT, NT> operator+(NT, const SignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
SignedInteger<MD, DT, NT> operator-(NT, const SignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
SignedInteger<MD, DT, NT> operator*(NT, const SignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
SignedInteger<MD, DT, NT> operator/(NT, const SignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
bool operator==(NT, const SignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
bool operator!=(NT, const SignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
bool operator<(NT, const SignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
bool operator>(NT, const SignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
bool operator<=(NT, const SignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
bool operator>=(NT, const SignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
SignedInteger<MD, DT, NT> abs(const SignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
SignedInteger<MD, DT, NT> operator-(const SignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
SignedInteger<MD, DT, NT> operator+(const SignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
std::ostream &operator<<(std::ostream &, const SignedInteger<MD, DT, NT> &);

// extended precision number class definition
template <int MaxDigits = 50,
          class DigitType = unsigned short,
	  class NumberType = std::vector<DigitType> > class SignedInteger
{
public:
	// types
	typedef NumberType Number;

	// constructors and destructor
	SignedInteger();
	SignedInteger(DigitType, int sign = 1);
	SignedInteger(const NumberType &, int sign = 1);
	SignedInteger(const std::string &);
	SignedInteger(const SignedInteger &);
	~SignedInteger();

	// arithmetic operations
	SignedInteger &operator=(const SignedInteger &);
	SignedInteger &operator=(const std::string &);
	SignedInteger &operator+=(const SignedInteger &);
	SignedInteger &operator-=(const SignedInteger &);
	SignedInteger &operator*=(const SignedInteger &);
	SignedInteger &operator/=(const SignedInteger &);
	SignedInteger &operator%=(const SignedInteger &);
	SignedInteger operator+(const SignedInteger &) const;
	SignedInteger operator-(const SignedInteger &) const;
	SignedInteger operator*(const SignedInteger &) const;
	SignedInteger operator/(const SignedInteger &) const;
	SignedInteger operator%(const SignedInteger &) const;

	// access operators
	DigitType &operator[](int i) { return number_[i]; }
	const DigitType &operator[](int i) const { return number_[i]; }
	int sign() const { return sign_; }
	bool isNegative() const { return sign_ < 0; }
	bool isPositive() const { return sign_ >= 0; }
	int numberOfDigits() const { return number_.numberOfDigits(); }
	int maxNumberOfDigits() const { return number_.maxNumberOfDigits(); }

	// logical operators 
	bool operator==(const SignedInteger &) const;
	bool operator!=(const SignedInteger &) const;
	bool operator<(const SignedInteger &) const;
	bool operator>(const SignedInteger &) const;
	bool operator<=(const SignedInteger &) const;
	bool operator>=(const SignedInteger &) const;

	// unary operators
	template <int MD, class DT, class NT> 
		friend SignedInteger<MD, DT, NT> 
			operator-(const SignedInteger<MD, DT, NT> &);
	template <int MD, class DT, class NT> 
		friend SignedInteger<MD, DT, NT> 
			operator+(const SignedInteger<MD, DT, NT> &);

	// misc functions
	template <int MD, class DT, class NT> 
		friend SignedInteger<MD, DT, NT> 
			abs(const SignedInteger<MD, DT, NT> &);
	template <int MD, class DT, class NT> 
		friend std::ostream 
			&operator<<(std::ostream &, const SignedInteger<MD, DT, NT> &);
	std::string toString() const;

protected:
	// normalize number form
	SignedInteger<MaxDigits, DigitType, NumberType> &normalize(int);
	SignedInteger<MaxDigits, DigitType, NumberType> &normalize(const std::string &);
	SignedInteger<MaxDigits, DigitType, NumberType> &normalize();

	// internal data 
	int sign_;
	UnsignedInteger<MaxDigits, DigitType, NumberType> number_;
};

#include "numerics/SignedInteger.i"

}

#endif
