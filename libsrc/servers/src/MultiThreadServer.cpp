//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// iterative server class

// system headers
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

// local headers
#include "system/Debug.h"
#include "atomic/UseCntPtr.h"
#include "hdr/MultiThreadServer.h"

namespace ombt {

// thread ctor and dtor
        // ctor and dtor
MultiThreadStreamServer::Task::Task(EndPoint *psubserverep, Handler *phandler):
    SimpleThread::Task(), psubserverep_(psubserverep), phandler_(phandler)
{
TRACE();
    if (psubserverep_->isOk() && phandler_->isOk())
        setOk(true);
    else
        setOk(false);
}

MultiThreadStreamServer::Task::~Task() { setOk(false); }

int
MultiThreadStreamServer::Task::init()
{
TRACE();
    if (isOk())
        return(OK);
    else
        return(NOTOK);
}

int
MultiThreadStreamServer::Task::run()
{
TRACE();
    if (isNotOk()) return(NOTOK);
TRACE();
    int status;
    while ((status = (*phandler_)(psubserverep_)) == OK) ;
TRACE();
    return(status < 0 ? NOTOK : OK);
}

int MultiThreadStreamServer::Task::finish()
{
TRACE();
    return(psubserverep_->close());
}

// ctor and dtor
MultiThreadStreamServer::MultiThreadStreamServer(
    EndPoint *pserverep, 
    MultiThreadStreamServer::Handler *phandler):
        Server(), pserverep_(pserverep), phandler_(phandler)
{
TRACE();
    if (pserverep_->isOk() && phandler_->isOk())
        setOk(true);
    else
        setOk(false);
}

MultiThreadStreamServer::~MultiThreadStreamServer()
{
TRACE();
    setOk(false);
}

// operations
int
MultiThreadStreamServer::init()
{
TRACE();
    if ((pserverep_->setReuseAddr(true) != 0) ||
        (pserverep_->bind() != 0) || 
        (pserverep_->listen() != 0))
        return(NOTOK);
    else
        return(OK);
}

int
MultiThreadStreamServer::run()
{
TRACE();
    if (isNotOk()) return(NOTOK);
TRACE();

    UseCntPtr<EndPoint> peer;

    while ( 1 )
    {
TRACE();
        bool retry = false;
        peer = pserverep_->accept(retry);
TRACE();
        if (peer == NULL)
        {
TRACE();
            if (retry)
                continue; // ignore interrupt from child HANGUP
            else
                return(NOTOK);
        }
TRACE();

        // do we need this? - mar, 5/5/11
        // Task task(peer, phandler_);

        SimpleThread thread(new Task(peer, phandler_));
TRACE();
        MustBeTrue(thread.isOk());

        thread.setDetachedAttr(true);
TRACE();
        MustBeTrue(thread.run() == OK);
    }
TRACE();

    return(OK);
}

int
MultiThreadStreamServer::finish()
{
TRACE();
    return(pserverep_->close());
}

}
