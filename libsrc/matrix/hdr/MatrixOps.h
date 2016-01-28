//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_MATRIXOPS_H
#define __OMBT_MATRIXOPS_H

// gaussian LUP decomposition definitions

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

// required math operations
float conj(const float &);
double conj(const double &);
long double conj(const long double &);

// standard matrix operations 
template <class T> int transpose(Matrix<T> &);
template <class T> int conjugate(Matrix<T> &);
template <class T> int adjoint(Matrix<T> &);
template <class T> int trace(const Matrix<T> &, T &);

// calculates gaussian LUP decomposition for a matrix.
template <class T>
int
gaussianLUP(Matrix<T> &, Vector<int> &, T, T &);

// solves set of linear equations using results of 
// gaussian LUP decomposition
template <class T>
int
solveLUP(Matrix<T> &, Vector<T> &, Vector<T> &, Vector<int> &, T);

// calculate the inverse using gaussian LUP results
template <class T>
int
inverseLUP(Matrix<T> &, Matrix<T> &, Vector<int> &, T);

// calculate the determinant using gaussian LUP results
template <class T>
int
determinantLUP(Matrix<T> &, T &);

}

#include "matrix/MatrixOps.i"

#endif
