//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_TRIDIAGONAL_H
#define __OMBT_TRIDIAGONAL_H

// use Thomas algorithm to solve tridiagonal band matrix

// headers
#include <stdlib.h>
#include <math.h>
#include <iostream>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "matrix/Vector.h"
#include "matrix/Matrix.h"
#include "matrix/Epsilon.h"

namespace ombt {

// forward declarations
template <class T> class TriDiagonalMatrix;

template <class T> 
std::ostream &
operator<<(std::ostream &, const TriDiagonalMatrix<T> &);

// tridiagonal wrapper class
template <class T> 
class TriDiagonalMatrix {
public:
	// types
	class A {
	public:
		A(Matrix<T> &m): m_(m) { }
		~A() { }
		T &operator[](unsigned int i) {
			static T a0(0);
			if (i != 0)
				return(m_(i,i-1));
			else
				return(a0);
		}
		T &operator[](unsigned int i) const {
			static T a0(0);
			if (i != 0)
				return(m_(i,i-1));
			else
				return(a0);
		}

	private:
		Matrix<T> &m_;
	};

	class B {
	public:
		B(Matrix<T> &m): m_(m) { }
		~B() { }
		T &operator[](unsigned int i) {
			return(m_(i,i));
		}
		T &operator[](unsigned int i) const {
			return(m_(i,i));
		}

	private:
		Matrix<T> &m_;
	};

	class C {
	public:
		C(Matrix<T> &m): m_(m) { max_ = m.getRows()-1; }
		~C() { }
		T &operator[](unsigned int i) {
			static T cmax(0);
			if (i != max_)
				return(m_(i,i+1));
			else
				return(cmax);
		}
		T &operator[](unsigned int i) const {
			static T cmax(0);
			if (i != max_)
				return(m_(i,i+1));
			else
				return(cmax);
		}

	private:
		int max_;
		Matrix<T> &m_;
	};

	// ctor and dtor
	TriDiagonalMatrix(Matrix<T> &m):
		m_(m) {
        	MustBeTrue(m.getRows() > 0);
        	MustBeTrue(m.getRows() == m.getCols());
        	max_ = m.getRows()-1;
	}
	~TriDiagonalMatrix() { }

	// access operators
	T &a(unsigned int i) {
		static T a0(0);
		if (i != 0)
			return(m_(i,i-1));
		else
			return(a0);
	}
	T &a(unsigned int i) const {
		static T a0(0);
		if (i != 0)
			return(m_(i,i-1));
		else
			return(a0);
	}
	T &b(unsigned int i) {
		return(m_(i,i));
	}
	T &b(unsigned int i) const {
		return(m_(i,i));
	}
	T &c(unsigned int i) {
		static T cmax(0);
		if (i != max_)
			return(m_(i,i+1));
		else
			return(cmax);
	}
	T &c(unsigned int i) const {
		static T cmax(0);
		if (i != max_)
			return(m_(i,i+1));
		else
			return(cmax);
	}
	T &operator()(unsigned int r, unsigned int c) const {
		static T offdiag(0);
		if (r==c)
			return(m_(r,c));
		else if (r<c && (c-r) == 1)
			return(m_(r,c));
		else if (r>c && (r-c) == 1)
			return(m_(r,c));
		else
			return(offdiag);
	}

	// print matrix
	void dump(std::ostream &os) const {
		os << "tridiagonal matrix[" << m_.getRows() << "," << m_.getCols() << "] = {" << std::endl;
		for (unsigned int ir = 0; ir < m_.getRows(); ir++)
		{
			for (unsigned int ic = 0; ic < m_.getCols(); ic++)
			{
				os << (*this)(ir,ic) << " ";
			}
			os << std::endl;
		}
		os << "}" << std::endl;
		return;
	}

	template <typename TT> 
	friend std::ostream &operator<<(std::ostream &os,
					const TriDiagonalMatrix<TT> &m)
	{
		m.dump(os);
		return(os);
	}

private:
	// data
	int max_;
	Matrix<T> &m_;
};

// use thomas algorithm to solve tridiagonal band matrix
template <class T>
int
SolveTriDiagonal(Matrix<T> &m, Vector<T> &x, Vector<T> &y, T &ep);

}

#include "matrix/TriDiagonal.i"

#endif


