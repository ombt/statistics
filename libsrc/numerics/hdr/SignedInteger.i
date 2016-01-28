//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// base-N integer

// constructors and destructor
template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT>::SignedInteger():
	sign_(1), number_()
{
	normalize(1);
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT>::SignedInteger(
	DT d, int sign): sign_(sign), number_(d)
{
	normalize(sign);
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT>::SignedInteger(
	const NT &n, int sign):
	sign_(sign), number_(n)
{
	normalize(sign);
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT>::SignedInteger(
	const SignedInteger &s):
	sign_(s.sign_), number_(s.number_)
{
	normalize(s.sign_);
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT>::SignedInteger(const std::string &sn):
	sign_(1), number_()
{
	normalize(sn);
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT>::~SignedInteger() 
{
}

// normalize number to standard form
template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT> &
SignedInteger<MD, DT, NT>::normalize(int sign)
{
	if (sign < 0)
		sign_ = -1;
	else
		sign_ = 1;
	return(*this);
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT> &
SignedInteger<MD, DT, NT>::normalize()
{
	static const UnsignedInteger<MD, DT, NT> UI_zero;

	if (sign_ < 0)
		sign_ = -1;
	else
		sign_ = 1;
	if (number_ == UI_zero)
		sign_ = 1;
	return(*this);
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT> &
SignedInteger<MD, DT, NT>::normalize(const std::string &sn)
{
	int imin = 0;

	if (sn[imin] == '-')
	{
		sign_ = -1;
		imin += 1;
	}
	else if (sn[imin] == '+')
	{
		sign_ = 1;
		imin += 1;
	}
	else
	{
		sign_ = 1;
	}

	if (imin == 0)
		number_ = UnsignedInteger<MD,DT,NT>(sn);
	else
		number_ = UnsignedInteger<MD,DT,NT>(sn.substr(imin));

	return(*this);
}


// print base-n number in raw form
template <int MD, class DT, class NT>
std::ostream &
operator<<(std::ostream &os, const SignedInteger<MD, DT, NT> &n)
{
	if (n.sign_ < 0)
		os << "-";
	else
		os << "+";
	os << n.number_;
	return(os);
}

template <int MD, class DT, class NT>
std::string
SignedInteger<MD, DT, NT>::toString() const
{
	if (isNegative())
		return(std::string("-")+number_.toString());
	else
		return(number_.toString());
}
// arithmetic operators
template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT> &
SignedInteger<MD, DT, NT>::operator=(const SignedInteger<MD, DT, NT> &n)
{
	if (&n == this) return(*this);
	sign_ = n.sign_;
	number_ = n.number_;
	return(normalize());
}

// arithmetic operators
template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT> &
SignedInteger<MD, DT, NT>::operator=(const std::string &sn)
{
	return(normalize(sn));
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT> &
SignedInteger<MD, DT, NT>::operator+=(const SignedInteger<MD, DT, NT> &n)
{
	if (sign_ >= 0 && n.sign_ >= 0)
	{
		sign_ = 1;
		number_ += n.number_;
		return *this;
	}
	else if (sign_ < 0 && n.sign_ < 0)
	{
		sign_ = 1;
		number_ += n.number_;
		return *this;
	}
	else if (sign_ < 0 && n.sign_ >= 0)
	{
		if (number_ <= n.number_)
		{
			sign_ = 1;
			number_ = n.number_ - number_;
		}
		else
		{
			sign_ = -1;
			number_ = number_ - n.number_;
		}
	}
	else
	{
		if (number_ <= n.number_)
		{
			sign_ = -1;
			number_ = n.number_ - number_;
		}
		else
		{
			sign_ = 1;
			number_ = number_ - n.number_;
		}
	}
	return(normalize());
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT>
SignedInteger<MD, DT, NT>::operator+(const SignedInteger<MD, DT, NT> &r) const
{
	return(SignedInteger<MD, DT, NT>(*this) += r);
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT> &
SignedInteger<MD, DT, NT>::operator-=(const SignedInteger<MD, DT, NT> &n)
{
	if (sign_ >= 0 && n.sign_ < 0)
	{
		sign_ = 1;
		number_ += n.number_;
		return *this;
	}
	else if (sign_ < 0 && n.sign_ >= 0)
	{
		sign_ = -1;
		number_ += n.number_;
		return *this;
	}
	else if (sign_ < 0 && n.sign_ < 0)
	{
		if (number_ <= n.number_)
		{
			sign_ = 1;
			number_ = n.number_ - number_;
		}
		else
		{
			sign_ = -1;
			number_ = number_ - n.number_;
		}
	}
	else
	{
		if (number_ <= n.number_)
		{
			sign_ = -1;
			number_ = n.number_ - number_;
		}
		else
		{
			sign_ = 1;
			number_ = number_ - n.number_;
		}
	}
	return(normalize());
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT>
SignedInteger<MD, DT, NT>::operator-(const SignedInteger<MD, DT, NT> &n) const
{
	return(SignedInteger<MD, DT, NT>(*this) -= n);
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT> &
SignedInteger<MD, DT, NT>::operator*=(const SignedInteger<MD, DT, NT> &n)
{
	number_ *= n.number_;
	if ((sign_ >= 0 && n.sign_ >= 0) ||
	    (sign_ < 0 && n.sign_ < 0))
		sign_ = 1;
	else
		sign_ = -1;
	return(normalize());
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT>
SignedInteger<MD, DT, NT>::operator*(const SignedInteger<MD, DT, NT> &n) const
{
	return(SignedInteger<MD, DT, NT>(*this) *= n);
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT> &
SignedInteger<MD, DT, NT>::operator/=(const SignedInteger<MD, DT, NT> &n)
{
	number_ /= n.number_;
	if ((sign_ >= 0 && n.sign_ >= 0) ||
	    (sign_ < 0 && n.sign_ < 0))
		sign_ = 1;
	else
		sign_ = -1;
	return(normalize());
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT>
SignedInteger<MD, DT, NT>::operator/(const SignedInteger<MD, DT, NT> &n) const
{
	return(SignedInteger<MD, DT, NT>(*this) /= n);
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT> &
SignedInteger<MD, DT, NT>::operator%=(const SignedInteger<MD, DT, NT> &n)
{
	number_ %= n.number_;
	if ((sign_ >= 0 && n.sign_ >= 0) ||
	    (sign_ < 0 && n.sign_ < 0))
		sign_ = 1;
	else
		sign_ = -1;
	return(normalize());
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT>
SignedInteger<MD, DT, NT>::operator%(const SignedInteger<MD, DT, NT> &n) const
{
	return(SignedInteger<MD, DT, NT>(*this) %= n);
}

// logical operators
template <int MD, class DT, class NT>
bool
SignedInteger<MD, DT, NT>::operator==(const SignedInteger<MD, DT, NT> &n) const
{
	return (((isPositive() && n.isPositive()) || 
		 (isNegative() && n.isNegative())) && 
		  number_ == n.number_);
}

template <int MD, class DT, class NT>
bool
SignedInteger<MD, DT, NT>::operator!=(const SignedInteger<MD, DT, NT> &n) const
{
	return(!(*this == n));
}

template <int MD, class DT, class NT>
bool
SignedInteger<MD, DT, NT>::operator<(const SignedInteger<MD, DT, NT> &n) const
{
	if (isNegative() && n.isPositive())
		return(true);
	else if (isPositive() && n.isNegative())
		return(false);
	else if (isNegative())
		return number_ > n.number_;
	else
		return number_ < n.number_;
}

template <int MD, class DT, class NT>
bool
SignedInteger<MD, DT, NT>::operator>(const SignedInteger<MD, DT, NT> &n) const
{
	if (isNegative() && n.isPositive())
		return(false);
	else if (isPositive() && n.isNegative())
		return(true);
	else if (isPositive())
		return number_ > n.number_;
	else
		return number_ < n.number_;
}

template <int MD, class DT, class NT>
bool
SignedInteger<MD, DT, NT>::operator<=(const SignedInteger<MD, DT, NT> &n) const
{
	return(!(*this > n));
}

template <int MD, class DT, class NT>
bool
SignedInteger<MD, DT, NT>::operator>=(const SignedInteger<MD, DT, NT> &n) const
{
	return(!(*this < n));
}

// misc
template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT>
abs(const SignedInteger<MD, DT, NT> &n)
{
	if (n.isNegative())
		return(-n);
	else
		return(n);
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT>
operator-(const SignedInteger<MD, DT, NT> &n)
{
	return(SignedInteger<MD, DT, NT>(-1)*n);
}

template <int MD, class DT, class NT>
SignedInteger<MD, DT, NT>
operator+(const SignedInteger<MD, DT, NT> &n)
{
	return(n);
}
