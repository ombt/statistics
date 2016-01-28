//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// list of semantic records

// local includes
#include "system/Debug.h"
#include "hdr/Program.h"

namespace ombt {

// ctors and dtor
Program::Program():
    form_type_(FT_Unknown),
    logic_type_(LT_Unknown),
    program_(),
    BaseObject()
{
    status(OK);
}

Program::Program(const Program &src): 
    form_type_(src.form_type_),
    logic_type_(src.logic_type_),
    program_(), 
    BaseObject(src)
{
    TypeCIter cit    = src.program().begin();
    TypeCIter citend = src.program().end();
    for ( ; cit!=citend; ++cit)
    {
        ExtUseCntPtr<Semantic> psem;
        psem = new Semantic(**cit);
        insert(psem);
    }
    status(src.status());
}

Program::~Program()
{
    clear_all();
    status(NOTOK);
}

// assignment
Program &
Program::operator=(const Program &rhs)
{
    if (this != &rhs)
    {
        clear_all();

        BaseObject::operator=(rhs);

        form_type_  = rhs.form_type_;
        logic_type_ = rhs.logic_type_;
        TypeCIter cit    = rhs.program().begin();
        TypeCIter citend = rhs.program().end();
        for ( ; cit!=citend; ++cit)
        {
            ExtUseCntPtr<Semantic> psem;
            psem = new Semantic(**cit);
            insert(psem);
        }
        status(rhs.status());
    }
    return(*this);
}

// output
void
Program::reconstruct(std::ostream &os) const
{
    Program::TypeCIter cit    = program_.begin();
    Program::TypeCIter citend = program_.end();

    for ( ; cit!=citend; ++cit)
    {
        if (*cit != NULL)
        {
            std::string x;
            (*cit)->reconstruct(x);
            os << x << std::endl;
        }
    }
}

std::ostream &
operator<<(std::ostream &os, const Program &p)
{
    Program::TypeCIter cit    = p.program_.begin();
    Program::TypeCIter citend = p.program_.end();

    for ( ; cit!=citend; ++cit)
    {
        if (*cit != NULL)
            os << **cit << std::endl;
    }

    return os;
}

}

