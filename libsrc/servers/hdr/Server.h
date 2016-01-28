//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_SERVER_H
#define __OMBT_SERVER_H

// local headers
#include <map>
#include "atomic/UCBaseObject.h"
#include "sockets/EndPoint.h"
#include "timers/Timer.h"

namespace ombt {

// server for a connection
class Server: public UCBaseObject
{
public:
    // reactor type
    enum MultiplexMode { None, Select, EPoll, Poll };
    enum { DefaultEPollSize = 128 };
    enum { DefaultEPollEventsToHandle = 128 };

    // ctors and dtor
    Server(): UCBaseObject(true) { }
    virtual ~Server() { };

    // operations
    virtual int init() { return(-1); }
    virtual int run() { return(-1); }
    virtual int finish() { return(-1); }

    // set a timer for an end point
    virtual void scheduleTimer(EndPoint *, Timer &) { }
    virtual void cancelTimer(EndPoint *, Timer &) { }

private:
    // leper colony
    Server(const Server &src);
    Server &operator=(const Server &rhs);

protected:
    bool done_;
};

}

#endif
