//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// timer class

// headers
#include "hdr/Timer.h"

namespace ombt {

const Timer::Tag Timer::InvalidTag = -1;

// assign next tag
Timer::Tag
Timer::nextTag()
{
    static Atomic<Tag> nexttag(1);
    return(nexttag.increment());
}

// ctors and dtor
Timer::Timer(): 
    BaseObject(false), 
    usertag_(InvalidTag), tag_(InvalidTag), 
    type_(OneShotTimer), seconds_(0), microseconds_(0),
    userseconds_(0), usermicroseconds_(0)
{
    tag_ = nextTag();
    setOk(false);
}

Timer::Timer(Seconds s, MicroSeconds us, Tag t, Type y): 
    BaseObject(true), 
    usertag_(t), tag_(InvalidTag), type_(y),
    seconds_(s+us/1000000), microseconds_(us%1000000), 
    userseconds_(0), usermicroseconds_(0)
{
    userseconds_ = seconds_;
    usermicroseconds_ = microseconds_;
    tag_ = nextTag();
    setOk(true);
}

Timer::Timer(const timeval &tv, Tag t, Type y): 
    BaseObject(true), 
    usertag_(t), tag_(InvalidTag), type_(y),
    seconds_(tv.tv_sec+tv.tv_usec/1000000), 
    microseconds_(tv.tv_usec%1000000), 
    userseconds_(0), usermicroseconds_(0)
{
    userseconds_ = seconds_;
    usermicroseconds_ = microseconds_;
    tag_ = nextTag();
    setOk(true);
}

Timer::Timer(const Timer &src):
    BaseObject(src), 
    usertag_(src.usertag_), tag_(src.tag_), type_(src.type_),
    seconds_(src.seconds_), microseconds_(src.microseconds_),
    userseconds_(src.userseconds_), usermicroseconds_(src.usermicroseconds_)
{
    // do nothing
}

Timer::~Timer()
{
    setOk(false);
}

}
