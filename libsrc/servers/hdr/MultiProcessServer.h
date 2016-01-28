//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_MULTI_PROCESS_SERVER_H
#define __OMBT_MULTI_PROCESS_SERVER_H

// multiprocess single-service server

// headers
#include <stdio.h>
#include "atomic/UCBaseObject.h"
#include "servers/Server.h"
#include "sockets/Address.h"
#include "sockets/EndPoint.h"

namespace ombt {

// forking server for a connection
class MultiProcessDatagramServer: public Server
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
    MultiProcessDatagramServer(EndPoint *serverep, Handler *handler);
    virtual ~MultiProcessDatagramServer();

    // operations
    virtual int init();
    virtual int run();
    virtual int finish();

private:
    // leper colony
    MultiProcessDatagramServer(const MultiProcessDatagramServer &src);
    MultiProcessDatagramServer &operator=(const MultiProcessDatagramServer &rhs);

protected:
    // track handlers
    UseCntPtr<EndPoint> pserverep_;
    UseCntPtr<Handler> phandler_;
};

class MultiProcessStreamServer: public Server
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

    // client process
    class SubServer: public Server
    {
    public:
        // ctors and dtor
        SubServer(EndPoint *subserverep, Handler *handler);
        virtual ~SubServer();

        // operations
        virtual int init();
        virtual int run();
        virtual int finish();

    protected:
        // data
        UseCntPtr<EndPoint> psubserverep_;
        UseCntPtr<Handler> phandler_;
    };

    // ctors and dtor
    MultiProcessStreamServer(EndPoint *serverep, Handler *handler);
    virtual ~MultiProcessStreamServer();

    // operations
    virtual int init();
    virtual int run();
    virtual int finish();

private:
    // leper colony
    MultiProcessStreamServer(const MultiProcessStreamServer &src);
    MultiProcessStreamServer &operator=(const MultiProcessStreamServer &rhs);

protected:
    // track handlers
    UseCntPtr<EndPoint> pserverep_;
    UseCntPtr<Handler> phandler_;
};

}

#endif
