//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// nano-second timer and basic math

// local includes
#include "system/Debug.h"
#include "hdr/NanoSecTime.h"

// high-res timer
namespace ombt {

// ctors and dtor
NanoSecTime::NanoSecTime(TimerType tt):
    _timer_type(tt),
    _seconds(0),
    _nanoseconds(0)
{
    _id = tt;
    MustBeTrue(::clock_gettime(_id, &_tp) == 0);
    _seconds     = _tp.tv_sec;
    _nanoseconds = _tp.tv_nsec;
}

NanoSecTime::NanoSecTime(long sec, long nsec, TimerType tt):
    _timer_type(tt),
    _seconds(sec),
    _nanoseconds(nsec)
{
    _id = tt;
    _tp.tv_sec  =  _seconds;
    _tp.tv_nsec = _nanoseconds;
}

NanoSecTime::NanoSecTime(const NanoSecTime &src):
    _timer_type(src._timer_type),
    _seconds(src._seconds),
    _nanoseconds(src._nanoseconds),
    _id(src._id)
{
    _tp.tv_sec  = src._tp.tv_sec;
    _tp.tv_nsec = src._tp.tv_nsec;
}

NanoSecTime::~NanoSecTime() { }

// assignment
NanoSecTime &
NanoSecTime::operator=(const NanoSecTime &rhs)
{
    if (this != &rhs)
    {
        _timer_type  = rhs._timer_type;
        _seconds     = rhs._seconds;
        _nanoseconds = rhs._nanoseconds;
        _id          = rhs._id;
        _tp.tv_sec   = rhs._tp.tv_sec;
        _tp.tv_nsec  = rhs._tp.tv_nsec;
    }
    return *this;
}

NanoSecTime &
NanoSecTime::set(long sec, long nsec, TimerType tt)
{
    _timer_type = tt;
    _seconds     = sec;
    _nanoseconds = nsec;

    _id = tt;
    _tp.tv_sec  =  _seconds;
    _tp.tv_nsec = _nanoseconds;
}


// take a snapshot
void
NanoSecTime::snapshot(TimerType tt)
{
    _timer_type = tt;
    _id = tt;

    MustBeTrue(::clock_gettime(_id, &_tp) == 0);

    _seconds     = _tp.tv_sec;
    _nanoseconds = _tp.tv_nsec;
}

// stop-watch mode
void
NanoSecTime::start(TimerType tt)
{
    snapshot(tt);
}

void
NanoSecTime::stop(TimerType tt)
{
    *this = NanoSecTime(tt) - *this;
}

// basic arithmetic
NanoSecTime &
NanoSecTime::operator+=(const NanoSecTime &rhs)
{
    long sec  = (_tp.tv_nsec + rhs._tp.tv_nsec)/1000000000;
    long nsec = (_tp.tv_nsec + rhs._tp.tv_nsec)%1000000000;

    _tp.tv_sec += rhs._tp.tv_sec + sec;
    _tp.tv_nsec = nsec;
    MustBeTrue((_tp.tv_sec >= 0) && (_tp.tv_nsec >= 0));

    _seconds     = _tp.tv_sec;
    _nanoseconds = _tp.tv_nsec;

    return *this;
}

NanoSecTime &
NanoSecTime::operator-=(const NanoSecTime &rhs)
{
    long nsec, sec;

    if (_tp.tv_nsec < rhs._tp.tv_nsec)
    {
        --_tp.tv_sec;
        _tp.tv_nsec += 1000000000;
    }

    _tp.tv_nsec -= rhs._tp.tv_nsec;
    _tp.tv_sec  -= rhs._tp.tv_sec;
    MustBeTrue((_tp.tv_sec >= 0) && (_tp.tv_nsec >= 0));

    _seconds     = _tp.tv_sec;
    _nanoseconds = _tp.tv_nsec;

    return *this;
}

NanoSecTime
NanoSecTime::operator+(const NanoSecTime &rhs) const
{
    return NanoSecTime(*this) += rhs;
}

NanoSecTime
NanoSecTime::operator-(const NanoSecTime &rhs) const
{
    return NanoSecTime(*this) -= rhs;
}

// comparisons
bool
NanoSecTime::operator==(const NanoSecTime &rhs) const
{
    return ((_tp.tv_sec == rhs._tp.tv_sec) &&
            (_tp.tv_nsec == rhs._tp.tv_nsec));
}

bool
NanoSecTime::operator!=(const NanoSecTime &rhs) const
{
    return !(*this == rhs);
}

bool
NanoSecTime::operator<(const NanoSecTime &rhs) const
{
    if ((_tp.tv_sec < rhs._tp.tv_sec) ||
        ((_tp.tv_sec == rhs._tp.tv_sec) &&
         (_tp.tv_nsec < rhs._tp.tv_nsec)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
NanoSecTime::operator<=(const NanoSecTime &rhs) const
{
    return !(rhs < *this);
}

bool
NanoSecTime::operator>(const NanoSecTime &rhs) const
{
    return (rhs < *this);
}

bool
NanoSecTime::operator>=(const NanoSecTime &rhs) const
{
    return !(*this < rhs);
}

// output
std::ostream &
operator<<(std::ostream &os, const NanoSecTime &o)
{
    os << "(s;ns)=(" 
       << o._seconds
       << ";"
       << o._nanoseconds
       << ")";
    return os;
}

}

