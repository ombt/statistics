//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// iterative server class

// headers
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <set>
#include "system/Debug.h"
#include "atomic/UseCntPtr.h"
#include "hdr/IterativeMultiServiceServer.h"

namespace ombt {

// ctors and dtor
IterativeMSDatagramServer::IterativeMSDatagramServer(MultiplexMode mode):
        Server(), mode_(mode), epollfd_(-1), epollsize_(DefaultEPollSize), handlers_(), timerhandlers_(), tmrqueue_()
{
    setOk(true);
}

IterativeMSDatagramServer::~IterativeMSDatagramServer()
{
    if (isOk())
    {
        EPIter epit = endpoints_.begin();
        EPIter epitend = endpoints_.end();
        for ( ; epit != epitend; ++epit)
        {
             UseCntPtr<EndPoint> pserverep = epit->second;
             pserverep->close();
        }
    }

    endpoints_.clear();
    handlers_.clear();
    timerhandlers_.clear();

    setOk(false);
}

// register and unregister handlers
int
IterativeMSDatagramServer::registerEndPoint(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    endpoints_.insert(std::pair<int, UseCntPtr<EndPoint> >(pep->getSocket(), pep));

    return(0);
}

int
IterativeMSDatagramServer::registerHandler(EndPoint *ep, Handler *h)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    UseCntPtr<Handler> ph(h);

    registerEndPoint(pep);
    ph->setServer(this);

    handlers_.insert(std::pair<int, std::pair<UseCntPtr<EndPoint>, UseCntPtr<Handler> > >(
        pep->getSocket(), std::pair<UseCntPtr<EndPoint>, UseCntPtr<Handler> >(pep, ph)));

    return(0);
}

int
IterativeMSDatagramServer::registerTimerHandler(EndPoint *ep, TimerHandler *th)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    UseCntPtr<TimerHandler> pth(th);

    registerEndPoint(pep);
    pth->setServer(this);

    timerhandlers_.insert(std::pair<int, std::pair<UseCntPtr<EndPoint>, UseCntPtr<TimerHandler> > >(
        pep->getSocket(), std::pair<UseCntPtr<EndPoint>, UseCntPtr<TimerHandler> >(pep, pth)));

    return(0);
}

int 
IterativeMSDatagramServer::unregisterEndPoint(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    endpoints_.erase(pep->getSocket());

    return(0);
}

int 
IterativeMSDatagramServer::unregisterHandler(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    handlers_.erase(pep->getSocket());

    return(0);
}

int 
IterativeMSDatagramServer::unregisterTimerHandler(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    timerhandlers_.erase(pep->getSocket());

    return(0);
}

int 
IterativeMSDatagramServer::unregisterAllHandlers(EndPoint *ep)
{
    CheckReturn(isOk());

    unregisterEndPoint(ep);
    unregisterHandler(ep);
    unregisterTimerHandler(ep);

    return(0);
}

// set a timer for an end point
void 
IterativeMSDatagramServer::scheduleTimer(EndPoint *myep, Timer &timer)
{
    Timer qtmr(timer);
    Timer now;
    now.setToNow();
    qtmr += now;
    UseCntPtr<EndPoint> pmyep(myep);
#if USESTL
    tmrqueue_.insert(std::pair<Timer, std::pair<Timer, UseCntPtr<EndPoint> > >(qtmr, std::pair<Timer, UseCntPtr<EndPoint> >(timer, pmyep)));
#else
    tmrqueue_.enqueue(Tuple<Timer, Tuple<Timer, UseCntPtr<EndPoint> > >(qtmr, Tuple<Timer, UseCntPtr<EndPoint> >(timer, pmyep)));
#endif
}

void 
IterativeMSDatagramServer::cancelTimer(EndPoint *myep, Timer &timer)
{
#if 0
    TMRQIter tmrit = tmrqueue_.find(myep);
    TMRQIter tmritend = tmrqueue_.end();

    if (tmrit == tmritend) return;

    for ( ; tmrit != tmritend; ++tmrit)
    {
        if (tmrit->second == myep) tmrqueue_.erase(tmrit);
    }
#endif
}

