//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// base-N integer

// constructors and destructor
template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>::UnsignedInteger():
	base_(0), maxdigits_(MD), ndigits_(0), n_()
{
	normalize(MD);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>::UnsignedInteger(DT value):
	base_(0), maxdigits_(MD), ndigits_(0), n_()
{
	normalize(MD, value);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>::UnsignedInteger(const NT &n):
	base_(0), maxdigits_(MD), ndigits_(0), n_()
{
	normalize(n, MD);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>::UnsignedInteger(
	const UnsignedInteger<MD, DT, NT> &n): 
	base_(n.base_), maxdigits_(n.maxdigits_), 
	ndigits_(n.ndigits_), n_()
{
	normalize(n);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>::UnsignedInteger(const std::string &cn):
	base_(0), maxdigits_(MD), ndigits_(0), n_()
{
	normalize(MD, cn);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>::~UnsignedInteger() 
{
}

// normalize number to standard form
template <int MD, class DT, class NT>
void
UnsignedInteger<MD, DT, NT>::normalizeZero()
{
	for (int i=ndigits_-1; i>=0; --i)
	{
		if (n_[i] == DT(0))
		{
			ndigits_ -= 1;
		}
		else
		{
			return;
		}
	}
	normalize(MD);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::normalize(int maxdigits, DT value)
{
	MustBeTrue((maxdigits_ = maxdigits) > 0);

	base_ = DT(~DT(0));
	ndigits_ = 1;
	n_.resize(maxdigits_);
	n_[0] = value;

	return(*this);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::normalize(int maxdigits, const std::string &cvalue)
{
	if (cvalue.size() == 0) 
		return(normalize(MD, 0));

	MustBeTrue((maxdigits_ = maxdigits) > 0);

        UnsignedInteger<MD, DT, NT> ten(DT(10));
        UnsignedInteger<MD, DT, NT> sum;

	for (int i=0; i<cvalue.size(); ++i)
	{
		sum *= ten;
		sum += DT(cvalue[i] - '0');
	}

	return(normalize(*this = sum));
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::normalize(const NT &n, int maxdigits)
{
	MustBeTrue((maxdigits_ = maxdigits) > 0);
	MustBeTrue((ndigits_ = n.size()) <= maxdigits_);

	n_.resize(maxdigits_);

	base_ = DT(~DT(0));

	for (int i=ndigits_-1; i>=0; --i)
	{
		MustBeTrue(n[i] <= base_);
		n_[i] = n[i];
	}
	normalizeZero();
	return(*this);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::normalize(const UnsignedInteger &n)
{
	if (this == &n)
	{
		MustBeTrue(maxdigits_ > 0);
		MustBeTrue(ndigits_ <= maxdigits_);
	}
	else
	{
		MustBeTrue((maxdigits_ = n.maxdigits_) > 0);
		MustBeTrue((ndigits_ = n.ndigits_) <= maxdigits_);

		n_.resize(maxdigits_);
		base_ = DT(~DT(0));

		for (int i=ndigits_-1; i>=0; --i)
		{
			MustBeTrue(n.n_[i] <= base_);
			n_[i] = n.n_[i];
		}
	}
	normalizeZero();
	return(*this);
}

// print base-n number in raw form
template <int MD, class DT, class NT>
std::ostream &
operator<<(std::ostream &os, const UnsignedInteger<MD, DT, NT> &n)
{
	char comma = '(';
	for (int i=n.ndigits_-1; i>=0; --i)
	{
		os << comma << (unsigned long)n.n_[i];
		comma = ',';
	}
	os << ")";
	return(os);
}

template <int MD, class DT, class NT>
std::string
UnsignedInteger<MD, DT, NT>::toString() const
{
        static const UnsignedInteger<MD, DT, NT> zero(DT(0));
        static const UnsignedInteger<MD, DT, NT> ten(DT(10));

        UnsignedInteger<MD, DT, NT> digit;
        UnsignedInteger<MD, DT, NT> n(*this);


	if (*this == zero)
		return(std::string("0"));

	std::string cn;
	while (n != zero)
	{
		digit = n%ten;
		n /= ten;
		cn.push_back(digit[0]+'0');
	}

	char tmp;
	int min=0;
	int max=cn.size()-1;
	while (min<max)
	{
		char tmp = cn[min];
		cn[min] = cn[max];
		cn[max] = tmp;
		min += 1;
		max -= 1;
	}

	return cn;
}

// access operators
template <int MD, class DT, class NT>
DT &
UnsignedInteger<MD, DT, NT>::operator[](int i)
{
	return(n_[i]);
}

template <int MD, class DT, class NT>
const DT &
UnsignedInteger<MD, DT, NT>::operator[](int i) const
{
	return(n_[i]);
}

template <int MD, class DT, class NT>
int
UnsignedInteger<MD, DT, NT>::numberOfDigits() const
{
	return ndigits_;
}

template <int MD, class DT, class NT>
int
UnsignedInteger<MD, DT, NT>::maxNumberOfDigits() const
{
	return maxdigits_;
}

// arithmetic operators
template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::operator=(const UnsignedInteger<MD, DT, NT> &n)
{
	if (&n == this) return(*this);

	base_ = n.base_;
	maxdigits_ = n.maxdigits_;
	ndigits_ = n.ndigits_;
	n_ = n.n_;

	return(normalize(*this));
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::operator=(const std::string &sn)
{
	return(normalize(maxdigits_, sn));
}

template <int MD, class DT, class NT>
void
UnsignedInteger<MD, DT, NT>::add(int imin, int imax, const NT &dmin, const NT &dmax)
{
	DT carryover = 0;
	for (int i=0; i<imin; ++i)
	{
		DT sum = dmin[i] + dmax[i] + carryover;
		if (sum < dmin[i] && sum < dmax[i])
			carryover = 1;
		else
			carryover = 0;
		n_[i] = sum;
	}
	for (int i=imin; i<imax; ++i)
	{
		DT sum = dmax[i] + carryover;
		if (sum < dmax[i])
			carryover = 1;
		else
			carryover = 0;
		n_[i] = sum;
	}
	ndigits_ = imax;
	if (carryover != DT(0))
	{
		ndigits_ += 1;
		n_[imax] = carryover;
	}
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::operator+=(const UnsignedInteger<MD, DT, NT> &n)
{
	if (ndigits_ < n.ndigits_)
		add(ndigits_, n.ndigits_, n_, n.n_);
	else
		add(n.ndigits_, ndigits_, n.n_, n_);
	return(normalize(*this));
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>
UnsignedInteger<MD, DT, NT>::operator+(const UnsignedInteger<MD, DT, NT> &n) const
{
	return(UnsignedInteger<MD, DT, NT>(*this) += n);
}

template <int MD, class DT, class NT>
void
UnsignedInteger<MD, DT, NT>::subtract(int imin, int imax, const NT &dmin, const NT &dmax)
{
	DT borrow = 0;
	for (int i=0; i<imin; ++i)
	{
		DT difference = 0;
		if (dmax[i] < (dmin[i] + borrow))
		{
			difference = dmax[i] + (base_ - dmin[i]) - borrow + 1;
			borrow = 1;
		}
		else
		{
			difference = dmax[i] - dmin[i] - borrow;
			borrow = 0;
		}
		n_[i] = difference;
	}

	for (int i=imin; i<imax; ++i)
	{
		DT difference = 0;
		if (dmax[i] < borrow)
		{
			difference = dmax[i] + (base_ - borrow) + 1;
			borrow = 1;
		}
		else
		{
			difference = dmax[i] - borrow;
			borrow = 0;
		}
		n_[i] = difference;
	}

	MustBeTrue(borrow == DT(0))

	for (int i=imax-1; i>0; --i)
	{
		if (dmax[i] == DT(0))
			ndigits_ -= 1;
		else
			break;
	}
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::operator-=(const UnsignedInteger<MD, DT, NT> &n)
{
	subtract(n.ndigits_, ndigits_, n.n_, n_);
	return(normalize(*this));
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>
UnsignedInteger<MD, DT, NT>::operator-(const UnsignedInteger<MD, DT, NT> &r) const
{
	return(UnsignedInteger<MD, DT, NT>(*this) -= r);
}

template <int MD, class DT, class NT>
void
UnsignedInteger<MD, DT, NT>::multiply(int imin, DT dmin, int imax, const NT &dmax, UnsignedInteger<MD, DT, NT> &product)
{
	for (int i=0; i<imin; ++i)
	{
		product.n_[i] = DT(0);
	}

	DT v0, v1;
	v0 = (dmin & DigitTypePolicy<DT, sizeof(DT)>::BitMask);
	v1 = (dmin >> DigitTypePolicy<DT, sizeof(DT)>::BitShift);
	v1 &= (DigitTypePolicy<DT, sizeof(DT)>::BitMask);

	DT carryover0 = DT(0);
	DT carryover1 = DT(0);

	for (int i=0; i<imax; ++i)
	{
		DT u0, u1;
		u0 = (dmax[i] & DigitTypePolicy<DT, sizeof(DT)>::BitMask);
		u1 = (dmax[i] >> DigitTypePolicy<DT, sizeof(DT)>::BitShift);
		u1 &= (DigitTypePolicy<DT, sizeof(DT)>::BitMask);

		DT c1, c2, x0, x1, x2;
		x0 = (v0*u0+carryover0)%DigitTypePolicy<DT, sizeof(DT)>::Base;
		c1 = (v0*u0+carryover0)/DigitTypePolicy<DT, sizeof(DT)>::Base;
		x1 = (v0*u1+c1+carryover1)%DigitTypePolicy<DT, sizeof(DT)>::Base;
		c2 = (v0*u1+c1+carryover1)/DigitTypePolicy<DT, sizeof(DT)>::Base;
		x2 = c2;

		DT c3, y0, y1, y2, y3;
		y0 = DT(0);
		y1 = (v1*u0)%DigitTypePolicy<DT, sizeof(DT)>::Base;
		c2 = (v1*u0)/DigitTypePolicy<DT, sizeof(DT)>::Base;
		y2 = (v1*u1+c2)%DigitTypePolicy<DT, sizeof(DT)>::Base;
		c3 = (v1*u1+c2)/DigitTypePolicy<DT, sizeof(DT)>::Base;
		y3 = c3;

		DT w0, w1, w2, w3;
		w0 = x0;
		w1 = (x1+y1)%DigitTypePolicy<DT, sizeof(DT)>::Base;
		c2 = (x1+y1)/DigitTypePolicy<DT, sizeof(DT)>::Base;
		w2 = (x2+y2+c2)%DigitTypePolicy<DT, sizeof(DT)>::Base;
		c3 = (x2+y2+c2)/DigitTypePolicy<DT, sizeof(DT)>::Base;
		w3 = (y3+c3)%DigitTypePolicy<DT, sizeof(DT)>::Base;

		carryover0 = w2;
		carryover1 = w3;

		product.n_[imin+i] = w0 + (w1 << DigitTypePolicy<DT, sizeof(DT)>::BitShift);
	}
	// product.n_[imin+imax] = carryover0 + (carryover1 << DigitTypePolicy<DT, sizeof(DT)>::BitShift);
	product.n_[imin+imax] = 0;
	product.n_[imin+imax] |= carryover0;
	product.n_[imin+imax] |= (carryover1 << DigitTypePolicy<DT, sizeof(DT)>::BitShift);

	product.ndigits_ = imin+imax;
	if (product.n_[imin+imax] != DT(0))
		product.ndigits_ += 1;
}

template <int MD, class DT, class NT>
void
UnsignedInteger<MD, DT, NT>::multiply(int imin, int imax, const NT &dmin, const NT &dmax)
{
	UnsignedInteger<MD, DT, NT> sum;
	for (int i=0; i<imin; ++i)
	{
		UnsignedInteger<MD, DT, NT> product;
		multiply(i, dmin[i], imax, dmax, product);
		sum += product;
	}
	*this = sum;
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::operator*=(const UnsignedInteger<MD, DT, NT> &r)
{
	if (ndigits_ < r.ndigits_)
		multiply(ndigits_, r.ndigits_, n_, r.n_);
	else
		multiply(r.ndigits_, ndigits_, r.n_, n_);
	return(normalize(*this));
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>
UnsignedInteger<MD, DT, NT>::operator*(const UnsignedInteger<MD, DT, NT> &r) const
{
	return(UnsignedInteger<MD, DT, NT>(*this) *= r);
}

template <int MD, class DT, class NT>
void
UnsignedInteger<MD, DT, NT>::divide(int imin, int imax, const NT &dmin, const NT &dmax)
{
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::operator/=(const UnsignedInteger<MD, DT, NT> &r)
{
	static const UnsignedInteger<MD, DT, NT> zero(DT(0));
	static const UnsignedInteger<MD, DT, NT> one(DT(1));
	static const UnsignedInteger<MD, DT, NT> maxdigit(DigitTypePolicy<DT, sizeof(DT)>::MaxDigit);

	MustBeTrue(r != zero);

	if (*this < r)
	{
		return(normalize(zero));
	}

	int newndigits = 0;
	DT maxpower = ndigits_ - r.ndigits_;
	NT finalquotient(maxpower+1);

	for (int i=maxpower; i>=0; --i)
	{
		NT quotient(i+1);
		for (int j=0; j<=i; ++j)
		{
			quotient[j] = DT(0);
		}
		DT qmin = DT(0);
		DT qmax = DigitTypePolicy<DT, sizeof(DT)>::FullMaxDigit;

		while (qmin<=qmax)
		{
			quotient[i] = (qmax+qmin)/2;
			if (r*quotient < *this)
			{
				qmin = (qmax+qmin)/2 + 1;
			}
			else if (*this < r*quotient)
			{
				qmax = (qmax+qmin)/2 - 1;
			}
			else
			{
				break;
			}
		}
		if (qmin>=qmax)
		{
			quotient[i] = qmax;
		}

		*this -= r*quotient;
		finalquotient[i] = quotient[i];
		if (newndigits == 0 && finalquotient[i] != DT(0))
		{
			newndigits = i+1;
		}
	}

	if (newndigits == 0)
	{
		return(normalize(MD, DT(0)));
	}
	else
	{
		*this = finalquotient;
		ndigits_ = newndigits;
		return(normalize(*this));
	}
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>
UnsignedInteger<MD, DT, NT>::operator/(const UnsignedInteger<MD, DT, NT> &r) const
{
	return(UnsignedInteger<MD, DT, NT>(*this) /= r);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::operator%=(const UnsignedInteger<MD, DT, NT> &r)
{
	return(*this -= (*this/r)*r);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>
UnsignedInteger<MD, DT, NT>::operator%(const UnsignedInteger<MD, DT, NT> &r) const
{
	return(UnsignedInteger<MD, DT, NT>(*this) %= r);
}

// logical operators
template <int MD, class DT, class NT>
int
UnsignedInteger<MD, DT, NT>::compare(const UnsignedInteger<MD, DT, NT> &r) const
{
	if (ndigits_ < r.ndigits_)
		return -1;
	else if (ndigits_ > r.ndigits_)
		return 1;
	else
	{
		for (int i=ndigits_-1; i>=0; --i)
		{
			if (n_[i] == r.n_[i])
				continue;
			else if (n_[i] < r.n_[i])
				return -1;
			else
				return 1;
		}
		return 0;
	}
}

template <int MD, class DT, class NT>
bool
UnsignedInteger<MD, DT, NT>::operator==(const UnsignedInteger<MD, DT, NT> &r) const
{
	return(compare(r) == 0);
}

template <int MD, class DT, class NT>
bool
UnsignedInteger<MD, DT, NT>::operator!=(const UnsignedInteger<MD, DT, NT> &r) const
{
	return(compare(r) != 0);
}

template <int MD, class DT, class NT>
bool
UnsignedInteger<MD, DT, NT>::operator<(const UnsignedInteger<MD, DT, NT> &r) const
{
	return(compare(r) < 0);
}

template <int MD, class DT, class NT>
bool
UnsignedInteger<MD, DT, NT>::operator>(const UnsignedInteger<MD, DT, NT> &r) const
{
	return(compare(r) > 0);
}

template <int MD, class DT, class NT>
bool
UnsignedInteger<MD, DT, NT>::operator<=(const UnsignedInteger<MD, DT, NT> &r) const
{
	return(compare(r) <= 0);
}

template <int MD, class DT, class NT>
bool
UnsignedInteger<MD, DT, NT>::operator>=(const UnsignedInteger<MD, DT, NT> &r) const
{
	return(compare(r) >= 0);
}

// arithmetic operators
template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::operator=(NT r)
{
	return(normalize(UnsignedInteger<MD, DT, NT>(r)));
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::operator+=(NT r)
{
	return(normalize(*this += UnsignedInteger<MD, DT, NT>(r)));
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>
UnsignedInteger<MD, DT, NT>::operator+(NT r) const
{
	return(UnsignedInteger<MD, DT, NT>(*this) += r);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::operator-=(NT r)
{
	return(normalize(*this -= UnsignedInteger<MD, DT, NT>(r)));
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>
UnsignedInteger<MD, DT, NT>::operator-(NT r) const
{
	return(UnsignedInteger<MD, DT, NT>(*this) -= r);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::operator*=(NT r)
{
	return(normalize(*this *= UnsignedInteger<MD, DT, NT>(r)));
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>
UnsignedInteger<MD, DT, NT>::operator*(NT r) const
{
	return(UnsignedInteger<MD, DT, NT>(*this) *= r);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::operator/=(NT r)
{
	return(normalize(*this /= UnsignedInteger<MD, DT, NT>(r)));
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>
UnsignedInteger<MD, DT, NT>::operator/(NT r) const
{
	return(UnsignedInteger<MD, DT, NT>(*this) /= r);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT> &
UnsignedInteger<MD, DT, NT>::operator%=(NT r)
{
	return(normalize(*this -= (*this/UnsignedInteger<MD, DT, NT>(r))*UnsignedInteger<MD, DT, NT>(r)));
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>
UnsignedInteger<MD, DT, NT>::operator%(NT r) const
{
	return(UnsignedInteger<MD, DT, NT>(*this) %= r);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>
operator+(NT n, const UnsignedInteger<MD, DT, NT> &r)
{
	return(UnsignedInteger<MD, DT, NT>(n) += r);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>
operator-(NT n, const UnsignedInteger<MD, DT, NT> &r)
{
	return(UnsignedInteger<MD, DT, NT>(n) -= r);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>
operator*(NT n, const UnsignedInteger<MD, DT, NT> &r)
{
	return(UnsignedInteger<MD, DT, NT>(n) *= r);
}

template <int MD, class DT, class NT>
UnsignedInteger<MD, DT, NT>
operator/(NT n, const UnsignedInteger<MD, DT, NT> &r)
{
	return(UnsignedInteger<MD, DT, NT>(n) /= r);
}

// logical operators
template <int MD, class DT, class NT>
bool
UnsignedInteger<MD, DT, NT>::operator==(NT r) const
{
	return(*this == UnsignedInteger<MD, DT, NT>(r));
}

template <int MD, class DT, class NT>
bool
UnsignedInteger<MD, DT, NT>::operator!=(NT r) const
{
	return(*this != UnsignedInteger<MD, DT, NT>(r));
}

template <int MD, class DT, class NT>
bool
UnsignedInteger<MD, DT, NT>::operator<(NT r) const
{
	return(*this < UnsignedInteger<MD, DT, NT>(r));
}

template <int MD, class DT, class NT>
bool
UnsignedInteger<MD, DT, NT>::operator>(NT r) const
{
	return(*this > UnsignedInteger<MD, DT, NT>(r));
}

template <int MD, class DT, class NT>
bool
UnsignedInteger<MD, DT, NT>::operator<=(NT r) const
{
	return(*this <= UnsignedInteger<MD, DT, NT>(r));
}

template <int MD, class DT, class NT>
bool
UnsignedInteger<MD, DT, NT>::operator>=(NT r) const
{
	return(*this >= UnsignedInteger<MD, DT, NT>(r));
}

template <int MD, class DT, class NT>
bool
operator==(NT n, const UnsignedInteger<MD, DT, NT> &r)
{
	return(r == n);
}

template <int MD, class DT, class NT>
bool
operator!=(NT n, const UnsignedInteger<MD, DT, NT> &r)
{
	return(!(r == n));
}

template <int MD, class DT, class NT>
bool
operator<(NT n, const UnsignedInteger<MD, DT, NT> &r)
{
	return(!(r >= n));
}

template <int MD, class DT, class NT>
bool
operator>(NT n, const UnsignedInteger<MD, DT, NT> &r)
{
	return(!(r <= n));
}

template <int MD, class DT, class NT>
bool
operator<=(NT n, const UnsignedInteger<MD, DT, NT> &r)
{
	return(!(r > n));
}

template <int MD, class DT, class NT>
bool
operator>=(NT n, const UnsignedInteger<MD, DT, NT> &r)
{
	return(!(r < n));
}
