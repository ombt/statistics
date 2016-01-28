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
#include <sys/wait.h>
#include <set>
#include "system/Debug.h"
#include "atomic/UseCntPtr.h"
#include "hdr/MultiProcessMultiServiceServer.h"

namespace ombt {

// signal handler for death-of-child
static void
reaper(int sig)
{
    int status;
    while (::wait3(&status, WNOHANG, (struct rusage *)0) >= 0) ;
    (void) signal(SIGCHLD, reaper);
}

// client server process
MultiProcMSDatagramServer::SubServer::SubServer(MultiplexMode mode):
        Server(), mode_(mode), epollfd_(-1), epollsize_(DefaultEPollSize), 
        handlers_(), timerhandlers_(), tmrqueue_()
{
    setOk(true);
}

MultiProcMSDatagramServer::SubServer::~SubServer()
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
MultiProcMSDatagramServer::SubServer::registerEndPoint(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    endpoints_.insert(std::pair<int, UseCntPtr<EndPoint> >(pep->getSocket(), pep));

    return(0);
}

int
MultiProcMSDatagramServer::SubServer::registerHandler(EndPoint *ep, Handler *h)
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
MultiProcMSDatagramServer::SubServer::registerTimerHandler(EndPoint *ep, TimerHandler *th)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    UseCntPtr<TimerHandler> pth(th);

    registerEndPoint(pep);
    pth->setServer(this);
    timerhandlers_.insert(std::pair<int, std::pair<UseCntPtr<EndPoint>, UseCntPtr<TimerHandler> > >(pep->getSocket(), std::pair<UseCntPtr<EndPoint>, UseCntPtr<TimerHandler> >(pep, pth)));

    return(0);
}

int 
MultiProcMSDatagramServer::SubServer::unregisterEndPoint(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    endpoints_.erase(pep->getSocket());

    return(0);
}

int 
MultiProcMSDatagramServer::SubServer::unregisterHandler(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    handlers_.erase(pep->getSocket());

    return(0);
}

int 
MultiProcMSDatagramServer::SubServer::unregisterTimerHandler(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    timerhandlers_.erase(pep->getSocket());

    return(0);
}

int 
MultiProcMSDatagramServer::SubServer::unregisterAllHandlers(EndPoint *ep)
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
MultiProcMSDatagramServer::SubServer::scheduleTimer(EndPoint *myep, Timer &timer)
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
MultiProcMSDatagramServer::SubServer::cancelTimer(EndPoint *myep, Timer &timer)
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
MultiProcMSDatagramServer::SubServer::init()
{
    CheckReturn(isOk());

    if (mode_ != EPoll) return(0);

    CheckReturn(epollsize_ >= 0);
    epollfd_ = ::epoll_create(epollsize_);
    CheckReturn(epollfd_ >= 0);

    HSIter hit = handlers_.begin();
    HSIter hitend = handlers_.end();
    for ( ; hit != hitend; ++hit)
    {
        UseCntPtr<EndPoint> pserverep(hit->second.first);
        pserverep->setNonBlocking(true);
        epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = pserverep->getSocket();
        CheckReturn(::epoll_ctl(epollfd_, EPOLL_CTL_ADD, 
                                pserverep->getSocket(), &ev) == 0);
    }

    return(0);
}

