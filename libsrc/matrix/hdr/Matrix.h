//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_MATRIX_H
#define __OMBT_MATRIX_H

// matrix class definitions

// headers
#include <stdlib.h>
#include <math.h>
#include <iostream>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "matrix/Vector.h"
#include "matrix/Epsilon.h"

namespace ombt {

// forward declarations
template <class T> class Matrix;

template <class T> 
Vector<T> 
operator*(const Vector<T> &, const Matrix<T> &);

template <class T> 
Matrix<T> 
operator*(const T &, const Matrix<T> &);

template <class T> 
std::ostream &
operator<<(std::ostream &, const Matrix<T> &);

// matrix class definition
template <class T> class Matrix
{
public:
	// constructors and destructor
	Matrix(unsigned int, unsigned int);
	Matrix(unsigned int, unsigned int, const T * );
	Matrix(const Matrix<T> &);
	~Matrix();

	// assignment operators and accessors
	Matrix<T> &operator=(const Matrix<T> &);
	T &operator[](unsigned int);
	T &operator[](unsigned int) const;
	T &operator()(unsigned int, unsigned int);
	T &operator()(unsigned int, unsigned int) const;

	// matrix operations
	Matrix<T> &operator+=(const Matrix<T> &);
	Matrix<T> &operator-=(const Matrix<T> &);
	Matrix<T> &operator*=(const Matrix<T> &);
	Matrix<T> operator+(const Matrix<T> &) const;
	Matrix<T> operator-(const Matrix<T> &) const;
	Matrix<T> operator*(const Matrix<T> &) const;

	// matrix and vector operations
	Vector<T> operator*(const Vector<T> &) const;
	template <typename TT> friend Vector<TT> operator*(const Vector<TT> &, const Matrix<TT> &);

	// matrix and scalar operations
	Matrix<T> &operator*=(const T &);
	Matrix<T> &operator/=(const T &);
	Matrix<T> operator*(const T &) const;
	Matrix<T> operator/(const T &) const;
	template <typename TT> friend Matrix<TT> operator*(const TT &, const Matrix<TT> &);

	// logical operators
	int operator==(const Matrix<T> &) const;
	int operator!=(const Matrix<T> &) const;

	// other functions
	inline unsigned int getRows() { return(nrows); }
	inline unsigned int getCols() { return(ncols); }
	void dump(std::ostream &) const;
	template <typename TT> friend std::ostream &operator<<(std::ostream &, const Matrix<TT> &);

	// get epsilon 
	T getEpsilon() {
		return(epsilon);
	}

protected:
#ifdef SORTANDADD
	// specialized routines for adding two arrays.
	T sortAndAdd(const Vector<T> &, const Vector<T> &, unsigned int) const;
#endif

protected:
	// internal data
	T *matrix;
	unsigned int nrows, ncols;
	T epsilon;
};

}

#include "matrix/Matrix.i"

#endif
