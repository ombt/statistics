//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// parse frame for logic-based languages

#include "hdr/ParseFrame.h"
#include "hdr/Semantic.h"

namespace ombt {

// stack frame data
ParseFrame::Data::Data():
    type_(Unknown),
    name_(), 
    value_(), 
    record_(NULL), 
    arglist_() { }

ParseFrame::Data::Data(const ParseFrame::Data &src):
    type_(src.type_), 
    name_(src.name_), 
    value_(src.value_),
    record_(src.record_), 
    arglist_(src.arglist_.begin(), src.arglist_.end()) { }

ParseFrame::Data::~Data() { }

ParseFrame::Data &
ParseFrame::Data::operator=(const ParseFrame::Data &rhs)
{
    if (this != &rhs)
    {
       type_ = rhs.type_;
       name_ = rhs.name_;
       value_ = rhs.value_;
       record_ = rhs.record_;
       arglist_.clear();
       arglist_.assign(rhs.arglist_.begin(), rhs.arglist_.end());
    }
    return *this;
}

// stack
ParseFrame::DataStack ParseFrame::stack_;

// stack frame access
ParseFrame::ParseFrame():
    BaseObject()
{
    // push a new frame on stack for *calls* 
    stack_.push_front(Data());
}

ParseFrame::~ParseFrame()
{
    // pop *calls* frame from stack
    stack_.pop_front();
}

ParseFrame::Data &
ParseFrame::global()
{
    // first stack is the *global* stack
    DataStackRIt dsrit = stack_.rbegin();
    return *(dsrit);
}

ParseFrame::Data &
ParseFrame::current()
{
    // current frame was already pushed by caller
    DataStackIt dsit = stack_.begin();
    return *(++dsit);
}

ParseFrame::Data &
ParseFrame::returned()
{
    // this is the callee frame, used by "calls"
    DataStackIt dsit = stack_.begin();
    return *dsit;
}

}

