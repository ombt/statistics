//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//

#ifndef __OMBT_ITERATIVE_REACTOR_H
#define __OMBT_ITERATIVE_REACTOR_H

// reactor for sockets

// system headers
#include <map>
#include <set>
#include <queue>

// local headers
#include "system/Debug.h"
#include "atomic/UCBaseObject.h"
#include "atomic/UseCntPtr.h"
#include "sockets/Address.h"
#include "sockets/EndPoint.h"
#include "signals/Signal.h"
#include "timers/Timer.h"
#include "logging/Logging.h"

namespace ombt {

// handlers follow this convention -
// return == 0 ==>> success and continue
// return < 0 ==>> error and terminate
// return > 0 ==>> success and close/remove end point.

// reactor for multiple connections
class IterativeReactor: public UCBaseObject {
public:
    // reactor type
    enum MultiplexMode { None, Select, EPoll, Poll };
    enum { DefaultEPollSize = 128 };
    enum { DefaultEPollEventsToHandle = 128 };

    // user-provided service handlers base classes
    class BaseHandler: public UCBaseObject {
    public:
        // ctor and dtor
        BaseHandler();
        virtual ~BaseHandler();
    
        // set the server
        void setServer(IterativeReactor *pserver);

    protected:
        UseCntPtr<IterativeReactor> pserver_;
    };

    // handle accepts
    class BaseAcceptHandler: public BaseHandler {
    public:
        // ctor and dtor
        BaseAcceptHandler();
        virtual ~BaseAcceptHandler();
        virtual int handler(EndPoint *serverep, EndPoint *acceptep);
    };

    // handle reads, writes and i/o exceptions
    class BaseIOHandler: public BaseHandler {
    public:
        // ctor and dtor
        BaseIOHandler();
        virtual ~BaseIOHandler();
        virtual int handler(EndPoint *serverep);
    };

    // handle signals
    class BaseSignalHandler: public BaseHandler {
    public:
        // ctor and dtor
        BaseSignalHandler();
        virtual ~BaseSignalHandler();
        virtual int handler(EndPoint *serverep, int signalno);
    };

    // handle timers
    class BaseTimerHandler: public BaseHandler {
    public:
        // ctor and dtor
        BaseTimerHandler();
        virtual ~BaseTimerHandler();
        virtual int handler(EndPoint *serverep, Timer timer);
    };

public:
    // ctors and dtor
    IterativeReactor(MultiplexMode mode = Select, int epollsize = DefaultEPollSize);
    virtual ~IterativeReactor();

    // register and unregister end-point handlers
    int registerEndPoint(EndPoint *pserver_ep);
    int unregisterEndPoint(EndPoint *pserver_ep);

    // register end-point handlers
    int registerAcceptHandler(EndPoint *pserver_ep, BaseAcceptHandler *phandler);
    int registerReadHandler(EndPoint *pserver_ep, BaseIOHandler *phandler);
    int registerWriteHandler(EndPoint *pserver_ep, BaseIOHandler *phandler);
    int registerExceptionHandler(EndPoint *pserver_ep, BaseIOHandler *phandler);

    // unregister end-point handlers
    int unregisterAcceptHandler(EndPoint *pserver_ep);
    int unregisterReadHandler(EndPoint *pserver_ep);
    int unregisterWriteHandler(EndPoint *pserver_ep);
    int unregisterExceptionHandler(EndPoint *pserver_ep);

    // register or unregister signal handlers.
    int registerSignalHandler(EndPoint *pserver_ep, BaseSignalHandler *phandler);
    int unregisterSignalHandler(EndPoint *pserver_ep); 

    // clean up routines
    void unregisterAllHandlers(EndPoint *pserver_ep); 

    // set a timer for an end point
    virtual int scheduleTimer(EndPoint *ep, Timer timer, BaseTimerHandler *handler);
    virtual int cancelTimer(EndPoint *ep, Timer timer, BaseTimerHandler *handler);

    // operations
    virtual int init();
    virtual int run();
    virtual int finish();

    // misc
    void setDone();

protected:
    // misc
    bool done_;

    // internal handler classes
    class InternalBaseHandler: public UCBaseObject {
    public:
        // ctor and dtor
        InternalBaseHandler(IterativeReactor *pserver);
        virtual ~InternalBaseHandler();
    
