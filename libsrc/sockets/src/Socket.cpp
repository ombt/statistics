//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// socket class

// system headers
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

// local headers
#include "hdr/Socket.h"
#include "hdr/EndPoint.h"

namespace ombt {

// base socket ctor and dtor
Socket::Socket():
    UCBaseObject(NOTOK),
    socket_(-1), domain_(-1), 
    type_(-1), protocol_(-1)
{
    // do nothing
}

Socket::Socket(int socket):
    UCBaseObject(OK),
    socket_(socket), domain_(-1), 
    type_(-1), protocol_(-1)
{
    // do nothing
}

Socket::Socket(int domain, int type, int protocol):
    UCBaseObject(NOTOK),
    socket_(-1), domain_(domain), 
    type_(type), protocol_(protocol)
{
    socket_ = ::socket(domain_, type_, protocol_);
    setOk(socket_ >= 0);
}

Socket::Socket(int socket, int domain, int type, int protocol):
    UCBaseObject(OK),
    socket_(socket), domain_(domain), 
    type_(type), protocol_(protocol)
{
    setOk(socket_ >= 0);
}

Socket::~Socket()
{
    // call close explicitly
    socket_ = -1;
    domain_ = -1;
    type_ = -1;
    protocol_ = -1;
    setOk(false);
}

// basic socket operations
int
Socket::setReuseAddr(bool onoff)
{
    if (isNotOk()) return(NOTOK);
    int flag = onoff ? 1 : 0;
    return(::setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)));
}

int
Socket::setNonBlocking(bool onoff)
{
    if (isNotOk()) return(NOTOK);
    int flag = ::fcntl(socket_, F_GETFL, 0);
    if (onoff)
        flag |= O_NONBLOCK;
    else
        flag &= ~O_NONBLOCK;
    return(::fcntl(socket_, F_SETFL, flag));
}

int
Socket::bind(const Address &address)
{
    if (address.isOk() && isOk() && 
        ::bind(socket_, address.getSockAddr(), 
               address.getSockAddrLen()) == 0)
    {
        setOk(true);
        return(0);
    }
    else
    {
        setOk(false);
        return(NOTOK);
    }
}

int
Socket::listen(int backlog)
{
    return(NOTOK);
}

EndPoint *
Socket::accept(bool &retry)
{
    retry = false;
    return(NULL);
}

int
Socket::connect(Address &address)
{
    if (isOk())
        return(::connect(socket_, 
                         address.getSockAddr(), 
                         address.getSockAddrLen()));
    else
        return(NOTOK);
}

int
Socket::read(void *inbuffer, size_t count)
{
    if (isOk())
        return(::read(socket_, inbuffer, count));
    else
        return(NOTOK);
}

int
Socket::read(void *inbuffer, size_t count, Address &peer_addr)
{
    if (isOk())
        return(::recvfrom(socket_, inbuffer, count, 0,
                          peer_addr.getSockAddr(), 
                          peer_addr.getSockAddrLenRef()));
    else
        return(NOTOK);
}

int
Socket::write(const void *outbuffer, size_t count)
{
    if (isOk())
        return(::write(socket_, outbuffer, count));
    else
        return(NOTOK);
}

int
Socket::write(const void *outbuffer, size_t count, const Address &peer_addr)
{
    if (isOk())
        return(::sendto(socket_, outbuffer, count, 0,
                        peer_addr.getSockAddr(), 
                        peer_addr.getSockAddrLen()));
    else
        return(NOTOK);
}