// operations
int
IterativeMSDatagramServer::init()
{
    CheckReturn(isOk());

    if (mode_ == EPoll)
    {
        CheckReturn(epollsize_ >= 0);
        epollfd_ = ::epoll_create(epollsize_);
        CheckReturn(epollfd_ >= 0);
    }

    HSIter hit = handlers_.begin();
    HSIter hitend = handlers_.end();
    for ( ; hit != hitend; ++hit)
    {
        UseCntPtr<EndPoint> pserverep = hit->second.first;
        CheckReturn(pserverep->setReuseAddr(true) == 0);
        CheckReturn(pserverep->bind() == 0);

        if (mode_ == EPoll)
        {
            pserverep->setNonBlocking(true);
            epoll_event ev;
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = pserverep->getSocket();
            CheckReturn(::epoll_ctl(epollfd_, EPOLL_CTL_ADD, 
                                    pserverep->getSocket(), &ev) == 0);
        }
    }

    return(0);
}

int
IterativeMSDatagramServer::run()
{
    CheckReturn(isOk());

    switch (mode_)
    {
    case Select:
        return(selectLoop());
    case EPoll:
        return(epollLoop());
    default:
        return(-1);
    }
}

int
IterativeMSDatagramServer::selectLoop()
{
    CheckReturn(isOk());
    CheckReturn(!handlers_.empty());

    fd_set readfds;
    FD_ZERO(&readfds);
    fd_set master_readfds;
    FD_ZERO(&master_readfds);

    int maxsocket = -1;
    EPIter epit = endpoints_.begin();
    EPIter epitend = endpoints_.end();
    for ( ; epit != epitend; ++epit)
    {
        int socket = epit->first;
        if (socket > maxsocket) maxsocket = socket;
        FD_SET(socket, &master_readfds);
    }

    for (Timer zero(0,0); !endpoints_.empty(); )
    {
        // reset descriptor list
        memcpy(&readfds, &master_readfds, sizeof(master_readfds));

        // do we have any timers?
        timeval *ptimer, timer;
#if USESTL
        TMRQIter tit = tmrqueue_.begin();
        if (tit != tmrqueue_.end())
        {
            Timer t2 = tit->first;
            Timer now;
            now.setToNow();
            t2 -= now;
            if (t2 < zero) t2 = zero;
            t2.setTimeval(timer);
            ptimer = &timer;
        }
        else
        {
            ptimer = NULL;
        }
#else
        if (!tmrqueue_.isEmpty())
        {
            Tuple<Timer, Tuple<Timer, UseCntPtr<EndPoint> > > qtimer;
            tmrqueue_.front(qtimer);
            Timer t2 = qtimer.key;
            Timer now;
            now.setToNow();
            t2 -= now;
            if (t2 < zero) t2 = zero;
            t2.setTimeval(timer);
            ptimer = &timer;
	}
	else
	{
            ptimer = NULL;
	}
#endif

        // wait for an event
        int status = ::select(maxsocket+1, &readfds, NULL, NULL, ptimer);
        if (status < 0)
        {
             // error of some type
             TMSG("select failed");
             DUMP(errno);
             return(-1);
        }
        else if (status == 0)
        {
            // did timer fire?
            if (ptimer == &timer)
            {
#if USESTL
                // we have a timeout. check queue
                TMRQIter titbegin = tmrqueue_.begin();
                TMRQIter titend = tmrqueue_.end();
                TMRQIter tit = tmrqueue_.begin();

                Timer now;
                now.setToNow();
                
                for ( ; tit != titend && tit->first <= now; ++tit)
                {
                    Timer tmr = tit->second.first;
                    EndPoint ep = tit->second.second;

                    int socket = ep.getSocket();
                    THS::iterator hit = timerhandlers_.find(socket);
                    if (hit == timerhandlers_.end()) continue;
                    TimerHandler *handler = hit->second.second;

                    int status = (*handler)(ep, tmr);
                    if (status > 0)
                    {
                        // success and done with this server
                        FD_CLR(socket, &master_readfds);
                        unregisterAllHandlers(ep);
	                ep.close();
                    }
                    else if (status < 0)
                    {
                        // some type of error. for now, close endpoint
                        TMSG("timer handler failed");
                        DUMP(ep); DUMP(socket); DUMP(errno);
                        FD_CLR(socket, &master_readfds);
                        unregisterAllHandlers(ep);
	                ep.close();
                    }
                }

                tmrqueue_.erase(titbegin, tit);
#else
                // we have a timeout. check queue
                Timer now;
                now.setToNow();
                
		while (!tmrqueue_.isEmpty())
		{
                    Tuple<Timer, Tuple<Timer, UseCntPtr<EndPoint> > > qtimer;
                    tmrqueue_.dequeue(qtimer);

                    if (qtimer.key > now)
                    {
                        tmrqueue_.enqueue(qtimer);
                        break;
                    }

                    Timer tmr = qtimer.data.key;
                    UseCntPtr<EndPoint> pep(qtimer.data.data);

                    int socket = pep->getSocket();
                    THS::iterator hit = timerhandlers_.find(socket);
                    if (hit == timerhandlers_.end()) continue;
                    UseCntPtr<TimerHandler> phandler = hit->second.second;

                    int status = (*phandler)(pep, tmr);
                    if (status > 0)
                    {
                        // success and done with this server
                        FD_CLR(socket, &master_readfds);
                        unregisterAllHandlers(pep);
	                pep->close();
                    }
                    else if (status < 0)
                    {
                        // some type of error. for now, close endpoint
                        TMSG("timer handler failed");
                        DUMP(pep); DUMP(socket); DUMP(errno);
                        FD_CLR(socket, &master_readfds);
                        unregisterAllHandlers(pep);
	                pep->close();
                    }
                }
#endif
            }
        }
        else
        {
            // we have i/o to handle
            for (int socket=0; socket<=maxsocket; ++socket)
            {
                // skip if not ready
                if (!FD_ISSET(socket, &readfds)) continue;
                // handle i/o
                HS::iterator hit = handlers_.find(socket);
                if (hit == handlers_.end()) continue;
                UseCntPtr<EndPoint> pep(hit->second.first);
                UseCntPtr<Handler> phandler(hit->second.second);
                int status = (*phandler)(pep);
                if (status > 0)
                {
                    // success and done with this server
                    FD_CLR(socket, &master_readfds);
                    unregisterAllHandlers(pep);
	            pep->close();
                }
                else if (status < 0)
                {
                    // some type of error. for now, close endpoint
                    TMSG("i/o handler failed");
                    DUMP(pep); DUMP(socket); DUMP(errno);
                    FD_CLR(socket, &master_readfds);
                    unregisterAllHandlers(pep);
	            pep->close();
                }
            }
        }
    }

    return(0);
}