        // universal handler 
        virtual int handler(EndPoint *serverep);

    private:
        // leper colony
        InternalBaseHandler(const InternalBaseHandler &);
        InternalBaseHandler &operator=(const InternalBaseHandler &);

    protected:
        UseCntPtr<IterativeReactor> pserver_;
    };

    // handle accepts
    class InternalAcceptHandler: public InternalBaseHandler {
    public:
        InternalAcceptHandler(IterativeReactor *pserver, BaseAcceptHandler *phandler);
        virtual ~InternalAcceptHandler();
        virtual int handler(EndPoint *serverep);

    private:
        // leper colony
        InternalAcceptHandler(const InternalAcceptHandler &);
        InternalAcceptHandler &operator=(const InternalAcceptHandler &);

    protected:
        UseCntPtr<BaseAcceptHandler> phandler_;
    };

    // handle reads, writes and i/o exceptions
    class InternalIOHandler: public InternalBaseHandler {
    public:
        InternalIOHandler(IterativeReactor *pserver, BaseIOHandler *phandler);
        virtual ~InternalIOHandler();
        virtual int handler(EndPoint *serverep);

    private:
        InternalIOHandler(const InternalIOHandler &);
        InternalIOHandler &operator=(const InternalIOHandler &);

    protected:
        UseCntPtr<BaseIOHandler> phandler_;
    };

    // handle signals
    class InternalSignalHandler: public InternalBaseHandler {
    public:
        InternalSignalHandler(IterativeReactor *pserver, BaseSignalHandler *phandler);
        virtual ~InternalSignalHandler();
        virtual int handler(EndPoint *serverep);

    private:
        InternalSignalHandler(const InternalSignalHandler &);
        InternalSignalHandler &operator=(const InternalSignalHandler &);

    protected:
        UseCntPtr<BaseSignalHandler> phandler_;
    };

    // handle timers
    class InternalTimerHandler: public InternalBaseHandler {
    public:
        InternalTimerHandler(IterativeReactor *pserver, BaseTimerHandler *phandler);
        virtual ~InternalTimerHandler();
        virtual int handler(EndPoint *serverep);

    private:
        InternalTimerHandler(const InternalTimerHandler &);
        InternalTimerHandler &operator=(const InternalTimerHandler &);

    protected:
        UseCntPtr<BaseTimerHandler> phandler_;
    };

    // internal server handlers
    int acceptHandler(BaseAcceptHandler *phandler, EndPoint *ep);
    int ioHandler(BaseIOHandler *phandler, EndPoint *ep);
    int signalHandler(BaseSignalHandler *phandler, EndPoint *ep);
    int timerHandler(BaseTimerHandler *phandler, EndPoint *ep);

    // register or unregister with multiplexor
    int registerWithMultiplexor(EndPoint *pserver_ep, int &counter, 
                                fd_set &master_fdset, uint32_t epollevent);
    int registerReaderWithMultiplexor(EndPoint *pserver_ep);
    int registerWriterWithMultiplexor(EndPoint *pserver_ep);
    int registerExceptionWithMultiplexor(EndPoint *pserver_ep);
    int unregisterFromMultiplexor(EndPoint *pserver_ep, int &counter, 
                                  fd_set &master_fdset, uint32_t epollevent);
    int unregisterReaderFromMultiplexor(EndPoint *pserver_ep);
    int unregisterWriterFromMultiplexor(EndPoint *pserver_ep);
    int unregisterExceptionFromMultiplexor(EndPoint *pserver_ep);

private:
    // leper colony
    IterativeReactor(const IterativeReactor &src);
    IterativeReactor &operator=(const IterativeReactor &rhs);

protected:
    // type of io multiplexing
    MultiplexMode mode_;
    int epollfd_;
    int epollsize_;

    // epoll event flags per descriptor/socket
    typedef std::map<int, uint32_t> EVENTFLAGS;
    typedef EVENTFLAGS::iterator EVENTFLAGSIter;
    EVENTFLAGS epolleventflags_;

    // multiplexors
    int maxsocket_;
    fd_set master_readfds_;
    fd_set master_writefds_;
    fd_set master_exceptionfds_;

