//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// thread class

// headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "hdr/Job.h"
#include "threads/ThreadMutex.h"

namespace ombt {

// generate next id
unsigned long 
Job::nextJobId()
{
    static ThreadMutex mutex;
    static unsigned long nextJobToUse = 0;
    ThreadMutexLock lock(mutex);
    return(++nextJobToUse);
}

// ctor and dtor
Job::Job(): UCBaseObject(OK), jobId_(nextJobId())
{
}
Job::~Job()
{
	// do nothing
}

// job services
unsigned long
Job::getJobId() const
{
    return jobId_;
}

Job::JobReturn
Job::run()
{
    setStatus(NOTOK);
    return Finished;
}

}
