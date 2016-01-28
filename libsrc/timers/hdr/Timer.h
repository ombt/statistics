//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_TIMER2_H
#define __OMBT_TIMER2_H
// timers in millisecodns

// system headers
#include <stdio.h>
#include <string.h>
#include <ostream>
#include <sys/time.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/Atomic.h"
#include "atomic/BaseObject.h"

namespace ombt {

// generic timer
class Timer: public BaseObject
{
public:
    // data type
    typedef unsigned long Tag;
    typedef long int Seconds;
    typedef long int MicroSeconds;

    // constants
    static const Tag InvalidTag;

    enum Type { OneShotTimer, RepeatTimer };

    // ctor and dtor
    Timer();
    Timer(Seconds secs, MicroSeconds usecs, Tag usertag = InvalidTag, Type y = OneShotTimer);
    Timer(const timeval &tv, Tag usertag = InvalidTag, Type y = OneShotTimer);
    Timer(const Timer &src);
    ~Timer();

    // assignment
    Timer &operator=(const timeval &tv) {
        return(setToTimeval(tv));
    }
    Timer &operator=(const Timer &rhs) {
        if (this != &rhs)
        {
            BaseObject::operator=(rhs);
            tag_ = rhs.tag_;
            usertag_ = rhs.usertag_;
            type_ = rhs.type_;
            seconds_ = rhs.seconds_;
            microseconds_ = rhs.microseconds_;
            userseconds_ = rhs.userseconds_;
            usermicroseconds_ = rhs.usermicroseconds_;
        }
        return(*this);
    }

    // type/tag accessors
    Type getType() const { return(type_); }
    void setType(Type y) { type_ = y; }
    Tag getUserTag() const { return(usertag_); }
    void setUserTag(Tag t) { usertag_ = t; }
    Tag getTag() const { return(tag_); }

    // timer accessors
    Seconds getSeconds() const { return(seconds_); }
    void setSeconds(const Seconds &s) { seconds_ = s; }
    MicroSeconds getMicroSeconds() const { return(microseconds_); }
    void setMicroSeconds(const MicroSeconds us) {
        microseconds_ = us%1000000;
        seconds_ += us/1000000;
    }
    Seconds getUserSeconds() const { return(userseconds_); }
    MicroSeconds getUserMicroSeconds() const { return(usermicroseconds_); }

    // timer convenience accessors
    Timer &setToTimer(const Timer &t) {
	seconds_ = t.seconds_;
	microseconds_ = t.microseconds_;
        return(*this);
    }
    Timer &setToTimeval(const timeval &tv) {
        microseconds_ = tv.tv_usec%1000000;
        seconds_ = tv.tv_sec + tv.tv_usec/1000000;
        return(*this);
    }
    Timer now() const {
        timeval nowtv;
        gettimeofday(&nowtv, NULL);
        return(Timer(nowtv));
    }
    Timer &setToNow() {
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
    int getTimeInMillisecs() const {
        return(1000*seconds_ + microseconds_/1000);
    }

    // simple arithmetic in microsecs
    Timer operator+(Timer const &rhs) const {
        Timer t(*this);
        t.seconds_ += rhs.seconds_;
        t.microseconds_ += rhs.microseconds_;
        t.seconds_ += t.microseconds_/1000000;
        t.microseconds_ %= 1000000;
        return(t);
    }
    Timer operator-(Timer const &rhs) const { 
        Timer t(*this);
        if (t.microseconds_ < rhs.microseconds_)
        {
            t.seconds_ -= 1;
            t.microseconds_ += 1000000;
        }
        t.microseconds_ -= rhs.microseconds_;
        t.seconds_ -= rhs.seconds_;
        return(t);
    }
    Timer &operator+=(Timer &rhs) { 
        seconds_ += rhs.seconds_;
        microseconds_ += rhs.microseconds_;
        seconds_ += microseconds_/1000000;
        microseconds_ %= 1000000;
        return(*this);
    }
    Timer &operator-=(Timer &rhs) { 
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
    bool operator<(const Timer &rhs) const {
        if ((seconds_ < rhs.seconds_) ||
            ((seconds_ == rhs.seconds_) &&
             (microseconds_ < rhs.microseconds_)))
            return(true);
        else
            return(false);
    }
    bool operator==(const Timer &rhs) const {
        return((seconds_ == rhs.seconds_) &&
               (microseconds_ == rhs.microseconds_));
    }
    bool operator>(const Timer &rhs) const {
        if ((seconds_ > rhs.seconds_) ||
            ((seconds_ == rhs.seconds_) &&
             (microseconds_ > rhs.microseconds_)))
            return(true);
        else
            return(false);
    }
    bool operator<=(const Timer &rhs) const {
        return(!(*this > rhs));
    }
    bool operator!=(const Timer &rhs) const {
        return(!(*this == rhs));
    }
    bool operator>=(const Timer &rhs) const {
        return(!(*this < rhs));
    }

    // output
    friend std::ostream &operator<<(std::ostream &os, const Timer &t)
    {
        os << "(secs,usecs,usertag,tag,type) = (" 
           << t.seconds_ << "," 
           << t.microseconds_ << ","
           << t.usertag_ << "," 
           << t.tag_ << "," 
           << (int)t.type_ << ")";
        return(os);
    }

private:
    // miscellaneous
    Tag nextTag();

protected:
    // data
    Tag tag_;
    Tag usertag_;
    Type type_;
    Seconds seconds_;
    MicroSeconds microseconds_;
    Seconds userseconds_;
    MicroSeconds usermicroseconds_;
};

}

#endif
