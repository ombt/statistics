//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
//
// this file contain functions for solving a linear set of 
// equations using gaussian eliminination or LUP decomposition.
//
// given the equation M*x = y where M is an nxn matrix and x, y
// n-dimensioned vectors. Solve for x given M and y. There are
// two direct methods of solving this set of equations. one uses
// gaussian elimination and the other uses an LU decomposition.
//
// gaussian elimination reduces a matrix to upper triangular
// form. once the matrix is in this form, the back substitution
// is used to solve for x.
//
// the second form factors M into two matrices, a lower triangular
// matrix L and an upper triangular matrix U, which satisfy
// relation, M = L*U. it works as follows.
//
// let M*x = y where M is an nxn matrix and x, and y are
// n-dimension vectors. Then let M = L*U where L is lower
// triangular and U is upper triangular. the solution for x
// is given by:
//
//	M*x = L*U*x = y
//
// let z = U*x, then
//
//	L*U*x = L*z = y and U*x = z.
//
// to solve for x, first solve L*z = y. Use forward substituion since
// L is lower triangular. Then once you have z, solve for x
// using the equation:
//
//	U*x = z.
//
// solve using back substitution since U is upper triangular. this then
// gives you x, the original problem.
// 
// there are two basic ways to calculate L and U. one makes L a unit
// lower triangular matrix (l(i, i) = 1, 1<= i <= n). this is doolittle's
// method. the second method makes U a unit upper triangular matrix
// (u(i, i) = 1, 1 <= i <= n). gaussian elimination generates a 
// doolittle factorization.
//

namespace ombt {

//
// given a matrix, generate an LUP decomposition using Gaussian
// elimination with scaling and pivoting.
//
template <class T>
int
GaussianLUP_Pivot(Matrix<T> &m, Vector<int> &p, T ep, T &sign)
{
	// must be a square matrix
	MustBeTrue(m.getRows() == m.getCols() && m.getRows() > 0);
	sign = -1;

	// check epsilon, set if invalid
	T minep = calcEpsilon(T(0));
	if ((ep = fabs(ep)) < minep)
		ep = minep;

	// get number of rows and columns
	int max = m.getRows();

	// generate scaling information for each row. initialize 
	// permutation array, p.
	int i;
	Vector<T> s(max);
	for (i = 0; i < max; i++)
	{
		p[i] = i;
		if (1 < max)
			s[i] = fabs(m(i, 1));
		else
			s[i] = fabs(m(i, 0));
		for (int j = 2; j < max; j++)
		{
			T tmp = fabs(m(i, j));
			if (tmp > s[i])
				s[i] = tmp;
		}
	}

	// start gaussian elimination process
	for (int k = 0; k < (max-1); k++)
	{
		// find pivot row
		int pivot = k;
		T tmpf = fabs(m(p[pivot], k))/s[p[pivot]];
		for (i = k+1; i < max; i++)
		{
			T tmpf2 = fabs(m(p[i], k))/s[p[i]];
			if (tmpf2 > tmpf)
			{
				pivot = i;
				tmpf = tmpf2;
			}
		}
		if (pivot != k)
		{
			int tmpp = p[k];
			p[k] = p[pivot];
			p[pivot] = tmpp;
			sign = -sign;
		}

		// check for division by zero
		if (fabs(m(p[k], k)) <= ep)
			return(NOTOK);

		// calculate L and U matrices
		for (i = k+1; i < max; i++)
		{
			// multiplier for column
			T d = m(p[i], k)/m(p[k], k);

			// save multiplier since it is L.
			m(p[i], k) = d;

			// reduce original matrix to get U.
			for (int j = k+1; j < max; j++)
			{
				CheckForOverFlow(d, m(p[k], j));
				m(p[i], j) -= d*m(p[k], j);
			}
		}
	}

	// all done
	return(OK);
}

//
// given a gaussian LUP decomposition of a matrix and a permutation vector,
// solve for x-vector using the given y-vector.
//
template <class T>
int
SolveUsingGaussianLUP_Pivot(Matrix<T> &m, 
	Vector<T> &x, Vector<T> &y, Vector<int> &p, T ep)
{
	// must be a square matrix
	MustBeTrue(m.getRows() == m.getCols() && m.getRows() > 0);

	// check epsilon, set if invalid
	T minep = calcEpsilon(T(0));
	if ((ep = fabs(ep)) < minep)
		ep = minep;

	// get number of rows and columns
	int max = m.getRows();

	// update y-vector
	for (int k = 0; k < (max-1); k++)
	{
		for (int i = k+1; i < max; i++)
		{
			CheckForOverFlow(m(p[i], k), y[p[k]]);
			y[p[i]] -= m(p[i], k)*y[p[k]];
		}
	}

	// start backward substitution
	for (int i = max-1; i >= 0; i--)
	{
		// check for a singular matrix
		if (fabs(m(p[i], i)) <= ep)
			return(NOTOK);

		// solve for x by substituting previous solutions
		x[i] = y[p[i]];
		for (int j = i+1; j < max; j++)
		{
			CheckForOverFlow(m(p[i], j), x[j]);
			x[i] -= m(p[i], j)*x[j];
		}
		x[i] /= m(p[i], i);
	}

	// all done
	return(OK);
}

//
// given a gaussian LUP decomposition of a matrix and a permutation vector,
// calculate the inverse of the original matrix.
//
template <class T>
int
GetInverseUsingGaussianLUP_Pivot(Matrix<T> &m, Matrix<T> &minv, Vector<int> &p, T ep)
{
	// must be a square matrix
	MustBeTrue(m.getRows() == m.getCols() && m.getRows() > 0);
	MustBeTrue(minv.getRows() == minv.getCols() && minv.getRows() > 0);
	MustBeTrue(minv.getRows() == m.getCols());

	// get number of rows and columns
	int max = m.getRows();

	// update inverse matrix
	for (int i = 0; i < max; i++)
	{
		// initialize column vector
		Vector<T> y(max);
		int j;
		for (j = 0; j < max; j++)
		{
			y[j] = 0;
		}
		y[i] = 1;

		// solve for corresponding vector in inverse
		Vector<T> x(max);
		if (SolveUsingGaussianLUP_Pivot(m, x, y, p, ep) != OK)
			return(NOTOK);

		// transfer results to inverse matrix
		for (j = 0; j < max; j++)
		{
			minv(j, i) = x[j];
		}
	}

	// all done
	return(OK);
}

//
// given a gaussian LUP decomposition of a matrix,
// calculate the determinant of the original matrix.
//
template <class T>
int
GetDeterminantUsingGaussianLUP_Pivot(Matrix<T> &m, T &d)
{
	// must be a square matrix
	MustBeTrue(m.getRows() == m.getCols() && m.getRows() > 0);

	// get number of rows and columns
	int max = m.getRows();

	// get determinant
	for (int i = 0; i < max; i++)
	{
		CheckForOverFlow(d, m(i, i));
		d = d*m(i, i);
	}

	// all done
	return(OK);
}

}
