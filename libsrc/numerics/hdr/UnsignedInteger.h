//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_UNSIGNED_INTEGER_H
#define __OMBT_UNSIGNED_INTEGER_H
// unsigned arbitrary precision integer

// headers
#include <string>
#include <iostream>
#include <vector>
#include "system/Returns.h"
#include "system/Debug.h"

namespace ombt {

// forward declarations 
template <int MD, class DT, class NT> 
	class UnsignedInteger;

template <int MD, class DT, class NT> 
	UnsignedInteger<MD, DT, NT> 
		operator+(NT, const UnsignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
	UnsignedInteger<MD, DT, NT> 
		operator-(NT, const UnsignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
	UnsignedInteger<MD, DT, NT> 
		operator*(NT, const UnsignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
	UnsignedInteger<MD, DT, NT> 
		operator/(NT, const UnsignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
	bool operator==(NT, const UnsignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
	bool operator!=(NT, const UnsignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
	bool operator<(NT, const UnsignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
	bool operator>(NT, const UnsignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
	bool operator<=(NT, const UnsignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
	bool operator>=(NT, const UnsignedInteger<MD, DT, NT> &);

template <int MD, class DT, class NT> 
	std::ostream &operator<<(std::ostream &, 
		const UnsignedInteger<MD, DT, NT> &);

// policy class
template <class DT, int DTSZ> class DigitTypePolicy;

template <> 
class DigitTypePolicy<unsigned long long, 8 > 
{
public:
	static const unsigned long long BitShift = 32ull;
	static const unsigned long long BitMask = 0x00000000ffffffffull;
	static const unsigned long long Base = 0x0000000100000000ull;
	static const unsigned long long MaxDigit = 0x00000000ffffffff;
	static const unsigned long long FullMaxDigit = 0xffffffffffffffffull;
	typedef unsigned long DataType;
};

template <> 
class DigitTypePolicy<unsigned long, 4 > 
{
public:
	static const unsigned long BitShift = 16;
	static const unsigned long BitMask = 0x0000ffff;
	static const unsigned long Base = 0x00010000;
	static const unsigned long MaxDigit = 0x0000ffff;
	static const unsigned long FullMaxDigit = 0xffffffff;
	typedef unsigned short DataType;
};

template <> 
class DigitTypePolicy<unsigned short, 2 > 
{
public:
	static const unsigned short BitShift = 8;
	static const unsigned short BitMask = 0x00ff;
	static const unsigned short Base = 0x0100;
	static const unsigned short MaxDigit = 0x00ff;
	static const unsigned short FullMaxDigit = 0xffff;
	typedef unsigned char DataType;
};

template <> 
class DigitTypePolicy<unsigned int, 4 > 
{
public:
	static const unsigned int BitShift = 16;
	static const unsigned int BitMask = 0x0000ffff;
	static const unsigned int Base = 0x00010000;
	static const unsigned int MaxDigit = 0x0000ffff;
	static const unsigned int FullMaxDigit = 0xffffffff;
	typedef unsigned short DataType;
};

template <> 
class DigitTypePolicy<unsigned int, 2 > 
{
public:
	static const unsigned int BitShift = 8;
	static const unsigned int BitMask = 0x00ff;
	static const unsigned int Base = 0x0100;
	static const unsigned int MaxDigit = 0x00ff;
	static const unsigned int FullMaxDigit = 0xffff;
	typedef unsigned char DataType;
};

// rational number class definition
template <int MaxDigits = 100, 
	  class DigitType = unsigned short,
	  class NumberType = std::vector<DigitType> > class UnsignedInteger
{
public:
	// types
	typedef NumberType Number;

	// constructors and destructor
	UnsignedInteger();
	UnsignedInteger(DigitType);
	UnsignedInteger(const NumberType &);
	UnsignedInteger(const UnsignedInteger &);
	UnsignedInteger(const std::string &);
	~UnsignedInteger();

	// arithmetic operations
	UnsignedInteger &operator=(const std::string &);
	UnsignedInteger &operator=(const UnsignedInteger &);
	UnsignedInteger &operator+=(const UnsignedInteger &);
	UnsignedInteger &operator-=(const UnsignedInteger &);
	UnsignedInteger &operator*=(const UnsignedInteger &);
	UnsignedInteger &operator/=(const UnsignedInteger &);
	UnsignedInteger &operator%=(const UnsignedInteger &);
	UnsignedInteger operator+(const UnsignedInteger &) const;
	UnsignedInteger operator-(const UnsignedInteger &) const;
	UnsignedInteger operator*(const UnsignedInteger &) const;
	UnsignedInteger operator/(const UnsignedInteger &) const;
	UnsignedInteger operator%(const UnsignedInteger &) const;

	// access operations
	DigitType &operator[](int);
	const DigitType &operator[](int) const;
	int numberOfDigits() const;
	int maxNumberOfDigits() const;

	// logical operators 
	bool operator==(const UnsignedInteger &) const;
	bool operator!=(const UnsignedInteger &) const;
	bool operator<(const UnsignedInteger &) const;
	bool operator>(const UnsignedInteger &) const;
	bool operator<=(const UnsignedInteger &) const;
	bool operator>=(const UnsignedInteger &) const;

	// arithmetic operations
	UnsignedInteger &operator=(NumberType);
	UnsignedInteger &operator+=(NumberType);
	UnsignedInteger &operator-=(NumberType);
	UnsignedInteger &operator*=(NumberType);
	UnsignedInteger &operator/=(NumberType);
	UnsignedInteger &operator%=(NumberType);
	UnsignedInteger operator+(NumberType) const;
	UnsignedInteger operator-(NumberType) const;
	UnsignedInteger operator*(NumberType) const;
	UnsignedInteger operator/(NumberType) const;
	UnsignedInteger operator%(NumberType) const;
	template<int MD, class DT, class NT> 
		friend UnsignedInteger<MD, DT, NT> 
			operator+(NT, const UnsignedInteger<MD, DT, NT> &);
	template<int MD, class DT, class NT> 
		friend UnsignedInteger<MD, DT, NT> 
			operator-(NT, const UnsignedInteger<MD, DT, NT> &);
	template<int MD, class DT, class NT> 
		friend UnsignedInteger<MD, DT, NT> 
			operator*(NT, const UnsignedInteger<MD, DT, NT> &);
	template<int MD, class DT, class NT> 
		friend UnsignedInteger<MD, DT, NT> 
			operator/(NT, const UnsignedInteger<MD, DT, NT> &);

	// logical operators 
	bool operator==(NumberType) const;
	bool operator!=(NumberType) const;
	bool operator<(NumberType) const;
	bool operator>(NumberType) const;
	bool operator<=(NumberType) const;
	bool operator>=(NumberType) const;
	template<int MD, class DT, class NT> 
		friend bool operator==(NT, const UnsignedInteger<MD, DT, NT> &);
	template<int MD, class DT, class NT> 
		friend bool operator!=(NT, const UnsignedInteger<MD, DT, NT> &);
	template<int MD, class DT, class NT> 
		friend bool operator<(NT, const UnsignedInteger<MD, DT, NT> &);
	template<int MD, class DT, class NT> 
		friend bool operator>(NT, const UnsignedInteger<MD, DT, NT> &);
	template<int MD, class DT, class NT> 
		friend bool operator<=(NT, const UnsignedInteger<MD, DT, NT> &);
	template<int MD, class DT, class NT> 
		friend bool operator>=(NT, const UnsignedInteger<MD, DT, NT> &);

	// I/O
	template <int MD, class DT, class NT> 
		friend std::ostream 
			&operator<<(std::ostream &, const UnsignedInteger<MD, DT, NT> &);
	std::string toString() const;

protected:
	// normalize number form
	UnsignedInteger<MaxDigits, DigitType, NumberType> 
		&normalize(int, DigitType = DigitType(0));
	UnsignedInteger<MaxDigits, DigitType, NumberType> 
		&normalize(int, const std::string &);
	UnsignedInteger<MaxDigits, DigitType, NumberType> 
		&normalize(const NumberType &, int);
	UnsignedInteger<MaxDigits, DigitType, NumberType> 
		&normalize(const UnsignedInteger<MaxDigits, DigitType, NumberType> &n);
	void normalizeZero();

	// utility functions
	void add(int, int, const NumberType &, const NumberType &);
	void subtract(int, int, const NumberType &, const NumberType &);
	void multiply(int, int, const NumberType &, const NumberType &);
	void divide(int, int, const NumberType &, const NumberType &);
	void multiply(int, DigitType, int, const NumberType &, UnsignedInteger<MaxDigits, DigitType, NumberType> &);
	int compare(const UnsignedInteger<MaxDigits, DigitType, NumberType> &n) const;

	// internal data 
	int maxdigits_;
	int ndigits_;
	DigitType base_;
	NumberType n_;
};

#include "numerics/UnsignedInteger.i"

}

#endif