int
MultiProcMSDatagramServer::SubServer::run()
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
MultiProcMSDatagramServer::SubServer::selectLoop()
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
                    UseCntPtr<EndPoint> pep(tit->second.second);

                    int socket = pep->getSocket();
                    THS::iterator hit = timerhandlers_.find(socket);
                    if (hit == timerhandlers_.end()) continue;
                    UseCntPtr<TimerHandler> phandler(hit->second.second);

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
MultiProcMSDatagramServer::SubServer::epollLoop()
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
                    unregisterAllHandlers(ep);
                    ep.close();
                }
                else if (status < 0)
                {
                    // some type of error. for now, close endpoint
                    TMSG("timer handler failed");
                    DUMP(ep); DUMP(socket); DUMP(errno);
                    ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
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
MultiProcMSDatagramServer::SubServer::finish()
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
MultiProcMSDatagramServer::MultiProcMSDatagramServer(MultiplexMode mode):
        Server(), mode_(mode), epollfd_(-1), epollsize_(DefaultEPollSize), handlers_(), timerhandlers_(), tmrqueue_()
{
    setOk(true);
}

MultiProcMSDatagramServer::~MultiProcMSDatagramServer()
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
MultiProcMSDatagramServer::registerEndPoint(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    endpoints_.insert(std::pair<int, UseCntPtr<EndPoint> >(pep->getSocket(), pep));

    return(0);
}

int
MultiProcMSDatagramServer::registerHandler(EndPoint *ep, Handler *h)
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
MultiProcMSDatagramServer::registerTimerHandler(EndPoint *ep, TimerHandler *th)
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
MultiProcMSDatagramServer::unregisterEndPoint(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    endpoints_.erase(pep->getSocket());

    return(0);
}

int 
MultiProcMSDatagramServer::unregisterHandler(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    handlers_.erase(pep->getSocket());

    return(0);
}

int 
MultiProcMSDatagramServer::unregisterTimerHandler(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    timerhandlers_.erase(pep->getSocket());

    return(0);
}

int 
MultiProcMSDatagramServer::unregisterAllHandlers(EndPoint *ep)
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
MultiProcMSDatagramServer::scheduleTimer(EndPoint *myep, Timer &timer)
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
MultiProcMSDatagramServer::cancelTimer(EndPoint *myep, Timer &timer)
{
#if 0
    UseCntPtr<EndPoint> pmyep(myep);

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
MultiProcMSDatagramServer::init()
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
MultiProcMSDatagramServer::run()
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
MultiProcMSDatagramServer::selectLoop()
{
    CheckReturn(isOk());
    CheckReturn(!handlers_.empty());

    (void) signal(SIGCHLD, reaper);

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
                    UseCntPtr<TimerHandler> phandler(hit->second.second);

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

		int pid = ::fork();
		if (pid == 0)
		{
                    // the child -

                    // close all sockets not the child
                    EPIter epit = endpoints_.begin();
                    EPIter epitend = endpoints_.end();
                    for ( ; epit != epitend; ++epit)
                    {
                        if (epit->first == socket) continue;
                        UseCntPtr<EndPoint> ptmpep(epit->second);
                        unregisterAllHandlers(ptmpep);
                        ptmpep->close();
                    }

                    // create subserver and run it.
                    SubServer child(mode_);
                    child.registerEndPoint(pep);
                    child.registerHandler(pep, phandler);

                    // run the server
                    if (child.init() != 0) exit(-1);
                    if (child.run() != 0) exit(-1);
                    if (child.finish() != 0) exit(-1);

                    // all done
                    exit(0);
		}
		else if (pid < 0)
		{
                    // error in parent
                    return(-1);
		}
            }
        }
    }

    return(0);
}

int
MultiProcMSDatagramServer::epollLoop()
{
    CheckReturn(isOk());
    CheckReturn(!handlers_.empty());

    (void) signal(SIGCHLD, reaper);

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
                UseCntPtr<EndPoint> pep(tit->second.second);

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

		int pid = ::fork();
		if (pid == 0)
		{
                    // the child -

                    // close all sockets not the child
                    EPIter epit = endpoints_.begin();
                    EPIter epitend = endpoints_.end();
                    for ( ; epit != epitend; ++epit)
                    {
                        if (epit->first == socket) continue;
                        UseCntPtr<EndPoint> ptmpep(epit->second);
                        unregisterAllHandlers(ptmpep);
                        ptmpep->close();
                    }

                    // create subserver and run it.
                    SubServer child(mode_);
                    child.registerEndPoint(pep);
                    child.registerHandler(pep, phandler);

                    // run the server
                    if (child.init() != 0) exit(-1);
                    if (child.run() != 0) exit(-1);
                    if (child.finish() != 0) exit(-1);

                    // all done
                    exit(0);
		}
		else if (pid < 0)
		{
                    // error in parent
                    return(-1);
		}
            }
        }
    }

    return(0);
}

