//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// member functions for generic string class

// local definitions
#define DefaultGStringSize 32

// generic string constructors and destructor
template <class DataType>
GString<DataType>::GString():
	size(0), maxsize(DefaultGStringSize), 
	buffer(new DataType [DefaultGStringSize])
{
	// check if buffer was allocated
        MustBeTrue(buffer != NULL);
}

template <class DataType>
GString<DataType>::GString(DataType c):
	size(1), maxsize(DefaultGStringSize), 
	buffer(new DataType [DefaultGStringSize])
{
	// check if buffer was allocated
        MustBeTrue(buffer != NULL);

	// store character 
	buffer[0] = c;
}

template <class DataType>
GString<DataType>::GString(int wsz, const DataType *pw):
	size(wsz), maxsize(wsz), buffer(new DataType [wsz])
{
	// check for invalid pointer
	MustBeTrue(buffer != NULL && maxsize > 0);

	// copy over data
	if (pw != NULL)
	{
		for (int i = 0; i < size; i++)
		{
			buffer[i] = pw[i];
		}
	}
}

template <class DataType>
GString<DataType>::GString(const GString<DataType> &w):
	size(w.size), maxsize(w.maxsize), 
	buffer(new DataType [w.maxsize])
{
	// check if buffer was allocated
        MustBeTrue(maxsize > 0 && buffer != NULL);

	// copy string
	wordncpy(buffer, w.buffer, size);
}

template <class DataType>
GString<DataType>::~GString()
{
	// clear everything
	delete [] buffer;
	maxsize = size = 0;
}

// assignment operators
template <class DataType>
GString<DataType> &
GString<DataType>::operator=(const GString<DataType> &rhs)
{
	// check for self-assignment
	if (this == &rhs) return(*this);

	// is buffer long enough
	if (rhs.size > maxsize)
	{
		// delete old buffer and allocate new one
		delete [] buffer;
		maxsize = rhs.maxsize;
        	MustBeTrue(maxsize >= 0);
		buffer = new DataType [maxsize];
        	MustBeTrue(buffer != NULL);
	}

	// copy string
	size = rhs.size;
	wordncpy(buffer, rhs.buffer, size);

	// return modified string
	return(*this);
}

// access operators
template <class DataType>
DataType &
GString<DataType>::operator[](int idx)
{
	MustBeTrue(0 <= idx && idx < maxsize);
	if ((idx+1) > size)
		size = idx+1;
	return(buffer[idx]);
}

template <class DataType>
const DataType &
GString<DataType>::operator[](int idx) const
{
	MustBeTrue(0 <= idx && idx < size);
	return(buffer[idx]);
}

// logical operators
template <class DataType>
int
GString<DataType>::operator==(const GString<DataType> &rhs) const
{
	return(wordcmp(rhs) == 0);
}

template <class DataType>
int
GString<DataType>::operator!=(const GString<DataType> &rhs) const
{
	return(wordcmp(rhs) != 0);
}

template <class DataType>
int
GString<DataType>::operator<(const GString<DataType> &rhs) const
{
	return(wordcmp(rhs) < 0);
}

template <class DataType>
int
GString<DataType>::operator<=(const GString<DataType> &rhs) const
{
	return(wordcmp(rhs) <= 0);
}

template <class DataType>
int
GString<DataType>::operator>(const GString<DataType> &rhs) const
{
	return(wordcmp(rhs) > 0);
}

template <class DataType>
int
GString<DataType>::operator>=(const GString<DataType> &rhs) const
{
	return(wordcmp(rhs) >= 0);
}

template <class DataType>
int
GString<DataType>::wordcmp(const GString<DataType> &w) const
{
	// check for self comparisons
	if (this == &w)
		return(0);

	// now compare the strings
	int imax = (size < w.size) ? size : w.size;
	for (int i = 0; i < imax; i++)
	{
		// check if equal
		if (buffer[i] != w.buffer[i])
		{
			// not equal, must < or >
			if (buffer[i] < w.buffer[i])
				return(-1);
			else
				return(1);
		}
	}

	// one is a prefix of the other or the same
	return(size - w.size);
}

// substring operator
template <class DataType>
GString<DataType>
GString<DataType>::operator()(int start) const
{
	return((*this)(start, maxsize));
}

template <class DataType>
GString<DataType>
GString<DataType>::operator()(int start, int ncpy) const
{
	// check start of substring.
	MustBeTrue(0 <= start && start < size);
	MustBeTrue(0 <= ncpy);

	// calculate length of string to copy
	if ((start+ncpy) > maxsize)
		ncpy = maxsize - start;
	if (ncpy < 0)
		ncpy = 0;

	// copy substring
	GString<DataType> substr(ncpy+1);
	wordncpy(substr.buffer, buffer+start, ncpy);
	substr.size = ncpy;

	// return substring
	return(substr);
}

// copy one string to another
template <class DataType>
void 
GString<DataType>::wordncpy(DataType *pwb, const DataType *pwb2, int iwmax) const
{
	for (int iw = 0; iw < iwmax; iw++)
	{
		*(pwb + iw) = *(pwb2 + iw);
	}
	return;
}

// string length
template <class DataType>
int
GString<DataType>::gstrlen() const
{
	return(size);
}

// casting string to character array
template <class DataType>
GString<DataType>::operator const DataType *() const
{
	return(buffer);
}

template <class DataType>
GString<DataType>::operator DataType *()
{
	return(buffer);
}

// input and output
template <class DataType>
std::ostream &
operator<<(std::ostream &os, const GString<DataType> &s)
{
	MustBeTrue(s.buffer != NULL);
	for (int i = 0; i < s.size; i++)
	{
		os << s.buffer[i];
	}
	return(os);
}
 

// concatenation operators
template <class DataType>
GString<DataType> &
GString<DataType>::operator+=(const GString<DataType> &rhs)
{
	// check if current buffer is long enough for new buffer
	int newsize = size + rhs.size;
	if (newsize > maxsize)
	{
		// save the old buffer for now
		DataType *pobuf = buffer;

		// allocate a new buffer
		buffer = new DataType [newsize];
		MustBeTrue(buffer != NULL);

		// copy old buffer
		wordncpy(buffer, pobuf, size);

		// delete old buffer
		delete [] pobuf;

		// set new maxsize
		maxsize = newsize;
	}

	// copy buffer at the end of the current buffer
	wordncpy(buffer+size, rhs.buffer, rhs.size);

	// new size of string
	size = newsize;

	// return new string
	return(*this);
}

template <class DataType>
GString<DataType>
GString<DataType>::operator+(const GString<DataType> &rhs) const
{
	return(GString<DataType>(*this) += rhs);
}

