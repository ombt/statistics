//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// solution of tridiagonal band matrix

namespace ombt {

// thomas algorithm 
//
// original matrix:
//
// | b(1) c(1)                       0      | |x(1)|   |d(1)|
// | a(2) b(2) c(2)                         | |x(2)|   |d(2)|
// |      a(3) b(3) c(3)                    | |x(3)|   |d(3)|
// |      .    .    .                       | | .  | = | .  |
// |           .    .    .                  | | .  |   | .  |
// |                .    .    .             | | .  |   | .  |
// |                                 c(n-1) | |    |   |    |
// | 0                        a(n-1) b(n)   | |x(n)|   |d(n)|
//
// reduction:
//
// c'(i) = c(1)/b(1)                for i=1
//       = c(i)/(b(i)-c'(i-1)*a(i)) for i=2,3,...,n-1
// 
// d'(i) = d(1)/b(1) for i=1                       for i=1
//       = (d(i)-d'(i-1)*a(i))/(b(i)-c'(i-1)*a(i)) for i=2,3,...,n
//
// solution is given by:
// 
// x(n) = d'(n)              for i=n
// x(i) = d'(i)-c'(i)*x(i+1) for i=n-1,n-2,...,1

template <class T>
int
SolveTriDiagonal(Matrix<T> &m, Vector<T> &x, Vector<T> &d, T &ep)
{
	// must be a square matrix
	MustBeTrue(m.getRows() == m.getCols() && m.getRows() > 0);

	// check epsilon, set if invalid
	T minep = calcEpsilon(T(0));
	if ((ep = fabs(ep)) < minep)
		ep = minep;

	// get number of rows and columns -- square matrix
	int max = m.getRows();

	// project matrix to virtual vectors
	typename TriDiagonalMatrix<T>::A a(m);
	typename TriDiagonalMatrix<T>::B b(m);
	typename TriDiagonalMatrix<T>::C c(m);

	c[0] = c[0]/b[0];
	for (int i=1; i<(max-1); ++i)
	{
		c[i] /= (b[i]-c[i-1]*a[i]);
	}

	// generate new 'd' particular values
	d[0] = d[0]/b[0];
	for (int i=1; i<max; ++i)
	{
		d[i] = (d[i]-d[i-1]*a[i])/(b[i]-c[i-1]*a[i]);
	}

	// solve
	x[max-1] = d[max-1];
	for (int i=max-2; i>=0; --i)
	{
		x[i] = d[i]-c[i]*x[i+1];
	}

	// all done
	return(0);
}

}
