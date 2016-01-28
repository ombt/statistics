//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_MULTI_PROCESS_MULTI_SERVICE_SERVER_H
#define __OMBT_MULTI_PROCESS_MULTI_SERVICE_SERVER_H

// iterative multi-service server

// system headers
#include <map>

// local headers
#include "system/Debug.h"
#include "atomic/UCBaseObject.h"
#include "atomic/UseCntPtr.h"
#include "servers/Server.h"
#include "sockets/Address.h"
#include "sockets/EndPoint.h"
#include "timers/Timer.h"
#include "adt/Tuple.h"
#include "adt/PriorityQueue_Array.h"

namespace ombt {

#define USESTL 0

// iterative server for a connection
class MultiProcMSDatagramServer: public Server
{
public:
    // message handler
    class Handler: public UCBaseObject {
    public:
        // ctor and dtor
        Handler(): UCBaseObject(true), pserver_(NULL) { }
        virtual ~Handler() { };
    
        // message handler
	//
	// return == 0 ==>> success and continue
	// return < 0 ==>> error and terminate
	// return > 0 ==>> success and close/remove end point.
	//
        virtual int operator()(EndPoint *pserverep) { return(1); };

        // set the server
        void setServer(Server *pserver) { pserver_ = pserver; } 

    protected:
        UseCntPtr<Server> pserver_;
    };

    // timer handler
    class TimerHandler: public UCBaseObject {
    public:
        // ctor and dtor
        TimerHandler(): UCBaseObject(true), pserver_(NULL) { }
        virtual ~TimerHandler() { };
    
        // timer handler
	//
	// return == 0 ==>> success and continue
	// return < 0 ==>> error and terminate
	// return > 0 ==>> success and close/remove end point.
	//
        virtual int operator()(EndPoint *pserverep, Timer &timer) { return(1); };

        // set the server
        void setServer(Server *pserver) { pserver_ = pserver; } 

    protected:
        UseCntPtr<Server> pserver_;
    };

    // types of containers to track handlers and endpoints
    typedef std::map<int, UseCntPtr<EndPoint> > EP;
    typedef EP::iterator EPIter;
    typedef EP::const_iterator EPConstIter;

    typedef std::map<int, std::pair<UseCntPtr<EndPoint>, UseCntPtr<Handler> > > HS;
    typedef HS::iterator HSIter;
    typedef HS::const_iterator HSConstIter;

    typedef std::map<int, std::pair<UseCntPtr<EndPoint>, UseCntPtr<TimerHandler> > > THS;
    typedef THS::iterator THSIter;
    typedef THS::const_iterator THSConstIter;

#if USESTL
    typedef std::multimap<Timer, std::pair<Timer, UseCntPtr<EndPoint> > > TMRQ;
    typedef TMRQ::iterator TMRQIter;
    typedef TMRQ::const_iterator TMRQConstIter;
#else
    typedef PriorityQueue_Array< Tuple<Timer, Tuple<Timer, UseCntPtr<EndPoint> > > > TMRQ;
#endif

    class SubServer: public Server
    {
    public:
        // ctors and dtor
        SubServer(MultiplexMode mode = Select);
        virtual ~SubServer();

        // register and unregister end-point handlers
        int registerEndPoint(EndPoint *server_ep);
        int unregisterEndPoint(EndPoint *server_ep);
        int registerHandler(EndPoint *server_ep, Handler *handler);
        int registerTimerHandler(EndPoint *server_ep, TimerHandler *handler);
        int unregisterHandler(EndPoint *server_ep);
        int unregisterTimerHandler(EndPoint *server_ep);
        int unregisterAllHandlers(EndPoint *server_ep);

        // set a timer for an end point
        virtual void scheduleTimer(EndPoint *myep, Timer &timer);
        virtual void cancelTimer(EndPoint *myep, Timer &timer);

        // operations
        virtual int init();
        virtual int run();
        virtual int finish();

