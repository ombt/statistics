//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// list of semantic records

// local includes
#include "system/Debug.h"
#include "hdr/ClausesProgram.h"

namespace ombt {

// ctors and dtor
ClausesProgram::ClausesProgram(): CNFProgram()
{
    form_type(Program::FT_ClauseForm);
}

ClausesProgram::ClausesProgram(const ClausesProgram &src): CNFProgram(src)
{
    MustBeTrue(src.form_type() == Program::FT_ClauseForm);
}

ClausesProgram::ClausesProgram(const CNFProgram &src): CNFProgram()
{
    MustBeTrue(src.form_type() == Program::FT_ConjunctiveNormalForm);

    // here's an interesting qualifier for protected
    // member variables. a derived class object can access
    // a protected base member variable for an object of the 
    // the same derived class. however, a derived class object
    // can not access a protected base member variable if the 
    // the the object is *not* of the same derived class. in this
    // case src is NOT a derived object, so this object can NOT
    // access the protected program variable. so we have to use
    // the program() accessor function.
    //
    Program::TypeCIter cit    = src.program().begin();
    Program::TypeCIter citend = src.program().end();
    for ( ; cit!=citend; ++cit)
    {
        std::list<ExtUseCntPtr<Semantic> > clist;
        MustBeTrue((*cit)->removeAnds(clist) == OK);

        std::list<ExtUseCntPtr<Semantic> >::iterator lit, litend;
        lit = clist.begin();
        litend = clist.end();
        for ( ; lit!=litend; ++lit)
        {
            if (((*lit)->type() != Semantic::StartArgument) &&
                ((*lit)->type() != Semantic::EndArgument))
            {
                MustBeTrue((*lit)->renameAgain() == OK);

                std::list<ExtUseCntPtr<Semantic> > clist2;
                MustBeTrue((*lit)->removeOrs(clist2) == OK);

                Clause clause(clist2);

                insert(clause);
            }
        }
    }
    form_type(Program::FT_ClauseForm);
    logic_type(src.logic_type());
}

ClausesProgram::~ClausesProgram()
{
}

// assignment
ClausesProgram &
ClausesProgram::operator=(const ClausesProgram &rhs)
{
    MustBeTrue(rhs.form_type() == Program::FT_ClauseForm);
    if (this != &rhs)
    {
        CNFProgram::operator=(rhs);
    }
    return(*this);
}

ClausesProgram &
ClausesProgram::operator=(const CNFProgram &rhs)
{
    MustBeTrue(rhs.form_type() == Program::FT_ConjunctiveNormalForm);
    if (this != &rhs)
    {
        program_.clear();
        clauses_.clear();

        BaseObject::operator=(rhs);

        // here's an interesting qualifier for protected
        // member variables. a derived class object can access
        // a protected base member variable for an object of the 
        // the same derived class. however, a derived class object
        // can not access a protected base member variable if the 
        // the the object is *not* of the same derived class. in this
        // case rhs is NOT a derived object, so this object can NOT
        // access the protected program variable. so we have to use
        // the program() accessor function.
        //
        Program::TypeCIter cit    = rhs.program().begin();
        Program::TypeCIter citend = rhs.program().end();
        for ( ; cit!=citend; ++cit)
        {
            std::list<ExtUseCntPtr<Semantic> > clist;
            MustBeTrue((*cit)->removeAnds(clist) == OK);

            std::list<ExtUseCntPtr<Semantic> >::iterator lit, litend;
            lit = clist.begin();
            litend = clist.end();
            for ( ; lit!=litend; ++lit)
            {
                MustBeTrue((*lit)->renameAgain() == OK);

                std::list<ExtUseCntPtr<Semantic> > clist2;
                MustBeTrue((*lit)->removeOrs(clist2) == OK);

                Clause clause;

                insert(clause);
            }
        }
        form_type(Program::FT_ClauseForm);
        logic_type(rhs.logic_type());
    }
    return(*this);
}

// output
void
ClausesProgram::reconstruct(std::ostream &os) const
{
    ClausesProgram::TypeCIter cit    = clauses_.begin();
    ClausesProgram::TypeCIter citend = clauses_.end();

    for ( ; cit!=citend; ++cit)
    {
        cit->reconstruct(os);
    }
}

std::ostream &
operator<<(std::ostream &os, const ClausesProgram &p)
{
    ClausesProgram::TypeCIter cit    = p.clauses_.begin();
    ClausesProgram::TypeCIter citend = p.clauses_.end();

    for ( ; cit!=citend; ++cit)
    {
        os << *cit << std::endl;
    }

    return os;
}

}

