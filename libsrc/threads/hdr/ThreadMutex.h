//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_THREAD_MUTEX_H
#define __OMBT_THREAD_MUTEX_H
// mutex class

// system headers
#include <pthread.h>

// local headers
#include "system/Returns.h"
#include "atomic/UCBaseObject.h"

namespace ombt {

// forward declaration
class ThreadConditionVariable;

// thread mutex class
class BaseMutex: public UCBaseObject {
public:
    // friends
    friend class ThreadConditionVariable;

    // ctor and dtor
    BaseMutex(): UCBaseObject(OK) { }
    virtual ~BaseMutex() { }

    // enable or disable on demand
    virtual void lock() = 0;
    virtual void unlock() = 0;
    
private:
    // leper colony
    BaseMutex(const BaseMutex &src);
    BaseMutex &operator=(const BaseMutex &rhs);
};

class NullMutex: public BaseMutex {
public:
    // friends
    friend class ThreadConditionVariable;

    // ctor and dtor
    NullMutex(): BaseMutex() { }
    ~NullMutex() { }

    // do nothing lock and unlock
    inline void lock() { }
    inline void unlock() { }
    
private:
    // leper colony
    NullMutex(const NullMutex &src);
    NullMutex &operator=(const NullMutex &rhs);
};

// thread mutex class
class ThreadMutex: public BaseMutex {
public:
    // friends
    friend class ThreadConditionVariable;

    // ctor and dtor
    ThreadMutex(): BaseMutex()  {
         pthread_mutex_init(&mutex_, 0);
    }
    ~ThreadMutex() {
         pthread_mutex_destroy(&mutex_);
    }

    // enable or disable on demand
    inline void lock() {
         pthread_mutex_lock(&mutex_);
    }
    inline void unlock() {
         pthread_mutex_unlock(&mutex_);
    }
    
private:
    // leper colony
    ThreadMutex(const ThreadMutex &src);
    ThreadMutex &operator=(const ThreadMutex &rhs);

protected:
    pthread_mutex_t mutex_;
};

class ThreadMutexLock: public UCBaseObject {
public:
    // ctor and dtor
    ThreadMutexLock(BaseMutex &mutex):
         UCBaseObject(OK),
         mutex_(mutex) {
         mutex_.lock();
    }
    ~ThreadMutexLock() {
         mutex_.unlock();
    }

private:
    // leper colony
    ThreadMutexLock(const ThreadMutexLock &src);
    ThreadMutexLock &operator=(const ThreadMutexLock &rhs);

protected:
    BaseMutex &mutex_;
};

}

#endif
