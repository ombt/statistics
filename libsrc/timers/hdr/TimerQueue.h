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
#include "timers/Timer.h"
#include "atomic/UCBaseObject.h"

namespace ombt {

// queue for generic timer
class TimerQueue: public UCBaseObject
{
public:
    // ctor and dtor
    TimerQueue();
    ~TimerQueue();

    // accessor functions to timer priority queue
    int isEmpty() const { return(pqueue_.empty()); }
    int enqueue(const Timer &timer);
    int dequeue(Timer &timer);
    int peek(Timer &timer) const;
    const Timer peek() const;

    // time to next timer
    timeval *getTimeOut(timeval *tv);

private:
    // not possible with stl pqueue
    TimerQueue(const TimerQueue &src);
    TimerQueue &operator=(const TimerQueue &rhs);

protected:
    // we want the queue to have times in increasing order
    std::priority_queue<Timer, std::vector<Timer>, std::greater<Timer> > pqueue_;
};

}

#endif
