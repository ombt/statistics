//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_TIMER_QUEUE_H
#define __OMBT_TIMER_QUEUE_H
// queue for timers 

// system headers
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <queue>
#include <vector>
#include <functional>
#include <algorithm>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/UCBaseObject.h"
#include "timers/OldTimer.h"

namespace ombt {

// queue for generic timer
class OldTimerQueue: public UCBaseObject
{
public:
    // ctor and dtor
    OldTimerQueue();
    ~OldTimerQueue();

    // accessor functions to timer priority queue
    int isEmpty() const { return(pqueue_.empty()); }
    int enqueue(const OldTimer &timer);
    int dequeue(OldTimer &timer);
    int peek(OldTimer &timer) const;
    const OldTimer peek() const;

    // time to next timer
    timeval *getTimeOut(timeval *tv);

private:
    // not possible with stl pqueue
    OldTimerQueue(const OldTimerQueue &src);
    OldTimerQueue &operator=(const OldTimerQueue &rhs);

protected:
    // we want the queue to have times in increasing order
    std::priority_queue<OldTimer, std::vector<OldTimer>, std::greater<OldTimer> > pqueue_;
};

}

#endif
