//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// matrix class functions

namespace ombt {

// local definitions
#define local_abs(x) (((x) < 0) ? (-(x)) : (x))

// constructors and destructor
template <class T>
Matrix<T>::Matrix(unsigned int rows, unsigned int cols):
	matrix(NULL), nrows(rows), ncols(cols), epsilon(0)
{
	// check dimensions
	MustBeTrue(nrows > 0 && ncols > 0);

	// allocate a matrix
	matrix = new T [nrows*ncols];
	MustBeTrue(matrix != NULL);
	for (unsigned int ia = 0; ia < nrows*ncols; ia++)
	{
		(*this)[ia] = 0;
	}

	// get epsilon
	epsilon = calcEpsilon((T)(0.0));
}

template <class T>
Matrix<T>::Matrix(unsigned int rows, unsigned int cols, const T *vals):
	matrix(NULL), nrows(rows), ncols(cols), epsilon(0)
{
	// check dimensions
	MustBeTrue(nrows > 0 && ncols > 0);
	MustBeTrue(vals != NULL);

	// allocate a matrix
	matrix = new T [nrows*ncols];
	MustBeTrue(matrix != NULL);
	for (unsigned int ia = 0; ia < nrows*ncols; ia++)
	{
		(*this)[ia] = vals[ia];
	}

	// get epsilon
	epsilon = calcEpsilon((T)(0.0));
}

template <class T>
Matrix<T>::Matrix(const Matrix<T> &m):
	matrix(NULL), nrows(m.nrows), ncols(m.ncols), epsilon(m.epsilon)
{
	// store dimensions
	MustBeTrue(nrows > 0 && ncols > 0);

	// allocate a matrix
	matrix = new T [nrows*ncols];
	MustBeTrue(matrix != NULL);
	for (unsigned int ia = 0; ia < nrows*ncols; ia++)
	{
		(*this)[ia] = m[ia];
	}
}

template <class T>
Matrix<T>::~Matrix()
{
	delete [] matrix;
	matrix = NULL;
}

// assignment operations and access functions
template <class T>
Matrix<T> &
Matrix<T>::operator=(const Matrix<T> &m)
{
	// check if assigning to itself
	if (this != &m)
	{
		// delete matrix
		delete [] matrix;
		matrix = NULL;

		// store new matrix dimension
		nrows = m.nrows;
		ncols = m.ncols;
		MustBeTrue(nrows > 0 && ncols > 0);

		// allocate a matrix
		matrix = new T [nrows*ncols];
		MustBeTrue(matrix != NULL);
		for (unsigned int ia = 0; ia < nrows*ncols; ia++)
		{
			(*this)[ia] = m[ia];
		}

		// everything else
		epsilon = m.epsilon;
	}
	return(*this);
}

template <class T>
T &
Matrix<T>::operator[](unsigned int idx)
{
	MustBeTrue(idx < (nrows*ncols));
	return(matrix[idx]);
}

template <class T>
T &
Matrix<T>::operator[](unsigned int idx) const
{
	MustBeTrue(idx < (nrows*ncols));
	return(matrix[idx]);
}

template <class T>
T &
Matrix<T>::operator()(unsigned int row, unsigned int col)
{
	MustBeTrue(row < nrows && col < ncols);
	MustBeTrue((row*ncols+col) < (nrows*ncols));
	return(matrix[row*ncols+col]);
}

template <class T>
T &
Matrix<T>::operator()(unsigned int row, unsigned int col) const
{
	MustBeTrue(row < nrows && col < ncols);
	MustBeTrue((row*ncols+col) < (nrows*ncols));
	return(matrix[row*ncols+col]);
}

// matrix operations
template <class T>
Matrix<T> &
Matrix<T>::operator+=(const Matrix<T> &m)
{
	// check that rows and columns match
	MustBeTrue(nrows == m.nrows && ncols == m.ncols);

	// add element by element
	for (unsigned int ia = 0; ia < nrows*ncols; ia++)
	{
		(*this)[ia] += m[ia];
	}

	// all done
	return(*this);
}

template <class T>
Matrix<T> &
Matrix<T>::operator-=(const Matrix<T> &m)
{
	// check that rows and columns match
	MustBeTrue(nrows == m.nrows && ncols == m.ncols);

	// subtract element by element
	for (unsigned int ia = 0; ia < nrows*ncols; ia++)
	{
		(*this)[ia] -= m[ia];
	}

	// all done
	return(*this);
}

template <class T>
Matrix<T> &
Matrix<T>::operator*=(const Matrix<T> &m)
{
	// check that rows and columns match
	MustBeTrue(ncols == m.nrows && ncols > 0);

	// get size of new matrix
	unsigned int newnrows = nrows;
	unsigned int newncols = m.ncols;
	MustBeTrue(newnrows > 0 && newncols > 0);
	unsigned int nsum = ncols;

	// allocate a new matrix
	T *newmatrix = new T [newnrows*newncols];
	MustBeTrue(newmatrix != NULL);

	// multiply element by element
	for (unsigned int ir = 0; ir < newnrows; ir++)
	{
		for (unsigned int ic = 0; ic < newncols; ic++)
		{
#ifdef SORTANDADD
			Vector<T> x(nsum), y(nsum);
			for (unsigned int is = 0; is < nsum; is++)
			{
				x[is] = (*this)(ir,is);
				y[is] = m(is,ic);
			}
			MustBeTrue(ir < newnrows && ic < newncols);
			MustBeTrue((ir*newncols+ic) < (newnrows*newncols));
			newmatrix[ir*newncols+ic] = sortAndAdd(x, y, nsum);
#else
			newmatrix[ir*newncols+ic] = 0;
			for (unsigned int is = 0; is < nsum; is++)
			{
				CheckForOverFlow((*this)(ir,is), m(is,ic));
				MustBeTrue(ir < newnrows && ic < newncols);
				MustBeTrue((ir*newncols+ic) < (newnrows*newncols));
				newmatrix[ir*newncols+ic] += (*this)(ir,is)*m(is,ic);
			}
#endif
		}
	}

	// delete old matrix and save new one
	delete [] matrix;
	matrix = newmatrix;
	nrows = newnrows;
	ncols = newncols;

	// all done
	return(*this);
}

template <class T>
Matrix<T>
Matrix<T>::operator+(const Matrix<T> &m) const
{
	return(Matrix<T>(*this) += m);
}

template <class T>
Matrix<T>
Matrix<T>::operator-(const Matrix<T> &m) const
{
	return(Matrix<T>(*this) -= m);
}

template <class T>
Matrix<T>
Matrix<T>::operator*(const Matrix<T> &m) const
{
	return(Matrix<T>(*this) *= m);
}

// matrix and vector operations
template <class T>
Vector<T>
Matrix<T>::operator*(const Vector<T> &v) const
{
	// check that rows and columns match
	MustBeTrue(ncols == v.getDimension() && ncols > 0);

	// new vector to hold results
	Vector<T> newv(nrows);

	// multiply element by element
	for (unsigned int ir = 0; ir < nrows; ir++)
	{
#ifdef SORTANDADD
		Vector<T> x(ncols);
		for (unsigned int is = 0; is < ncols; is++)
		{
			x[is] = (*this)(ir,is);
		}
		newv[ir] = sortAndAdd(v, x, ncols);
#else
		newv[ir] = 0;
		for (unsigned int is = 0; is < ncols; is++)
		{
			CheckForOverFlow((*this)(ir,is), v[is]);
			newv[ir] += (*this)(ir,is)*v[is];
		}
#endif
	}

	// all done
	return(newv);
}

template <class T>
Vector<T>
operator*(const Vector<T> &v, const Matrix<T> &m)
{
	// check that rows and columns match
	MustBeTrue(v.getDimension() == m.nrows && m.nrows > 0);

	// new vector to hold results
	Vector<T> newv(m.ncols);

	// multiply element by element
	for (unsigned int ic = 0; ic < m.ncols; ic++)
	{
#ifdef SORTANDADD
		Vector<T> x(m.nrows);
		for (unsigned int is = 0; is < m.nrows; is++)
		{
			x[is] = m(is, ic);
		}
		newv[ic] = sortAndAdd(v, x, m.nrows);
#else
		newv[ic] = 0;
		for (unsigned int is = 0; is < m.nrows; is++)
		{
			CheckForOverFlow(m(is,ic), v[is]);
			newv[ic] += v[is]*m(is,ic);
		}
#endif
	}

	// all done
	return(newv);
}

// matrix and scalar operations
template <class T>
Matrix<T> &
Matrix<T>::operator*=(const T &n)
{
	// check dimensions
	MustBeTrue(nrows > 0 && ncols > 0);

	// multiply matrix by scalar
	for (unsigned int ia = 0; ia < nrows*ncols; ia++)
	{
		CheckForOverFlow((*this)[ia], n);
		(*this)[ia] *= n;
	}

	// all done
	return(*this);
}

template <class T>
Matrix<T> &
Matrix<T>::operator/=(const T &n)
{
	// check dimensions
	MustBeTrue(nrows > 0 && ncols > 0);
	MustBeTrue(n != 0.0);

	// divide matrix by scalar
	for (unsigned int ia = 0; ia < nrows*ncols; ia++)
	{
		(*this)[ia] /= n;
	}

	// all done 
	return(*this);
}


template <class T>
Matrix<T>
Matrix<T>::operator*(const T &n) const
{
	return(Matrix<T>(*this) *= n);
}

template <class T>
Matrix<T>
Matrix<T>::operator/(const T &n) const
{
	return(Matrix<T>(*this) /= n);
}

template <class T>
Matrix<T>
operator*(const T &n, const Matrix<T> &m)
{
	// switch around and multiply
	return(m*n);
}

// logical operators
template <class T>
int
Matrix<T>::operator==(const Matrix<T> &m) const
{
	// check if the same matrix
	if (this == &m) return(1);

	// check if dimensions are the same
	if (nrows != m.nrows || ncols != m.ncols) return(0);

	// compare element by element
	for (unsigned int ia = 0; ia < nrows*ncols; ia++)
	{
		T delta = (*this)[ia] - m[ia];
		if (delta < 0.0)
			delta = -1.0*delta;
		if (delta > epsilon)
		{      
			// a mismatch
			return(0);
		}
	}

	// matrices are the same
	return(1);
}

template <class T>
int
Matrix<T>::operator!=(const Matrix<T> &m) const
{
	return( ! (*this == m));
}

// print matrix
template <class T>
void
Matrix<T>::dump(std::ostream &os) const
{
	os << "matrix[" << nrows << "," << ncols << "] = {" << std::endl;
	for (unsigned int ir = 0; ir < nrows; ir++)
	{
		for (unsigned int ic = 0; ic < ncols; ic++)
		{
			os << (*this)(ir,ic) << " ";
		}
		os << std::endl;
	}
	os << "}" << std::endl;
	return;
}

template <class T>
std::ostream &
operator<<(std::ostream &os, const Matrix<T> &m)
{
	m.dump(os);
	return(os);
}

#ifdef SORTANDADD
// sort two arrays in ascending order to minimized truncation errors.
template <class T>
T
Matrix<T>::sortAndAdd(const Vector<T> &x, const Vector<T> &y, unsigned int howmany) const
{
	// sanity check
	MustBeTrue((x.getDimension() == y.getDimension()) && (x.getDimension() > 0));

	// multiply results and sort
	Vector<T> prods(howmany);
	int i;
	for (i = 0; i < howmany; i++)
	{
		CheckForOverFlow(x[i], y[i]);
		prods[i] = x[i]*y[i];
	}

	// sort array in ascending order. for now use
	// an O(N*N) sort routine. later switch to 
	// a more efficient O(N*logN) routine.
	for (unsigned int j = 0; j < howmany; j++)
	{
		int jmin = j;
		for (unsigned int k = j+1; k < howmany; k++)
		{
			if (prods[k] < prods[jmin])
				jmin = k;
		}
		if (jmin != j)
		{
			T tmp = prods[j];
			prods[j] = prods[jmin];
			prods[jmin] = tmp;
		}
	}

	// find out where negatives and positives start
	unsigned int firstpos;
	for (firstpos = 0; prods[firstpos] < 0; firstpos++) ;

	// add up the negatives and positives separately
	T possum = 0;
	for (i = firstpos; i < howmany; i++)
	{
		possum += prods[i];
	}
	T negsum = 0;
	for (i = firstpos-1; i >= 0; i--)
	{
		negsum += prods[i];
	}

	// now add up and return
	return(possum+negsum);
}
#endif

}
