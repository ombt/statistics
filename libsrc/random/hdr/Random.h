//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// simple pseudo-random number generator
#ifndef __RANDOM_H
#define __RANDOM_H

// headers
#include "atomic/BaseObject.h"

extern void setKey(unsigned long);
extern unsigned long myrandom();
extern unsigned char myrandomchar();

namespace ombt {

// random class
class Random: public BaseObject {
public:
    // ctors and dtor
    Random();
    Random(unsigned long newKey, unsigned long m = (unsigned long)(-1),
           unsigned long a = 663608941, unsigned long c = 0);
    virtual ~Random();

    // operations
    virtual void setKey(unsigned long);
    virtual unsigned long random();
    virtual double random0to1();
    virtual unsigned char randomchar();

protected:
    // data
    unsigned long m_;
    unsigned long a_;
    unsigned long c_;
    unsigned long Ikey_;
};

}

#endif
