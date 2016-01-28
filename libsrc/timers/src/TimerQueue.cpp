//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// timer queue class

// headers
#include "hdr/TimerQueue.h"

namespace ombt {

// ctors and dtor
TimerQueue::TimerQueue(): 
    UCBaseObject(true), pqueue_()
{
    setOk(true);
}

TimerQueue::~TimerQueue()
{
    for ( ; !pqueue_.empty(); pqueue_.pop()) ;
    setOk(false);
}

// accessor functions to timer priority queue
int 
TimerQueue::enqueue(const Timer &duration)
{
    Timer now;
    now.setToNow();
    Timer timeout(duration);
    timeout += now;
    pqueue_.push(timeout);
    return(0);
}

int 
TimerQueue::dequeue(Timer &timer)
{
    if (pqueue_.empty()) return(-1);
    timer = pqueue_.top();
    pqueue_.pop();
    return(0);
}

int 
TimerQueue::peek(Timer &timer) const
{
    if (pqueue_.empty()) return(-1);
    timer = pqueue_.top();
    return(0);
}

const Timer
TimerQueue::peek() const
{
    // check for empty before calling this !!!
    Timer timer = pqueue_.top();
    return(timer);
}

timeval *
TimerQueue::getTimeOut(timeval *tv)
{
    if (pqueue_.empty() || 
        tv == NULL) return(NULL);

    Timer zero(0,0);
    Timer now;
    Timer timer;

    dequeue(timer);
    now.setToNow();
    timer -= now;
    if (timer < zero) timer.setToTimer(zero);
    timer.setTimeval(*tv);

    return(tv);
}

}


