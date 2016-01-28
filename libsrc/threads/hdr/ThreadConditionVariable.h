//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_THREAD_COND_VAR_H
#define __OMBT_THREAD_COND_VAR_H

// thread condition variable

// system headers
#include <pthread.h>

// system headers
#include "system/Returns.h"
#include "atomic/UCBaseObject.h"
#include "atomic/UseCntPtr.h"
#include "threads/ThreadMutex.h"

namespace ombt {

// thread condition variable class
class ThreadConditionVariable: public UCBaseObject {
public:
    // ctor and dtor
    ThreadConditionVariable(): UCBaseObject(OK) {
        pthread_cond_init(&cv_, NULL);
    }
    ~ThreadConditionVariable() {
        pthread_cond_destroy(&cv_);
    }

    // signal done 
    void signal() {
        pthread_cond_signal(&cv_);
    }
    void broadcast() {
        pthread_cond_broadcast(&cv_);
    }

    // wait for signal
    void wait(ThreadMutex *pm) {
        UseCntPtr<ThreadMutex> pmutex(pm);
        pthread_cond_wait(&cv_, &(pmutex->mutex_));
    }

private:
    // leper colony
    ThreadConditionVariable(const ThreadConditionVariable &src);
    ThreadConditionVariable &operator=(const ThreadConditionVariable &rhs);

private:
    pthread_cond_t cv_;
};

}

#endif
