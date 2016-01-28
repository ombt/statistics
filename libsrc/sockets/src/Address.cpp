//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// address class

// headers
#include "hdr/Address.h"

namespace ombt {

// ctor and dtor
Address::Address(const std::string &address):
    UCBaseObject(OK), 
    address_(address), sock_addr_len_(0) { }

Address::~Address() { }

// factory function
Address *
Address::create() const
{
    return(new Address());
}

}

