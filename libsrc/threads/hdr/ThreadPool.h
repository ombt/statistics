//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_THREAD__POOL_H
#define __OMBT_THREAD__POOL_H
// thread pool class

// system headers
#include <pthread.h>
#include <list>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/UseCntPtr.h"
#include "threads/Queue.h"
#include "threads/Task.h"
#include "threads/Thread.h"
#include "threads/Job.h"

namespace ombt {

// thread pool class
class ThreadPool: public UCBaseObject {
protected:
    // internal types
    typedef std::list<UseCntPtr<Thread> > Pool;
    typedef Queue<UseCntPtr<Job> > JobQueue;

    // thread pool task
    class PoolTask: public Task {
    public:
        PoolTask(JobQueue &jobqueue):
            Task(), jobqueue_(jobqueue) {
        }
        virtual ~PoolTask() {
        }

        virtual PoolTask *clone() const {
            return new PoolTask(jobqueue_);
	}
        virtual int init() {
            return 0;
        }
        virtual int run() {
            UseCntPtr<Job> pjob;
            while (jobqueue_.dequeue(pjob))
            {
                if (pjob == NULL) continue;
                switch (pjob->run())
                {
                case Job::Finished:
                    // done.
                    break;
                case Job::Blocked:
                    // still running
                    jobqueue_.enqueue(pjob);
                    break;
                case Job::Error:
                default:
                    // error. do nothing for now.
                    break;
                }
            }
            return 0;
        }
        virtual int finish() {
            return 0;
        }

    private:
        PoolTask();
        PoolTask(const PoolTask &);
        PoolTask &operator=(const PoolTask &);

    protected:
        JobQueue &jobqueue_;
    };

public:
    // ctor and dtor
    ThreadPool(unsigned howmany): 
        UCBaseObject(OK), howmany_(howmany), 
        done_(false), jobqueue_(), pool_() {
        MustBeTrue(howmany_ != 0);

        for (unsigned i=1; i<=howmany_; ++i)
        {
            pool_.push_back(UseCntPtr<Thread>(new Thread((new PoolTask(jobqueue_)))));
        }

        Pool::iterator it = pool_.begin();
        Pool::iterator itend = pool_.end();
        for ( ; it != itend; ++it)
        {
            MustBeTrue((*it)->run() == 0 && (*it)->isOk());
        }
    }
    ~ThreadPool() {
        jobqueue_.done();
        Pool::iterator it = pool_.begin();
        Pool::iterator itend = pool_.end();
        for ( ; it != itend; ++it)
        {
            void *retval;
            (*it)->join(retval);
        }
    }

    void addJob(Job *newjob)
    {
        jobqueue_.enqueue(UseCntPtr<Job>(newjob));
    }

private:
    // leper colony
    ThreadPool();
    ThreadPool(const ThreadPool &src);
    ThreadPool &operator=(const ThreadPool &rhs);

protected:
    // internal data
    unsigned howmany_;
    bool done_;
    JobQueue jobqueue_;
    Pool pool_;
};

}

#endif
