//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_SOCKET_H
#define __OMBT_SOCKET_H

// system headers
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/UCBaseObject.h"
#include "sockets/Address.h"
#include "sockets/InetAddress.h"
#include "sockets/LocalAddress.h"

namespace ombt {

// forward declaration
class EndPoint;

// socket class
class Socket: public UCBaseObject {
public:
    enum {
         DefaultQueueSize = 5,
    };

    Socket();
    Socket(int socket);
    Socket(int domain, int type, int protocol);
    Socket(int socket, int domain, int type, int protocol);
    virtual ~Socket();

    // comparison
    bool operator<(const Socket &rhs) {
       return(socket_ < rhs.socket_);
    }

    // accessor
    int getSocket() const { return(socket_); }

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

    // set/clear FS sets
    void setFDSet(fd_set &fdset) const {
        FD_SET(socket_, &fdset);
    }
    void clearFDSet(fd_set &fdset) const {
        FD_CLR(socket_, &fdset);
    }

protected:
    inline bool isSocketSane() {
        if (isNotOk())
        {
            return(false);
        }
        else if (socket_ < 0)
        {
            setOk(false);
            return(false);
        }
        return(true);
    }

private:
    // not allowed
    Socket(const Socket &src);
    Socket &operator=(const Socket &rhs);

protected:
    int socket_;
    int domain_;
    int type_;
    int protocol_;
};

// TCP IPv4 socket class
class TcpSocket: public Socket {
public:
    TcpSocket();
    TcpSocket(int socket);
    ~TcpSocket();

    // basic socket operations
    int listen(int backlog);
    EndPoint *accept(bool &retry);

private:
    // not allowed
    TcpSocket(const TcpSocket &src);
    TcpSocket &operator=(const TcpSocket &rhs);
};

// UDP IPv4 socket class
class UdpSocket: public Socket {
public:
    UdpSocket();
    UdpSocket(int socket);
    ~UdpSocket();

private:
    // not allowed
    UdpSocket(const UdpSocket &src);
    UdpSocket &operator=(const UdpSocket &rhs);
};

// Local/Unix stream socket class
class StreamSocket: public Socket {
public:
    StreamSocket();
    StreamSocket(int socket);
    ~StreamSocket();

    // basic socket operations
    int listen(int backlog);
    EndPoint *accept(bool &retry);

private:
    // not allowed
    StreamSocket(const StreamSocket &src);
    StreamSocket &operator=(const StreamSocket &rhs);
};

// Local/Unix datagram socket class
class DatagramSocket: public Socket {
public:
    DatagramSocket();
    DatagramSocket(int socket);
    ~DatagramSocket();

private:
    // not allowed
    DatagramSocket(const DatagramSocket &src);
    DatagramSocket &operator=(const DatagramSocket &rhs);
};

}

#endif