int
IterativeMSDatagramServer::epollLoop()
{
    CheckReturn(isOk());
    CheckReturn(!handlers_.empty());

    epoll_event ev;
    ::memset(&ev,0,sizeof(ev));

    epoll_event events[DefaultEPollEventsToHandle];

    for (Timer zero(0,0); !endpoints_.empty(); )
    {
        // do we have any timers?
        int timer;
#if USESTL
        TMRQIter tit = tmrqueue_.begin();
        if (tit != tmrqueue_.end())
        {
            Timer t2 = tit->first;
            Timer now;
            now.setToNow();
            t2 -= now;
            if (t2 < zero) t2 = zero;
            timer = t2.getTimeInMillisecs();
        }
        else
        {
            timer = -1;
        }
#else
        if (!tmrqueue_.isEmpty())
        {
            Tuple<Timer, Tuple<Timer, UseCntPtr<EndPoint> > > qtimer;
            tmrqueue_.front(qtimer);
            Timer t2 = qtimer.key;
            Timer now;
            now.setToNow();
            t2 -= now;
            if (t2 < zero) t2 = zero;
            timer = t2.getTimeInMillisecs();
	}
	else
	{
            timer = -1;
	}
#endif

        // wait for an event
        int eventnum = ::epoll_wait(epollfd_, events, 
                                    DefaultEPollEventsToHandle, timer);
        if (eventnum < 0)
        {
             // error of some type
             TMSG("epoll_wait failed");
             DUMP(errno);
             return(-1);
        }
        else if (eventnum == 0)
        {
#if USESTL
            // we have a timeout. check queue
            TMRQIter titbegin = tmrqueue_.begin();
            TMRQIter titend = tmrqueue_.end();
            TMRQIter tit = tmrqueue_.begin();

            Timer now;
            now.setToNow();
        
            for ( ; tit != titend && tit->first <= now; ++tit)
            {
                Timer tmr = tit->second.first;
                UseCntPtt<EndPoint> pep = tit->second.second;

                int socket = pep->getSocket();
                THS::iterator hit = timerhandlers_.find(socket);
                if (hit == timerhandlers_.end()) continue;
                UseCntPtr<TimerHandler> phandler = hit->second.second;

                int status = (*phandler)(pep, tmr);
                if (status > 0)
                {
                    // success and done with this server
                    ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                    unregisterAllHandlers(pep);
                    pep->close();
                }
                else if (status < 0)
                {
                    // some type of error. for now, close endpoint
                    TMSG("timer handler failed");
                    DUMP(ep); DUMP(socket); DUMP(errno);
                    ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                    unregisterAllHandlers(pep);
                    pep->close();
                }
            }

            tmrqueue_.erase(titbegin, tit);
#else
            // we have a timeout. check queue
            Timer now;
            now.setToNow();
            
            while (!tmrqueue_.isEmpty())
            {
                Tuple<Timer, Tuple<Timer, UseCntPtr<EndPoint> > > qtimer;
                tmrqueue_.dequeue(qtimer);

                if (qtimer.key > now)
                {
                    tmrqueue_.enqueue(qtimer);
                    break;
                }

                Timer tmr = qtimer.data.key;
                UseCntPtr<EndPoint> pep(qtimer.data.data);

                int socket = pep->getSocket();
                THS::iterator hit = timerhandlers_.find(socket);
                if (hit == timerhandlers_.end()) continue;
                UseCntPtr<TimerHandler> phandler(hit->second.second);

                int status = (*phandler)(pep, tmr);
                if (status > 0)
                {
                    // success and done with this server
                    ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                    unregisterAllHandlers(pep);
                    pep->close();
                }
                else if (status < 0)
                {
                    // some type of error. for now, close endpoint
                    TMSG("timer handler failed");
                    DUMP(pep); DUMP(socket); DUMP(errno);
                    ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                    unregisterAllHandlers(pep);
                    pep->close();
                }
            }
#endif
        }
        else
        {
            // we have i/o to handle
            for (int ievent=0; ievent<eventnum; ++ievent)
            {
                // socket with an event
                int socket = events[ievent].data.fd;

                // handle i/o
                HS::iterator hit = handlers_.find(socket);
                if (hit == handlers_.end()) continue;
                UseCntPtr<EndPoint> pep(hit->second.first);
                UseCntPtr<Handler> phandler(hit->second.second);
                int status = (*phandler)(pep);
                if (status > 0)
                {
                    // success and done with this server
                    ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                    unregisterAllHandlers(pep);
                    pep->close();
                }
                else if (status < 0)
                {
                    // some type of error. for now, close endpoint
                    TMSG("i/o handler failed");
                    DUMP(pep); DUMP(socket); DUMP(errno);
                    ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                    unregisterAllHandlers(pep);
                    pep->close();
                }
            }
        }
    }

    return(0);
}