int
MultiProcMSDatagramServer::finish()
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
MultiProcMSStreamServer::SubServer::SubServer(MultiplexMode mode):
        Server(), mode_(mode), epollfd_(-1), epollsize_(DefaultEPollSize), 
        handlers_(), timerhandlers_(), tmrqueue_()
{
    setOk(true);
}

MultiProcMSStreamServer::SubServer::~SubServer()
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
MultiProcMSStreamServer::SubServer::registerEndPoint(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    endpoints_.insert(std::pair<int, UseCntPtr<EndPoint> >(pep->getSocket(), pep));

    return(0);
}

int
MultiProcMSStreamServer::SubServer::registerHandler(EndPoint *ep, Handler *h)
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
MultiProcMSStreamServer::SubServer::registerTimerHandler(EndPoint *ep, TimerHandler *th)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    UseCntPtr<TimerHandler> pth(th);

    registerEndPoint(ep);
    pth->setServer(this);
    timerhandlers_.insert(std::pair<int, std::pair<UseCntPtr<EndPoint>, UseCntPtr<TimerHandler> > >(pep->getSocket(), std::pair<UseCntPtr<EndPoint>, UseCntPtr<TimerHandler> >(pep, pth)));

    return(0);
}

int 
MultiProcMSStreamServer::SubServer::unregisterEndPoint(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    endpoints_.erase(pep->getSocket());

    return(0);
}

int 
MultiProcMSStreamServer::SubServer::unregisterHandler(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    handlers_.erase(pep->getSocket());

    return(0);
}

int 
MultiProcMSStreamServer::SubServer::unregisterTimerHandler(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    timerhandlers_.erase(pep->getSocket());

    return(0);
}

int 
MultiProcMSStreamServer::SubServer::unregisterAllHandlers(EndPoint *ep)
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
MultiProcMSStreamServer::SubServer::scheduleTimer(EndPoint *myep, Timer &timer)
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
MultiProcMSStreamServer::SubServer::cancelTimer(EndPoint *myep, Timer &timer)
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
MultiProcMSStreamServer::SubServer::init()
{
    CheckReturn(isOk());

    if (mode_ != EPoll) return(0);

    CheckReturn(epollsize_ >= 0);
    epollfd_ = ::epoll_create(epollsize_);
    CheckReturn(epollfd_ >= 0);

    HSIter hit = handlers_.begin();
    HSIter hitend = handlers_.end();
    for ( ; hit != hitend; ++hit)
    {
        UseCntPtr<EndPoint> pserverep(hit->second.first);
        pserverep->setNonBlocking(true);
        epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = pserverep->getSocket();
        CheckReturn(::epoll_ctl(epollfd_, EPOLL_CTL_ADD, 
                                pserverep->getSocket(), &ev) == 0);
    }

    return(0);
}

int
MultiProcMSStreamServer::SubServer::run()
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
MultiProcMSStreamServer::SubServer::selectLoop()
{
    CheckReturn(isOk());
    CheckReturn(!endpoints_.empty());

    fd_set readfds;
    FD_ZERO(&readfds);
    fd_set exceptfds;
    FD_ZERO(&exceptfds);
    fd_set master_readfds;
    FD_ZERO(&master_readfds);

    int maxsocket = -1;
    HSIter hit = handlers_.begin();
    HSIter hitend = handlers_.end();
    for ( ; hit != hitend; ++hit)
    {
        int socket = hit->first;
        if (socket > maxsocket) maxsocket = socket;
        FD_SET(socket, &master_readfds);
    }

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
                    UseCntPtr<TimerHandler> phandler(hit->second.second);

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
            HSIter hit = handlers_.begin();
            HSIter hitend = handlers_.end();
            for ( ; hit != hitend; ++hit)
            {
                // skip if not ready
                int socket = hit->first;
                assert(!FD_ISSET(socket, &exceptfds));
                if (!FD_ISSET(socket, &readfds)) continue;
                // get handler
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
                    if (errno == ECONNRESET)
                    {
                        // other side closed the connection.
                        FD_CLR(socket, &master_readfds);
                        unregisterAllHandlers(pep);
                        pep->close();
                    }
                    else
                    {
                        TMSG("i/o handler failed");
                        DUMP(pep); DUMP(socket); DUMP(errno);
                        FD_CLR(socket, &master_readfds);
                        unregisterAllHandlers(pep);
                        pep->close();
                    }
                }
            }
        }
    }

    return(0);
}