        // parameters
        int getEPollSize() const { return(epollsize_); }
        void setEPollSize(int epollsize) { epollsize_ = epollsize; }

    private:
        // leper colony
        SubServer(const SubServer &src);
        SubServer &operator=(const SubServer &rhs);

    protected:
        // type of io multiplexing
        Server::MultiplexMode mode_;
        int epollfd_;
        int epollsize_;

        // multiplexors
        int selectLoop();
        int epollLoop();

        // track all the end points
        EP endpoints_;

        // track handlers by socket
        HS handlers_;
        THS timerhandlers_;

        // timer queue
        TMRQ tmrqueue_;
    };

    // ctors and dtor
    MultiProcMSDatagramServer(MultiplexMode mode = Select);
    virtual ~MultiProcMSDatagramServer();

    // register and unregister end-point handlers
    int registerEndPoint(EndPoint *server_ep);
    int unregisterEndPoint(EndPoint *server_ep);
    int registerHandler(EndPoint *server_ep, Handler *handler);
    int registerTimerHandler(EndPoint *server_ep, TimerHandler *handler);
    int unregisterHandler(EndPoint *server_ep);
    int unregisterTimerHandler(EndPoint *server_ep);
    int unregisterAllHandlers(EndPoint *server_ep);

    // set a timer for an end point
    virtual void scheduleTimer(EndPoint *myep, Timer &timer);
    virtual void cancelTimer(EndPoint *myep, Timer &timer);

    // operations
    virtual int init();
    virtual int run();
    virtual int finish();

    // parameters
    int getEPollSize() const { return(epollsize_); }
    void setEPollSize(int epollsize) { epollsize_ = epollsize; }

private:
    // leper colony
    MultiProcMSDatagramServer(const MultiProcMSDatagramServer &src);
    MultiProcMSDatagramServer &operator=(const MultiProcMSDatagramServer &rhs);

protected:
    // type of io multiplexing
    Server::MultiplexMode mode_;
    int epollfd_;
    int epollsize_;

    // multipexors
    int selectLoop();
    int epollLoop();

    // track all the end points
    EP endpoints_;

    // track handlers by socket
    HS handlers_;
    THS timerhandlers_;

    // timer queue
    TMRQ tmrqueue_;
};

class MultiProcMSStreamServer: public Server
{
public:
    // message handler for server endpoints AND accepted endpoints
    class Handler: public UCBaseObject {
    public:
        // ctor and dtor
        Handler(): UCBaseObject(true), pserver_(NULL) { }
        virtual ~Handler() { };
    
        // client handler
	//
	// return == 0 ==>> success and continue
	// return < 0 ==>> error and terminate
	// return > 0 ==>> success and close/remove end point.
	//
        virtual int operator()(EndPoint *acceptep) { return(1); };

        // set the server
        void setServer(Server *pserver) { pserver_ = pserver; } 

    protected:
        UseCntPtr<Server> pserver_;
    };

    // timer handler
    class TimerHandler: public UCBaseObject {
    public:
        // ctor and dtor
        TimerHandler(): UCBaseObject(true), pserver_(NULL) { }
        virtual ~TimerHandler() { };
    
        // timer handler
	//
	// return == 0 ==>> success and continue
	// return < 0 ==>> error and terminate
	// return > 0 ==>> success and close/remove end point.
	//
        virtual int operator()(EndPoint *acceptep, Timer &timer) { return(1); };

        // set the server
        void setServer(Server *pserver) { pserver_ = pserver; } 

    protected:
        UseCntPtr<Server> pserver_;
    };

    // types of containers to track handlers and endpoints
    typedef std::map<int, UseCntPtr<EndPoint> > EP;
    typedef EP::iterator EPIter;
    typedef EP::const_iterator EPConstIter;

