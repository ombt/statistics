//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//

// iterative multi-service server

// system headers
#include <sys/epoll.h>
#include <stdint.h>

// local headers
#include "hdr/IterativeReactor.h"

namespace ombt {

// iterative server for a connection base handler classes
IterativeReactor::BaseHandler::BaseHandler(): 
    UCBaseObject(OK), pserver_(NULL) 
{
    // do nothing
}

IterativeReactor::BaseHandler::~BaseHandler() 
{
    // do nothing
}
void 
IterativeReactor::BaseHandler::setServer(IterativeReactor *pserver) 
{
    pserver_ = pserver;
} 

// handle accepts
IterativeReactor::BaseAcceptHandler::BaseAcceptHandler():
    BaseHandler()
{
    // do nothing
}

IterativeReactor::BaseAcceptHandler::~BaseAcceptHandler()
{
    // do nothing
}

int
IterativeReactor::BaseAcceptHandler::handler(
    EndPoint *serverep, EndPoint *acceptep)
{
    return(1);
}

// handle reads, writes and i/o exceptions
IterativeReactor::BaseIOHandler::BaseIOHandler():
    BaseHandler()
{
    // do nothing
}

IterativeReactor::BaseIOHandler::~BaseIOHandler()
{
    // do nothing
}

int
IterativeReactor::BaseIOHandler::handler(EndPoint *serverep)
{
    return(1);
}

// handle signals
IterativeReactor::BaseSignalHandler::BaseSignalHandler():
    BaseHandler()
{
    // do nothing
}

IterativeReactor::BaseSignalHandler::~BaseSignalHandler()
{
    // do nothing
}

int
IterativeReactor::BaseSignalHandler::handler(
    EndPoint *serverep, int signalno)
{
    return(1);
}

// handle timers
IterativeReactor::BaseTimerHandler::BaseTimerHandler():
    BaseHandler()
{
    // do nothing
}

IterativeReactor::BaseTimerHandler::~BaseTimerHandler()
{
    // do nothing
}

int
IterativeReactor::BaseTimerHandler::handler(
    EndPoint *serverep, Timer timer)
{
    return(1);
}

//////////////////////////////////////////////////////////////////////////

IterativeReactor::InternalBaseHandler::InternalBaseHandler(IterativeReactor *pserver): 
    UCBaseObject(OK), pserver_(pserver) 
{
    // do nothing
}

IterativeReactor::InternalBaseHandler::~InternalBaseHandler() 
{
    // do nothing
}

int
IterativeReactor::InternalBaseHandler::handler(EndPoint *serverep)
{
    return(1);
}

// handle accepts
IterativeReactor::InternalAcceptHandler::InternalAcceptHandler(
    IterativeReactor *pserver, BaseAcceptHandler *phandler):
    InternalBaseHandler(pserver), phandler_(phandler)
{
    // do nothing
}

IterativeReactor::InternalAcceptHandler::~InternalAcceptHandler()
{
    // do nothing
}

int
IterativeReactor::InternalAcceptHandler::handler(EndPoint *serverep)
{
    return pserver_->acceptHandler(phandler_, serverep);
}

// handle reads, writes and i/o exceptions
IterativeReactor::InternalIOHandler::InternalIOHandler(
    IterativeReactor *pserver, BaseIOHandler *phandler):
    InternalBaseHandler(pserver), phandler_(phandler)
{
    // do nothing
}

IterativeReactor::InternalIOHandler::~InternalIOHandler()
{
    // do nothing
}

int
IterativeReactor::InternalIOHandler::handler(EndPoint *serverep)
{
    return pserver_->ioHandler(phandler_, serverep);
}

// handle signals
IterativeReactor::InternalSignalHandler::InternalSignalHandler(
    IterativeReactor *pserver, BaseSignalHandler *phandler):
    InternalBaseHandler(pserver), phandler_(phandler)
{
    // do nothing
}

IterativeReactor::InternalSignalHandler::~InternalSignalHandler()
{
    // do nothing
}

int
IterativeReactor::InternalSignalHandler::handler(EndPoint *serverep)
{
    return pserver_->signalHandler(phandler_, serverep);
}

// handle timers
IterativeReactor::InternalTimerHandler::InternalTimerHandler(
    IterativeReactor *pserver, BaseTimerHandler *phandler):
    InternalBaseHandler(pserver), phandler_(phandler)
{
    // do nothing
}

IterativeReactor::InternalTimerHandler::~InternalTimerHandler()
{
    // do nothing
}

int
IterativeReactor::InternalTimerHandler::handler(EndPoint *serverep)
{
    return pserver_->timerHandler(phandler_, serverep);
}

// timer queue item
IterativeReactor::TimerQueueItem::TimerQueueItem():
    timer_(), pendpoint_(), phandler_()
{
    // do nothing
}

IterativeReactor::TimerQueueItem::TimerQueueItem(
    Timer timer, EndPoint *pep, IterativeReactor::InternalBaseHandler *phandler):
    timer_(timer), pendpoint_(pep), phandler_(phandler)
{
    // do nothing
}

IterativeReactor::TimerQueueItem::TimerQueueItem(const TimerQueueItem &tqi):
    timer_(tqi.timer_), pendpoint_(tqi.pendpoint_), phandler_(tqi.phandler_)
{
    // do nothing
}

IterativeReactor::TimerQueueItem::~TimerQueueItem()
{
    // do nothing
}

// operators
IterativeReactor::TimerQueueItem &
IterativeReactor::TimerQueueItem::operator=(const TimerQueueItem &rhs)
{
    if (this != &rhs)
    {
        timer_ = rhs.timer_;
        pendpoint_ = rhs.pendpoint_;
        phandler_ = rhs.phandler_;
    }
    return(*this);
}

bool
IterativeReactor::TimerQueueItem::operator==(const TimerQueueItem &rhs) const
{
    return(timer_ == rhs.timer_);
}

bool
IterativeReactor::TimerQueueItem::operator!=(const TimerQueueItem &rhs) const
{
    return(timer_ != rhs.timer_);
}

bool
IterativeReactor::TimerQueueItem::operator<=(const TimerQueueItem &rhs) const
{
    return(timer_ <= rhs.timer_);
}

bool
IterativeReactor::TimerQueueItem::operator>=(const TimerQueueItem &rhs) const
{
    return(timer_ >= rhs.timer_);
}

bool
IterativeReactor::TimerQueueItem::operator<(const TimerQueueItem &rhs) const
{
    return(timer_ < rhs.timer_);
}

bool
IterativeReactor::TimerQueueItem::operator>(const TimerQueueItem &rhs) const
{
    return(timer_ > rhs.timer_);
}

//////////////////////////////////////////////////////////////////////////

// ctors and dtor
IterativeReactor::IterativeReactor(MultiplexMode mode, int epollsize):
    UCBaseObject(NOTOK), 
    done_(false), mode_(mode), epollfd_(-1), epollsize_(epollsize),
    epolleventflags_(), maxsocket_(-1), nreaders_(0), nwriters_(0), nexceptions_(0),
    endpoints_(), handlers_()
{
    setOk(false);

    if (mode_ == Select)
    {
        FD_ZERO(&master_readfds_);
        FD_ZERO(&master_writefds_);
        FD_ZERO(&master_exceptionfds_);
        setOk(true);
    }
    else if (mode_ == EPoll)
    {
        if ((epollsize_ > 0) &&
            ((epollfd_ = ::epoll_create(epollsize_)) >= 0))
        {
            setOk(true);
        }
    }
}

IterativeReactor::~IterativeReactor()
{
    if ((mode_ = EPoll) && (epollfd_ >= 0))
    {
        ::close(epollfd_);
    }    
    done_ = true;
    epollfd_ = -1;
    setOk(false);
}

// register and unregister end-points
int
IterativeReactor::registerEndPoint(EndPoint *pserver_ep)
{
    if (isNotOk()) return(NOTOK);

    UseCntPtr<EndPoint> pep(pserver_ep);
    int socket = pep->getSocket();
    if (endpoints_.find(socket) != endpoints_.end()) return(NOTOK);
    endpoints_[socket] = pep;
    if (socket > maxsocket_) maxsocket_ = socket;

    return(OK);
}

int
IterativeReactor::unregisterEndPoint(EndPoint *pserver_ep)
{
    if (isNotOk()) return(NOTOK);

    UseCntPtr<EndPoint> pep(pserver_ep);
    int socket = pep->getSocket();
    endpoints_.erase(socket);

    return(OK);
}

// register and unregister end-point handlers
int
IterativeReactor::registerWithMultiplexor(
    EndPoint *pserver_ep, int &counter, fd_set &master_fdset, uint32_t epollevent)
{
    if (isNotOk()) return(NOTOK);

    UseCntPtr<EndPoint> pep(pserver_ep);
    int socket = pep->getSocket();
    if (endpoints_.find(socket) == endpoints_.end()) return(NOTOK);

    if (mode_ == EPoll)
    {
        epoll_event ev;
        ev.data.fd = socket;

        EVENTFLAGSIter efit = epolleventflags_.find(socket);
        if (efit == epolleventflags_.end())
        {
            // first handler
            ev.events = epollevent;
            if (::epoll_ctl(epollfd_, EPOLL_CTL_ADD, socket, &ev) != OK) return(NOTOK);
            epolleventflags_[socket] = ev.events;
            counter += 1;
        }
        else
        {
            // a handler already exists
            if ((efit->second & epollevent) == 0)
            {
                // not monitored for reads: add it
                ev.events = efit->second | epollevent;
                if (::epoll_ctl(epollfd_, EPOLL_CTL_MOD, socket, &ev) != OK) return(NOTOK);
                epolleventflags_[socket] = ev.events;
                counter += 1;
            }
        }
    }
    else if (mode_ == Select)
    {
        if ( ! FD_ISSET(socket, &master_fdset))
        {
            FD_SET(socket, &master_fdset);
            counter += 1;
        }
    }
    else
    {
        return(NOTOK);
    }

    return(OK);
}

int
IterativeReactor::registerReaderWithMultiplexor(EndPoint *pserver_ep)
{
    return(registerWithMultiplexor(pserver_ep, nreaders_, master_readfds_, EPOLLIN));
}

int
IterativeReactor::registerWriterWithMultiplexor(EndPoint *pserver_ep)
{
    return(registerWithMultiplexor(pserver_ep, nwriters_, master_writefds_, EPOLLOUT));
}

int
IterativeReactor::registerExceptionWithMultiplexor(EndPoint *pserver_ep)
{
    return(registerWithMultiplexor(pserver_ep, nexceptions_, master_exceptionfds_, EPOLLPRI));
}

int
IterativeReactor::registerAcceptHandler(EndPoint *pserver_ep, BaseAcceptHandler *phandler)
{
    if (isNotOk() || (registerReaderWithMultiplexor(pserver_ep) != OK)) return(NOTOK);

    UseCntPtr<BaseHandler> pbh(phandler);
    pbh->setServer(this);

    UseCntPtr<EndPoint> pep(pserver_ep);
    UseCntPtr<InternalBaseHandler> pibh(new InternalAcceptHandler(this, phandler));
    handlers_[KeySocketOper(pep->getSocket(), KeySocketOper::Read)] = EPIBHPair(pep, pibh);

    return(OK);
}

int
IterativeReactor::registerReadHandler(EndPoint *pserver_ep, BaseIOHandler *phandler)
{
    if (isNotOk() || (registerReaderWithMultiplexor(pserver_ep) != OK)) return(NOTOK);

    UseCntPtr<BaseHandler> pbh(phandler);
    pbh->setServer(this);

    UseCntPtr<EndPoint> pep(pserver_ep);
    UseCntPtr<InternalBaseHandler> pibh(new InternalIOHandler(this, phandler));
    handlers_[KeySocketOper(pep->getSocket(), KeySocketOper::Read)] = EPIBHPair(pep, pibh);

    return(OK);
}

int
IterativeReactor::registerWriteHandler(EndPoint *pserver_ep, BaseIOHandler *phandler)
{
    if (isNotOk() || (registerReaderWithMultiplexor(pserver_ep) != OK)) return(NOTOK);

    UseCntPtr<BaseHandler> pbh(phandler);
    pbh->setServer(this);

    UseCntPtr<EndPoint> pep(pserver_ep);
    UseCntPtr<InternalBaseHandler> pibh(new InternalIOHandler(this, phandler));
    handlers_[KeySocketOper(pep->getSocket(), KeySocketOper::Write)] = EPIBHPair(pep, pibh);

    return(OK);
}

int
IterativeReactor::registerExceptionHandler(EndPoint *pserver_ep, BaseIOHandler *phandler)
{
    if (isNotOk() || (registerExceptionWithMultiplexor(pserver_ep) != OK)) return(NOTOK);

    UseCntPtr<BaseHandler> pbh(phandler);
    pbh->setServer(this);

    UseCntPtr<EndPoint> pep(pserver_ep);
    UseCntPtr<InternalBaseHandler> pibh(new InternalIOHandler(this, phandler));
    handlers_[KeySocketOper(pep->getSocket(), KeySocketOper::Exception)] = EPIBHPair(pep, pibh);

    return(OK);
}

int
IterativeReactor::registerSignalHandler(EndPoint *pserver_ep, BaseSignalHandler *phandler)
{
    if (isNotOk() || (registerReaderWithMultiplexor(pserver_ep) != OK)) return(NOTOK);

    UseCntPtr<BaseHandler> pbh(phandler);
    pbh->setServer(this);

    UseCntPtr<EndPoint> pep(pserver_ep);
    UseCntPtr<InternalBaseHandler> pibh(new InternalSignalHandler(this, phandler));
    handlers_[KeySocketOper(pep->getSocket(), KeySocketOper::Read)] = EPIBHPair(pep, pibh);

    return(OK);
}

int
IterativeReactor::unregisterFromMultiplexor(
    EndPoint *pserver_ep, int &counter, fd_set &master_fdset, uint32_t epollevent)
{
    if (isNotOk()) return(NOTOK);

    UseCntPtr<EndPoint> pep(pserver_ep);
    int socket = pep->getSocket();

    if (mode_ == EPoll)
    {
        epoll_event ev;
        ev.data.fd = socket;

        EVENTFLAGSIter efit = epolleventflags_.find(socket);
        if (efit != epolleventflags_.end())
        {
            if ((efit->second & epollevent) != 0)
            {
                ev.events = efit->second & ~epollevent;
                if (ev.events != 0)
                {
                    if (::epoll_ctl(epollfd_, EPOLL_CTL_MOD, socket, &ev) != OK) return(NOTOK);
                    epolleventflags_[socket] = ev.events;
                }
                else
                {
                    ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                    epolleventflags_.erase(socket);
                }
                counter -= 1;
            }
        }
    }
    else if (mode_ == Select)
    {
        if (FD_ISSET(socket, &master_fdset))
        {
            FD_CLR(socket, &master_fdset);
            counter -= 1;
        }
    }

    return(OK);
}

int
IterativeReactor::unregisterReaderFromMultiplexor(EndPoint *pserver_ep)
{
    return(unregisterFromMultiplexor(pserver_ep, nreaders_, master_readfds_, EPOLLIN));
}

int
IterativeReactor::unregisterWriterFromMultiplexor(EndPoint *pserver_ep)
{
    return(unregisterFromMultiplexor(pserver_ep, nwriters_, master_writefds_, EPOLLOUT));
}

int
IterativeReactor::unregisterExceptionFromMultiplexor(EndPoint *pserver_ep)
{
    return(unregisterFromMultiplexor(pserver_ep, nexceptions_, master_exceptionfds_, EPOLLPRI));
}

int
IterativeReactor::unregisterAcceptHandler(EndPoint *pserver_ep)
{
    if (isNotOk()) return(NOTOK);

    UseCntPtr<EndPoint> pep(pserver_ep);
    int socket = pep->getSocket();
    handlers_.erase(KeySocketOper(socket, KeySocketOper::Read));

    if (unregisterReaderFromMultiplexor(pserver_ep) != OK) return(NOTOK);

    return(OK);
}

int
IterativeReactor::unregisterReadHandler(EndPoint *pserver_ep)
{
    if (isNotOk()) return(NOTOK);

    UseCntPtr<EndPoint> pep(pserver_ep);
    int socket = pep->getSocket();
    handlers_.erase(KeySocketOper(socket, KeySocketOper::Read));

    if (unregisterReaderFromMultiplexor(pserver_ep) != OK) return(NOTOK);

    return(OK);
}

int
IterativeReactor::unregisterWriteHandler(EndPoint *pserver_ep)
{
    if (isNotOk()) return(NOTOK);

    UseCntPtr<EndPoint> pep(pserver_ep);
    int socket = pep->getSocket();
    handlers_.erase(KeySocketOper(socket, KeySocketOper::Write));

    if (unregisterWriterFromMultiplexor(pserver_ep) != OK) return(NOTOK);

    return(OK);
}

int
IterativeReactor::unregisterExceptionHandler(EndPoint *pserver_ep)
{
    if (isNotOk()) return(NOTOK);

    UseCntPtr<EndPoint> pep(pserver_ep);
    int socket = pep->getSocket();
    handlers_.erase(KeySocketOper(socket, KeySocketOper::Exception));

    if (unregisterExceptionFromMultiplexor(pserver_ep) != OK) return(NOTOK);

    return(OK);
}

int
IterativeReactor::unregisterSignalHandler(EndPoint *pserver_ep)
{
    if (isNotOk()) return(NOTOK);

    UseCntPtr<EndPoint> pep(pserver_ep);
    int socket = pep->getSocket();
    handlers_.erase(KeySocketOper(socket, KeySocketOper::Read));

    if (unregisterReaderFromMultiplexor(pserver_ep) != OK) return(NOTOK);

    return(OK);
}

// set a timer for an end point
int
IterativeReactor::scheduleTimer(EndPoint *pserver_ep, Timer timer, BaseTimerHandler *phandler)
{
    Timer now;
    now.setToNow();
    Timer qtimer(timer);
    qtimer += now;

    UseCntPtr<EndPoint> pep(pserver_ep);
    if ((pep != NULL) && endpoints_.find(pep->getSocket()) == endpoints_.end()) return(NOTOK);
    UseCntPtr<BaseHandler> pbh(phandler);
    pbh->setServer(this);
    UseCntPtr<InternalBaseHandler> pibh(new InternalTimerHandler(this, phandler));
    TimerQueueItem tqi(qtimer, pep, pibh);
    timerqueue_.push(tqi);
    return(OK);
}

int
IterativeReactor::cancelTimer(EndPoint *pserver_ep, Timer timer, BaseTimerHandler *phandler)
{
    UseCntPtr<EndPoint> pep(pserver_ep);
    if ((pep != NULL) && endpoints_.find(pep->getSocket()) == endpoints_.end()) return(NOTOK);
    UseCntPtr<InternalBaseHandler> pibh(new InternalTimerHandler(this, phandler));
    TimerQueueItem ctqi(timer, pep, pibh);
    canceledtimers_.insert(ctqi);
    return(OK);
}

// internal handlers
int
IterativeReactor::acceptHandler(BaseAcceptHandler *phandler, EndPoint *pendpoint)
{
    UseCntPtr<EndPoint> pep(pendpoint);
    UseCntPtr<BaseAcceptHandler> pah(phandler);

    bool retry;
    UseCntPtr<EndPoint> pclient;

    pclient = pep->accept(retry);
    if (retry)
        return(OK);
    else if (pclient == NULL)
        return(NOTOK);

    return(pah->handler(pep, pclient));
}

int
IterativeReactor::ioHandler(BaseIOHandler *phandler, EndPoint *pendpoint)
{
    UseCntPtr<EndPoint> pep(pendpoint);
    UseCntPtr<BaseIOHandler> pioh(phandler);
    return(pioh->handler(pep));
}

int
IterativeReactor::signalHandler(BaseSignalHandler *phandler, EndPoint *pendpoint)
{
    UseCntPtr<EndPoint> pep(pendpoint);
    UseCntPtr<BaseSignalHandler> psh(phandler);

    signalfd_siginfo sigbuf;
    size_t count = sizeof(sigbuf);
    if (pep->read((void*)&sigbuf, count) == NOTOK) return(NOTOK);

    return(psh->handler(pep, sigbuf.ssi_signo));
}

int
IterativeReactor::timerHandler(BaseTimerHandler *phandler, EndPoint *pendpoint)
{
    UseCntPtr<EndPoint> pep(pendpoint);
    UseCntPtr<BaseTimerHandler> pth(phandler);

    TimerQueueItem tqi = timerqueue_.top();

    return(pth->handler(pep, tqi.timer_));
}

// clean up routines
void
IterativeReactor::unregisterAllHandlers(EndPoint *pep)
{
    unregisterAcceptHandler(pep);
    unregisterReadHandler(pep);
    unregisterWriteHandler(pep);
    unregisterExceptionHandler(pep);
    unregisterSignalHandler(pep);
}

// operations
int
IterativeReactor::init()
{
    switch (mode_)
    {
    case Select:
        return(OK);
    case EPoll:
    {
        EPIter epit = endpoints_.begin();
        EPIter epitend = endpoints_.end();
        for ( ; epit != epitend; ++epit)
        {
            UseCntPtr<EndPoint> pep(epit->second);
            pep->setNonBlocking(true);
        }
        return(OK);
    }
    default:
        return(NOTOK);
    }
}

int
IterativeReactor::run()
{
    switch (mode_)
    {
    case Select:
        return(selectLoop());
    case EPoll:
        return(epollLoop());
    default:
        return(NOTOK);
    }
}

int
IterativeReactor::finish()
{
    if ((mode_ = EPoll) && (epollfd_ >= 0))
    {
        ::close(epollfd_);
    }    
    epollfd_ = -1;

    EPIter epit = endpoints_.begin();
    EPIter epitend = endpoints_.end();
    for ( ; epit != epitend; ++epit)
    {
        UseCntPtr<EndPoint> pep(epit->second);
        pep->close();
    }

    setDone();

    return(OK);
}

// multipexors
void
IterativeReactor::initselectfds(fd_set &readfds, fd_set &writefds, fd_set &exceptionfds)
{
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptionfds);

#if 0
    EPIter epit = endpoints_.begin();
    EPIter epitend = endpoints_.end();
    for (maxsocket_ = -1; epit != epitend; ++epit)
    {
        int socket = epit->first;
        if (socket > maxsocket_) maxsocket_ = socket;
    }
#endif
}

