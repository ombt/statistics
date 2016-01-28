//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_GENERIC_H
#define __OMBT_GENERIC_H

// generic conversion templates

// headers
#include <string>
#include <sstream>
#include <iomanip>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"

namespace ombt {

// generic to-string function
template <class T> 
inline std::string to_string(const T &t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

// generic from-string function
template <class T>
inline T from_string(const std::string &s)
{
    T x;
    std::istringstream iss(s);
    if (!(iss >> x))
    {
        return T();
    }
    else
    {
        return x;
    }
}

// specializations
template <> 
inline std::string to_string(const double &t)
{
    std::stringstream ss;
    ss << std::setprecision(14) << t;
    return ss.str();
}

template <> 
inline std::string to_string(const float &t)
{
    std::stringstream ss;
    ss << std::setprecision(7) << t;
    return ss.str();
}

}

#endif
