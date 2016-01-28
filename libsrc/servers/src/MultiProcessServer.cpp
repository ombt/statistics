//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// iterative server class

// headers
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include "system/Debug.h"
#include "atomic/UseCntPtr.h"
#include "hdr/MultiProcessServer.h"

namespace ombt {

// ctors and dtor
MultiProcessDatagramServer::MultiProcessDatagramServer(
    EndPoint *pserverep, 
    MultiProcessDatagramServer::Handler *phandler):
        Server(), pserverep_(pserverep), phandler_(phandler)
{
    if (pserverep_->isOk() && phandler_->isOk())
        setOk(true);
    else
        setOk(false);
}

MultiProcessDatagramServer::~MultiProcessDatagramServer()
{
    setOk(false);
}

// operations
int
MultiProcessDatagramServer::init()
{
    if ((pserverep_->setReuseAddr(true) != 0) ||
        (pserverep_->bind() != 0))
        return(-1);
    else
        return(0);
}

static void 
reaper(int sig)
{
    int status;
    while (::wait3(&status, WNOHANG, (struct rusage *)0) >= 0) ;
    (void) signal(SIGCHLD, reaper);
}

int
MultiProcessDatagramServer::run()
{
    if (isNotOk()) return(-1);

    char buf[BUFSIZ+1];
    size_t count;

    UseCntPtr<Address> peer_address(pserverep_->getInternalAddress().create());

    (void) signal(SIGCHLD, reaper);

    while ( true )
    {
        count = BUFSIZ;
        count = pserverep_->read(buf, count, *peer_address);
        if (count > 0)
        {
            int pid = fork();
            if (pid == 0)
            {
                // child
                int status = (*phandler_)(pserverep_, peer_address, buf, count);
                pserverep_->close();
                exit(status);
            }
            else if (pid < 0)
            {
                // error in parent
                return(-1);
            }
        }
        else if (count == 0)
        {
             // end-of-file -- done.
             break;
        }
        else
        {
            // error. ignore interrupts
            if (errno == EINTR)
                continue;
            else
                return(-1);
        }
    }

    return(0);
}

int
MultiProcessDatagramServer::finish()
{
    return(pserverep_->close());
}

// child process server ctor and dtor
MultiProcessStreamServer::SubServer::SubServer(
    EndPoint *psubserverep, 
    MultiProcessStreamServer::Handler *phandler):
        Server(), psubserverep_(psubserverep), phandler_(phandler)
{
    if (psubserverep_->isOk() && phandler_->isOk())
        setOk(true);
    else
        setOk(false);
}

MultiProcessStreamServer::SubServer::~SubServer()
{
    setOk(false);
}

// operations
int
MultiProcessStreamServer::SubServer::init()
{
    return(0);
}

int
MultiProcessStreamServer::SubServer::run()
{
    if (isNotOk()) return(-1);
    int status;
    while ((status = (*phandler_)(psubserverep_)) == 0) ;
    return(status < 0 ? -1 : 0);
}

int
MultiProcessStreamServer::SubServer::finish()
{
    return(psubserverep_->close());
}

// ctor and dtor
MultiProcessStreamServer::MultiProcessStreamServer(
    EndPoint *pserverep, 
    MultiProcessStreamServer::Handler *phandler):
        Server(), pserverep_(pserverep), phandler_(phandler)
{
    if (pserverep_->isOk() && phandler_->isOk())
        setOk(true);
    else
        setOk(false);
}

MultiProcessStreamServer::~MultiProcessStreamServer()
{
    setOk(false);
}

// operations
int
MultiProcessStreamServer::init()
{
    if ((pserverep_->setReuseAddr(true) != 0) ||
        (pserverep_->bind() != 0) || 
        (pserverep_->listen() != 0))
        return(-1);
    else
        return(0);
}

int
MultiProcessStreamServer::run()
{
    if (isNotOk()) return(-1);

    UseCntPtr<EndPoint> peer;

    (void) signal(SIGCHLD, reaper);

    while ( 1 )
    {
        bool retry = false;
        peer = pserverep_->accept(retry);
        if (peer == NULL)
        {
            if (retry)
                continue; // ignore interrupt from child HANGUP
            else
                return(-1);
        }

        int pid = fork();
        if (pid == 0)
        {
            int status;

            // child -- close parent socket
            pserverep_->close();

            // let handler do its work
            SubServer child(peer, phandler_);
            if ((status = child.init()) != 0) exit(status);
            if ((status = child.run()) != 0) exit(status);
            if ((status = child.finish()) != 0) exit(status);

            // all done
            exit(0);
        }
        else if (pid < 0)
        {
            // error in parent
            peer->close();
            return(-1);
        }
        else
        {
            // close child socket in parent
            peer->close();
        }
    }

    return(0);
}

int
MultiProcessStreamServer::finish()
{
    return(pserverep_->close());
}

}