int
IterativeMSDatagramServer::finish()
{
    CheckReturn(isOk());

    if (mode_ == EPoll)
    {
        ::close(epollfd_);
        epollfd_ = -1;
    }

    EPIter epit = endpoints_.begin();
    EPIter epitend = endpoints_.end();
    for ( ; epit != epitend; ++epit)
    {
         UseCntPtr<EndPoint> pserverep(epit->second);
         pserverep->close();
    }

    endpoints_.clear();
    handlers_.clear();
    timerhandlers_.clear();

    setOk(false);

    return(0);
}

// ctors and dtor
IterativeMSStreamServer::IterativeMSStreamServer(MultiplexMode mode):
        Server(), mode_(mode), epollfd_(-1), epollsize_(DefaultEPollSize), handlers_(), timerhandlers_(), tmrqueue_()
{
    setOk(true);
}

IterativeMSStreamServer::~IterativeMSStreamServer()
{
    if (isOk())
    {
        EPIter epit = endpoints_.begin();
        EPIter epitend = endpoints_.end();
        for ( ; epit != epitend; ++epit)
        {
             UseCntPtr<EndPoint> pserverep(epit->second);
             pserverep->close();
        }
    }

    endpoints_.clear();
    handlers_.clear();
    timerhandlers_.clear();

    setOk(false);
}

// register and unregister handlers
int
IterativeMSStreamServer::registerEndPoint(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    endpoints_.insert(std::pair<int, UseCntPtr<EndPoint> >(pep->getSocket(), pep));

    return(0);
}

int
IterativeMSStreamServer::registerHandler(EndPoint *ep, Handler *h)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    UseCntPtr<Handler> ph(h);

    registerEndPoint(pep);
    ph->setServer(this);

    handlers_.insert(std::pair<int, std::pair<UseCntPtr<EndPoint>, UseCntPtr<Handler> > >(
        pep->getSocket(), std::pair<UseCntPtr<EndPoint>, UseCntPtr<Handler> >(pep, ph)));

    return(0);
}