// utility functions
void
IterativeReactor::setDone()
{
    done_ = true;
}

void
IterativeReactor::removeEndPoint(int socket)
{
    EPIter epit = endpoints_.find(socket);
    if (epit != endpoints_.end())
    {
        UseCntPtr<EndPoint> pep(epit->second);
        unregisterAcceptHandler(pep);
        unregisterReadHandler(pep);
        unregisterWriteHandler(pep);
        unregisterExceptionHandler(pep);
        unregisterSignalHandler(pep);
        unregisterEndPoint(pep);
        pep->close();
    }
    else
    {
        ::close(socket);
    }

    if (mode_ == Select)
    {
        FD_CLR(socket, &master_exceptionfds_);
        FD_CLR(socket, &master_readfds_);
        FD_CLR(socket, &master_writefds_);
    }
    else if (mode_ == EPoll && epollfd_ >= 0)
    {
        epoll_event ev;
        ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
        epolleventflags_.erase(socket);

        nreaders_ = 0;
        nwriters_ = 0;
        nexceptions_ = 0;
    }
}

void
IterativeReactor::callHandler(KeySocketOper &key, int &status)
{
    IBHSIter hit;
    if ((hit = handlers_.find(key)) != handlers_.end())
    {
        UseCntPtr<EndPoint> pep = hit->second.first;
        UseCntPtr<InternalBaseHandler> pibh = hit->second.second;
        status = pibh->handler(pep);
        handleStatus(status, pep);
    }
    else
    {
        // no handler??? remove and close the EP.
        removeEndPoint(key.sock_);
    }
}