    typedef std::map<int, std::pair<UseCntPtr<EndPoint>, UseCntPtr<Handler> > > HS;
    typedef HS::iterator HSIter;
    typedef HS::const_iterator HSConstIter;

    typedef std::map<int, std::pair<UseCntPtr<EndPoint>, UseCntPtr<TimerHandler> > > THS;
    typedef THS::iterator THSIter;
    typedef THS::const_iterator THSConstIter;

#if USESTL
    typedef std::multimap<Timer, std::pair<Timer, UseCntPtr<EndPoint> > > TMRQ;
    typedef TMRQ::iterator TMRQIter;
    typedef TMRQ::const_iterator TMRQConstIter;
#else
    typedef PriorityQueue_Array< Tuple<Timer, Tuple<Timer, UseCntPtr<EndPoint> > > > TMRQ;
#endif

    class SubServer: public Server
    {
    public:

        // ctors and dtor
        SubServer(MultiplexMode mode = Select);
        virtual ~SubServer();

        // register and unregister end-point handlers
        int registerEndPoint(EndPoint *server_ep);
        int unregisterEndPoint(EndPoint *server_ep);
        int registerHandler(EndPoint *server_ep, Handler *handler);
        int registerTimerHandler(EndPoint *server_ep, TimerHandler *handler);
        int unregisterHandler(EndPoint *server_ep);
        int unregisterTimerHandler(EndPoint *server_ep);
        int unregisterAllHandlers(EndPoint *server_ep);

        // set a timer for an end point
        virtual void scheduleTimer(EndPoint *myep, Timer &timer);
        virtual void cancelTimer(EndPoint *myep, Timer &timer);

        // operations
        virtual int init();
        virtual int run();
        virtual int finish();

        // parameters
        int getEPollSize() const { return(epollsize_); }
        void setEPollSize(int epollsize) { epollsize_ = epollsize; }

    private:
        // leper colony
        SubServer(const SubServer &src);
        SubServer &operator=(const SubServer &rhs);

    protected:
        // type of io multiplexing
        Server::MultiplexMode mode_;
        int epollfd_;
        int epollsize_;

        // multipexors
        int selectLoop();
        int epollLoop();

        // track all the end points
        EP endpoints_;

        // track handlers by socket
        HS handlers_;
        THS timerhandlers_;

        // timer queue
        TMRQ tmrqueue_;
    };

    // ctors and dtor
    MultiProcMSStreamServer(MultiplexMode mode = Select);
    virtual ~MultiProcMSStreamServer();

    // register and unregister end-point handlers
    int registerEndPoint(EndPoint *server_ep);
    int unregisterEndPoint(EndPoint *server_ep);
    int registerHandler(EndPoint *server_ep, Handler *handler);
    int registerTimerHandler(EndPoint *server_ep, TimerHandler *handler);
    int unregisterHandler(EndPoint *server_ep);
    int unregisterTimerHandler(EndPoint *server_ep);
    int unregisterAllHandlers(EndPoint *server_ep);

    // set a timer for an end point
    virtual void scheduleTimer(EndPoint *myep, Timer &timer);
    virtual void cancelTimer(EndPoint *myep, Timer &timer);

    // operations
    virtual int init();
    virtual int run();
    virtual int finish();

    // parameters
    int getEPollSize() const { return(epollsize_); }
    void setEPollSize(int epollsize) { epollsize_ = epollsize; }

private:
    // leper colony
    MultiProcMSStreamServer(const MultiProcMSStreamServer &src);
    MultiProcMSStreamServer &operator=(const MultiProcMSStreamServer &rhs);

protected:
    // type of io multiplexing
    Server::MultiplexMode mode_;
    int epollfd_;
    int epollsize_;

    // multipexors
    int selectLoop();
    int epollLoop();

    // track all the end points
    EP endpoints_;

    // track handlers by socket
    HS handlers_;
    THS timerhandlers_;

    // timer queue
    TMRQ tmrqueue_;
};

}

#endif