    // how many of each select type
    int nreaders_;
    int nwriters_;
    int nexceptions_;

    // track all the end points
    typedef std::map<int, UseCntPtr<EndPoint> > EP;
    typedef EP::iterator EPIter;
    typedef EP::const_iterator EPConstIter;
    EP endpoints_;

    // track handlers by socket
    class KeySocketOper {
    public:
        enum Oper { Read, Write, Exception };

        KeySocketOper(int sock, Oper oper):
            sock_(sock), oper_(oper) { }
        KeySocketOper(const KeySocketOper &src):
            sock_(src.sock_), oper_(src.oper_) { }
        ~KeySocketOper() { }

        KeySocketOper &operator=(const KeySocketOper &rhs)
        {
            sock_ = rhs.sock_;
            oper_ = rhs.oper_;
            return *this;
        }

        bool operator==(const KeySocketOper &rhs) const {
            return (sock_ == rhs.sock_) && (oper_ == rhs.oper_);
        }
        bool operator!=(const KeySocketOper &rhs) const {
            return !(*this == rhs);
        }
        bool operator<(const KeySocketOper &rhs) const {
            if (sock_ < rhs.sock_)
                return true;
            else if (sock_ == rhs.sock_)
            {
                if (oper_ < rhs.oper_)
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
        bool operator>=(const KeySocketOper &rhs) const {
            return !(*this < rhs);
        }
        bool operator>(const KeySocketOper &rhs) const {
            if (sock_ > rhs.sock_)
                return true;
            else if (sock_ == rhs.sock_)
            {
                if (oper_ > rhs.oper_)
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
        bool operator<=(const KeySocketOper &rhs) const {
            return !(*this > rhs);
        }

    public:
        int sock_;
        Oper oper_;
    };

    typedef std::pair<UseCntPtr<EndPoint>, UseCntPtr<InternalBaseHandler> > EPIBHPair;
    typedef std::map<KeySocketOper, EPIBHPair> IBHS;
    typedef IBHS::iterator IBHSIter;
    typedef IBHS::const_iterator IBHSConstIter;
    IBHS handlers_;

    // timer queue
    class TimerQueueItem {
    public:
        // ctors and dtor
        TimerQueueItem();
        TimerQueueItem(Timer timer, EndPoint *pep, InternalBaseHandler *phandler);
        TimerQueueItem(const TimerQueueItem &);
        ~TimerQueueItem();

        // assignment
        TimerQueueItem &operator=(const TimerQueueItem &);

        // comparison
        bool operator==(const TimerQueueItem &qi) const;
        bool operator!=(const TimerQueueItem &qi) const;
        bool operator<(const TimerQueueItem &qi) const;
        bool operator>(const TimerQueueItem &qi) const;
        bool operator<=(const TimerQueueItem &qi) const;
        bool operator>=(const TimerQueueItem &qi) const;

    public:
        // data
        Timer timer_;
        UseCntPtr<EndPoint> pendpoint_;
        UseCntPtr<InternalBaseHandler> phandler_;
    };

    typedef std::priority_queue<TimerQueueItem, std::vector<TimerQueueItem>, std::greater<TimerQueueItem> > TMRQ;
    TMRQ timerqueue_;

    struct SORTBYTAG {
        bool operator()(const TimerQueueItem &item1, const TimerQueueItem &item2)
        {
            return(item1.timer_.getTag() < item2.timer_.getTag());
        }
    };
    typedef std::set<TimerQueueItem, SORTBYTAG> CTMRS;
    typedef CTMRS::iterator CTMRSIter;
    CTMRS canceledtimers_;

protected:
    // utility functions
    int selectLoop();
    int epollLoop();
    void initselectfds(fd_set &readfds, fd_set &writefds, fd_set &exceptionsfds);
    void removeEndPoint(int socket);
    int handleStatus(int status, EndPoint *pserver_ep);
    void callHandler(KeySocketOper &key, int &status);
    void resetfdset(int count, fd_set *&pfdset, fd_set &fdset, const fd_set &master_fdset, int nbytes);
    void rescheduleTimer(TimerQueueItem &tqi);
};

}

#endif