int
IterativeReactor::handleStatus(int status, EndPoint *pserver_ep)
{
    UseCntPtr<EndPoint> pep(pserver_ep); 
    if (pep != NULL)
    {
        if (status > 0)
        {
            unregisterAllHandlers(pep);
            unregisterEndPoint(pep);
            pep->close();
        }
        else if (status < 0)
        {
            // TBD: do a back trace
            unregisterAllHandlers(pep);
            unregisterEndPoint(pep);
            pep->close();
        }
    }
    return(status);
}

void
IterativeReactor::resetfdset(int count, fd_set *&pfdset, fd_set &fdset, 
                             const fd_set &master_fdset, int nbytes)
{
    if (count != 0)
    {
        memcpy(&fdset, &master_fdset, nbytes);
        pfdset = &fdset;
    }
    else
    {
        pfdset = NULL;
    }
}

void
IterativeReactor::rescheduleTimer(TimerQueueItem &tqi)
{
    if (tqi.timer_.getType() != Timer::RepeatTimer) return;

    Timer::Seconds usersecs = tqi.timer_.getUserSeconds();
    Timer::MicroSeconds userusecs = tqi.timer_.getUserMicroSeconds();
    if (usersecs == 0 && userusecs == 0) return;

    CTMRSIter ctit = canceledtimers_.find(tqi);
    if (ctit != canceledtimers_.end())
    {
        canceledtimers_.erase(ctit);
    }
    else
    {
        tqi.timer_.setSeconds(usersecs);
        tqi.timer_.setMicroSeconds(userusecs);

        Timer now;
        now.setToNow();
        tqi.timer_ += now;

        timerqueue_.push(tqi);
    }

    return;
}

