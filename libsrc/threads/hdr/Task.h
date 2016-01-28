//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_TASK_H
#define __OMBT_TASK_H
// task class

// system headers
#include <pthread.h>

// local headers
#include "atomic/UCBaseObject.h"

namespace ombt {

// base class for all runnable classes
class Task: public UCBaseObject {
public:
    // ctor and dtor
    Task();
    virtual ~Task();

    // duplicates instead of copy ctor
    virtual Task *clone() const;

    // operations
    virtual int init();
    virtual int run();
    virtual int finish();

private:
    // leper colony
    Task(const Task &src);
    Task &operator=(const Task &rhs);
};

}

#endif
