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
#include "logging/Logging.h"
#include "hdr/InitServer.h"

namespace ombt {

// class statics
UseCntPtr<InitServer> InitServer::pobject_(NULL);

void
InitServer::handler(int sig)
{
    if (pobject_ != NULL) pobject_->finish();
    exit(0);
}

// ctors and dtor
InitServer::InitServer(int maxstartups):
    Server(), services_(), pid2services_(), pids_(), 
    registered_services_(0), maxstartups_(maxstartups)
{
    MustBeTrue(maxstartups_ > 0);
    setOk(true);
}

InitServer::~InitServer()
{
    setOk(false);
}

// register services and handlers
void
InitServer::registerService(
    const std::string &service, 
    InitServer::SubServer *subserver)
{
    UseCntPtr<InitServer::SubServer> psubserver(subserver);
    services_.insert(std::pair<std::string, UseCntPtr<InitServer::SubServer> >(service, psubserver));
}

// operations
int
InitServer::init()
{
    if (isNotOk())
        return(NOTOK);
    else
        return(OK);
}

int
InitServer::run()
{
    if (isNotOk()) return(NOTOK);
    if (services_.empty()) return(NOTOK);

    InitServer::pobject_ = this;
    ::signal(SIGINT, InitServer::handler);
    ::signal(SIGQUIT, InitServer::handler);
    ::signal(SIGTERM, InitServer::handler);

    SVCSIter sit = services_.begin();
    SVCSIter sitend = services_.end();

    registered_services_ = services_.size();

    for ( ; sit != sitend; ++sit)
    {
        dbgprintf("Starting service %s:\n", sit->first.c_str());

        std::string uri = sit->first;
        UseCntPtr<InitServer::SubServer> psubserver(sit->second);

        int pid = ::fork();
        if (pid == 0)
        {
            // child server
            MustBeTrue(psubserver != NULL);
            exit((*psubserver)(uri));
        }
        else if (pid > 0)
        {
            // parent -- track pid
            dbgprintf("Service %s started (pid=%d).\n", uri.c_str(), pid);
            PID2ServiceData data(1,maxstartups_,uri,psubserver);
            pid2services_.insert(std::pair<int, PID2ServiceData>(pid, data));
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

        PID2SERVICESIter piter = pid2services_.find(pid);
        if (piter == pid2services_.end()) continue;

        dbgprintf("Service %s (pid=%d) died.\n",
                  piter->second.uri_.c_str(), pid);

        if (piter->second.startups_ >= piter->second.maxstartups_)
        {
            dbgprintf("Service %s (pid=%d) NOT restarted (%d out of %d).\n",
                      piter->second.uri_.c_str(), pid, 
                      piter->second.startups_, piter->second.maxstartups_);
            pids_.erase(pid);
            pid2services_.erase(pid);
            continue;
        }
 
        dbgprintf("Service %s (pid=%d) attempting to restart (%d out of %d).\n",
                  piter->second.uri_.c_str(), pid, 
                  piter->second.startups_, piter->second.maxstartups_);
 
        // save new data for new child pid
        PID2ServiceData pdata(piter->second.startups_+1,
                              piter->second.maxstartups_,
                              piter->second.uri_,
                              piter->second.psubserver_);

        // erase old data
        pids_.erase(pid);
        pid2services_.erase(pid);

        int childpid = ::fork();
        if (childpid == 0)
        {
            // child -- restart server
            MustBeTrue(pdata.psubserver_ != NULL);
            exit((*pdata.psubserver_)(pdata.uri_));
        }
        else if (childpid > 0)
        {
            // parent -- track pid
            dbgprintf("Service %s (pid=%d) restarted (%d out of %d).\n",
                      pdata.uri_.c_str(), childpid, 
                      pdata.startups_, pdata.maxstartups_);
            pid2services_.insert(std::pair<int, PID2ServiceData>(childpid, pdata));
            pids_.insert(childpid);
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
InitServer::finish()
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

}
