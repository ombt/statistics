//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_INCLUDE_FILES
#define __OMBT_INCLUDE_FILES

// read in an #include file, search path for a match.

// os headers
#include <string>
#include <list>

// headers
#include "atomic/BaseObject.h"

namespace ombt {

// include file class
class IncludeFiles: public BaseObject
{
public:
    // ctors and dtor
    IncludeFiles();
    IncludeFiles(const std::list<std::string> &directories);
    IncludeFiles(const IncludeFiles &src);
    ~IncludeFiles();

    // assignment
    IncludeFiles &operator=(const IncludeFiles &rhs);

    // operations
    void clearDirectories();
    void addDirectory(const std::string &directory);
    bool readFile(const std::string &file, std::string &contents) const;

protected:
    // data
    std::list<std::string> directories_;
};

}

#endif
