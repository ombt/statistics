//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// simple class to tokenize a string

#include "hdr/LogicTokenizer.h"

namespace ombt {

// ctors and dtor
LogicTokenizer::LogicTokenizer(): BaseObject() { }
LogicTokenizer::LogicTokenizer(const LogicTokenizer &src): BaseObject(src) { }
LogicTokenizer::~LogicTokenizer() { }

// assignment
LogicTokenizer &
LogicTokenizer::operator=(const LogicTokenizer &rhs)
{
	BaseObject::operator=(rhs);
	return(*this);
}

// tokenizing functor
int
LogicTokenizer::operator()(const std::string &line, 
                      std::vector<std::string> &tokens,
                      const std::string &delimiters, bool trim_front)
{
    static std::string whitespace = "\t ";
    std::string::size_type lastpos = 0;

    if (trim_front)
    	lastpos = line.find_first_not_of(whitespace, 0);

    std::string::size_type pos = line.find_first_of(delimiters, lastpos);

    while (std::string::npos != pos || std::string::npos != lastpos)
    {
        tokens.push_back(line.substr(lastpos, pos - lastpos));
        lastpos = line.find_first_not_of(delimiters, pos);
        pos = line.find_first_of(delimiters, lastpos);
    }

    return(0);
}

}