int
IterativeMSStreamServer::registerTimerHandler(EndPoint *ep, TimerHandler *th)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    UseCntPtr<TimerHandler> pth(th);

    registerEndPoint(pep);
    pth->setServer(this);

    timerhandlers_.insert(std::pair<int, std::pair<UseCntPtr<EndPoint>, UseCntPtr<TimerHandler> > >(
        pep->getSocket(), std::pair<UseCntPtr<EndPoint>, UseCntPtr<TimerHandler> >(pep, pth)));

    return(0);
}

int 
IterativeMSStreamServer::unregisterEndPoint(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    endpoints_.erase(pep->getSocket());

    return(0);
}

int 
IterativeMSStreamServer::unregisterHandler(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    handlers_.erase(pep->getSocket());

    return(0);
}

int 
IterativeMSStreamServer::unregisterTimerHandler(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    timerhandlers_.erase(pep->getSocket());

    return(0);
}

int 
IterativeMSStreamServer::unregisterAllHandlers(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    unregisterEndPoint(pep);
    unregisterHandler(pep);
    unregisterTimerHandler(pep);

    return(0);
}

// set a timer for an end point
void 
IterativeMSStreamServer::scheduleTimer(EndPoint *myep, Timer &timer)
{
    Timer qtmr(timer);
    Timer now;
    now.setToNow();
    qtmr += now;
    UseCntPtr<EndPoint> pmyep(myep);
#if USESTL
    tmrqueue_.insert(std::pair<Timer, std::pair<Timer, UseCntPtr<EndPoint> > >(qtmr, std::pair<Timer, UseCntPtr<EndPoint> >(timer, pmyep)));
#else
    tmrqueue_.enqueue(Tuple<Timer, Tuple<Timer, UseCntPtr<EndPoint> > >(qtmr, Tuple<Timer, UseCntPtr<EndPoint> >(timer, pmyep)));
#endif
}

void 
IterativeMSStreamServer::cancelTimer(EndPoint *myep, Timer &timer)
{
#if 0
    TMRQIter tmrit = tmrqueue_.find(myep);
    TMRQIter tmritend = tmrqueue_.end();

    if (tmrit == tmritend) return;

    for ( ; tmrit != tmritend; ++tmrit)
    {
        if (tmrit->second == myep) tmrqueue_.erase(tmrit);
    }
#endif
}

// operations
int
IterativeMSStreamServer::init()
{
    CheckReturn(isOk());

    if (mode_ == EPoll)
    {
        CheckReturn(epollsize_ >= 0);
        epollfd_ = ::epoll_create(epollsize_);
        CheckReturn(epollfd_ >= 0);
    }

    HSIter hit = handlers_.begin();
    HSIter hitend = handlers_.end();
    for ( ; hit != hitend; ++hit)
    {
        UseCntPtr<EndPoint> pserverep(hit->second.first);
        CheckReturn(pserverep->setReuseAddr(true) == 0);
        CheckReturn(pserverep->setNonBlocking(true) == 0);
        CheckReturn(pserverep->bind() == 0);
        CheckReturn(pserverep->listen() == 0);

        if (mode_ == EPoll)
        {
            pserverep->setNonBlocking(true);
            epoll_event ev;
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = pserverep->getSocket();
            CheckReturn(::epoll_ctl(epollfd_, EPOLL_CTL_ADD, 
                                    pserverep->getSocket(), &ev) == 0);
        }
    }

    return(0);
}

int
IterativeMSStreamServer::run()
{
    CheckReturn(isOk());

    switch (mode_)
    {
    case Select:
        return(selectLoop());
    case EPoll:
        return(epollLoop());
    default:
        return(-1);
    }
}

