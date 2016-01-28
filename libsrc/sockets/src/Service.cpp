//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// iterative server class

// headers
#include "hdr/Service.h"

namespace ombt {

// ctors and dtor
Service::Service():
    UCBaseObject(NOTOK), uri_(), host_(), service_(), protocol_(), port_(-1)
{
    // do nothing
}

Service::Service(const std::string &uri):
    UCBaseObject(NOTOK), uri_(uri), host_(), service_(), protocol_(), port_(-1)
{
    parser();
}

Service::~Service()
{
    setOk(false);
}

// parse uri
void
Service::parser()
{
    // initialize
    setOk(false);
    host_ = "";
    service_ = "";
    protocol_ = "";
    port_ = -1;
    
    // split into parts
    //
    // address:service:protocol or 
    // address:port:protocol or
    // address:filepath:protocol
    //
    std::vector<std::string> tokens;
    Tokenizer tokenizer;
    if (tokenizer(uri_, tokens, ":") != 0) return;
    if (tokens.size() != 3) return;

    // assign for now
    host_ = tokens[0];
    service_ = tokens[1];
    protocol_ = tokens[2];

    // look at parts
    if (host_ == "") host_ = "localhost";

    if (protocol_ == "udp" || protocol_ == "tcp")
    {
        servent *pse;
        pse = ::getservbyname(service_.c_str(), protocol_.c_str());
        if (pse)
            port_ = ntohs(pse->s_port);
        else
        {
            port_ = (unsigned short)(atoi(service_.c_str()));
            if (port_ == 0) return;
        }
    }
    else if (protocol_ == "datagram" || protocol_ == "stream")
    {
        host_ = "localhost";
        port_ = -1;
    }
    else
        return;

    setOk(true);
    return;
}

}
