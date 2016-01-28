//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// timer class

// headers
#include "hdr/OldTimer.h"

namespace ombt {

const OldTimer::Tag OldTimer::InvalidTag = -1;

// ctors and dtor
OldTimer::OldTimer(): 
    BaseObject(false), tag_(InvalidTag), type_(OneShotTimer),
    seconds_(0), microseconds_(0)
{
    setOk(false);
}

OldTimer::OldTimer(Seconds s, MicroSeconds us, Tag t, Type y): 
    BaseObject(true), seconds_(s), 
    microseconds_(us), tag_(t), type_(y)
{
    setOk(true);
}

OldTimer::OldTimer(const OldTimer &src):
    BaseObject(src), tag_(src.tag_), type_(src.type_),
    seconds_(src.seconds_), microseconds_(src.microseconds_)
{
    // do nothing
}

OldTimer::~OldTimer()
{
    setOk(false);
}

}
