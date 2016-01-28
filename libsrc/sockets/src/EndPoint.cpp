//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// connection class - socket and address combined

// headers
#include "system/Debug.h"
#include "logging/Logging.h"
#include "sockets/Socket.h"
#include "sockets/EndPoint.h"

namespace ombt {

// ctors and dtor
EndPoint::EndPoint(Socket *sock, Address *addr):
    UCBaseObject(NOTOK), 
    socket_(sock), address_(addr)
{
    if (socket_.ptr() != NULL && address_.ptr() != NULL)
        setOk(socket_->isOk() && address_->isOk());
    else if (socket_.ptr() != NULL && address_.ptr() == NULL)
        setOk(socket_->isOk());
    else
        setOk(false);
}

EndPoint::~EndPoint()
{
    setOk(false);
}

// accessor function
bool
EndPoint::isOk() const
{
    if (socket_.ptr() != NULL && address_.ptr() != NULL)
        const_cast<EndPoint *>(this)->setOk(socket_->isOk() && address_->isOk());
    else if (socket_.ptr() != NULL && address_.ptr() == NULL)
        const_cast<EndPoint *>(this)->setOk(socket_->isOk());
    else
        const_cast<EndPoint *>(this)->setOk(false);
    return(getOk());
}

int 
EndPoint::getSocket() const
{
    if (isOk() && socket_.ptr() != NULL)
        return(socket_->getSocket());
    else
        return(NOTOK);
}

Address &
EndPoint::getInternalAddress()
{
    return(*address_);
}


// socket operations
int
EndPoint::bind()
{
    if (isOk() && socket_.ptr() != NULL && address_.ptr() != NULL)
        return(socket_->bind(*address_));
    else
        return(NOTOK);
}

int
EndPoint::listen(int backlog)
{
    if (isOk() && socket_.ptr() != NULL && address_.ptr() != NULL)
        return(socket_->listen(backlog));
    else
        return(NOTOK);
}

EndPoint *
EndPoint::accept(bool &retry)
{
    retry = false;
    if (isOk() && socket_.ptr() != NULL && address_.ptr() != NULL)
        return(socket_->accept(retry));
    else
        return(NULL);
}

int
EndPoint::connect(Address &address)
{
    if (isOk() && socket_.ptr() != NULL)
        return(socket_->connect(address));
    else
        return(NOTOK);
}

int
EndPoint::read(void *inbuffer, size_t count)
{
    if (isOk() && socket_.ptr() != NULL)
        return(socket_->read(inbuffer, count));
    else
        return(NOTOK);
}

int
EndPoint::readn(void *inbuffer, size_t count)
{
    if (isOk() && socket_.ptr() != NULL)
        return(socket_->readn(inbuffer, count));
    else
        return(NOTOK);
}

int
EndPoint::read(void *inbuffer, size_t count, Address &peer_address)
{
    if (isOk() && socket_.ptr() != NULL)
        return(socket_->read(inbuffer, count, peer_address));
    else
        return(NOTOK);
}

int
EndPoint::write(void *outbuffer, size_t count)
{
    if (isOk() && socket_.ptr() != NULL)
        return(socket_->write(outbuffer, count));
    else
        return(NOTOK);
}

int
EndPoint::writen(void *outbuffer, size_t count)
{
    if (isOk() && socket_.ptr() != NULL)
        return(socket_->writen(outbuffer, count));
    else
        return(NOTOK);
}

int
EndPoint::write(void *outbuffer, size_t count, const Address &peer_address)
{
    if (isOk() && socket_.ptr() != NULL)
        return(socket_->write(outbuffer, count, peer_address));
    else
        return(NOTOK);
}

int
EndPoint::close()
{
    if (isOk() && socket_.ptr() != NULL)
        return(socket_->close());
    else
        return(NOTOK);
}

int
EndPoint::open()
{
    if (isOk() && socket_.ptr() != NULL)
        return(socket_->open());
    else
        return(NOTOK);
}

}
