//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_ITERATIVE_SERVER_H
#define __OMBT_ITERATIVE_SERVER_H

// iterative single-service server

// headers
#include <stdio.h>
#include <errno.h>
#include "system/Debug.h"
#include "servers/Server.h"
#include "sockets/Address.h"
#include "sockets/EndPoint.h"
#include "atomic/UCBaseObject.h"
#include "atomic/UseCntPtr.h"

namespace ombt {

// iterative server for a connection
class IterativeDatagramServer: public Server
{
public:
    // message handler
    class Handler: public UCBaseObject {
    public:
        // ctor and dtor
        Handler(): UCBaseObject(true) { }
        virtual ~Handler() { };
    
        // message handler
        //
        // return == 0 - ok and continue
        // return < 0 - error and terminate
        // return > 0 - ok, but terminate
        virtual int operator()(EndPoint *serverep, Address *peer_address, void *buffer, int count) { return(1); };
    };

    // ctors and dtor
    IterativeDatagramServer(EndPoint *serverep, Handler *handler);
    virtual ~IterativeDatagramServer();

    // operations
    virtual int init();
    virtual int run();
    virtual int finish();

private:
    // leper colony
    IterativeDatagramServer(const IterativeDatagramServer &src);
    IterativeDatagramServer &operator=(const IterativeDatagramServer &rhs);

protected:
    // track handlers
    UseCntPtr<EndPoint> pserverep_;
    UseCntPtr<Handler> phandler_;
};

class IterativeStreamServer: public Server
{
public:
    // message handler
    class Handler: public UCBaseObject {
    public:
        // ctor and dtor
        Handler(): UCBaseObject(true) { }
        virtual ~Handler() { }
    
        // message handler
        //
        // return == 0 - ok and continue
        // return < 0 - error and terminate
        // return > 0 - ok, but terminate
        virtual int operator()(EndPoint *peer) { return(1); }
    };

    // ctors and dtor
    IterativeStreamServer(EndPoint *serverep, Handler *handler);
    virtual ~IterativeStreamServer();

    // operations
    virtual int init();
    virtual int run();
    virtual int finish();

private:
    // leper colony
    IterativeStreamServer(const IterativeStreamServer &src);
    IterativeStreamServer &operator=(const IterativeStreamServer &rhs);

protected:
    // track handlers
    UseCntPtr<EndPoint> pserverep_;
    UseCntPtr<Handler> phandler_;
};

}

#endif
