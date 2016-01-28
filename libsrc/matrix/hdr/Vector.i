//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// vector class functions

namespace ombt {

// local functions
long double
conj(const long double &d)
{
	return(d);
}

double
conj(const double &d)
{
	return(d);
}

float
conj(const float &f)
{
	return(f);
}

// constructors and destructor
template <class T>
Vector<T>::Vector(unsigned int argd):
	dimension(argd), vector(new T [argd])
{
	// check dimension and vector
	MustBeTrue(dimension > 0);
	MustBeTrue(vector != NULL);

	// initialize to zero
	for (int id = 0; id < dimension; id++)
	{
		vector[id] = 0;
	}
}

template <class T>
Vector<T>::Vector(const T *argn, unsigned int argd):
	dimension(argd), vector(new T [argd])
{
	// check dimension and vector
	MustBeTrue(dimension > 0);
	MustBeTrue(vector != NULL);

	// copy coordinates
	MustBeTrue(argn != NULL);
	for (int id = 0; id < dimension; id++)
	{
		vector[id] = argn[id];
	}
}

template <class T>
Vector<T>::Vector(const Vector<T> &argv):
	dimension(argv.dimension), vector(new T [argv.dimension])
{
	// check dimension and vector
	MustBeTrue(dimension > 0);
	MustBeTrue(vector != NULL);

	// copy coordinates
	for (unsigned int id = 0; id < dimension; id++)
	{
		vector[id] = argv.vector[id];
	}
}

template <class T>
Vector<T>::~Vector()
{
	// delete vector
	delete [] vector;
	vector = NULL;
	dimension = 0;
}

// assignment 
template <class T>
Vector<T> &
Vector<T>::operator=(const Vector<T> &v)
{
	// check if assigning to itself
	if (this == &v) return(*this);

	// clear vector
	delete [] vector;

	// store vctor dimension
	dimension = v.dimension;
	MustBeTrue(dimension > 0);

	// allocate a vector
	vector = new T [dimension];
	MustBeTrue(vector != NULL);

	// copy vector to new vector
	for (int ic = 0; ic < dimension; ic++)
	{
		vector[ic] = v.vector[ic];
	}

	// all done
	return(*this);
}

template <class T>
T &
Vector<T>::operator[](unsigned int ic)
{
	// check index
	MustBeTrue(0 <= ic && ic < dimension);

	// return coordinate
	return(vector[ic]);
}

template <class T>
T &
Vector<T>::operator[](unsigned int ic) const
{
	// check index
	MustBeTrue(0 <= ic && ic < dimension);

	// return coordinate
	return(vector[ic]);
}

// vector operations
template <class T>
Vector<T> &
Vector<T>::operator+=(const Vector<T> &v)
{
	// check dimension
	MustBeTrue(dimension == v.dimension);

	// add vector
	for (int id = 0; id < dimension; id++)
	{
		vector[id] += v.vector[id];
	}

	// all done
	return(*this);
}

template <class T>
Vector<T> &
Vector<T>::operator-=(const Vector<T> &v)
{
	// check dimension
	MustBeTrue(dimension == v.dimension);

	// add vector
	for (int id = 0; id < dimension; id++)
	{
		vector[id] -= v.vector[id];
	}

	// all done
	return(*this);
}

template <class T>
Vector<T>
Vector<T>::operator+(const Vector<T> &v) const
{
	// add vectors
	return(Vector<T>(*this) += v);
}

template <class T>
Vector<T>
Vector<T>::operator-(const Vector<T> &v) const
{
	// subtract vectors
	return(Vector<T>(*this) -= v);
}

// vector and scalar operations
template <class T>
Vector<T> &
Vector<T>::operator*=(const T &n)
{
	// check dimension
	MustBeTrue(dimension > 0);

	// multiply vector by a scalar
	for (int ic = 0; ic < dimension; ic++)
	{
		CheckForOverFlow(vector[ic], n);
		vector[ic] *= n;
	}

	// all done
	return(*this);
}

template <class T>
Vector<T> &
Vector<T>::operator/=(const T &n)
{
	// check dimension and scalar
	MustBeTrue(dimension > 0);
	MustBeTrue(n != 0.0);

	// divide vector by a scalar
	for (int ic = 0; ic < dimension; ic++)
	{
		vector[ic] /= n;
	}

	// all done
	return(*this);
}

template <class T>
Vector<T>
Vector<T>::operator*(const T &n) const
{
	// multiply vector by a scalar
	return(Vector<T>(*this) *= n);
}

template <class T>
Vector<T>
Vector<T>::operator/(const T &n) const
{
	// divide vector by a scalar
	return(Vector<T>(*this) /= n);
}

// logical operators
template <class T>
int
Vector<T>::operator==(const Vector<T> &v) const
{
	// check dimension
	MustBeTrue(dimension > 0);

	// check if vectors are equal
	unsigned int id;
	if (v.dimension < dimension)
	{
		for (id = 0; id < v.dimension; id++)
		{
			if (v[id] != vector[id]) return(0);
		}
		for ( ; id < dimension; id++)
		{
			if (vector[id] != 0.0) return(0);
		}
	}
	else
	{
		for (id = 0; id < dimension; id++)
		{
			if (v[id] != vector[id]) return(0);
		}
		for ( ; id < v.dimension; id++)
		{
			if (v[id] != 0.0) return(0);
		}
	}

	// vectors are equal
	return(1);
}

template <class T>
int
Vector<T>::operator!=(const Vector<T> &v) const
{
	return( ! (*this== v));
}

// vector products
template <class T>
Vector<T>
conj(const Vector<T> &v)
{
	// check dimension
	MustBeTrue(v.dimension > 0);

	// copy vector and conjugate
	Vector<T> tmp(v.dimension);
	for (int id = 0; id < v.dimension; id++)
	{
		tmp[id] = conj(v[id]);
	}

	// all done
	return(tmp);
}

template <class T>
T
dot(const Vector<T> &v1, const Vector<T> &v2)
{
	// check dimensions
	MustBeTrue(v1.dimension > 0 && v2.dimension > 0);

	// calculate dot product
	T sum = 0.0;
	unsigned int maxd = 
		(v1.dimension < v2.dimension) ? v1.dimension : v2.dimension;
	for (int id = 0; id < maxd; id++)
	{
		CheckForOverFlow(v1[id], conj(v2[id]));
		sum += v1[id]*conj(v2[id]);
	}

	// return the sum
	return(sum);
}

template <class T>
T
norm(const Vector<T> &v)
{
	// return vector magnitudes
	return(sqrt(dot(v, v)));
}

// print vector
template <class T>
void
Vector<T>::dump(std::ostream &os) const
{
	os << "{";
	for (int ic = 0; ic < dimension; )
	{
		os << vector[ic];
		if (++ic != dimension) os << ",";
	}
	os << "}";
	return;
}

}
