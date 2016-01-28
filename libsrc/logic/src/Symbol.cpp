//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// simple class for symbol table 

#include "hdr/Symbol.h"

namespace ombt {

// static class members
Symbol::StackType Symbol::scope_;
Symbol::TableType Symbol::table_;

// static operations
bool
Symbol::retrieve(Symbol &data)
{
    TableTypeCIter cit = table_.find(data.name_);
    if (cit != table_.end())
    {
        data = cit->second;
        return true;
    }
    else
    {
        return false;
    }
}

bool
Symbol::update(const Symbol &data)
{
    TableTypeIter it = table_.find(data.name_);
    if (it != table_.end())
    {
        it->second = data;
        return true;
    }
    else
    {
        return false;
    }
}

bool
Symbol::insert(const Symbol &data)
{
    TableTypeIter it = table_.find(data.name_);
    if (it == table_.end())
    {
        table_.insert(TableType::value_type(data.name_, data));
        return true;
    }
    else
    {
        return false;
    }
}

bool
Symbol::remove(const Symbol &data)
{
    TableTypeIter it = table_.find(data.name_);
    if (it != table_.end())
    {
        table_.erase(it);
        return true;
    }
    else
    {
        return false;
    }
}

bool
Symbol::push(const Symbol &data)
{
    scope_.push_front(data);
    return true;
}

bool
Symbol::top(Symbol &data)
{
    StackTypeIter it = scope_.begin();
    if (it != scope_.end())
    {
        data = *it;
        return true;
    }
    else
    {
        return false;
    }
}

bool
Symbol::pop()
{
    if (!scope_.empty())
    {
        scope_.pop_front();
        return true;
    }
    else
    {
        return false;
    }
}

bool
Symbol::find(Symbol &data)
{
    StackTypeCIter cit = scope_.begin();
    StackTypeCIter citend = scope_.end();
    for ( ; cit!=citend; ++cit)
    {
        if (*cit == data)
        {
            data = *cit;
            return true;
        }
    }
    return false;
}

bool
Symbol::increment(const Symbol &data)
{
    StackTypeIter it = scope_.begin();
    StackTypeIter itend = scope_.end();
    for ( ; it!=itend; ++it)
    {
        if (*it == data)
        {
            it->used_ += 1;
            return true;
        }
    }
    return false;
}


// ctors and dtor
Symbol::Symbol():
    name_(""),
    unique_name_(""),
    type_(Unknown),
    number_of_arguments_(0),
    used_(0),
    BaseObject()
{
}

Symbol::Symbol(const Symbol &src):
    name_(src.name_),
    unique_name_(src.unique_name_),
    type_(src.type_),
    number_of_arguments_(src.number_of_arguments_),
    used_(src.used_),
    BaseObject(src)
{
}

Symbol::Symbol(const std::string &n):
    name_(n),
    unique_name_(""),
    type_(Unknown),
    number_of_arguments_(0),
    used_(0),
    BaseObject()
{
}

Symbol::Symbol(const std::string &n, Type t, int na):
    name_(n),
    unique_name_(""),
    type_(t),
    number_of_arguments_(na),
    used_(0),
    BaseObject()
{
}

Symbol::Symbol(const std::string &n, const std::string &un):
    name_(n),
    unique_name_(un),
    type_(Unknown),
    number_of_arguments_(0),
    used_(0),
    BaseObject()
{
}

Symbol::Symbol(const std::string &n, const std::string &un, Type t, int an):
    name_(n),
    unique_name_(un),
    type_(t),
    number_of_arguments_(an),
    used_(0),
    BaseObject()
{
}

Symbol::~Symbol()
{
}

#undef STRINGIFYENUM
#define STRINGIFYENUM(ENUM) { ENUM, #ENUM }

const std::string
Symbol::type_name(Type type)
{
    static struct {
        Type type_;
        const char *name_;
    } names[] = {
        STRINGIFYENUM(PredicateFunction),
        STRINGIFYENUM(PredicateConstant),
        STRINGIFYENUM(Function),
        STRINGIFYENUM(Variable),
        STRINGIFYENUM(Constant),
        STRINGIFYENUM(UniversalVariable),
        STRINGIFYENUM(ExistentialVariable),
        STRINGIFYENUM(Unknown),
        STRINGIFYENUM(Last)
    };
    return names[type].name_;
}

#undef STRINGIFYENUM

void
Symbol::clear_scope()
{
    scope_.clear();
}

void
Symbol::clear_table()
{
    table_.clear();
}

// output operator
std::ostream &
operator<<(std::ostream &os, const Symbol &sym)
{
	os << "symbol name  : " << sym.name_ << std::endl;
	os << "symbol uname : " << sym.unique_name_ << std::endl;
	os << "symbol type  : " << Symbol::type_name(sym.type_) << std::endl;
	os << "symbol args  : " << sym.number_of_arguments_ << std::endl;
	os << "symbol used  : " << sym.used_ << std::endl;
	return(os);
}

}

