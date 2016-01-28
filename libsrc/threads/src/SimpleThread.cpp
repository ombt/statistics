//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// thread class

// headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "hdr/SimpleThread.h"
#include "hdr/ThreadMutex.h"
#include "logging/Logging.h"

namespace ombt {

// simple thread class
void *
SimpleThread::execute(void *data)
{
TRACE();
    SimpleThread *pthread = static_cast<SimpleThread * >(data);
TRACE();
    MustBeTrue(pthread != NULL);
TRACE();
    return(pthread->dispatch());
}

void *
SimpleThread::dispatch()
{
TRACE();
    int status;
TRACE();
    status = ptask_->init();
TRACE();
    if (status != 0) return((void*)status);
TRACE();
    status = ptask_->run();
TRACE();
    if (status != 0) return((void*)status);
TRACE();
    return((void*)(ptask_->finish()));
}

// ctor and dtor
SimpleThread::SimpleThread(Task *ptask):
    UCBaseObject(OK), ptask_(ptask), state_()
{
    MustBeTrue(ptask != NULL);
    ::pthread_attr_init(&state_.attr_);
}

SimpleThread::~SimpleThread()
{
TRACE();
    ThreadMutexLock lock(state_.cs_);
TRACE();
    if (isOk() && isStarted() && !isDetached())
    {
TRACE();
        void *retval = NULL;
        if (::pthread_cancel(state_.id_) == 0)
        {
TRACE();
            ::pthread_join(state_.id_, &retval);
        }
TRACE();
    }
TRACE();
    ::pthread_attr_destroy(&state_.attr_);
TRACE();
    setStatus(NOTOK);
}

// set/get attributes
bool
SimpleThread::isDetached() const
{
TRACE();
    int state;
TRACE();
    ::pthread_attr_getdetachstate(&state_.attr_, &state);
TRACE();
    return(state == PTHREAD_CREATE_DETACHED);
}

bool
SimpleThread::isJoinable() const
{
TRACE();
    int state;
TRACE();
    ::pthread_attr_getdetachstate(&state_.attr_, &state);
TRACE();
    return(state == PTHREAD_CREATE_JOINABLE);
}

void
SimpleThread::setDetachedAttr(bool flag)
{
TRACE();
    if (isNotOk() || isStarted()) return;
TRACE();
    ThreadMutexLock lock(state_.cs_);
TRACE();
    if (flag)
        ::pthread_attr_setdetachstate(&state_.attr_, PTHREAD_CREATE_DETACHED);
    else
        ::pthread_attr_setdetachstate(&state_.attr_, PTHREAD_CREATE_JOINABLE);
TRACE();
}

// maintenance
int
SimpleThread::detach()
{
TRACE();
    if (isNotOk() || !isStarted()) return(-1);
TRACE();
    ThreadMutexLock lock(state_.cs_);
TRACE();
    ::pthread_detach(state_.id_);
TRACE();
    ::pthread_attr_setdetachstate(&state_.attr_, PTHREAD_CREATE_DETACHED);
TRACE();
    return(0);
}

int
SimpleThread::join(void *&retval)
{
TRACE();
    if (isNotOk() || !isStarted() || isDetached()) return(-1);
TRACE();
    retval = NULL;
TRACE();
    int status = ::pthread_join(state_.id_, &retval);
TRACE();
    ThreadMutexLock lock(state_.cs_);
TRACE();
    setOk(false);
TRACE();
    return(status);
}

int
SimpleThread::cancel()
{
TRACE();
    if (isNotOk() || !isStarted() || isDetached()) return(-1);
TRACE();
    int status = ::pthread_cancel(state_.id_);
TRACE();
    ThreadMutexLock lock(state_.cs_);
TRACE();
    setOk(status == 0);
TRACE();
    return(status);
}


int
SimpleThread::run()
{
TRACE();
    if (isNotOk() || isStarted()) return(-1);
TRACE();
    int status = ::pthread_create(&state_.id_, &state_.attr_, execute, this);
TRACE();
    ThreadMutexLock lock(state_.cs_);
TRACE();
    setOk(status == 0);
TRACE();
    return(status);
}

}
