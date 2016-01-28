//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_MULTI_PROCESS_SERVER_PER_PROCESS_H
#define __OMBT_MULTI_PROCESS_SERVER_PER_PROCESS_H

// multiprocess single-service server per process

// headers
#include <stdio.h>
#include "servers/Server.h"
#include "sockets/Address.h"
#include "sockets/EndPoint.h"
#include "servers/MultiProcessServer.h"

namespace ombt {

// forking server for a connection
class MultiProcessDatagramServerPerProcess: public Server
{
public:
    // ctors and dtor
    MultiProcessDatagramServerPerProcess(int maxstartups=2);
    virtual ~MultiProcessDatagramServerPerProcess();

    // register service and handlers
    // form of service is address:service:protocol
    // address: X.X.X.X, localhost, N.N.N.N
    // service: port or service name
    // protocol: udp or datagram
    //
    void registerService(const std::string &service, MultiProcessDatagramServer::Handler *handler);

    // operations
    virtual int init();
    virtual int run();
    virtual int finish();

private:
    // leper colony
    MultiProcessDatagramServerPerProcess(const MultiProcessDatagramServerPerProcess &src);
    MultiProcessDatagramServerPerProcess &operator=(const MultiProcessDatagramServerPerProcess &rhs);

protected:
    // track services
    typedef std::map<const std::string, UseCntPtr<MultiProcessDatagramServer::Handler> > SVCS;
    typedef SVCS::const_iterator SVCSConstIter;
    typedef SVCS::iterator SVCSIter;
    SVCS services_;

    // track service pids
    struct PIDData {
        int startups_;
        int maxstartups_;
        std::string uri_;
        std::string service_;
        std::string host_;
        std::string protocol_;
        int port_;

        PIDData(): 
            startups_(0), maxstartups_(-1), uri_(),
            service_(), host_(), protocol_(), port_(-1) { };
        PIDData(const PIDData &pd): 
            startups_(pd.startups_), maxstartups_(pd.maxstartups_), 
            uri_(pd.uri_), service_(pd.service_), host_(pd.host_),
            protocol_(pd.protocol_), port_(pd.port_) { };
        PIDData(int startups, int maxstartups, const std::string &uri, 
                const std::string &service, const std::string &host, 
                const std::string &protocol, int port):
                startups_(startups), maxstartups_(maxstartups), 
                uri_(uri), service_(service), host_(host),
                protocol_(protocol), port_(port) { };
        PIDData &operator=(const PIDData &rhs) {
            if (this != &rhs)
            {
                startups_ = rhs.startups_;
                maxstartups_ = rhs.maxstartups_;
                uri_ = rhs.uri_;
                service_ = rhs.service_;
                host_ = rhs.host_;
                protocol_ = rhs.protocol_;
                port_ = rhs.port_;
            }
            return(*this);
        }
    };
    typedef std::map<int, PIDData> PIDS;
    typedef PIDS::const_iterator PIDSConstIter;
    typedef PIDS::iterator PIDSIter;
    PIDS pids_;

    int maxstartups_;

    // signal handling
    static void handler(int);
    static UseCntPtr<MultiProcessDatagramServerPerProcess> pobject_;
};

class MultiProcessStreamServerPerProcess: public Server
{
public:
    // ctors and dtor
    MultiProcessStreamServerPerProcess(int maxstartups=2);
    virtual ~MultiProcessStreamServerPerProcess();

    // register service and handlers
    // form of service is address:service:protocol
    // address: X.X.X.X, localhost, N.N.N.N
    // service: port or service name
    // protocol: tcp or stream
    //
    void registerService(const std::string &service, MultiProcessStreamServer::Handler *handler);

    // operations
    virtual int init();
    virtual int run();
    virtual int finish();

private:
    // leper colony
    MultiProcessStreamServerPerProcess(const MultiProcessStreamServerPerProcess &src);
    MultiProcessStreamServerPerProcess &operator=(const MultiProcessStreamServerPerProcess &rhs);

protected:
    // track services
    typedef std::map<const std::string, UseCntPtr<MultiProcessStreamServer::Handler> > SVCS;
    typedef SVCS::const_iterator SVCSConstIter;
    typedef SVCS::iterator SVCSIter;
    SVCS services_;

    // track service pids
    struct PIDData {
        int startups_;
        int maxstartups_;
        std::string uri_;
        std::string service_;
        std::string host_;
        std::string protocol_;
        int port_;

        PIDData(): 
            startups_(0), maxstartups_(-1), uri_(),
            service_(), host_(), protocol_(), port_(-1) { };
        PIDData(const PIDData &pd): 
            startups_(pd.startups_), maxstartups_(pd.maxstartups_), 
            uri_(pd.uri_), service_(pd.service_), host_(pd.host_),
            protocol_(pd.protocol_), port_(pd.port_) { };
        PIDData(int startups, int maxstartups, const std::string &uri, 
                const std::string &service, const std::string &host, 
                const std::string &protocol, int port):
                startups_(startups), maxstartups_(maxstartups), 
                uri_(uri), service_(service), host_(host),
                protocol_(protocol), port_(port) { };
        PIDData &operator=(const PIDData &rhs) {
            if (this != &rhs)
            {
                startups_ = rhs.startups_;
                maxstartups_ = rhs.maxstartups_;
                uri_ = rhs.uri_;
                service_ = rhs.service_;
                host_ = rhs.host_;
                protocol_ = rhs.protocol_;
                port_ = rhs.port_;
            }
            return(*this);
        }
    };
    typedef std::map<int, PIDData> PIDS;
    typedef PIDS::const_iterator PIDSConstIter;
    typedef PIDS::iterator PIDSIter;
    PIDS pids_;
    int maxstartups_;

    // signal handling
    static void handler(int);
    static UseCntPtr<MultiProcessStreamServerPerProcess> pobject_;
};

}

#endif
