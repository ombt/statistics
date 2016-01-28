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
#include "hdr/Signal.h"
#include "logging/Logging.h"

namespace ombt {

// signal mask class

// ctor and dtor
Signal::Mask::Mask()
{
    ::sigemptyset(&mask_);
}
Signal::Mask::Mask(const Signal::Mask &src)
{
    ::sigemptyset(&mask_);
    ::memcpy(&mask_, &src.mask_, sizeof(src.mask_));
}
Signal::Mask::~Mask()
{
    ::sigemptyset(&mask_);
}

// assignment
Signal::Mask &
Signal::Mask::operator=(const Signal::Mask &rhs)
{
    if (this != &rhs)
    {
        ::sigemptyset(&mask_);
        ::memcpy(&mask_, &rhs.mask_, sizeof(rhs.mask_));
    }
    return *this;
}

// operations
void
Signal::Mask::set(int signalno)
{
    ::sigaddset(&mask_, signalno);
}
void
Signal::Mask::clear(int signalno)
{
    ::sigdelset(&mask_, signalno);
}
void
Signal::Mask::clearall()
{
    ::sigemptyset(&mask_);
}
int
Signal::Mask::block()
{
    //return::sigprocmask(SIG_BLOCK, &mask_, NULL);
    return::pthread_sigmask(SIG_BLOCK, &mask_, NULL);
}
int
Signal::Mask::unblock()
{
    //return::sigprocmask(SIG_UNBLOCK, &mask_, NULL);
    return::pthread_sigmask(SIG_UNBLOCK, &mask_, NULL);
}

bool
Signal::Mask::isMember(int signalno)
{
    return(::sigismember(&mask_, signalno) == 1);
}

// ctors and dtor
Signal::Signal(): 
    Socket(), mask_()
{
}
Signal::Signal(const Signal::Mask &mask): 
    Socket(), mask_(mask)
{
    mask_.block();
    socket_ = ::signalfd(-1, &mask_.mask_, 0);
    setOk(socket_ != NOTOK);
}
Signal::~Signal()
{
    if (socket_ != NOTOK)
    {
        mask_.unblock();
        ::close(socket_);
        socket_ = NOTOK;
        setOk(false);
    }
}

// basic socket operations
int Signal::setReuseAddr(bool onoff)
{
    return NOTOK;
}
int Signal::setNonBlocking(bool onoff)
{
    if (isNotOk()) return NOTOK;
    int flag = ::fcntl(socket_, F_GETFL, 0);
    if (onoff)
        flag |= O_NONBLOCK;
    else
        flag &= ~O_NONBLOCK;
    return(::fcntl(socket_, F_SETFL, flag));
}
int Signal::bind(const Address &address)
{
    return NOTOK;
}
int Signal::listen(int backlog)
{
    return NOTOK;
}
EndPoint *Signal::accept(bool &retry)
{
    return NULL;
}
int Signal::connect(Address &address)
{
    return NOTOK;
}
int Signal::read(void *inbuffer, size_t count)
{
    if (isNotOk()) return NOTOK;
    if (count < sizeof(signalfd_siginfo))
    {
        errno = EINVAL;
        return(NOTOK);
    }
    return ::read(socket_, inbuffer, sizeof(signalfd_siginfo));
}
int Signal::read(void *inbuffer, size_t count, Address &peer_addr)
{
    return NOTOK;
}
int Signal::write(const void *outbuffer, size_t count)
{
    return NOTOK;
}
int Signal::write(const void *outbuffer, size_t count, const Address &peer_addr)
{
    return NOTOK;
}
int Signal::readn(void *inbuffer, size_t count)
{
    if (isNotOk()) return NOTOK;
    if (count < sizeof(signalfd_siginfo))
    {
        errno = EINVAL;
        return(NOTOK);
    }
    return ::read(socket_, inbuffer, sizeof(signalfd_siginfo));
}
int Signal::writen(const void *outbuffer, size_t count)
{
    return NOTOK;
}
int Signal::open()
{
    mask_.block();
    socket_ = ::signalfd(-1, &mask_.mask_, 0);
    setOk(socket_ != NOTOK);
    return (socket_ != NOTOK) ? OK : NOTOK;
}
int Signal::close()
{
    if (socket_ != NOTOK)
    {
        mask_.unblock();
        ::close(socket_);
        socket_ = NOTOK;
        setOk(false);
    }
    return OK;
}

// specific to signals
void Signal::block()
{
    mask_.block();
}
void Signal::unblock()
{
    mask_.unblock();
}

// connection class - socket and address combined.
SignalEndPoint::SignalEndPoint(Socket *sock):
    EndPoint(sock, NULL)
{
}
SignalEndPoint::~SignalEndPoint()
{
}

// socket-base operations
int SignalEndPoint::bind()
{
    return NOTOK;
}
int SignalEndPoint::listen(int backlog)
{
    return NOTOK;
}
EndPoint *SignalEndPoint::accept(bool &retry)
{
    return NULL;
}
int SignalEndPoint::connect(Address &address)
{
    return NOTOK;
}
int SignalEndPoint::read(void *inbuffer, size_t count, Address &peer_addr)
{
    return NOTOK;
}
int SignalEndPoint::write(void *outbuffer, size_t count)
{
    return NOTOK;
}
int SignalEndPoint::write(void *outbuffer, size_t count, const Address &peer_addr)
{
    return NOTOK;
}
int SignalEndPoint::writen(void *outbuffer, size_t count)
{
    return NOTOK;
}

// signal operations
void SignalEndPoint::block()
{
    Signal *psig = dynamic_cast<Signal *>(socket_.ptr());
    if (psig) psig->block();
}
void SignalEndPoint::unblock()
{
    Signal *psig = dynamic_cast<Signal *>(socket_.ptr());
    if (psig) psig->unblock();
}

}

