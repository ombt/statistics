//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_PRIORITY_QUEUE_H
#define __OMBT_PRIORITY_QUEUE_H

// threadpool priority queue. the queue item class must support the
// the operator<(), that is, less than.

// system headers
#include <queue>
#include <vector>
#include <functional>
#include <algorithm>
#include <limits>
#include <pthread.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/UCBaseObject.h"

namespace ombt {

// generic priority queue class
template <class Type, 
	  class Container = std::vector<Type>,
	  class Compare = std::less<typename Container::value_type> >
class PriorityQueue: public UCBaseObject {
public:
    // ctor and dtor
    PriorityQueue(int maxsize = std::numeric_limits<int>::max()): 
        UCBaseObject(OK), 
        done_(false), maxsize_(maxsize), events_() {
        MustBeTrue(maxsize_ >= 1);
        pthread_cond_init(&cv_, NULL);
        pthread_mutex_init(&cv_mutex_, NULL);
    }
    ~PriorityQueue() {
        pthread_cond_destroy(&cv_);
        pthread_mutex_destroy(&cv_mutex_);
    }

    // accessing the queue
    void enqueue(Type &event) {
        pthread_mutex_lock(&cv_mutex_);
        if (events_.size() <= maxsize_)
            events_.push(event);
        pthread_mutex_unlock(&cv_mutex_);
        pthread_cond_signal(&cv_);
    }
    bool dequeue(Type &event) {
        pthread_mutex_lock(&cv_mutex_);
        while (events_.empty())
        {
            if (done_)
            {
                pthread_mutex_unlock(&cv_mutex_);
                pthread_cond_signal(&cv_);
                return false;
            }
            pthread_cond_wait(&cv_, &cv_mutex_);
        }
        event = events_.top();
        events_.pop();
        pthread_mutex_unlock(&cv_mutex_);
        pthread_cond_signal(&cv_);
        return true;
    }
    void done() {
        pthread_mutex_lock(&cv_mutex_);
        done_ = true;
        pthread_mutex_unlock(&cv_mutex_);
        pthread_cond_broadcast(&cv_);
    }

private:
    // leper colony
    PriorityQueue(const PriorityQueue &);
    PriorityQueue &operator=(const PriorityQueue &);

protected:
    // data
    typedef std::priority_queue<Type,Container,Compare> Queue;

    bool done_;
    int maxsize_;
    Queue events_;
    pthread_cond_t cv_;
    pthread_mutex_t cv_mutex_;
};

}

#endif
