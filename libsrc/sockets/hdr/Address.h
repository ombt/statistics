//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_ADDRESS_H
#define __OMBT_ADDRESS_H

// system headers
#include <string>
#include <sys/socket.h>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "atomic/UCBaseObject.h"

namespace ombt {

// address class
class Address: public UCBaseObject {
public:
    // ctor and dtor
    Address(const std::string &address = "");
    virtual ~Address();

    // factory
    virtual Address *create() const;

    inline virtual sockaddr *getSockAddr() const {
        return(NULL);
    };
    inline virtual socklen_t getSockAddrLen() const {
        return(sock_addr_len_);
    };
    inline virtual socklen_t *getSockAddrLenRef() const {
        return(&sock_addr_len_);
    };
    inline virtual const std::string &getAddress() const {
        return(address_);
    };
    inline virtual void setAddress(const std::string &address) {
        address_ = address;
    }

private:
    // leper colony
    Address(const Address &src);
    Address &operator=(const Address &rhs);

protected:
    mutable socklen_t sock_addr_len_;
    std::string address_;
};

}

#endif