int
Socket::readn(void *inbuffer, size_t count)
{
    if (isNotOk()) return(NOTOK);

    char *ptr = (char *)inbuffer;
    size_t nleft = count;

    size_t nread;
    while (nleft > 0)
    {
        if ((nread = ::read(socket_, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
                nread = 0;
            else
                return(NOTOK);
        }
        else if (nread == 0)
        {
            break;
        }

        nleft -= nread;
        ptr += nread;
    }

    return(count - nread);
}

int
Socket::writen(const void *outbuffer, size_t count)
{
    if (isNotOk()) return(NOTOK);

    char *ptr = (char *)outbuffer;
    size_t nleft = count;

    size_t nwritten;
    while (nleft > 0)
    {
        if ((nwritten = ::write(socket_, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
                nwritten = 0;
            else
                return(NOTOK);
        }

        nleft -= nwritten;
        ptr += nwritten;
    }

    return(count);
}

int
Socket::open()
{
    if (socket_ != -1)
    {
        errno = EISCONN;
        return(NOTOK);
    }
    socket_ = ::socket(domain_, type_, protocol_);
    if (socket >= 0)
    {
        setOk(true);
        return(0);
    }
    else
    {
        socket_ = -1;
        setOk(false);
        return(NOTOK);
    }
}

int
Socket::close()
{
    if (isOk() && ::close(socket_) == 0)
    {
        socket_ = -1;
        setOk(false);
        return(0);
    }
    else
    {
        setOk(false);
        return(NOTOK);
    }
}

// TCP socket ctor and dtor
TcpSocket::TcpSocket():
    Socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)
{
    // do nothing
}

TcpSocket::TcpSocket(int socket):
    Socket(socket, PF_INET, SOCK_STREAM, IPPROTO_TCP)
{
    // do nothing
}

TcpSocket::~TcpSocket()
{
    // do nothing
}

// TCP-specific basic socket operations
int
TcpSocket::listen(int backlog)
{
    if (isOk() && backlog >= 0 && ::listen(socket_, backlog) == 0)
    {
        setOk(true);
        return(0);
    }
    else
    {
        return(NOTOK);
    }
}

EndPoint *
TcpSocket::accept(bool &retry)
{
    retry = false;
    sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    int client_socket = ::accept(socket_, (sockaddr *)&client_address,
                                 &client_address_len);
    if (client_socket < 0)
    {
        // during heavy traffic, the client may timeout
        // abort
        if (errno == EWOULDBLOCK || 
            errno == EINTR ||
            errno == ECONNABORTED) retry = true;
        return(NULL);
    }

    return(new EndPoint(new TcpSocket(client_socket), 
                        new InetAddress(client_address)));
}

// UDP socket ctor and dtor
UdpSocket::UdpSocket():
    Socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)
{
    // do nothing
}

UdpSocket::UdpSocket(int socket):
    Socket(socket, PF_INET, SOCK_DGRAM, IPPROTO_UDP)
{
    // do nothing
}

UdpSocket::~UdpSocket()
{
    // do nothing
}

// Local/Unix stream ctor and dtor
StreamSocket::StreamSocket():
    Socket(PF_LOCAL, SOCK_STREAM, 0)
{
    // do nothing
}

StreamSocket::StreamSocket(int socket):
    Socket(socket, PF_LOCAL, SOCK_STREAM, 0)
{
    // do nothing
}

StreamSocket::~StreamSocket()
{
    // do nothing
}

// unix-specific basic socket operations
int
StreamSocket::listen(int backlog)
{
    if (isOk() && backlog >= 0 && ::listen(socket_, backlog) == 0)
    {
        setOk(true);
        return(0);
    }
    else
    {
        return(NOTOK);
    }
}

EndPoint *
StreamSocket::accept(bool &retry)
{
    retry = false;
    sockaddr_un client_address;
    socklen_t client_address_len = sizeof(client_address);
    int client_socket = ::accept(socket_, (sockaddr *)&client_address,
                                 &client_address_len);
    if (client_socket < 0)
    {
        // during heavy traffic, the client may timeout
        // abort
        if (errno == EWOULDBLOCK ||
            errno == ECONNABORTED) retry = true;
        return(NULL);
    }

    return(new EndPoint(new StreamSocket(client_socket), 
                        new LocalAddress(client_address)));
}

// Local/Unix datagram ctor and dtor
DatagramSocket::DatagramSocket():
    Socket(PF_LOCAL, SOCK_DGRAM, 0)
{
    // do nothing
}

DatagramSocket::DatagramSocket(int socket):
    Socket(socket, PF_LOCAL, SOCK_DGRAM, 0)
{
    // do nothing
}

DatagramSocket::~DatagramSocket()
{
    // do nothing
}

}
