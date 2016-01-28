//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// iterative server class

// headers
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "system/Debug.h"
#include "atomic/UseCntPtr.h"
#include "sockets/EndPoint.h"
#include "sockets/Service.h"
#include "hdr/IterativeServerPerProcess.h"
#include "parsing/Tokenizer.h"
#include "logging/Logging.h"

namespace ombt {

// class statics
UseCntPtr<IterativeDatagramServerPerProcess> IterativeDatagramServerPerProcess::pobject_(NULL);

void
ombt::IterativeDatagramServerPerProcess::handler(int sig)
{
    if (pobject_ != NULL) pobject_->finish();
    exit(0);
}

// ctors and dtor
IterativeDatagramServerPerProcess::IterativeDatagramServerPerProcess(int maxs):
    Server(), services_(), pids_(), maxstartups_(maxs)
{
    setOk(true);
}

IterativeDatagramServerPerProcess::~IterativeDatagramServerPerProcess()
{
    setOk(false);
}

// register services and handlers
void
IterativeDatagramServerPerProcess::registerService(
    const std::string &service, 
    IterativeDatagramServer::Handler *handler)
{
    UseCntPtr<IterativeDatagramServer::Handler> phandler(handler);
    services_.insert(std::pair<std::string, UseCntPtr<IterativeDatagramServer::Handler> >(service, phandler));
}

// operations
int
IterativeDatagramServerPerProcess::init()
{
    if (isNotOk())
        return(NOTOK);
    else
        return(0);
}

int
IterativeDatagramServerPerProcess::run()
{
    if (isNotOk()) return(NOTOK);
    if (services_.empty()) return(NOTOK);

    IterativeDatagramServerPerProcess::pobject_ = this;
    ::signal(SIGINT, IterativeDatagramServerPerProcess::handler);
    ::signal(SIGQUIT, IterativeDatagramServerPerProcess::handler);
    ::signal(SIGTERM, IterativeDatagramServerPerProcess::handler);

    SVCSIter sit = services_.begin();
    SVCSIter sitend = services_.end();

    for ( ; sit != sitend; ++sit)
    {
        std::string uri(sit->first);
        UseCntPtr<IterativeDatagramServer::Handler> phdlr(sit->second);

        Service s(uri);
        if (!s.isOk()) return(NOTOK);

        int port = s.getPort();
        std::string service = s.getService();
        std::string host = s.getHost();
        std::string protocol = s.getProtocol();

        dbgprintf("Starting service %s.\n", uri.c_str());

        int pid = ::fork();
        if (pid == 0)
        {
            // child server
            UseCntPtr<Socket> psocket;
            UseCntPtr<Address> paddress;
            if (protocol == "udp")
            {
                psocket = new UdpSocket;
                MustBeTrue(psocket->isOk());
                paddress = new InetAddress(host, port);
                MustBeTrue(paddress->isOk());
            }
            else if (protocol == "datagram")
            {
                psocket = new DatagramSocket;
                MustBeTrue(psocket->isOk());
                ::unlink(service.c_str());
                paddress = new LocalAddress(service);
                MustBeTrue(paddress->isOk());
            }
            else
            {
                exit(NOTOK);
            }

            UseCntPtr<EndPoint> pserver_ep(new EndPoint(psocket, paddress));
            MustBeTrue(pserver_ep->isOk());

            IterativeDatagramServer server(pserver_ep, phdlr);
            MustBeTrue(server.isOk());

            if (server.init() != 0) exit(NOTOK);
            if (server.run() != 0) exit(NOTOK);
            if (server.finish() != 0) exit(NOTOK);

            exit(0);
        }
        else if (pid > 0)
        {
            // parent -- track pid
            dbgprintf("Service %s (pid=%d) started.\n", uri.c_str(), pid);
            PIDData data(1,maxstartups_,uri,service,host,protocol,port);
            pids_.insert(std::pair<int, PIDData>(pid, data));
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
        PIDSIter pit = pids_.find(pid);
        if (pit != pids_.end())
        {
            dbgprintf("Service %s (pid=%d) died.\n",
                      pit->second.uri_.c_str(), pid);

            if (pit->second.startups_ < pit->second.maxstartups_)
            {
                dbgprintf("Service %s (pid=%d) attempting to restart.\n",
                          pit->second.uri_.c_str(), pid);

                std::string uri(pit->second.uri_);
                SVCSIter sit = services_.find(uri);
                if (sit == services_.end())
                {
                    pids_.erase(pit);
                    continue;
                }

                PIDData newdata(pit->second);
                newdata.startups_ += 1;
                UseCntPtr<IterativeDatagramServer::Handler> phdlr(sit->second);

                pids_.erase(pit);

               int port = newdata.port_;
               std::string host = newdata.host_;
               std::string service = newdata.service_;
               std::string protocol = newdata.protocol_;

               int childpid = ::fork();
               if (childpid == 0)
               {
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
                       exit(NOTOK);
                   }
       
                   UseCntPtr<EndPoint> pserver_ep(new EndPoint(psocket, paddress));
                   MustBeTrue(pserver_ep->isOk());
       
                   IterativeDatagramServer server(pserver_ep, phdlr);
                   MustBeTrue(server.isOk());
       
                   if (server.init() != 0) exit(NOTOK);
                   if (server.run() != 0) exit(NOTOK);
                   if (server.finish() != 0) exit(NOTOK);
       
                   exit(0);
               }
               else if (childpid > 0)
               {
                   // parent -- track pid
		   dbgprintf("Service %s (pid=%d) restarted (%d out of %d).\n",
                             uri.c_str(), childpid, 
                             newdata.startups_, newdata.maxstartups_ );
                   pids_.insert(std::pair<int, PIDData>(childpid, newdata));
               }
               else
               {
                   // some type of error
                   MustBeTrue(childpid >= 0);
               }
            }
            else
            {
		   dbgprintf("Service %s (pid=%d) NOT restarted (%d out of %d).\n",
                             pit->second.uri_.c_str(), 
                             pid, 
                             pit->second.startups_, 
                             pit->second.maxstartups_);
                pids_.erase(pit);
            }
        }
    }

    return(0);
}

int
IterativeDatagramServerPerProcess::finish()
{
    if (isNotOk()) return(NOTOK);
    if (pids_.empty()) return(0);

    PIDSIter pit = pids_.begin();
    PIDSIter pitend = pids_.end();

    ::signal(SIGCHLD, SIG_IGN);

    for ( ; pit != pitend; ++pit)
    {
        pid_t pid = pit->first;
        ::kill(pid, SIGINT);
    }

    pids_.clear();

    return(0);
}

// class statics
UseCntPtr<IterativeStreamServerPerProcess> IterativeStreamServerPerProcess::pobject_(NULL);

void
IterativeStreamServerPerProcess::handler(int sig)
{
    if (pobject_ != NULL) pobject_->finish();
    exit(0);
}

// ctor and dtor
IterativeStreamServerPerProcess::IterativeStreamServerPerProcess(int maxs):
    Server(), services_(), pids_(), maxstartups_(maxs)
{
    setOk(true);
}

IterativeStreamServerPerProcess::~IterativeStreamServerPerProcess()
{
    setOk(false);
}

void
IterativeStreamServerPerProcess::registerService(
    const std::string &service, 
    IterativeStreamServer::Handler *handler)
{
    UseCntPtr<IterativeStreamServer::Handler> phandler(handler);
    services_.insert(std::pair<std::string, UseCntPtr<IterativeStreamServer::Handler> >(service, phandler));
}

// operations
int
IterativeStreamServerPerProcess::init()
{
    if (isNotOk()) return(NOTOK);
    return(0);
}

int
IterativeStreamServerPerProcess::run()
{
    if (isNotOk()) return(NOTOK);
    if (services_.empty()) return(NOTOK);

    IterativeStreamServerPerProcess::pobject_ = this;
    ::signal(SIGINT, IterativeStreamServerPerProcess::handler);
    ::signal(SIGQUIT, IterativeStreamServerPerProcess::handler);
    ::signal(SIGTERM, IterativeStreamServerPerProcess::handler);

    SVCSIter sit = services_.begin();
    SVCSIter sitend = services_.end();

    for ( ; sit != sitend; ++sit)
    {
        std::string uri(sit->first);
        UseCntPtr<IterativeStreamServer::Handler> phdlr(sit->second);

        Service s(uri);
        if (!s.isOk()) return(NOTOK);
        int port = s.getPort();
        std::string service = s.getService();
        std::string host = s.getHost();
        std::string protocol = s.getProtocol();

        dbgprintf("Starting service %s.\n", uri.c_str());

        int pid = ::fork();
        if (pid == 0)
        {
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
                exit(NOTOK);
            }

            UseCntPtr<EndPoint> pserver_ep(new EndPoint(psocket, paddress));
            MustBeTrue(pserver_ep->isOk());

            IterativeStreamServer server(pserver_ep, phdlr);
            MustBeTrue(server.isOk());

            if (server.init() != 0) exit(NOTOK);
            if (server.run() != 0) exit(NOTOK);
            if (server.finish() != 0) exit(NOTOK);

            exit(0);
        }
        else if (pid > 0)
        {
            // parent -- track pid
            dbgprintf("Service %s (pid=%d) started.\n", uri.c_str(), pid);
            PIDData data(1,maxstartups_,uri,service,host,protocol,port);
            pids_.insert(std::pair<int, PIDData>(pid, data));
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
       PIDSIter pit = pids_.find(pid);
       if (pit != pids_.end())
       {
           dbgprintf("Service %s (pid=%d) died.\n",
                     pit->second.uri_.c_str(), pid);
           if (pit->second.startups_ < pit->second.maxstartups_)
           {
               dbgprintf("Service %s (pid=%d) attempting to restart.\n",
                          pit->second.uri_.c_str(), pid);

               std::string uri(pit->second.uri_);
               SVCSIter sit = services_.find(uri);
               if (sit == services_.end())
               {
                   pids_.erase(pit);
                   continue;
               }

               PIDData newdata(pit->second);
               newdata.startups_ += 1;
               UseCntPtr<IterativeStreamServer::Handler> phdlr(sit->second);

               pids_.erase(pit);

               int port = newdata.port_;
               std::string host = newdata.host_;
               std::string service = newdata.service_;
               std::string protocol = newdata.protocol_;

               int childpid = ::fork();
               if (childpid == 0)
               {
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
                       exit(NOTOK);
                   }
       
                   UseCntPtr<EndPoint> pserver_ep(new EndPoint(psocket, paddress));
                   MustBeTrue(pserver_ep->isOk());
        
                   IterativeStreamServer server(pserver_ep, phdlr);
                   MustBeTrue(server.isOk());
       
                   if (server.init() != 0) exit(NOTOK);
                   if (server.run() != 0) exit(NOTOK);
                   if (server.finish() != 0) exit(NOTOK);
      
                   exit(0);
               }
               else if (childpid > 0)
               {
                   // parent -- track pid
                   dbgprintf("Service %s (pid=%d) restarted (%d out of %d).\n",
                             uri.c_str(), childpid, 
                             newdata.startups_, newdata.maxstartups_ );
                   pids_.insert(std::pair<int, PIDData>(childpid, newdata));
               }
               else
               {
                   // some type of error
                   MustBeTrue(childpid >= 0);
               }
            }
            else
            {
                dbgprintf("Service %s (pid=%d) NOT restarted (%d out of %d).\n",
                          pit->second.uri_.c_str(), 
                          pid, 
                          pit->second.startups_, 
                          pit->second.maxstartups_);
                pids_.erase(pit);
            }
        }
    }

    return(0);
}

int
IterativeStreamServerPerProcess::finish()
{
    if (isNotOk()) return(NOTOK);

    if (pids_.empty()) return(0);

    PIDSIter pit = pids_.begin();
    PIDSIter pitend = pids_.end();

    ::signal(SIGCHLD, SIG_IGN);

    for ( ; pit != pitend; ++pit)
    {
        pid_t pid = pit->first;
        ::kill(pid, SIGINT);
    }

    pids_.clear();

    return(0);
}

}