int
IterativeMSStreamServer::selectLoop()
{
    CheckReturn(isOk());
    CheckReturn(!endpoints_.empty());

    fd_set readfds;
    FD_ZERO(&readfds);
    fd_set exceptfds;
    FD_ZERO(&exceptfds);
    fd_set master_readfds;
    FD_ZERO(&master_readfds);
    fd_set server_readfds;
    FD_ZERO(&server_readfds);

    int maxsocket = -1;
    HSIter hit = handlers_.begin();
    HSIter hitend = handlers_.end();
    for ( ; hit != hitend; ++hit)
    {
        int socket = hit->first;
        if (socket > maxsocket) maxsocket = socket;
        FD_SET(socket, &server_readfds);
    }
    memcpy(&master_readfds, &server_readfds, sizeof(server_readfds));

    for (Timer zero(0,0); !endpoints_.empty(); )
    {
        memcpy(&readfds, &master_readfds, sizeof(master_readfds));
        memcpy(&exceptfds, &master_readfds, sizeof(master_readfds));

        // do we have any timers?
        timeval *ptimer, timer;
#if USESTL
        TMRQIter tit = tmrqueue_.begin();
        if (tit != tmrqueue_.end())
        {
            Timer t2 = tit->first;
            Timer now;
            now.setToNow();
            t2 -= now;
            if (t2 < zero) t2 = zero;
            t2.setTimeval(timer);
            ptimer = &timer;
        }
        else
        {
            ptimer = NULL;
        }
#else
        if (!tmrqueue_.isEmpty())
        {
            Tuple<Timer, Tuple<Timer, UseCntPtr<EndPoint> > > qtimer;
            tmrqueue_.front(qtimer);
            Timer t2 = qtimer.key;
            Timer now;
            now.setToNow();
            t2 -= now;
            if (t2 < zero) t2 = zero;
            t2.setTimeval(timer);
            ptimer = &timer;
	}
	else
	{
            ptimer = NULL;
	}
#endif

        int status = ::select(maxsocket+1, &readfds, NULL, &exceptfds, ptimer);
        if (status < 0)
        {
            if (errno == EINTR) continue;
            TMSG("select failed");
            DUMP(errno);
            return(-1);
        }
        else if (status == 0)
        {
            // did timer fire?
            if (ptimer == &timer)
            {
                // we have a timeout. check queue
#if USESTL
                TMRQIter titbegin = tmrqueue_.begin();
                TMRQIter titend = tmrqueue_.end();
                TMRQIter tit = tmrqueue_.begin();

                Timer now;
                now.setToNow();

                for ( ; tit != titend && tit->first <= now; ++tit)
                {
                    Timer tmr = tit->second.first;
                    EndPoint ep = tit->second.second;

                    int socket = ep.getSocket();
                    THS::iterator hit = timerhandlers_.find(socket);
                    if (hit == timerhandlers_.end()) continue;
                    TimerHandler *handler = hit->second.second;

                    int status = (*handler)(ep, tmr);
                    if (status > 0)
                    {
                        // success and done with this server
                        FD_CLR(socket, &master_readfds);
                        FD_CLR(socket, &server_readfds);
                        unregisterAllHandlers(ep);
                        ep.close();
                    }
                    else if (status < 0)
                    {
                        // some type of error. for now, close endpoint
                        TMSG("timer handler failed");
                        DUMP(ep); DUMP(socket); DUMP(errno);
                        FD_CLR(socket, &master_readfds);
                        FD_CLR(socket, &server_readfds);
                        unregisterAllHandlers(ep);
                        ep.close();
                    }
                }

                tmrqueue_.erase(titbegin, tit);
#else
                // we have a timeout. check queue
                Timer now;
                now.setToNow();
                
		while (!tmrqueue_.isEmpty())
		{
                    Tuple<Timer, Tuple<Timer, UseCntPtr<EndPoint> > > qtimer;
                    tmrqueue_.dequeue(qtimer);

                    if (qtimer.key > now)
                    {
                        tmrqueue_.enqueue(qtimer);
                        break;
                    }

                    Timer tmr = qtimer.data.key;
                    UseCntPtr<EndPoint> pep(qtimer.data.data);

                    int socket = pep->getSocket();
                    THS::iterator hit = timerhandlers_.find(socket);
                    if (hit == timerhandlers_.end()) continue;
                    UseCntPtr<TimerHandler> phandler(hit->second.second);

                    int status = (*phandler)(pep, tmr);
                    if (status > 0)
                    {
                        // success and done with this server
                        FD_CLR(socket, &master_readfds);
                        FD_CLR(socket, &server_readfds);
                        unregisterAllHandlers(pep);
	                pep->close();
                    }
                    else if (status < 0)
                    {
                        // some type of error. for now, close endpoint
                        TMSG("timer handler failed");
                        DUMP(pep); DUMP(socket); DUMP(errno);
                        FD_CLR(socket, &master_readfds);
                        FD_CLR(socket, &server_readfds);
                        unregisterAllHandlers(pep);
	                pep->close();
                    }
                }
#endif
            }
        }
        else
        {
            HSIter hit = handlers_.begin();
            HSIter hitend = handlers_.end();
            for ( ; hit != hitend; ++hit)
            {
                // skip if not ready
                int socket = hit->first;
                assert(!FD_ISSET(socket, &exceptfds));
                if (!FD_ISSET(socket, &readfds)) continue;
                // get handler
                UseCntPtr<EndPoint> pep( hit->second.first);
                UseCntPtr<Handler> phandler(hit->second.second);
                // is it a server socket or an accepted socket?
                if (FD_ISSET(socket, &server_readfds))
                {
                    // we have a server. do an accept and store
                    // in master fd set.
                    bool retry;
                    UseCntPtr<EndPoint> peer(pep->accept(retry));
                    if (peer == NULL) continue;
                    int accept_socket = peer->getSocket();
                    registerHandler(peer, phandler);
                    FD_SET(accept_socket, &master_readfds);
                    if (accept_socket > maxsocket) maxsocket = accept_socket;
                }
                else
                {
                    int status = (*phandler)(pep);
                    if (status > 0)
                    {
                        // success and done with this server
                        FD_CLR(socket, &master_readfds);
                        FD_CLR(socket, &server_readfds);
                        unregisterAllHandlers(pep);
	                pep->close();
                    }
                    else if (status < 0)
                    {
                        // some type of error. for now, close endpoint
                        if (errno == ECONNRESET)
                        {
                            // other side closed the connection.
                            FD_CLR(socket, &master_readfds);
                            FD_CLR(socket, &server_readfds);
                            unregisterAllHandlers(pep);
	                    pep->close();
                        }
                        else
                        {
                            TMSG("i/o handler failed");
                            DUMP(pep); DUMP(socket); DUMP(errno);
                            FD_CLR(socket, &master_readfds);
                            FD_CLR(socket, &server_readfds);
                            unregisterAllHandlers(pep);
	                    pep->close();
                        }
                    }
                }
            }
        }
    }

    return(0);
}

