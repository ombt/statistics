//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_REFCNTPTR_H
#define __OMBT_REFCNTPTR_H
//
// reference-counted pointer. one very important point, since
// this class has the the class member access operator defined 
// (for example, pa->b), it can not be used with primitive
// data types. The following statements are nonsensical for primitive
// type.
//
//    int *pi = new int;    // allocate an int
//    pi->something = 1;    // error: 
//
// you must remove the declaration for operator->() to use this
// pointer class with primitive types, i.e., int, short, long, char, etc.
//
// WARNING: reference-counted pointers do not work when cyclic data
// structures. cycles can cause some major memory leaks since the
// use counts never got to zero and the objects are never deleted.
//

// headers
#include <assert.h>
#include "system/Debug.h"
#include "threads/ThreadMutex.h"

// reference-counted pointer
template <class DataType> 
class RefCntPtr {
public:
    // reference-counted pointer representation
    class RefCntPtrRep {
    public:
        RefCntPtrRep(DataType *pd = NULL):
            pdata_(pd), counter_(0) {
        }
        ~RefCntPtrRep() {
            counter_ = 0;
            if (pdata_ != NULL) delete pdata_;
            pdata_ = NULL;
        }
        int increment() {
            ++counter_;
            return(counter_);
        }
        int decrement() {
            --counter_;
            return(counter_);
        }

        // internal data
        DataType *pdata_;
        int counter_;
    };

    explicit RefCntPtr():
        prep_(new RefCntPtrRep(NULL)) {
        prep_->increment();
    }
    explicit RefCntPtr(DataType *pd):
        prep_(new RefCntPtrRep(pd)) {
        prep_->increment();
    }
    RefCntPtr(const RefCntPtr &p):
        prep_(p.prep_) {
        prep_->increment();
    }
    ~RefCntPtr() {
        if (prep_->decrement() <= 0)
        {
            delete prep_;
        }
        prep_ = NULL;
    }

    // assignment
    RefCntPtr<DataType> &operator=(DataType *rhs) {
        if (prep_->pdata_ != rhs)
        {
            if (prep_->decrement() <= 0)
            {
                delete prep_;
                prep_ = NULL;
            }
            prep_ = new RefCntPtrRep(rhs);
            prep_->increment();
        }
        return(*this);
    }
    RefCntPtr<DataType> &operator=(const RefCntPtr<DataType> &rhs) {
        if (prep_ != rhs.prep_)
        {
	    rhs.prep_->increment();
            if (prep_->decrement() <= 0)
            {
                delete prep_;
                prep_ = NULL;
            }
            prep_ = rhs.prep_;
	}
        return(*this);
    }

    // sanity checks
    int isNull() const {
        return(prep_->pdata_ == NULL);
    }
    int isNotNull() const {
        return(prep_->pdata_ != NULL);
    }

    // pointer operators
    operator DataType *() const {
        return(prep_->pdata_);
    }
    DataType *operator->() const {
        return(prep_->pdata_);
    }
    DataType &operator*() {
        return(*(prep_->pdata_));
    }
    DataType &operator*() const {
        return(*(prep_->pdata_));
    }
    DataType &operator[](int idx) {
        return(prep_->pdata_[idx]);
    }
    DataType &operator[](int idx) const {
        return(prep_->pdata_[idx]);
    }

    // comparators for containers
    bool operator<(const RefCntPtr<DataType> &rhs) const {
        return(prep_->pdata_ < rhs.prep_->pdata_);
    }
    bool operator<=(const RefCntPtr<DataType> &rhs) const {
        return(prep_->pdata_ <= rhs.prep_->pdata_);
    }
    bool operator>(const RefCntPtr<DataType> &rhs) const {
        return(prep_->pdata_ > rhs.prep_->pdata_);
    }
    bool operator>=(const RefCntPtr<DataType> &rhs) const {
        return(prep_->pdata_ >= rhs.prep_->pdata_);
    }
    bool operator==(const RefCntPtr<DataType> &rhs) const {
        return(prep_->pdata_ == rhs.prep_->pdata_);
    }
    bool operator!=(const RefCntPtr<DataType> &rhs) const {
        return(prep_->pdata_ != rhs.prep_->pdata_);
    }

protected:
    // internal data
    RefCntPtrRep *prep_;
};

