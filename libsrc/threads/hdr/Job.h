//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_JOB_H
#define __OMBT_JOB_H
// job class

// system headers
#include <pthread.h>

// local headers
#include "atomic/UCBaseObject.h"

namespace ombt {

// base class for all runnable classes
class Job: public UCBaseObject {
public:
    enum JobReturn { Finished, Error, Blocked };

    // ctor and dtor
    Job();
    virtual ~Job();

    // service
    virtual JobReturn run();
    unsigned long getJobId() const;

private:
    static unsigned long nextJobId();

private:
    // leper colony
    Job(const Job &src);
    Job &operator=(const Job &rhs);

protected:
    unsigned long jobId_;
};

}

#endif
