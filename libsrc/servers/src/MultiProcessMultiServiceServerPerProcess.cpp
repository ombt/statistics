//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// iterative server class

// headers
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "system/Debug.h"
#include "atomic/UseCntPtr.h"
#include "parsing/Tokenizer.h"
#include "logging/Logging.h"
#include "hdr/MultiProcessMultiServiceServerPerProcess.h"

namespace ombt {

// class statics
UseCntPtr<MultiProcMSDatagramServerPerProcess> MultiProcMSDatagramServerPerProcess::pobject_(NULL);

void
ombt::MultiProcMSDatagramServerPerProcess::handler(int sig)
{
    if (pobject_ != NULL) pobject_->finish();
    exit(0);
}

// ctors and dtor
MultiProcMSDatagramServerPerProcess::MultiProcMSDatagramServerPerProcess(int maxprocnum, int maxs, Server::MultiplexMode mpmode):
    Server(), services_(), pid2services_(), pids_(), maxprocnum_(maxprocnum), registered_services_(0), maxstartups_(maxs), mpmode_(mpmode)
{
    MustBeTrue(maxprocnum_ > 0);
    MustBeTrue(maxstartups_ > 0);
    setOk(true);
}

MultiProcMSDatagramServerPerProcess::~MultiProcMSDatagramServerPerProcess()
{
    setOk(false);
}

// register services and handlers
void
MultiProcMSDatagramServerPerProcess::registerService(
    const std::string &service, 
    MultiProcMSDatagramServer::Handler *handler)
{
    UseCntPtr<MultiProcMSDatagramServer::Handler> phandler(handler);
    services_.insert(std::pair<std::string, UseCntPtr<MultiProcMSDatagramServer::Handler> >(service, phandler));
}

// operations
int
MultiProcMSDatagramServerPerProcess::init()
{
    if (isNotOk())
        return(NOTOK);
    else
        return(OK);
}

int
MultiProcMSDatagramServerPerProcess::run()
{
    if (isNotOk()) return(NOTOK);
    if (services_.empty()) return(NOTOK);

    MultiProcMSDatagramServerPerProcess::pobject_ = this;
    ::signal(SIGINT, MultiProcMSDatagramServerPerProcess::handler);
    ::signal(SIGQUIT, MultiProcMSDatagramServerPerProcess::handler);
    ::signal(SIGTERM, MultiProcMSDatagramServerPerProcess::handler);

    SVCSIter sit = services_.begin();
    SVCSIter sitend = services_.end();

    registered_services_ = services_.size();
    int services_per_proc = registered_services_/maxprocnum_;
    if (registered_services_%maxprocnum_ > 0) ++services_per_proc;

    for (int iproc=1; (iproc<=maxprocnum_) && (sit != sitend); ++iproc)
    {
        SVCSList proc_svcs;
        for (int iservice=1; 
            (iservice<=services_per_proc) &&
            (sit != sitend); ++sit, ++iservice)
        {
            dbgprintf("Starting service %s in process %d.\n", 
                      sit->first.c_str(), iproc);
            proc_svcs.push_back(
                std::pair<std::string, UseCntPtr<MultiProcMSDatagramServer::Handler> >( sit->first, sit->second));
        }

        int pid = ::fork();
        if (pid == 0)
        {
            // child server
            MultiProcMSDatagramServer server(mpmode_);
            assert(server.isOk());

            SVCSListConstIter psit = proc_svcs.begin();
            SVCSListConstIter psitend = proc_svcs.end();
            
            for ( ; psit != psitend; ++psit)
            {
                std::string uri(psit->first);
                UseCntPtr<MultiProcMSDatagramServer::Handler> phdlr(psit->second);

                Service s(uri);
                MustBeTrue(s.isOk());
                int port = s.getPort();
                std::string service = s.getService();
                std::string host = s.getHost();
                std::string protocol = s.getProtocol();

                // child server
                UseCntPtr<Socket> psocket;
                UseCntPtr<Address> paddress;
                if (protocol == "udp")
                {
                    psocket = new UdpSocket();
                    MustBeTrue(psocket->isOk());
                    paddress = new InetAddress(host, port);
                    MustBeTrue(paddress->isOk());
                }
                else if (protocol == "datagram")
                {
                    psocket = new DatagramSocket();
                    MustBeTrue(psocket->isOk());
                    ::unlink(service.c_str());
                    paddress = new LocalAddress(service);
                    MustBeTrue(paddress->isOk());
                }
                else
                {
                    MustBeTrue(0);
                }

                UseCntPtr<EndPoint> pserver_ep(new EndPoint(psocket, paddress));
                MustBeTrue(pserver_ep->isOk());

                MustBeTrue(server.registerHandler(pserver_ep, phdlr) == 0);
            }

            MustBeTrue(server.init() == 0);
            MustBeTrue(server.run() == 0);
            MustBeTrue(server.finish() == 0);

            exit(OK);
        }
        else if (pid > 0)
        {
            // parent -- track pid
            SVCSListConstIter psit = proc_svcs.begin();
            SVCSListConstIter psitend = proc_svcs.end();
            
            for ( ; psit != psitend; ++psit)
            {
                std::string uri(psit->first);

                Service s(uri);
                MustBeTrue(s.isOk());

                int port = s.getPort();
                std::string service = s.getService();
                std::string host = s.getHost();
                std::string protocol = s.getProtocol();

                UseCntPtr<MultiProcMSDatagramServer::Handler> phdlr(psit->second);

                dbgprintf("Service %s (pid=%d) started.\n", uri.c_str(), pid);

                PID2ServiceData data(1,maxstartups_,uri,service,host,protocol,port,phdlr);
                pid2services_.insert(std::pair<int, PID2ServiceData>(pid, data));
            }
            pids_.insert(pid);
        }
        else
        {
            // some type of error
            MustBeTrue(pid >= 0);
        }
    }

    for (int status; !pids_.empty(); )
    {
        pid_t pid = ::waitpid(-1, &status, 0);
        if (errno == ECHILD)
        {
            pids_.clear();
            return(NOTOK);
        }

        PID2SERVICESIterPair piters = pid2services_.equal_range(pid);
        if (piters.first == piters.second) continue;

        dbgprintf("Process %d died.\n", pid);

        SVCDATAList proc_svcs_data;
        PID2SERVICESIter lbit = piters.first;
        PID2SERVICESIter ubit = piters.second;

        for ( ; lbit != ubit; ++lbit)
        {
            dbgprintf("Service %s (pid=%d) died.\n",
                      lbit->second.uri_.c_str(), pid);
            if (lbit->second.startups_ < lbit->second.maxstartups_)
            {
                dbgprintf("Service %s (pid=%d) attempting to restart (%d out of %d).\n",
                          lbit->second.uri_.c_str(), pid, 
                          lbit->second.startups_, lbit->second.maxstartups_);

                PID2ServiceData pdata(lbit->second.startups_+1,
                                      lbit->second.maxstartups_,
                                      lbit->second.uri_,
                                      lbit->second.service_,
                                      lbit->second.host_,
                                      lbit->second.protocol_,
                                      lbit->second.port_,
                                      lbit->second.phandler_);
                proc_svcs_data.push_back(pdata);
            }
            else
            {
                dbgprintf("Service %s (pid=%d) NOT restarted (%d out of %d).\n",
                          lbit->second.uri_.c_str(), pid, 
                          lbit->second.startups_, lbit->second.maxstartups_);
            }
        }

        if (proc_svcs_data.empty())
        {
            dbgprintf("Process %d NOT restarted since there are no services to restart.\n", pid);
            pids_.erase(pid);
            pid2services_.erase(pid);
            continue;
        }

        int childpid = ::fork();
        if (childpid == 0)
        {
            // child server
            MultiProcMSDatagramServer server(mpmode_);
            assert(server.isOk());

            SVCDATAListIter psdit = proc_svcs_data.begin();
            SVCDATAListIter psditend = proc_svcs_data.end();
            
            for ( ; psdit != psditend; ++psdit)
            {
                // child server
                UseCntPtr<Socket> psocket;
                UseCntPtr<Address> paddress;
                if (psdit->protocol_ == "udp")
                {
                    psocket = new UdpSocket();
                    MustBeTrue(psocket->isOk());
                    paddress = new InetAddress(psdit->host_, psdit->port_);
                    MustBeTrue(paddress->isOk());
                }
                else if (psdit->protocol_ == "datagram")
                {
                    psocket = new DatagramSocket();
                    MustBeTrue(psocket->isOk());
                    ::unlink(psdit->service_.c_str());
                    paddress = new LocalAddress(psdit->service_);
                    MustBeTrue(paddress->isOk());
                }
                else
                {
                    MustBeTrue(0);
                }

                UseCntPtr<EndPoint> pserver_ep(new EndPoint(psocket, paddress));
                MustBeTrue(pserver_ep->isOk());

                MustBeTrue(server.registerHandler(pserver_ep, psdit->phandler_) == 0);
            }

            MustBeTrue(server.init() == 0);
            MustBeTrue(server.run() == 0);
            MustBeTrue(server.finish() == 0);

            exit(0);
        }
        else if (childpid > 0)
        {
            // parent -- track pid
            SVCDATAListIter pdit = proc_svcs_data.begin();
            SVCDATAListIter pditend = proc_svcs_data.end();

            for ( ; pdit != pditend; ++pdit)
            {
                dbgprintf("Service %s (pid=%d) restarted (%d out of %d).\n",
                      pdit->uri_.c_str(), childpid, 
                      pdit->startups_, pdit->maxstartups_ );
                pid2services_.insert(std::pair<int, PID2ServiceData>(childpid, *pdit));
                pids_.insert(childpid);
            }
        }
        else
        {
            // some type of error
            MustBeTrue(childpid >= 0);
        }
    }

    return(OK);
}

int
MultiProcMSDatagramServerPerProcess::finish()
{
    if (isNotOk()) return(NOTOK);
    if (pids_.empty()) return(OK);

    PIDSIter pit = pids_.begin();
    PIDSIter pitend = pids_.end();

    ::signal(SIGCHLD, SIG_IGN);

    for ( ; pit != pitend; ++pit)
    {
        pid_t pid = *pit;
        ::kill(pid, SIGINT);
    }

    pids_.clear();

    return(OK);
}

// class statics
UseCntPtr<MultiProcMSStreamServerPerProcess> MultiProcMSStreamServerPerProcess::pobject_(NULL);

void
MultiProcMSStreamServerPerProcess::handler(int sig)
{
    if (pobject_ != NULL) pobject_->finish();
    exit(OK);
}

// ctors and dtor
MultiProcMSStreamServerPerProcess::MultiProcMSStreamServerPerProcess(int maxprocnum, int maxs, Server::MultiplexMode mpmode):
    Server(), services_(), pid2services_(), pids_(), maxprocnum_(maxprocnum), registered_services_(0), maxstartups_(maxs), mpmode_(mpmode)
{
    MustBeTrue(maxprocnum_ > 0);
    MustBeTrue(maxstartups_ > 0);
    setOk(true);
}

MultiProcMSStreamServerPerProcess::~MultiProcMSStreamServerPerProcess()
{
    setOk(false);
}

// register services and handlers
void
MultiProcMSStreamServerPerProcess::registerService(
    const std::string &service, 
    MultiProcMSStreamServer::Handler *handler)
{
    UseCntPtr<MultiProcMSStreamServer::Handler> phandler(handler);
    services_.insert(std::pair<std::string, UseCntPtr<MultiProcMSStreamServer::Handler > >(service, phandler));
}

// operations
int
MultiProcMSStreamServerPerProcess::init()
{
    if (isNotOk())
        return(NOTOK);
    else
        return(0);
}

int
MultiProcMSStreamServerPerProcess::run()
{
    if (isNotOk()) return(NOTOK);
    if (services_.empty()) return(NOTOK);

    MultiProcMSStreamServerPerProcess::pobject_ = this;
    ::signal(SIGINT, MultiProcMSStreamServerPerProcess::handler);
    ::signal(SIGQUIT, MultiProcMSStreamServerPerProcess::handler);
    ::signal(SIGTERM, MultiProcMSStreamServerPerProcess::handler);

    SVCSIter sit = services_.begin();
    SVCSIter sitend = services_.end();

    registered_services_ = services_.size();
    int services_per_proc = registered_services_/maxprocnum_;
    if (registered_services_%maxprocnum_ > 0) ++services_per_proc;

    for (int iproc=1; (iproc<=maxprocnum_) && (sit != sitend); ++iproc)
    {
        SVCSList proc_svcs;
        for (int iservice=1; 
            (iservice<=services_per_proc) &&
            (sit != sitend); ++sit, ++iservice)
        {
            dbgprintf("Starting service %s in process %d.\n", 
                      sit->first.c_str(), iproc);
            proc_svcs.push_back(
                std::pair<std::string, MultiProcMSStreamServer::Handler * >(
                    sit->first, sit->second));
        }

        int pid = ::fork();
        if (pid == 0)
        {
            // child server
            MultiProcMSStreamServer server(mpmode_);
            assert(server.isOk());

            SVCSListConstIter psit = proc_svcs.begin();
            SVCSListConstIter psitend = proc_svcs.end();
            
            for ( ; psit != psitend; ++psit)
            {
                std::string uri(psit->first);
                UseCntPtr<MultiProcMSStreamServer::Handler> phdlr(psit->second);

                Service s(uri);
                MustBeTrue(s.isOk());
                int port = s.getPort();
                std::string service = s.getService();
                std::string host = s.getHost();
                std::string protocol = s.getProtocol();

                // child server
                UseCntPtr<Socket> psocket;
                UseCntPtr<Address> paddress;
                if (protocol == "tcp")
                {
                    psocket = new TcpSocket();
                    MustBeTrue(psocket->isOk());
                    paddress = new InetAddress(host, port);
                    MustBeTrue(paddress->isOk());
                }
                else if (protocol == "stream")
                {
                    psocket = new StreamSocket();
                    MustBeTrue(psocket->isOk());
                    ::unlink(service.c_str());
                    paddress = new LocalAddress(service);
                    MustBeTrue(paddress->isOk());
                }
                else
                {
                    MustBeTrue(0);
                }

                UseCntPtr<EndPoint> pserver_ep(new EndPoint(psocket, paddress));
                MustBeTrue(pserver_ep->isOk());

                MustBeTrue(server.registerHandler(pserver_ep, phdlr) == 0);
            }

            MustBeTrue(server.init() == 0);
            MustBeTrue(server.run() == 0);
            MustBeTrue(server.finish() == 0);

            exit(0);
        }
        else if (pid > 0)
        {
            // parent -- track pid
            SVCSListConstIter psit = proc_svcs.begin();
            SVCSListConstIter psitend = proc_svcs.end();
            
            for ( ; psit != psitend; ++psit)
            {
                std::string uri(psit->first);

                Service s(uri);
                MustBeTrue(s.isOk());

                int port = s.getPort();
                std::string service = s.getService();
                std::string host = s.getHost();
                std::string protocol = s.getProtocol();

                UseCntPtr<MultiProcMSStreamServer::Handler> phdlr(psit->second);

                dbgprintf("Service %s (pid=%d) started.\n", uri.c_str(), pid);

                PID2ServiceData data(1,maxstartups_,uri,service,host,protocol,port,phdlr);
                pid2services_.insert(std::pair<int, PID2ServiceData>(pid, data));
            }
            pids_.insert(pid);
        }
        else
        {
            // some type of error
            MustBeTrue(pid >= 0);
        }
    }

    for (int status; !pids_.empty(); )
    {
        pid_t pid = ::waitpid(-1, &status, 0);
        if (errno == ECHILD)
        {
            pids_.clear();
            return(NOTOK);
        }

        PID2SERVICESIterPair piters = pid2services_.equal_range(pid);
        if (piters.first == piters.second) continue;

        dbgprintf("Process %d died.\n", pid);

        SVCDATAList proc_svcs_data;
        PID2SERVICESIter lbit = piters.first;
        PID2SERVICESIter ubit = piters.second;

        for ( ; lbit != ubit; ++lbit)
        {
            dbgprintf("Service %s (pid=%d) died.\n",
                      lbit->second.uri_.c_str(), pid);
            if (lbit->second.startups_ < lbit->second.maxstartups_)
            {
                dbgprintf("Service %s (pid=%d) attempting to restart (%d out of %d).\n",
                          lbit->second.uri_.c_str(), pid, 
                          lbit->second.startups_, lbit->second.maxstartups_);

                PID2ServiceData pdata(lbit->second.startups_+1,
                                      lbit->second.maxstartups_,
                                      lbit->second.uri_,
                                      lbit->second.service_,
                                      lbit->second.host_,
                                      lbit->second.protocol_,
                                      lbit->second.port_,
                                      lbit->second.phandler_);
                proc_svcs_data.push_back(pdata);
            }
            else
            {
                dbgprintf("Service %s (pid=%d) NOT restarted (%d out of %d).\n",
                          lbit->second.uri_.c_str(), pid, 
                          lbit->second.startups_, lbit->second.maxstartups_);
            }
        }

        if (proc_svcs_data.empty())
        {
            dbgprintf("Process %d NOT restarted since there are no services to restart.\n", pid);
            pids_.erase(pid);
            pid2services_.erase(pid);
            continue;
        }

        int childpid = ::fork();
        if (childpid == 0)
        {
            // child server
            MultiProcMSStreamServer server(mpmode_);
            assert(server.isOk());

            SVCDATAListIter psdit = proc_svcs_data.begin();
            SVCDATAListIter psditend = proc_svcs_data.end();
            
            for ( ; psdit != psditend; ++psdit)
            {
                // child server
                UseCntPtr<Socket> psocket;
                UseCntPtr<Address> paddress;
                if (psdit->protocol_ == "tcp")
                {
                    psocket = new TcpSocket();
                    MustBeTrue(psocket->isOk());
                    paddress = new InetAddress(psdit->host_, psdit->port_);
                    MustBeTrue(paddress->isOk());
                }
                else if (psdit->protocol_ == "stream")
                {
                    psocket = new StreamSocket();
                    MustBeTrue(psocket->isOk());
                    ::unlink(psdit->service_.c_str());
                    paddress = new LocalAddress(psdit->service_);
                    MustBeTrue(paddress->isOk());
                }
                else
                {
                    MustBeTrue(0);
                }

                UseCntPtr<EndPoint> pserver_ep(new EndPoint(psocket, paddress));
                MustBeTrue(pserver_ep->isOk());

                MustBeTrue(server.registerHandler(pserver_ep, psdit->phandler_) == 0);
            }

            MustBeTrue(server.init() == 0);
            MustBeTrue(server.run() == 0);
            MustBeTrue(server.finish() == 0);

            exit(0);
        }
        else if (childpid > 0)
        {
            // parent -- track pid
            SVCDATAListIter pdit = proc_svcs_data.begin();
            SVCDATAListIter pditend = proc_svcs_data.end();

            for ( ; pdit != pditend; ++pdit)
            {
                dbgprintf("Service %s (pid=%d) restarted (%d out of %d).\n",
                      pdit->uri_.c_str(), childpid, 
                      pdit->startups_, pdit->maxstartups_ );
                pid2services_.insert(std::pair<int, PID2ServiceData>(childpid, *pdit));
                pids_.insert(childpid);
            }
        }
        else
        {
            // some type of error
            MustBeTrue(childpid >= 0);
        }
    }

    return(0);
}

int
MultiProcMSStreamServerPerProcess::finish()
{
    if (isNotOk()) return(NOTOK);
    if (pids_.empty()) return(0);

    PIDSIter pit = pids_.begin();
    PIDSIter pitend = pids_.end();

    ::signal(SIGCHLD, SIG_IGN);

    for ( ; pit != pitend; ++pit)
    {
        pid_t pid = *pit;
        ::kill(pid, SIGINT);
    }

    pids_.clear();

    return(0);
}

}
