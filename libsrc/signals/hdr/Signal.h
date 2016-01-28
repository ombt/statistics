//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_SIGNAL_H
#define __OMBT_SIGNAL_H

// system headers
#include <signal.h>
#include <pthread.h>
#include <signal.h>
#include <sys/signalfd.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/UCBaseObject.h"
#include "atomic/UseCntPtr.h"
#include "sockets/Address.h"
#include "sockets/Socket.h"
#include "sockets/EndPoint.h"

namespace ombt {

// forward declaration
class Signal;
class EndPoint;

// signal class
class Signal: public Socket {
public:
    // signal mask
    class Mask {
    public:
        // ctor dtor
        Mask();
        Mask(const Mask &src);
        ~Mask();
    
        // assignment
        Mask &operator=(const Mask &rhs);

        // operations
        void set(int signalno);
        void clear(int signalno);
        void clearall();
        bool isMember(int signalno);
        int block();
        int unblock();

        // data
        sigset_t mask_;
    };

    // ctors and dtor
    Signal();
    Signal(const Mask &mask);
    virtual ~Signal();

    // basic socket operations
    virtual int setReuseAddr(bool onoff);
    virtual int setNonBlocking(bool onoff);
    virtual int bind(const Address &address);
    virtual int listen(int backlog = DefaultQueueSize);
    virtual EndPoint *accept(bool &retry);
    virtual int connect(Address &address);
    virtual int read(void *inbuffer, size_t count);
    virtual int read(void *inbuffer, size_t count, Address &peer_addr);
    virtual int write(const void *outbuffer, size_t count);
    virtual int write(const void *outbuffer, size_t count, const Address &peer_addr);
    virtual int readn(void *inbuffer, size_t count);
    virtual int writen(const void *outbuffer, size_t count);
    virtual int open();
    virtual int close();

    // specific to signals
    void block();
    void unblock();

private:
    // not allowed
    Signal(const Signal &src);
    Signal &operator=(const Signal &rhs);

protected:
    Mask mask_;
};

// connection class - socket and address combined.
class SignalEndPoint: public EndPoint {
public:
    SignalEndPoint(Socket *sock = NULL);
    virtual ~SignalEndPoint();

    // socket-base operations 
    virtual int bind();
    virtual int listen(int backlog = Socket::DefaultQueueSize);
    virtual EndPoint *accept(bool &retry);
    virtual int connect(Address &address);
    //virtual int read(void *inbuffer, size_t count);
    virtual int read(void *inbuffer, size_t count, Address &peer_addr);
    virtual int write(void *outbuffer, size_t count);
    virtual int write(void *outbuffer, size_t count, const Address &peer_addr);
    //virtual int readn(void *inbuffer, size_t count);
    virtual int writen(void *outbuffer, size_t count);
    //virtual int close();
    //virtual int open();

    // specific to signal end points;
    void block();
    void unblock();

private:
    // leper colony
    SignalEndPoint(const SignalEndPoint &src);
    SignalEndPoint &operator=(const SignalEndPoint &rhs);
};

}

#endif 

