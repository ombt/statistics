//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// local/unix address class

// headers
#include <string.h>
#include "sockets/LocalAddress.h"

namespace ombt {

// ctor and dtor
LocalAddress::LocalAddress():
    Address()
{
    ::memset(&local_address_, 0, sizeof(local_address_));
    setOk(true);
}

LocalAddress::LocalAddress(const std::string &address):
    Address(address)
{
    setOk(false);
    setLocalAddress();
}

LocalAddress::LocalAddress(const sockaddr_un &local_address):
    Address()
{
    setOk(true);
    memcpy(&local_address_, &local_address, sizeof(local_address));
}

LocalAddress::~LocalAddress()
{
    // do nothing
}

// factory
LocalAddress *
LocalAddress::create() const
{
    return(new LocalAddress());
}

void
LocalAddress::setLocalAddress()
{
    ::memset(&local_address_, 0, sizeof(local_address_));

    local_address_.sun_family = AF_LOCAL;

    if (address_ != "")
    {
        setOk(true);
        memcpy(local_address_.sun_path, address_.c_str(), address_.length());
    }
    else
    {
        setOk(false);
    }
}

}

