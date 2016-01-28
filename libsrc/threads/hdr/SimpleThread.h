//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_SIMPLE_THREAD_H
#define __OMBT_SIMPLE_THREAD_H
// thread class

// system headers
#include <pthread.h>

// local headers
#include "atomic/UCBaseObject.h"
#include "atomic/UseCntPtr.h"
#include "threads/ThreadMutex.h"

namespace ombt {

// simple thread class
class SimpleThread: public UCBaseObject {
public:
    // base class for all runnable classes
    class Task: public UCBaseObject {
    public:
        // ctor and dtor
        Task(): UCBaseObject(OK) { }
        virtual ~Task() { }

        virtual int init() { return 0; }
        virtual int run() { return 0; }
        virtual int finish() { return 0; }

    private:
        // leper colony
        Task(const Task &src) { }
        Task &operator=(const Task &rhs) { return(*this); }
    };

    // thread entry point
    static void *execute(void *data);
    void *dispatch();

public:
    // ctor and dtor
    SimpleThread(Task *runnable);
    ~SimpleThread();

    // maintenance
    int run();
    int join(void *&retval);
    int detach();
    int cancel();

    // set attributes
    void setDetachedAttr(bool flag);

    // get attributes
    inline bool isJoinable() const;
    inline bool isDetached() const;
    inline bool isStarted() const { return(state_.id_ != -1); }

private:
    // leper colony
    SimpleThread();
    SimpleThread(const SimpleThread &);
    SimpleThread &operator=(const SimpleThread &);

protected:
    // internal data
    UseCntPtr<Task> ptask_;
    struct State {
        ThreadMutex cs_;
        pthread_t id_;
        pthread_attr_t attr_;
        State(): cs_(), id_(-1) { }
    } state_;
};

}

#endif
