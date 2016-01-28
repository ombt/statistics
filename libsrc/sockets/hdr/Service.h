//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_SERVICE_H
#define __OMBT_SERVICE_H

// system headers
#include <stdlib.h>
#include <netdb.h>
#include <string>
#include <vector>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/UCBaseObject.h"
#include "parsing/Tokenizer.h"

namespace ombt {

// services class
class Service: public UCBaseObject {
public:
    Service();
    Service(const std::string &uri);
    ~Service();

    // comparison
    bool operator<(const Service &rhs) {
       return(uri_ < rhs.uri_);
    }

    // accessor
    void setUri(const std::string &uri) { uri_ = uri; parser(); }
    const std::string getUri() const { return(uri_); }
    const std::string getHost() const { return(host_); }
    const std::string getService() const { return(service_); }
    const std::string getProtocol() const { return(protocol_); }
    const int getPort() const { return(port_); }

private:
    // leper colony
    Service(const Service &src);
    Service &operator=(const Service &rhs);

private:
    void parser();

    std::string uri_;
    std::string host_;
    std::string service_;
    std::string protocol_;
    int port_;
};

}

#endif