// mulitplexor loops
int
IterativeReactor::selectLoop()
{
    MustBeTrue(isOk());
    MustBeTrue(!endpoints_.empty() || !timerqueue_.empty());

    fd_set readfds;
    fd_set writefds;
    fd_set exceptionfds;
    initselectfds(readfds, writefds, exceptionfds);

    for (Timer zero(0,0); !done_ && (!endpoints_.empty() || !timerqueue_.empty()); )
    {
        fd_set *preadfds;
        fd_set *pwritefds;
        fd_set *pexceptionfds;

        // reset descriptor list
        resetfdset(nreaders_, preadfds, readfds, 
                   master_readfds_, sizeof(master_readfds_));
        resetfdset(nwriters_, pwritefds, writefds, 
                   master_writefds_, sizeof(master_writefds_));
        resetfdset(nexceptions_, pexceptionfds, exceptionfds, 
                   master_exceptionfds_, sizeof(master_exceptionfds_));

        // do we have any timers?
        timeval *ptimer, timer;
        for (ptimer=NULL; !done_ && !timerqueue_.empty(); )
        {
            TimerQueueItem tqi = timerqueue_.top();
            CTMRSIter ctit = canceledtimers_.find(tqi);
            if (ctit != canceledtimers_.end())
            {
                canceledtimers_.erase(ctit);
                timerqueue_.pop();
                continue;
            }

            Timer timeout = tqi.timer_;
            Timer now;
            now.setToNow();
            timeout -= now;
            if (timeout < zero) timeout = zero;
            timeout.setTimeval(timer);
            ptimer = &timer;
            break;
        }
        if (done_) break;

        // wait for an event
        int status = ::select(maxsocket_+1, preadfds, pwritefds, pexceptionfds, ptimer);
        if (status < 0)
        {
             // error of some type
             if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) continue;
             setOk(false);
             return(NOTOK);
        }
        else if (status == 0)
        {
            // did timer fire?
            if (ptimer == &timer)
            {
                // we have a timeout. check queue
                Timer now;
                now.setToNow();
                
                while (!done_ && !timerqueue_.empty())
                {
                    if (timerqueue_.top().timer_ > now) break;

                    TimerQueueItem tqi = timerqueue_.top();
                    CTMRSIter ctit = canceledtimers_.find(tqi);
                    if (ctit != canceledtimers_.end())
                    {
                        canceledtimers_.erase(ctit);
                        timerqueue_.pop();
                        continue;
                    }

                    int status = tqi.phandler_->handler(tqi.pendpoint_);
                    handleStatus(status, tqi.pendpoint_);

                    rescheduleTimer(tqi);
                    timerqueue_.pop();
                }
            }
        }
        else
        {
            // we have i/o to handle
            for (int socket=0; !done_ && socket<=maxsocket_; ++socket)
            {
                // skip if not ready
                int status;
                IBHSIter hit;
                if (pexceptionfds != NULL && FD_ISSET(socket, pexceptionfds))
                {
                    KeySocketOper ksx(socket, KeySocketOper::Exception);
                    callHandler(ksx, status);
                }
                if (preadfds != NULL && FD_ISSET(socket, preadfds))
                {
                    KeySocketOper ksr(socket, KeySocketOper::Read);
                    callHandler(ksr, status);
                }
                if (pwritefds != NULL && FD_ISSET(socket, pwritefds))
                {
                    KeySocketOper ksw(socket, KeySocketOper::Write);
                    callHandler(ksw, status);
                }
            }
        }
    }

    return 0;
}