int
IterativeMSStreamServer::epollLoop()
{
    CheckReturn(isOk());
    CheckReturn(!endpoints_.empty());

    std::set<int> servers;

    HSIter hit = handlers_.begin();
    HSIter hitend = handlers_.end();
    for ( ; hit != hitend; ++hit)
    {
        int socket = hit->first;
        servers.insert(socket);
    }

    epoll_event ev;
    ::memset(&ev,0,sizeof(ev));

    epoll_event events[DefaultEPollEventsToHandle];

    for (Timer zero(0,0); !endpoints_.empty(); )
    {
        // do we have any timers?
        int timer;
#if USESTL
        TMRQIter tit = tmrqueue_.begin();
        if (tit != tmrqueue_.end())
        {
            Timer t2 = tit->first;
            Timer now;
            now.setToNow();
            t2 -= now;
            if (t2 < zero) t2 = zero;
            timer = t2.getTimeInMillisecs();
        }
        else
        {
            timer = -1;
        }
#else
        if (!tmrqueue_.isEmpty())
        {
            Tuple<Timer, Tuple<Timer, UseCntPtr<EndPoint> > > qtimer;
            tmrqueue_.front(qtimer);
            Timer t2 = qtimer.key;
            Timer now;
            now.setToNow();
            t2 -= now;
            if (t2 < zero) t2 = zero;
            timer = t2.getTimeInMillisecs();
	}
	else
	{
            timer = -1;
	}
#endif

        int eventnum = ::epoll_wait(epollfd_, events, 
                                    DefaultEPollEventsToHandle, timer);
        if (eventnum < 0)
        {
            if (errno == EINTR) continue;
            TMSG("epoll_wait failed");
            DUMP(errno);
            return(-1);
        }
        else if (eventnum == 0)
        {
            // we have a timeout. check queue
#if USESTL
            TMRQIter titbegin = tmrqueue_.begin();
            TMRQIter titend = tmrqueue_.end();
            TMRQIter tit = tmrqueue_.begin();

            Timer now;
            now.setToNow();

            for ( ; tit != titend && tit->first <= now; ++tit)
            {
                Timer tmr = tit->second.first;
                EndPoint ep = tit->second.second;

                int socket = ep.getSocket();
                THS::iterator hit = timerhandlers_.find(socket);
                if (hit == timerhandlers_.end()) continue;
                TimerHandler *handler = hit->second.second;

                int status = (*handler)(ep, tmr);
                if (status > 0)
                {
                    // success and done with this server
                    ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                    servers.erase(socket);
                    unregisterAllHandlers(ep);
                    ep.close();
                }
                else if (status < 0)
                {
                    // some type of error. for now, close endpoint
                    TMSG("timer handler failed");
                    DUMP(ep); DUMP(socket); DUMP(errno);
                    ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                    servers.erase(socket);
                    unregisterAllHandlers(ep);
                    ep.close();
                }
            }

            tmrqueue_.erase(titbegin, tit);
#else
            // we have a timeout. check queue
            Timer now;
            now.setToNow();
            
            while (!tmrqueue_.isEmpty())
            {
                Tuple<Timer, Tuple<Timer, UseCntPtr<EndPoint> > > qtimer;
                tmrqueue_.dequeue(qtimer);

                if (qtimer.key > now)
                {
                    tmrqueue_.enqueue(qtimer);
                    break;
                }

                Timer tmr = qtimer.data.key;
                UseCntPtr<EndPoint> pep(qtimer.data.data);

                int socket = pep->getSocket();
                THS::iterator hit = timerhandlers_.find(socket);
                if (hit == timerhandlers_.end()) continue;
                UseCntPtr<TimerHandler> phandler(hit->second.second);

                int status = (*phandler)(pep, tmr);
                if (status > 0)
                {
                    // success and done with this server
                    ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                    servers.erase(socket);
                    unregisterAllHandlers(pep);
                    pep->close();
                }
                else if (status < 0)
                {
                    // some type of error. for now, close endpoint
                    TMSG("timer handler failed");
                    DUMP(pep); DUMP(socket); DUMP(errno);
                    ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                    servers.erase(socket);
                    unregisterAllHandlers(pep);
                    pep->close();
                }
            }
#endif
        }
        else
        {
            for (int ievent=0; ievent<eventnum; ++ievent)
            {
                // socket with an event
                int socket = events[ievent].data.fd;

                // get handler
                HS::iterator hit = handlers_.find(socket);
                if (hit == handlers_.end()) continue;
                UseCntPtr<EndPoint> pep(hit->second.first);
                UseCntPtr<Handler> phandler(hit->second.second);

                // is it a server socket or an accepted socket?
                if (servers.find(socket) != servers.end())
                {
                    // we have a server. do an accept and store
                    // in master fd set.
                    bool retry;
                    UseCntPtr<EndPoint> peer(pep->accept(retry));
                    if (peer == NULL) continue;
                    int accept_socket = peer->getSocket();
                    peer->setNonBlocking(true);
                    registerHandler(peer, phandler);
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = accept_socket;
                    CheckReturn(::epoll_ctl(epollfd_, EPOLL_CTL_ADD, 
                                            accept_socket, &ev) == 0);
                }
                else
                {
                    int status = (*phandler)(pep);
                    if (status > 0)
                    {
                        // success and done with this server
                        ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                        servers.erase(socket);
                        unregisterAllHandlers(pep);
	                pep->close();
                    }
                    else if (status < 0)
                    {
                        // some type of error. for now, close endpoint
                        if (errno == ECONNRESET)
                        {
                            // other side closed the connection.
                            ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                            servers.erase(socket);
                            unregisterAllHandlers(pep);
	                    pep->close();
                        }
                        else
                        {
                            TMSG("i/o handler failed");
                            DUMP(pep); DUMP(socket); DUMP(errno);
                            ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                            servers.erase(socket);
                            unregisterAllHandlers(pep);
	                    pep->close();
                        }
                    }
                }
            }
        }
    }

    return(0);
}

int
IterativeMSStreamServer::finish()
{
    CheckReturn(isOk());

    if (mode_ == EPoll)
    {
        ::close(epollfd_);
        epollfd_ = -1;
    }

    EPIter epit = endpoints_.begin();
    EPIter epitend = endpoints_.end();
    for ( ; epit != epitend; ++epit)
    {
         UseCntPtr<EndPoint> serverep(epit->second);
         serverep->close();
    }

    endpoints_.clear();
    handlers_.clear();
    timerhandlers_.clear();

    setOk(false);

    return(0);
}

}

