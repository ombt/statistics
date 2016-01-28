//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_EXTERNAL_USECNT_PTR_H
#define __OMBT_EXTERNAL_USECNT_PTR_H
//
// NON-intrusive reference-counted pointer. one very important point, since
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
#include "atomic/Atomic.h"

namespace ombt {

// reference-counted pointer
template <class DataType> 
class ExtUseCntPtr {
public:
    // reference-counted pointer representation
    class ExtUseCntPtrRep {
    public:
        ExtUseCntPtrRep(DataType *pd = NULL):
            pdata_(pd), counter_(0) {
        }
        ~ExtUseCntPtrRep() {
            if (pdata_ != NULL) delete pdata_;
            pdata_ = NULL;
        }
        int increment() {
            return(counter_.increment());
        }
        int decrement() {
            return(counter_.decrement());
        }

        // internal data
        DataType *pdata_;
        Atomic<int> counter_;

    private:
        // not allowed
        ExtUseCntPtrRep(const ExtUseCntPtrRep &);
        ExtUseCntPtrRep &operator=(const ExtUseCntPtrRep &);
    };

    explicit ExtUseCntPtr():
        prep_(new ExtUseCntPtrRep(NULL)) {
        prep_->increment();
    }
    explicit ExtUseCntPtr(DataType *pd):
        prep_(new ExtUseCntPtrRep(pd)) {
        prep_->increment();
    }
    ExtUseCntPtr(const ExtUseCntPtr &p):
        prep_(p.prep_) {
        prep_->increment();
    }
    ~ExtUseCntPtr() {
        if (prep_->decrement() <= 0)
        {
            delete prep_;
        }
        prep_ = NULL;
    }

    // assignment
    ExtUseCntPtr<DataType> &operator=(DataType *rhs) {
        if (prep_->pdata_ != rhs)
        {
            if (prep_->decrement() <= 0)
            {
                delete prep_;
                prep_ = NULL;
            }
            prep_ = new ExtUseCntPtrRep(rhs);
            prep_->increment();
        }
        return(*this);
    }
    ExtUseCntPtr<DataType> &operator=(const ExtUseCntPtr<DataType> &rhs) {
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
    bool operator<(const ExtUseCntPtr<DataType> &rhs) const {
        return(prep_->pdata_ < rhs.prep_->pdata_);
    }
    bool operator<=(const ExtUseCntPtr<DataType> &rhs) const {
        return(prep_->pdata_ <= rhs.prep_->pdata_);
    }
    bool operator>(const ExtUseCntPtr<DataType> &rhs) const {
        return(prep_->pdata_ > rhs.prep_->pdata_);
    }
    bool operator>=(const ExtUseCntPtr<DataType> &rhs) const {
        return(prep_->pdata_ >= rhs.prep_->pdata_);
    }
    bool operator==(const ExtUseCntPtr<DataType> &rhs) const {
        return(prep_->pdata_ == rhs.prep_->pdata_);
    }
    bool operator!=(const ExtUseCntPtr<DataType> &rhs) const {
        return(prep_->pdata_ != rhs.prep_->pdata_);
    }

protected:
    // internal data
    ExtUseCntPtrRep *prep_;
};

}

#endif
