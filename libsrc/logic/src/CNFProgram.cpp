//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// list of semantic records

// local includes
#include "system/Debug.h"
#include "hdr/CNFProgram.h"

namespace ombt {

// ctors and dtor
CNFProgram::CNFProgram(): Program()
{
    form_type(Program::FT_ConjunctiveNormalForm);
}

CNFProgram::CNFProgram(const CNFProgram &src): Program(src)
{
    MustBeTrue(src.form_type() == Program::FT_ConjunctiveNormalForm);
}

CNFProgram::CNFProgram(const Program &src): Program()
{
    MustBeTrue(src.form_type() == Program::FT_ParsedForm);

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
    TypeCIter cit    = src.program().begin();
    TypeCIter citend = src.program().end();
    for ( ; cit!=citend; ++cit)
    {
        ExtUseCntPtr<Semantic> psem;
        psem = new Semantic(**cit);
        MustBeTrue(psem->removeBiconditionals() == OK);
        MustBeTrue(psem->removeConditionals() == OK);
        MustBeTrue(psem->demorgans() == OK);
        MustBeTrue(psem->renameVariables() == OK);
        MustBeTrue(psem->skolemize() == OK);
        MustBeTrue(psem->removeUniversals() == OK);
        MustBeTrue(psem->distributionOrsOverAnds() == OK);
        insert(psem);
    }
    form_type(Program::FT_ConjunctiveNormalForm);
    logic_type(src.logic_type());
}

CNFProgram::~CNFProgram()
{
}

// assignment
CNFProgram &
CNFProgram::operator=(const CNFProgram &rhs)
{
    MustBeTrue(rhs.form_type() == Program::FT_ConjunctiveNormalForm);
    if (this != &rhs)
    {
        Program::operator=(rhs);
    }
    return(*this);
}

CNFProgram &
CNFProgram::operator=(const Program &rhs)
{
    MustBeTrue(rhs.form_type() == Program::FT_ParsedForm);
    if (this != &rhs)
    {
        program_.clear();

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
        TypeCIter cit    = rhs.program().begin();
        TypeCIter citend = rhs.program().end();
        for ( ; cit!=citend; ++cit)
        {
            ExtUseCntPtr<Semantic> psem;
            psem = new Semantic(**cit);
            MustBeTrue(psem->removeBiconditionals() == OK);
            MustBeTrue(psem->removeConditionals() == OK);
            MustBeTrue(psem->demorgans() == OK);
            MustBeTrue(psem->renameVariables() == OK);
            MustBeTrue(psem->skolemize() == OK);
            MustBeTrue(psem->removeUniversals() == OK);
            MustBeTrue(psem->distributionOrsOverAnds() == OK);
            insert(psem);
        }
        form_type(Program::FT_ConjunctiveNormalForm);
        logic_type(rhs.logic_type());
    }
    return(*this);
}

// output
std::ostream &
operator<<(std::ostream &os, const CNFProgram &p)
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