// mutli-thread-safe reference-counted pointer
template <class DataType, class MutexType = NullMutex> 
class MTRefCntPtr {
public:
    // reference-counted pointer representation
    class MTRefCntPtrRep {
    public:
        MTRefCntPtrRep(DataType *pd = NULL):
            mutex_(), pdata_(pd), counter_(0) {
        }
        ~MTRefCntPtrRep() {
            ThreadMutexLock lock(mutex_);
            counter_ = 0;
            if (pdata_ != NULL) delete pdata_;
            pdata_ = NULL;
        }
        int increment() {
            ThreadMutexLock lock(mutex_);
            if (pdata_ != NULL) ++counter_;
            return(counter_);
        }
        int decrement() {
            ThreadMutexLock lock(mutex_);
            if (pdata_ != NULL) --counter_;
            return(counter_);
        }

        // internal data
        MutexType mutex_;
        DataType *pdata_;
        int counter_;
    };

    explicit MTRefCntPtr():
        prep_(new MTRefCntPtrRep(NULL)) {
    }
    explicit MTRefCntPtr(DataType *pd):
        prep_(new MTRefCntPtrRep(pd)) {
        prep_->increment();
    }
    MTRefCntPtr(const MTRefCntPtr &p):
        prep_(p.prep_) {
        prep_->increment();
    }
    ~MTRefCntPtr() {
        if (prep_->decrement() <= 0)
            delete prep_;
        prep_ = NULL;
    }

    // assignment
    MTRefCntPtr<DataType, MutexType> &operator=(DataType *rhs) {
        if (prep_->pdata_ != rhs)
        {
            if (prep_->decrement() <= 0)
            {
                delete prep_;
                prep_ = NULL;
            }
            prep_ = new MTRefCntPtrRep(rhs);
            prep_->increment();
        }
        return(*this);
    }
    MTRefCntPtr<DataType, MutexType> &operator=(const MTRefCntPtr<DataType, MutexType> &rhs) {
        rhs.prep_->increment();
        if (prep_->decrement() <= 0)
        {
            delete prep_;
            prep_ = NULL;
        }
        prep_ = rhs.prep_;
        return(*this);
    }

    // sanity checks
    int isNull() const {
        return(prep_->pdata_ == NULL);
    }
    int isNotNull() const {
        return(prep_->pdata_ != NULL);
    }

    // pointer operators
    operator DataType *() const {
        return(prep_->pdata_);
    }
    DataType *operator->() const {
        return(prep_->pdata_);
    }
    DataType &operator*() {
        return(*(prep_->pdata_));
    }
    DataType &operator*() const {
        return(*(prep_->pdata_));
    }
    DataType &operator[](int idx) {
        return(prep_->pdata_[idx]);
    }
    DataType &operator[](int idx) const {
        return(prep_->pdata_[idx]);
    }

    // comparators for containers
    bool operator<(const MTRefCntPtr<DataType, MutexType> &rhs) const {
        return(prep_->pdata_ < rhs.prep_->pdata_);
    }
    bool operator<=(const MTRefCntPtr<DataType, MutexType> &rhs) const {
        return(prep_->pdata_ <= rhs.prep_->pdata_);
    }
    bool operator>(const MTRefCntPtr<DataType, MutexType> &rhs) const {
        return(prep_->pdata_ > rhs.prep_->pdata_);
    }
    bool operator>=(const MTRefCntPtr<DataType, MutexType> &rhs) const {
        return(prep_->pdata_ >= rhs.prep_->pdata_);
    }
    bool operator==(const MTRefCntPtr<DataType, MutexType> &rhs) const {
        return(prep_->pdata_ == rhs.prep_->pdata_);
    }
    bool operator!=(const MTRefCntPtr<DataType, MutexType> &rhs) const {
        return(prep_->pdata_ != rhs.prep_->pdata_);
    }

protected:
    // internal data
    MTRefCntPtrRep *prep_;
};

#endif
