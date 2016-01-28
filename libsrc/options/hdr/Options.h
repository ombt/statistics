//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_OPTIONW_H
#define __OMBT_OPTIONW_H

// maintain program options

// os headers
#include <string>
#include <map>
#include <iostream>

// headers
#include "atomic/BaseObject.h"
#include "logic/Program.h"

namespace ombt {

// maintain program options
class Options: public BaseObject
{
public:
    // data type
    typedef std::map<std::string, std::string> Type;
    typedef Type::iterator TypeIt;
    typedef Type::const_iterator TypeCIt;

    // ctors and dtor
    Options();
    Options(const Program &program);
    Options(const Options &src);
    ~Options();

    // assignment
    Options &operator=(const Program &program);
    Options &operator=(const Options &rhs);

    // access
    void set(const std::string &name, const std::string &value);
    bool get(const std::string &name, std::string &value) const;

    // print data
    friend std::ostream &operator<<(std::ostream &, const Options &);

protected:
    // internal data
    Type data_;
};

}

#endif

