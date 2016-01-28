//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_LOCAL_ADDRESS_H
#define __OMBT_LOCAL_ADDRESS_H

// headers
#include <sys/socket.h>
#include <sys/un.h>
#include "sockets/Address.h"

namespace ombt {

// Local/Unix address class
class LocalAddress: public Address {
public:
    // ctors and dtor
    LocalAddress();
    LocalAddress(const std::string &address);
    LocalAddress(const sockaddr_un &address);
    ~LocalAddress();

    // factory
    LocalAddress *create() const;

    sockaddr *getSockAddr() const {
        return((sockaddr *)&local_address_);
    }
    socklen_t getSockAddrLen() const {
        sock_addr_len_ = sizeof(sockaddr_un);
        // sock_addr_len_ = SUN_LEN(&local_address_);
        return(sock_addr_len_);
    }
    socklen_t *getSockAddrLenRef() const {
        sock_addr_len_ = sizeof(sockaddr_un);
        // sock_addr_len_ = SUN_LEN(&local_address_);
        return(&sock_addr_len_);
    }
    const sockaddr_un *getLocalAddress() const {
        return(&local_address_);
    }

private:
    // leper colony
    LocalAddress(const LocalAddress &src);
    LocalAddress &operator=(const LocalAddress &rhs);

protected:
    void setLocalAddress();

protected:
    sockaddr_un local_address_;
};

}

#endif
