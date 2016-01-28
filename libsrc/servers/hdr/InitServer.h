//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_INIT_SERVER_H
#define __OMBT_INIT_SERVER_H

// init-like server

// headers
#include <stdio.h>
#include <set>
#include <map>
#include <list>
#include <string>
#include "servers/Server.h"

namespace ombt {

class InitServer: public Server
{
public:
    // ctors and dtor
    InitServer(int maxstartups);
    virtual ~InitServer();

    // server process to start up
    class SubServer: public Server
    {
    public:
       SubServer(): Server() { }
       virtual ~SubServer() { }

       // service to run
       virtual int operator()(const std::string &uri) { return(-1); }
    };

    // register service and handlers
    // form of service is address:service:protocol
    // address: X.X.X.X, localhost, N.N.N.N
    // service: port or service name
    // protocol: udp, tcp, datagram, stream
    //
    void registerService(const std::string &uri, 
                         InitServer::SubServer *subserver);

    // operations
    virtual int init();
    virtual int run();
    virtual int finish();

private:
    // leper colony
    InitServer(const InitServer &src);
    InitServer &operator=(const InitServer &rhs);

protected:
    // track services
    typedef std::map<const std::string, UseCntPtr<InitServer::SubServer> > SVCS;
    typedef SVCS::const_iterator SVCSConstIter;
    typedef SVCS::iterator SVCSIter;
    SVCS services_;

    // track service pids
    struct PID2ServiceData {
        int startups_;
        int maxstartups_;
        std::string uri_;
        UseCntPtr<InitServer::SubServer> psubserver_;

        PID2ServiceData(): 
            startups_(0), maxstartups_(-1), 
            uri_(), psubserver_(NULL) { }
        PID2ServiceData(const PID2ServiceData &pd): 
            startups_(pd.startups_), maxstartups_(pd.maxstartups_), 
            uri_(pd.uri_), psubserver_(pd.psubserver_) { }
        PID2ServiceData(int startups, int maxstartups, 
                const std::string &uri, 
                InitServer::SubServer *subserver_):
                startups_(startups), maxstartups_(maxstartups), 
                uri_(uri), psubserver_(psubserver_) { }
        PID2ServiceData &operator=(const PID2ServiceData &rhs) {
            if (this != &rhs)
            {
                startups_ = rhs.startups_;
                maxstartups_ = rhs.maxstartups_;
                uri_ = rhs.uri_;
                psubserver_ = rhs.psubserver_;
            }
            return(*this);
        }
    };

    typedef std::map<int, PID2ServiceData> PID2SERVICES;
    typedef PID2SERVICES::const_iterator PID2SERVICESConstIter;
    typedef PID2SERVICES::iterator PID2SERVICESIter;
    PID2SERVICES pid2services_;

    typedef std::set<int> PIDS;
    typedef PIDS::const_iterator PIDSConstIter;
    typedef PIDS::iterator PIDSIter;
    PIDS pids_;

    int registered_services_;
    int maxstartups_;

    // signal handling
    static void handler(int);
    static UseCntPtr<InitServer> pobject_;
};

}

#endif
