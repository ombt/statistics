//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// include file, seach the given path for file.

// system includes
#include <ctype.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

// local includes
#include "system/Debug.h"
#include "hdr/IncludeFiles.h"

namespace ombt {

// ctors and dtor
IncludeFiles::IncludeFiles():
    directories_(),
    BaseObject()
{
}

IncludeFiles::IncludeFiles(const std::list<std::string> &directories):
    directories_(directories.begin(), directories.end()),
    BaseObject()
{
}

IncludeFiles::IncludeFiles(const IncludeFiles &src): 
    directories_(src.directories_.begin(), src.directories_.end()),
        BaseObject(src)
{
}

IncludeFiles::~IncludeFiles()
{
}

// assignment
IncludeFiles &
IncludeFiles::operator=(const IncludeFiles &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        directories_.clear();
        directories_.assign(rhs.directories_.begin(), 
                            rhs.directories_.end());
    }
    return(*this);
}

// operations functions
void
IncludeFiles::clearDirectories()
{
    directories_.clear();
}

void
IncludeFiles::addDirectory(const std::string &directory)
{
    directories_.push_back(directory);
}

bool
IncludeFiles::readFile(const std::string &filename,
                       std::string &content) const
{
    std::list<std::string>::const_iterator i, iend;
    i    = directories_.begin();
    iend = directories_.end();

    for ( ; i!=iend; ++i)
    {
        std::string full_path = *i + "/" + filename;

        std::ifstream in(full_path.c_str());
        if (!in)
        {
            continue;
        }
        else
        {
            std::stringstream buffer;
            buffer << in.rdbuf();
            content = buffer.str();
            return true;
        }
    }
    return false;
}

}

