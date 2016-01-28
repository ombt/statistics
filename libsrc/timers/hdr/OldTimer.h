//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_OLD_TIMER_H
#define __OMBT_OLD_TIMER_H
// timers in millisecodns

// system headers
#include <stdio.h>
#include <string.h>
#include <ostream>
#include <sys/time.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/BaseObject.h"

namespace ombt {

// generic timer
class OldTimer: public BaseObject
{
public:
    // data type
    typedef unsigned int Tag;
    typedef long int Seconds;
    typedef long int MicroSeconds;

    // constants
    static const Tag InvalidTag;

    enum Type { OneShotTimer, RepeatTimer };

    // ctor and dtor
    OldTimer();
    OldTimer(Seconds secs, MicroSeconds usecs, Tag tag = InvalidTag, Type y = OneShotTimer);
    OldTimer(const OldTimer &src);
    virtual ~OldTimer();

    // assignment
    OldTimer &operator=(const OldTimer &rhs) {
        if (this != &rhs)
        {
            BaseObject::operator=(rhs);
            tag_ = rhs.tag_;
            type_ = rhs.type_;
            seconds_ = rhs.seconds_;
            microseconds_ = rhs.microseconds_;
        }
        return(*this);
    }

    // type/tag accessors
    Type getType() const { return(type_); }
    void setType(Type y) { type_ = y; }
    Tag getTag() const { return(tag_); }
    void setTag(Tag t) { tag_ = t; }

    // timer accessors
    Seconds getSeconds() const { return(seconds_); }
    void setSeconds(const Seconds &s) { seconds_ = s; }
    MicroSeconds getMicroSeconds() const { return(microseconds_); }
    void setMicroSeconds(const MicroSeconds us) { microseconds_ = us; }

    // timer convenience accessors
    OldTimer &setToTimer(const OldTimer &t) {
	seconds_ = t.seconds_;
	microseconds_ = t.microseconds_;
        return(*this);
    }
    OldTimer &setToTimeval(const timeval &tv) {
        seconds_ = tv.tv_sec;
        microseconds_ = tv.tv_usec;
        return(*this);
    }
    OldTimer now() const {
        timeval nowtv;
        gettimeofday(&nowtv, NULL);
        OldTimer tv;
        tv.setToTimeval(nowtv);
        return(tv);
    }
    OldTimer &setToNow() {
        timeval nowtv;
        gettimeofday(&nowtv, NULL);
        return(setToTimeval(nowtv));
    }
    timeval getTimeval() const {
        timeval tv;
        tv.tv_sec = seconds_;
        tv.tv_usec = microseconds_;
        return(tv);
    }
    void setTimeval(timeval &tv) const {
        tv.tv_sec = seconds_;
        tv.tv_usec = microseconds_;
    }

    // simple arithmetic in microsecs
    OldTimer operator+(OldTimer const &rhs) const {
        OldTimer t(*this);
        t.seconds_ += rhs.seconds_;
        t.microseconds_ += rhs.microseconds_;
        t.seconds_ += t.microseconds_/1000000;
        t.microseconds_ %= 1000000;
        return(t);
    }
    OldTimer operator-(OldTimer const &rhs) const { 
        OldTimer t(*this);
        if (t.microseconds_ < rhs.microseconds_)
        {
            t.seconds_ -= 1;
            t.microseconds_ += 1000000;
        }
        t.microseconds_ -= rhs.microseconds_;
        t.seconds_ -= rhs.seconds_;
        return(t);
    }
    OldTimer &operator+=(OldTimer &rhs) { 
        seconds_ += rhs.seconds_;
        microseconds_ += rhs.microseconds_;
        seconds_ += microseconds_/1000000;
        microseconds_ %= 1000000;
        return(*this);
    }
    OldTimer &operator-=(OldTimer &rhs) { 
        if (microseconds_ < rhs.microseconds_)
        {
            seconds_ -= 1;
            microseconds_ += 1000000;
        }
        microseconds_ -= rhs.microseconds_;
        seconds_ -= rhs.seconds_;
        return(*this);
    }

    // comparisons
    bool operator<(const OldTimer &rhs) const {
        if ((seconds_ < rhs.seconds_) ||
            ((seconds_ == rhs.seconds_) &&
             (microseconds_ < rhs.microseconds_)))
            return(true);
        else
            return(false);
    }
    bool operator==(const OldTimer &rhs) const {
        return((seconds_ == rhs.seconds_) &&
               (microseconds_ == rhs.microseconds_));
    }
    bool operator>(const OldTimer &rhs) const {
        if ((seconds_ > rhs.seconds_) ||
            ((seconds_ == rhs.seconds_) &&
             (microseconds_ > rhs.microseconds_)))
            return(true);
        else
            return(false);
    }
    bool operator<=(const OldTimer &rhs) const {
        return(!(*this > rhs));
    }
    bool operator!=(const OldTimer &rhs) const {
        return(!(*this == rhs));
    }
    bool operator>=(const OldTimer &rhs) const {
        return(!(*this < rhs));
    }

    // output
    friend std::ostream &operator<<(std::ostream &os, const OldTimer &t)
    {
        os << "(secs,usecs) = (" 
           << t.seconds_ << "," 
           << t.microseconds_ << ")";
        return(os);
    }

protected:
    // data
    Tag tag_;
    Type type_;
    Seconds seconds_;
    MicroSeconds microseconds_;
};

}

#endif
