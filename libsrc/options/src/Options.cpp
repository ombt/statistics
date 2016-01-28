//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// maintain program options

// local includes
#include "system/Debug.h"
#include "hdr/Options.h"

namespace ombt {

// ctors and dtor
Options::Options():
    data_(),
    BaseObject()
{
}

Options::Options(const Program &program): 
    data_(),
    BaseObject()
{
    Program::OptionsTypeCIter cit = program.options().begin();
    Program::OptionsTypeCIter citend = program.options().end();
    for ( ; cit!=citend; ++cit)
    {
        data_.insert(Type::value_type(cit->second->name(),
                                      cit->second->value()));
    }
}

Options::Options(const Options &src): 
    data_(src.data_.begin(), src.data_.end()),
    BaseObject(src)
{
}

Options::~Options()
{
}

// assignment
Options &
Options::operator=(const Program &program)
{
    data_.clear();
    Program::OptionsTypeCIter cit = program.options().begin();
    Program::OptionsTypeCIter citend = program.options().end();
    for ( ; cit!=citend; ++cit)
    {
        data_.insert(Type::value_type(cit->second->name(),
                                      cit->second->value()));
    }
    return(*this);
}

Options &
Options::operator=(const Options &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        data_.clear();
        data_.insert(rhs.data_.begin(), rhs.data_.end());
    }
    return(*this);
}

// access methods
void
Options::set(const std::string &name, const std::string &value)
{
    data_.insert(Type::value_type(name, value));
}

bool
Options::get(const std::string &name, std::string &value) const
{
    TypeCIt cit = data_.find(name);
    if (cit != data_.end())
    {
        value = cit->second;
        return true;
    }
    else
    {
        return false;
    }
}

// output operators
std::ostream &
operator<<(std::ostream &os, const Options &o)
{
    Options::TypeCIt cit = o.data_.begin();
    Options::TypeCIt citend = o.data_.end();
    for ( ; cit!=citend; ++cit)
    {
        os << "<" << cit->first 
           << "," 
           << cit->second 
           << ">" 
           << std::endl;
    }
    return os;
}

}

