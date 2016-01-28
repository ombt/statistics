//
// Copyright (C) 2012, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//

#ifndef	__STD_STRING_UTILS_H
#define	__STD_STRING_UTILS_H

// os headers
#include <string>
#include <vector>

namespace ombt {

//
// split - split string using a set of delimiters.
// trim_left - remove WS on left side of string.
// trim_right - remove WS on right side of string.
// trim - remove WS on both ends of string.
// read_file - file text file into a string.
// to_uppercase - convert string to upper case.
//
template <class ContainerType>
void split(const std::string &str, 
           ContainerType &tokens,
           const std::string &delimiters, 
           bool trim_empty);
std::string trim_left(const std::string &source, 
                      const std::string &list = "\"\t\n\r ");
std::string trim_right(const std::string &source, 
                       const std::string &list = "\"\t\n\r ");
std::string trim(const std::string &source, 
                 const std::string &list = "\"\t\n\r ");
void read_file(const std::string &path, std::string &contents);
void to_uppercase(std::string &s);
void to_lowercase(std::string &s);

#include "stringutils/StdStringUtils.i"

}

#endif

