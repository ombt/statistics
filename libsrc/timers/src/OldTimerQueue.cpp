//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// timer queue class

// headers
#include "hdr/OldTimerQueue.h"

namespace ombt {

// ctors and dtor
OldTimerQueue::OldTimerQueue(): 
    UCBaseObject(), pqueue_()
{
    setOk(true);
}

OldTimerQueue::~OldTimerQueue()
{
    for ( ; !pqueue_.empty(); pqueue_.pop()) ;
    setOk(false);
}

// accessor functions to timer priority queue
int 
OldTimerQueue::enqueue(const OldTimer &duration)
{
    OldTimer now;
    now.setToNow();
    OldTimer timeout(duration);
    timeout += now;
    pqueue_.push(timeout);
    return(0);
}

int 
OldTimerQueue::dequeue(OldTimer &timer)
{
    if (pqueue_.empty()) return(-1);
    timer = pqueue_.top();
    pqueue_.pop();
    return(0);
}

int 
OldTimerQueue::peek(OldTimer &timer) const
{
    if (pqueue_.empty()) return(-1);
    timer = pqueue_.top();
    return(0);
}

const OldTimer
OldTimerQueue::peek() const
{
    // check for empty before calling this !!!
    OldTimer timer = pqueue_.top();
    return(timer);
}

timeval *
OldTimerQueue::getTimeOut(timeval *tv)
{
    if (pqueue_.empty() || 
        tv == NULL) return(NULL);

    OldTimer zero(0,0);
    OldTimer now;
    OldTimer timer;

    dequeue(timer);
    now.setToNow();
    timer -= now;
    if (timer < zero) timer.setToTimer(zero);
    timer.setTimeval(*tv);

    return(tv);
}

}


