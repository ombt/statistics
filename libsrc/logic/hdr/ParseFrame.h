//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_PARSE_FRAME
#define __OMBT_PARSE_FRAME

// parse frame class

// os headers
#include <string>
#include <list>

// local headers
#include "atomic/BaseObject.h"
#include "atomic/ExtUseCntPtr.h"
#include "logic/Semantic.h"

namespace ombt {

// tokenizer class
class ParseFrame: public BaseObject
{
public:
    // frame-specific data
    struct Data
    {
        enum Type
        {
            Unknown,
            Equal,
            NotEqual,
            SetOfSupport,
            String,
            Number,
            NegativeNumber,
            Last
        };

        // ctors and dtor
        Data();
        Data(const Data &);
        ~Data();

        // assignment
        Data &operator=(const Data &);

        // data
        Type type_;
        std::string name_;
        std::string value_;
        ExtUseCntPtr<Semantic> record_;
        std::list<ExtUseCntPtr<Semantic> > arglist_;
    };

    typedef std::list<Data> DataStack;
    typedef DataStack::reverse_iterator DataStackRIt;
    typedef DataStack::iterator DataStackIt;

public:
    // ctors and dtor
    ParseFrame();
    ~ParseFrame();

    // stack access
    Data &global();
    Data &current();
    Data &returned();

    // stack frame for parser
    static DataStack stack_;

private:
    // do not allow these
    ParseFrame(const ParseFrame &);
    ParseFrame &operator=(const ParseFrame &);
};

}

#endif