int
MultiProcMSStreamServer::SubServer::epollLoop()
{
    CheckReturn(isOk());
    CheckReturn(!endpoints_.empty());

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
                    unregisterAllHandlers(ep);
                    ep.close();
                }
                else if (status < 0)
                {
                    // some type of error. for now, close endpoint
                    TMSG("timer handler failed");
                    DUMP(ep); DUMP(socket); DUMP(errno);
                    ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
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
            for (int ievent=0; ievent<eventnum; ++ievent)
            {
                // socket with an event
                int socket = events[ievent].data.fd;

                // get handler
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
                    if (errno == ECONNRESET)
                    {
                        // other side closed the connection.
                        ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                        unregisterAllHandlers(pep);
                        pep->close();
                    }
                    else
                    {
                        TMSG("i/o handler failed");
                        DUMP(pep); DUMP(socket); DUMP(errno);
                        ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
                        unregisterAllHandlers(pep);
                        pep->close();
                    }
                }
            }
        }
    }

    return(0);
}

int
MultiProcMSStreamServer::SubServer::finish()
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
MultiProcMSStreamServer::MultiProcMSStreamServer(MultiplexMode mode):
        Server(), mode_(mode), epollfd_(-1), epollsize_(DefaultEPollSize), handlers_(), timerhandlers_(), tmrqueue_()
{
    setOk(true);
}

MultiProcMSStreamServer::~MultiProcMSStreamServer()
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
MultiProcMSStreamServer::registerEndPoint(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    endpoints_.insert(std::pair<int, UseCntPtr<EndPoint> >(pep->getSocket(), pep));

    return(0);
}

int
MultiProcMSStreamServer::registerHandler(EndPoint *ep, Handler *h)
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
MultiProcMSStreamServer::registerTimerHandler(EndPoint *ep, TimerHandler *th)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    UseCntPtr<TimerHandler> pth(th);

    registerEndPoint(ep);
    pth->setServer(this);
    timerhandlers_.insert(std::pair<int, std::pair<UseCntPtr<EndPoint>, UseCntPtr<TimerHandler> > >(pep->getSocket(), std::pair<UseCntPtr<EndPoint>, UseCntPtr<TimerHandler> >(pep, pth))); 
    return(0);
}

int 
MultiProcMSStreamServer::unregisterEndPoint(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    endpoints_.erase(pep->getSocket());

    return(0);
}

int 
MultiProcMSStreamServer::unregisterHandler(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    handlers_.erase(pep->getSocket());

    return(0);
}

int 
MultiProcMSStreamServer::unregisterTimerHandler(EndPoint *ep)
{
    CheckReturn(isOk());

    UseCntPtr<EndPoint> pep(ep);
    timerhandlers_.erase(pep->getSocket());

    return(0);
}

