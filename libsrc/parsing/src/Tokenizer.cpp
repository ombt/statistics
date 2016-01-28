//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// simple class to tokenize a string

#include "hdr/Tokenizer.h"

namespace ombt {

// ctors and dtor
Tokenizer::Tokenizer(): BaseObject() { }
Tokenizer::Tokenizer(const Tokenizer &src): BaseObject(src) { }
Tokenizer::~Tokenizer() { }

// assignment
Tokenizer &
Tokenizer::operator=(const Tokenizer &rhs)
{
	BaseObject::operator=(rhs);
	return(*this);
}

// tokenizing functor
int
Tokenizer::operator()(const std::string &line, 
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

