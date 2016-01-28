//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_THREAD_SAFE_PTR_H
#define __OMBT_THREAD_SAFE_PTR_H

// thread safe pointers -- BROKEN !!!

// local headers
#include "system/Debug.h"
#include "threads/ThreadMutex.h"

namespace ombt {

// thread safe pointer
template <class DT> 
class ThreadSafePtr
{
public:
    // ctor and dtor
    ThreadSafePtr(DT *pd):
        pdata_(pd), mutex_() {
        MustBeTrue(pd != NULL);
    }
    ~ThreadSafePtr() {
        pdata_ = NULL;
    }

    // explicit clean up
    void remove() {
        if (pdata_ != NULL) delete pdata_;
        pdata_ = NULL;
    }

    // lock/unlock class
    template <class DT2>
    class Lock
    {
    public:
        // ctor and dtor
        Lock(DT2 *&pd, ThreadMutex &mutex):
            pdata_(pd), mutex_(mutex) {
            mutex_.lock();
        }
        ~Lock() {
            mutex_.unlock();
        }

        // return locked pointer
        DT2 *operator->() {
            return(pdata_);
        }

    private:
        // data
        DT *&pdata_;
        ThreadMutex &mutex_;
    };

    // returned lock pointer
    Lock<DT> operator->() const {
        return(Lock<DT>(pdata_, mutex_));
    }
    Lock<DT> operator->() {
        return(Lock<DT>(pdata_, mutex_));
    }

private:
    // leper colony
    ThreadSafePtr(const ThreadSafePtr &src);
    ThreadSafePtr &operator=(const ThreadSafePtr &src);

protected:
    // internal data
    DT *pdata_;
    ThreadMutex mutex_;
};

}

#endif
