//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_VECTOR_H
#define __OMBT_VECTOR_H
// vector class

// headers
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <iostream>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"

namespace ombt {

// forward declarations
template <class T> class Vector;
template <class T> Vector<T> conj(const Vector<T> &);
template <class T> T dot(const Vector<T> &, const Vector<T> &);
template <class T> T norm(const Vector<T> &);

// vector class definition
template <class T> class Vector
{
public:
	 // constructors and destructor
	 Vector(unsigned int);
	 Vector(const T *, unsigned int);
	 Vector(const Vector<T> &);
	 ~Vector();

	 // assignment
	 Vector<T> &operator=(const Vector<T> &);
	 T &operator[](unsigned int);
	 T &operator[](unsigned int) const;

	 // vector operations
	 Vector<T> &operator+=(const Vector<T> &);
	 Vector<T> &operator-=(const Vector<T> &);
	 Vector<T> operator+(const Vector<T> &) const;
	 Vector<T> operator-(const Vector<T> &) const;

	 // arithmetic operations
	 Vector<T> &operator*=(const T &);
	 Vector<T> &operator/=(const T &);
	 Vector<T> operator*(const T &) const;
	 Vector<T> operator/(const T &) const;
	 friend Vector<T> operator*(const T &n, const Vector<T> &v) {
		return(Vector<T>(v) *= n);
	 }

	 // logical operators
	 int operator==(const Vector<T> &) const;
	 int operator!=(const Vector<T> &) const;

	 // vector products
	 template <typename TT> friend Vector<TT> conj(const Vector<TT> &);
	 template <typename TT> friend TT dot(const Vector<TT> &, const Vector<TT> &);
	 template <typename TT> friend TT norm(const Vector<TT> &);

	 // miscellaneous 
	 inline unsigned int getDimension() const { 
		  return(dimension);
	 }
	 void dump(std::ostream &) const;
	 friend std::ostream &operator<<(std::ostream &os, const Vector<T> &v) {
		v.dump(os);
		return(os);
	}

protected:
	 // data
	 unsigned int dimension;
	 T *vector;
};

}

#include "matrix/Vector.i"

#endif
