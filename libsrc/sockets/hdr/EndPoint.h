//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_CONNECTION_H
#define __OMBT_CONNECTION_H

// system headers
#include <string>
#include <iostream>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/UCBaseObject.h"
#include "atomic/UseCntPtr.h"
#include "sockets/Address.h"
#include "sockets/Socket.h"

namespace ombt {

// connection class - socket and address combined.
class EndPoint: public UCBaseObject {
public:
    EndPoint(Socket *sock = NULL, Address *addr = NULL);
    virtual ~EndPoint();

    // for maps
    bool operator<(const EndPoint &rhs) const {
        return(socket_ < rhs.socket_);
    }

    // internal status
    bool isOk() const;

    // accessor methods
    inline const sockaddr *getSockAddr() const {
        return(address_->getSockAddr());
    };
    inline socklen_t getSockAddrLen() const {
        return(address_->getSockAddrLen());
    };
    inline const std::string &getAddress() const {
        return(address_->getAddress());
    };

    int getSocket() const;
    Address &getInternalAddress();

    // socket-base operations 
    virtual int setReuseAddr(bool onoff) {
        return(socket_->setReuseAddr(onoff));
    }
    virtual int setNonBlocking(bool onoff) {
        return(socket_->setNonBlocking(onoff));
    }
    virtual int bind();
    virtual int listen(int backlog = Socket::DefaultQueueSize);
    virtual EndPoint *accept(bool &retry);
    virtual int connect(Address &address);
    virtual int read(void *inbuffer, size_t count);
    virtual int read(void *inbuffer, size_t count, Address &peer_addr);
    virtual int write(void *outbuffer, size_t count);
    virtual int write(void *outbuffer, size_t count, const Address &peer_addr);
    virtual int readn(void *inbuffer, size_t count);
    virtual int writen(void *outbuffer, size_t count);
    virtual int close();
    virtual int open();

    friend std::ostream &operator<<(std::ostream &os, EndPoint &) {
        return(os);
    }

private:
    // leper colony
    EndPoint(const EndPoint &src);
    EndPoint &operator=(const EndPoint &rhs);

protected:
    UseCntPtr<Socket> socket_;
    UseCntPtr<Address> address_;
};

}

#endif 

