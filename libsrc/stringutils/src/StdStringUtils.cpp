//
// Copyright (C) 2012, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//

// os headers
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

// headers
#include "stringutils/StdStringUtils.h"

namespace ombt {

std::string
trim_left(const std::string &source, const std::string &list)
{
    std::string work = source;
    return work.erase(work.find_last_not_of(list)+1);
}

std::string
trim_right(const std::string &source, const std::string &list)
{
    std::string work = source;
    return work.erase(0, work.find_first_not_of(list));
}

std::string
trim(const std::string &source, const std::string &list)
{
    std::string work = source;
    return trim_left(trim_right(work, list), list);
}

void
read_file(const std::string &path, std::string &contents)
{
    std::ifstream infile(path.c_str());
    std::stringstream buffer;
    buffer << infile.rdbuf();
    contents = buffer.str();
    infile.close();
}

void
to_uppercase(std::string &s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
}

void
to_lowercase(std::string &s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

}
