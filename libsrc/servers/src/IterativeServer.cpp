//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// iterative server class

// headers
#include "hdr/IterativeServer.h"

namespace ombt {

// ctors and dtor
IterativeDatagramServer::IterativeDatagramServer(
    EndPoint *serverep, 
    IterativeDatagramServer::Handler *handler):
        Server(), pserverep_(serverep), phandler_(handler)
{
    if (pserverep_->isOk() && phandler_->isOk())
        setOk(true);
    else
        setOk(false);
}

IterativeDatagramServer::~IterativeDatagramServer()
{
    setOk(false);
}

// operations
int
IterativeDatagramServer::init()
{
    if ((pserverep_->setReuseAddr(true) != 0) ||
        (pserverep_->bind() != 0))
    {
        return(-1);
    }
    else
    {
        return(0);
    }
}

int
IterativeDatagramServer::run()
{
    if (isOk()) return(-1);

    char buf[BUFSIZ+1];
    size_t count = BUFSIZ;

    UseCntPtr<Address> peer_address(pserverep_->getInternalAddress().create());

    while (pserverep_->read(buf, count, *peer_address) > 0)
    {
        int status = (*phandler_)(pserverep_, peer_address, buf, count);
        if (status < 0)
        {
            // error of some type
            return(-1);
        }
        else if (status > 0)
        {
            // done, just exit.
            break;
        }
    }

    return(0);
}

int
IterativeDatagramServer::finish()
{
    return(pserverep_->close());
}

// ctor and dtor
IterativeStreamServer::IterativeStreamServer(
    EndPoint *serverep, 
    IterativeStreamServer::Handler *handler):
        Server(), pserverep_(serverep), phandler_(handler)
{
    if (pserverep_->isOk() && phandler_->isOk())
        setOk(true);
    else
        setOk(false);
}

IterativeStreamServer::~IterativeStreamServer()
{
    setOk(false);
}

// operations
int
IterativeStreamServer::init()
{
    if ((pserverep_->setReuseAddr(true) != 0) ||
        (pserverep_->bind() != 0) || 
        (pserverep_->listen() != 0))
    {
        return(-1);
    }
    else
    {
        return(0);
    }
}

int
IterativeStreamServer::run()
{
    if (isNotOk()) return(-1);

    UseCntPtr<EndPoint> peer;

    while ( 1 )
    {
        bool retry = false;
        peer = pserverep_->accept(retry);
        if (peer.ptr() == NULL)
        {
            if (retry)
            {
                continue;
            }
            else
            {
                return(-1);
            }
        }

        (void) (*phandler_)(peer);

        peer->close();
    }

    return(0);
}

int
IterativeStreamServer::finish()
{
    return(pserverep_->close());
}

}