int 
MultiProcMSStreamServer::unregisterAllHandlers(EndPoint *ep)
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
MultiProcMSStreamServer::scheduleTimer(EndPoint *myep, Timer &timer)
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
MultiProcMSStreamServer::cancelTimer(EndPoint *myep, Timer &timer)
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
MultiProcMSStreamServer::init()
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
MultiProcMSStreamServer::run()
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
MultiProcMSStreamServer::selectLoop()
{
    CheckReturn(isOk());
    CheckReturn(!endpoints_.empty());

    (void) signal(SIGCHLD, reaper);

    fd_set readfds;
    FD_ZERO(&readfds);
    fd_set exceptfds;
    FD_ZERO(&exceptfds);
    fd_set master_readfds;
    FD_ZERO(&master_readfds);

    int maxsocket = -1;
    HSIter hit = handlers_.begin();
    HSIter hitend = handlers_.end();
    for ( ; hit != hitend; ++hit)
    {
        int socket = hit->first;
        if (socket > maxsocket) maxsocket = socket;
        FD_SET(socket, &master_readfds);
    }

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
                    UseCntPtr<EndPoint> pep(tit->second.second);

                    int socket = pep->getSocket();
                    THS::iterator hit = timerhandlers_.find(socket);
                    if (hit == timerhandlers_.end()) continue;
                    UseCntPtr<TimerHandler> phandler(hit->second.second);

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
            HSIter hit = handlers_.begin();
            HSIter hitend = handlers_.end();
            for ( ; hit != hitend; ++hit)
            {
                // skip if not ready
                int socket = hit->first;
                assert(!FD_ISSET(socket, &exceptfds));
                if (!FD_ISSET(socket, &readfds)) continue;
                // get handler
                UseCntPtr<EndPoint> pep(hit->second.first);
                UseCntPtr<Handler> phandler(hit->second.second);
                // accept connection and start up handler
                bool retry;
                UseCntPtr<EndPoint> peer(pep->accept(retry));
                if (peer == NULL) continue;
                int peer_socket = peer->getSocket();

		int pid = ::fork();
		if (pid == 0)
		{
                    // the child -

                    // close all sockets not the child
                    EPIter epit = endpoints_.begin();
                    EPIter epitend = endpoints_.end();
                    for ( ; epit != epitend; ++epit)
                    {
                        if (epit->first == peer_socket) continue;
                        UseCntPtr<EndPoint> ptmpep(epit->second);
                        unregisterAllHandlers(ptmpep);
                        ptmpep->close();
                    }

                    // create subserver and run it.
                    SubServer child(mode_);
                    child.registerEndPoint(peer);
                    child.registerHandler(peer, phandler);

                    // run the server
                    if (child.init() != 0) exit(-1);
                    if (child.run() != 0) exit(-1);
                    if (child.finish() != 0) exit(-1);

                    // all done
                    exit(0);
		}
                else if (pid > 0)
                {
                    // parent -- close child socket
                    peer->close();
                }
		else if (pid < 0)
		{
                    // error in parent
                    return(-1);
		}
            }
        }
    }

    return(0);
}

int
MultiProcMSStreamServer::epollLoop()
{
    CheckReturn(isOk());
    CheckReturn(!endpoints_.empty());

    (void) signal(SIGCHLD, reaper);

    epoll_event ev;
    memset(&ev,0,sizeof(ev));

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
                UseCntPtr<EndPoint> pep(tit->second.second);

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
            for (int ievent=0; ievent<eventnum; ++ievent)
            {
                // socket with an event
                int socket = events[ievent].data.fd;

                // get handler
                HS::iterator hit = handlers_.find(socket);
                if (hit == handlers_.end()) continue;
                UseCntPtr<EndPoint> pep(hit->second.first);
                UseCntPtr<Handler> phandler(hit->second.second);

                // do an accept ans start up a client
                bool retry;
                UseCntPtr<EndPoint> peer(pep->accept(retry));
                if (peer == NULL) continue;
                peer->setNonBlocking(true);
                int peer_socket = peer->getSocket();

		int pid = ::fork();
		if (pid == 0)
		{
                    // the child -

                    // close all sockets not the child
                    EPIter epit = endpoints_.begin();
                    EPIter epitend = endpoints_.end();
                    for ( ; epit != epitend; ++epit)
                    {
                        if (epit->first == peer_socket) continue;
                        UseCntPtr<EndPoint> ptmpep(epit->second);
                        unregisterAllHandlers(ptmpep);
                        ptmpep->close();
                    }

                    // create subserver and run it.
                    SubServer child(mode_);
                    child.registerEndPoint(peer);
                    child.registerHandler(peer, phandler);

                    // run the server
                    if (child.init() != 0) exit(-1);
                    if (child.run() != 0) exit(-1);
                    if (child.finish() != 0) exit(-1);

                    // all done
                    exit(0);
		}
                else if (pid > 0)
                {
                    // parent -- close child socket
                    peer->close();
                }
		else if (pid < 0)
		{
                    // error in parent
                    return(-1);
		}
            }
        }
    }

    return(0);
}

int
MultiProcMSStreamServer::finish()
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

}
