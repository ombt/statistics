//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// list of semantic records

// local includes
#include "system/Debug.h"
#include "hdr/Clause.h"

namespace ombt {

// ctors and dtor
Clause::Clause():
    clause_(),
    BaseObject()
{
}

Clause::Clause(std::list<ExtUseCntPtr<Semantic> > &clist):
    clause_(), BaseObject()
{
    std::list<ExtUseCntPtr<Semantic> >::iterator it, itend;
    it    = clist.begin();
    itend = clist.end();
    for ( ; it!=itend; ++it)
    {
        ExtUseCntPtr<Semantic> psem;
        psem = new Semantic(**it);
        insert(psem);
    }
}

Clause::Clause(const Clause &src): 
    clause_(), BaseObject(src)
{
    TypeCIter cit    = src.clause_.begin();
    TypeCIter citend = src.clause_.end();
    for ( ; cit!=citend; ++cit)
    {
        ExtUseCntPtr<Semantic> psem;
        psem = new Semantic(**cit);
        insert(psem);
    }
}

Clause::~Clause()
{
    clause_.clear();
}

// assignment
Clause &
Clause::operator=(const Clause &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);
        TypeCIter cit    = rhs.clause_.begin();
        TypeCIter citend = rhs.clause_.end();
        for ( ; cit!=citend; ++cit)
        {
            ExtUseCntPtr<Semantic> psem;
            psem = new Semantic(**cit);
            insert(psem);
        }
    }
    return(*this);
}

// output
void
Clause::reconstruct(std::ostream &os) const
{
    TypeCIter cit    = clause_.begin();
    TypeCIter citend = clause_.end();

    os << "{ ";
    for (bool first=true; cit!=citend; ++cit)
    {
        if (*cit != NULL)
        {
            std::string x;
            (*cit)->reconstruct(x);
            if (!first) os << ", ";
            os << x;
            first = false;
        }
    }
    os << "}" << std::endl;
}

std::ostream &
operator<<(std::ostream &os, const Clause &c)
{
    Clause::TypeCIter cit    = c.clause_.begin();
    Clause::TypeCIter citend = c.clause_.end();

    os << "<<<===" << std::endl;
    for ( ; cit!=citend; ++cit)
    {
        if (*cit != NULL)
            os << **cit << std::endl;
    }
    os << "===>>>" << std::endl;

    return os;
}

}