int
IterativeReactor::epollLoop()
{
    MustBeTrue(isOk());
    MustBeTrue(!endpoints_.empty() || !timerqueue_.empty());

    epoll_event ev;
    ::memset(&ev,0,sizeof(ev));

    epoll_event events[DefaultEPollEventsToHandle];

    for (Timer zero(0,0); !done_ && (!endpoints_.empty() || !timerqueue_.empty()); )
    {
        // do we have any timers?
        int timer;
        for (timer=-1; !done_ && !timerqueue_.empty(); )
        {
            TimerQueueItem tqi = timerqueue_.top();
            CTMRSIter ctit = canceledtimers_.find(tqi);
            if (ctit != canceledtimers_.end())
            {
                canceledtimers_.erase(ctit);
                timerqueue_.pop();
                continue;
            }
            
            Timer timeout = tqi.timer_;
            Timer now;
            now.setToNow();
            timeout -= now;
            if (timeout < zero) timeout = zero;
            timer = timeout.getTimeInMillisecs();
            break;
        }
        if (done_) break;

        // wait for an event
        int eventnum = ::epoll_wait(epollfd_, events, 
                                    DefaultEPollEventsToHandle, timer);
        if (eventnum < 0)
        {
             // error of some type
             if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) continue;
             setOk(false);
             return(NOTOK);
        }
        else if (eventnum == 0)
        {
            // did timer fire?
            if (timer != -1)
            {
                // we have a timeout. check queue
                Timer now;
                now.setToNow();
                
                while (!done_ && !timerqueue_.empty())
                {
                    if (timerqueue_.top().timer_ > now) break;

                    TimerQueueItem tqi = timerqueue_.top();
                    CTMRSIter ctit = canceledtimers_.find(tqi);
                    if (ctit != canceledtimers_.end())
                    {
                        canceledtimers_.erase(ctit);
                        timerqueue_.pop();
                        continue;
                    }

                    int status = tqi.phandler_->handler(tqi.pendpoint_);
                    handleStatus(status, tqi.pendpoint_);

                    rescheduleTimer(tqi);
                    timerqueue_.pop();
                }
            }
        }
        else
        {
            // we have i/o to handle
            for (int ievent=0; !done_ && ievent<eventnum; ++ievent)
            {
                // socket with an event
                int socket = events[ievent].data.fd;
                uint32_t socketevents = events[ievent].events;

                // handle i/o
                int status;
                IBHSIter hit;
                if ((socketevents & EPOLLPRI) != 0)
                {
                    KeySocketOper ksx(socket, KeySocketOper::Exception);
                    callHandler(ksx, status);
                }
                if ((socketevents & EPOLLIN) != 0)
                {
                    KeySocketOper ksr(socket, KeySocketOper::Read);
                    callHandler(ksr, status);
                }
                if ((socketevents & EPOLLOUT) != 0)
                {
                    KeySocketOper ksw(socket, KeySocketOper::Write);
                    callHandler(ksw, status);
                }
            }
        }
    }

    return 0;
}

}
