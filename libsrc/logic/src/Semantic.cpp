//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// semantic records for logic-based languages.

// system includes
#include <string>
#include <list>

// local includes
#include "system/Debug.h"
#include "conversions/Generic.h"
#include "hdr/Semantic.h"

namespace ombt {

// ctors and dtor
Semantic::Semantic():
    conclusion_(false),
    set_of_support_(false),
    query_(false),
    number_of_arguments_(0),
    type_(Unknown),
    name_(""),
    value_(""),
    left_(NULL),
    right_(NULL),
    arguments_(),
    BaseObject()
{
}

Semantic::Semantic(const Semantic &src): 
    conclusion_(src.conclusion_),
    set_of_support_(src.set_of_support_),
    query_(src.query_),
    number_of_arguments_(src.number_of_arguments_),
    type_(src.type_),
    name_(src.name_),
    value_(src.value_),
    left_(NULL),
    right_(NULL),
    arguments_(),
    BaseObject(src)
{
    if (src.left_ != NULL)
    {
        left_ = new Semantic(*src.left_);
    }
    if (src.right_ != NULL)
    {
        right_ = new Semantic(*src.right_);
    }

    ArgType pa;
    ArgListTypeCIter cit    = src.arguments_.begin();
    ArgListTypeCIter citend = src.arguments_.end();
    for ( ; cit!=citend; ++cit)
    {
        pa = new Semantic(**cit);
        MustBeTrue(pa != NULL);
        arguments_.push_back(pa);
    }
}

Semantic::Semantic(Type type, 
                   ExtUseCntPtr<Semantic> &right):
    conclusion_(false),
    set_of_support_(false),
    query_(false),
    number_of_arguments_(0),
    type_(type),
    name_(""),
    value_(""),
    left_(NULL),
    right_(right),
    arguments_(),
    BaseObject()
{
}

Semantic::Semantic(Type type, 
                   ExtUseCntPtr<Semantic> &left, 
                   ExtUseCntPtr<Semantic> &right):
    conclusion_(false),
    set_of_support_(false),
    query_(false),
    number_of_arguments_(0),
    type_(type),
    name_(""),
    value_(""),
    left_(left),
    right_(right),
    arguments_(),
    BaseObject()
{
}

Semantic::Semantic(Type type, 
                   const std::string &name, 
                   const std::string &value):
    conclusion_(false),
    set_of_support_(false),
    query_(false),
    number_of_arguments_(0),
    type_(type),
    name_(name),
    value_(value),
    left_(NULL),
    right_(NULL),
    arguments_(),
    BaseObject()
{
}

Semantic::Semantic(Type type, 
                   const std::string &data):
    conclusion_(false),
    set_of_support_(false),
    query_(false),
    number_of_arguments_(0),
    type_(type),
    name_(data),
    value_(data),
    left_(NULL),
    right_(NULL),
    arguments_(),
    BaseObject()
{
}

Semantic::Semantic(Type type, 
                   const std::string &name,
                   ExtUseCntPtr<Semantic> &right):
    conclusion_(false),
    set_of_support_(false),
    query_(false),
    number_of_arguments_(0),
    type_(type),
    name_(name),
    value_(""),
    left_(NULL),
    right_(right),
    arguments_(),
    BaseObject()
{
}

Semantic::Semantic(Type type):
    conclusion_(false),
    set_of_support_(false),
    query_(false),
    number_of_arguments_(0),
    type_(type),
    name_(""),
    value_(""),
    left_(NULL),
    right_(NULL),
    arguments_(),
    BaseObject()
{
}

Semantic::~Semantic()
{
    left_ = NULL;
    right_ = NULL;
    arguments_.clear();
}

// assignment
Semantic &
Semantic::operator=(const Semantic &rhs)
{
    if (this != &rhs)
    {
        BaseObject::operator=(rhs);

        conclusion_ = rhs.conclusion_;
        set_of_support_ = rhs.set_of_support_;
        query_ = rhs.query_;
        number_of_arguments_ = rhs.number_of_arguments_;
        type_ = rhs.type_;
        name_ = rhs.name_;
        value_ = rhs.value_;

        if (rhs.left_ != NULL)
        {
            left_ = new Semantic(*rhs.left_);
        }
        if (rhs.right_ != NULL)
        {
            right_ = new Semantic(*rhs.right_);
        }

        ArgType pa;
        ArgListTypeCIter cit    = rhs.arguments_.begin();
        ArgListTypeCIter citend = rhs.arguments_.end();
        for ( ; cit!=citend; ++cit)
        {
            pa = new Semantic(**cit);
            MustBeTrue(pa != NULL);
            arguments_.push_back(pa);
        }
    }
    return(*this);
}

// functions for putting expression in CNF.

// replace (A <--> B) with (A -> B) && (B -> A)
int 
Semantic::removeBiconditionals()
{
    if ((left_ != NULL) && (left_->removeBiconditionals() != OK))
    {
        return NOTOK;
    }
    if ((right_ != NULL) && (right_->removeBiconditionals() != OK))
    {
        return NOTOK;
    }

    if (type_ == Biconditional)
    {
        ExtUseCntPtr<Semantic> p;

        p = new Semantic(*left_);
        MustBeTrue(p != NULL);

        Semantic *pnewr = new Semantic(Conditional, p, right_);
        MustBeTrue(pnewr != NULL);

        p = new Semantic(*right_);
        MustBeTrue(p != NULL);

        Semantic *pnewl = new Semantic(Conditional, p, left_);
        MustBeTrue(pnewl != NULL);

        left_ = pnewl;
        right_ = pnewr;
        type_ = And;
    }

    return OK;
}

// replace (A -> B) with (~A || B)
int 
Semantic::removeConditionals()
{
    if ((left_ != NULL) && (left_->removeConditionals() != OK))
    {
        return NOTOK;
    }
    if ((right_ != NULL) && (right_->removeConditionals() != OK))
    {
        return NOTOK;
    }

    if (type_ == Conditional)
    {
        Semantic *pnewl = new Semantic(Negation, left_);
        MustBeTrue(pnewl != NULL);

        left_ = pnewl;
        type_ = Or;
    }

    return OK;
}

// replace ~~A with A
int 
Semantic::removeExtraNots()
{
    while ((type_ == Negation) && (right_->type_ == Negation))
    {
        number_of_arguments_ = right_->right_->number_of_arguments_;
        type_ = right_->right_->type_;
        name_ = right_->right_->name_;
        value_ = right_->right_->value_;
        arguments_.assign(right_->right_->arguments_.begin(), 
                          right_->right_->arguments_.end());
        ExtUseCntPtr<Semantic> ps;
        ps = right_->right_->left_;
        left_ = ps;
        ps = right_->right_->right_;
        right_ = ps;
    }
    return OK;
}

// apply demorgans laws:
//
// replace ~(A && B) with (~A || ~B)
// replace ~(A || B) with (~A && ~B)
// replace ~forall(x)(F(x)) with forsome(x)(~F(x))
// replace ~forsome(x)(F(x)) with forall(x)(~F(x))
int 
Semantic::demorgans()
{
    // remove multiple NOTs: ~~A <--> A
    removeExtraNots();

    // check if a negation
    if (type_ == Negation)
    {
        // we have a negation. check the operator type.
        switch (right_->type_)
        {
        case Or:
        {
            // replace ~(A || B) with (~A && ~B)
            left_ = new Semantic(Negation, right_->left_);
            MustBeTrue(left_ != NULL);
            right_ = new Semantic(Negation, right_->right_);
            MustBeTrue(right_ != NULL);
            type_ = And;
            break;
        }
        case And:
        {
            // replace ~(A && B) with (~A || ~B)
            left_ = new Semantic(Negation, right_->left_);
            MustBeTrue(left_ != NULL);
            right_ = new Semantic(Negation, right_->right_);
            MustBeTrue(right_ != NULL);
            type_ = Or;
            break;
        }
        case Universal:
        {
            // replace ~forall(x)(F(x)) with forsome(x)(~F(x))
            type_ = Existential;
            name_ = right_->name_;
            value_ = right_->value_;
            right_ = new Semantic(Negation, right_->right_);
            MustBeTrue(right_ != NULL);
            break;
        }
        case Existential:
        {
            // replace ~forsome(x)(F(x)) with forall(x)(~F(x))
            type_ = Universal;
            name_ = right_->name_;
            value_ = right_->value_;
            right_ = new Semantic(Negation, right_->right_);
            MustBeTrue(right_ != NULL);
            break;
        }
        }
    }

    if ((left_ != NULL) && (left_->demorgans() != OK))
    {
        return NOTOK;
    }
    if ((right_ != NULL) && (right_->demorgans() != OK))
    {
        return NOTOK;
    }

    return OK;
}

int 
Semantic::renameVariables()
{
    Symbol::StackType renames;
    return renameVariables(renames);
}

int 
Semantic::skolemize()
{
    Symbol::StackType uvars_scope;
    SkolemMapType skolem_fns;
    return skolemize(uvars_scope, skolem_fns);
}

int 
Semantic::removeUniversals()
{
    if ((left_ != NULL) && 
        (left_->removeUniversals() != OK))
    {
        return NOTOK;
    }
    if ((right_ != NULL) && 
        (right_->removeUniversals() != OK))
    {
        return NOTOK;
    }

    if (type_ == Universal)
    {
        number_of_arguments_ = right_->number_of_arguments_;
        type_ = right_->type_;
        name_ = right_->name_;
        value_ = right_->value_;
        arguments_.assign(right_->arguments_.begin(), 
                          right_->arguments_.end());
        ExtUseCntPtr<Semantic> ps;
        ps = right_->left_;
        left_ = ps;
        ps = right_->right_;
        right_ = ps;
    }
    return OK;
}

// replace A || (B && C) with (A || B) && (A || C)
//     OR
// replace (A && B) || C with (A || C) && (B || C)
int 
Semantic::distributionOrsOverAnds()
{
    for (bool changed = true; changed; )
    {
        changed = false;
        if (distributionOrsOverAnds(changed) != OK)
        {
            return NOTOK;
        }
    }
    return OK;
}

// replace A && (B || C) with (A && B) || (A && C)
//     OR
// replace (A || B) && C with (A && C) || (B && C)
int 
Semantic::distributionAndsOverOrs()
{
    for (bool changed = true; changed; )
    {
        changed = false;
        if (distributionAndsOverOrs(changed) != OK)
        {
            return NOTOK;
        }
    }
    return OK;
}

int 
Semantic::removeAnds(std::list<ExtUseCntPtr<Semantic> > &clist)
{
    return removeAnds(clist, conclusion_, set_of_support_, query_);
}

int 
Semantic::renameAgain()
{
    RenameMapType names;
    return renameAgain(names);
}

int 
Semantic::removeOrs(std::list<ExtUseCntPtr<Semantic> > &clist)
{
    return removeOrs(clist, conclusion_, set_of_support_, query_);
}


#undef STRINGIFYENUM
#define STRINGIFYENUM(ENUM) { ENUM, #ENUM }

const std::string
Semantic::type_name(Type type)
{
    static struct {
        Semantic::Type type_;
        const char *name_;
    } names[] = {
        STRINGIFYENUM(And),
        STRINGIFYENUM(StartArgument),
        STRINGIFYENUM(EndArgument),
        STRINGIFYENUM(Biconditional),
        STRINGIFYENUM(Conditional),
        STRINGIFYENUM(Constant),
        STRINGIFYENUM(Equal),
        STRINGIFYENUM(Existential),
        STRINGIFYENUM(Expression),
        STRINGIFYENUM(Function),
        STRINGIFYENUM(LogicalConstant),
        STRINGIFYENUM(Negation),
        STRINGIFYENUM(Number),
        STRINGIFYENUM(Option),
        STRINGIFYENUM(Or),
        STRINGIFYENUM(Predicate),
        STRINGIFYENUM(PredicateConstant),
        STRINGIFYENUM(PredicateFunction),
        STRINGIFYENUM(QuotedString),
        STRINGIFYENUM(Term),
        STRINGIFYENUM(Universal),
        STRINGIFYENUM(Unknown),
        STRINGIFYENUM(Variable),
        STRINGIFYENUM(Last)
    };
    return names[type].name_;
}

#undef STRINGIFYENUM

std::ostream &
operator<<(std::ostream &os, const Semantic &s)
{
    static int ioffset = 0;

    ioffset += 1;
    std::string offset(5*ioffset, ' ');

    os << offset << "(c,sos,q,na,t,nm,val) = (" 
       << s.conclusion_ << "," 
       << s.set_of_support_ << ","
       << s.query_ << ","
       << s.number_of_arguments_ << ","
       << Semantic::type_name(s.type_) << ","
       << s.name_ << ","
       << s.value_ << ")";

    if (s.left_ != NULL)
        os << std::endl << *s.left_;
    if (s.right_ != NULL)
        os << std::endl << *s.right_;

    std::list<ExtUseCntPtr<Semantic> >::const_iterator cit, citend;
    cit = s.arguments_.begin();
    citend = s.arguments_.end();
    for ( ; cit!=citend; ++cit)
    {
        if (*cit != NULL)
            os << std::endl << **cit;
    }

    ioffset -= 1;

    return os;
}

void
Semantic::reconstruct(std::string &x) const
{
    if (set_of_support_) x += "SOS ";
    if (query_) x += "?- ";
    if (conclusion_) x += "|- ";

    switch (type_)
    {
    case And:
    {
        x += "( ";
        left_->reconstruct(x);
        x += "&& ";
        right_->reconstruct(x);
        x += ") ";
        break;
    }
    case StartArgument:
    {
        x += "start { ";
        break;
    }
    case EndArgument:
    {
        x += "} ";
        break;
    }
    case Biconditional:
    {
        x += "( ";
        left_->reconstruct(x);
        x += "<--> ";
        right_->reconstruct(x);
        x += ") ";
        break;
    }
    case Conditional:
    {
        x += "( ";
        left_->reconstruct(x);
        x += "--> ";
        right_->reconstruct(x);
        x += ") ";
        break;
    }
    case Constant:
    {
        x += name_ + " ";
        break;
    }
    case Equal:
    {
        x += "( ";
        left_->reconstruct(x);
        x += "= ";
        right_->reconstruct(x);
        x += ") ";
        break;
    }
    case Existential:
    {
        x += "forsome( ";
        x += name_ + " ";
        x += ") ( ";
        right_->reconstruct(x);
        x += ") ";
        break;
    }
    case Expression:
    {
        MustBeTrue(type_ != Expression);
        break;
    }
    case Function:
    {
        x += name_ + " ( ";
        ArgListTypeCIter cit    = arguments_.begin();
        ArgListTypeCIter citend = arguments_.end();
        if (cit != citend) 
        {
            (*cit)->reconstruct(x);
            for (++cit; cit!=citend; ++cit)
            {
                x += ", ";
                (*cit)->reconstruct(x);
            }
        }
        x += ") ";
        break;
    }
    case LogicalConstant:
    {
        x += value_ + " ";
        break;
    }
    case Negation:
    {
        x += "~ ";
        right_->reconstruct(x);
        break;
    }
    case Number:
    {
        x += value_ + " ";
        break;
    }
    case Option:
    {
        x += "option { " + name_ + " = " + value_ + " } ";
        break;
    }
    case Or:
    {
        x += "( ";
        left_->reconstruct(x);
        x += "|| ";
        right_->reconstruct(x);
        x += ") ";
        break;
    }
    case Predicate:
    {
        MustBeTrue(type_ != Predicate);
        break;
    }
    case PredicateConstant:
    {
        x += value_ + " ";
        break;
    }
    case PredicateFunction:
    {
        x += name_ + " ( ";
        ArgListTypeCIter cit    = arguments_.begin();
        ArgListTypeCIter citend = arguments_.end();
        if (cit != citend) 
        {
            (*cit)->reconstruct(x);
            for (++cit; cit!=citend; ++cit)
            {
                x += ", ";
                (*cit)->reconstruct(x);
            }
        }
        x += ") ";
        break;
    }
    case QuotedString:
    {
        x += "\"" + value_ + "\" ";
        break;
    }
    case Term:
    {
        MustBeTrue(type_ != Term);
        break;
    }
    case Universal:
    {
        x += "forall( ";
        x += name_ + " ) (";
        right_->reconstruct(x);
        x += ") ";
        break;
    }
    case Unknown:
    {
        MustBeTrue(type_ != Unknown);
        break;
    }
    case Variable:
    {
        x += name_ + " ";
        break;
    }
    case Last:
    {
        MustBeTrue(type_ != Last);
        break;
    }
    default:
    {
        MustBeTrue(0);
        break;
    }
    }
}

// helper functions
std::string
Semantic::uniqueName(const std::string &prefix)
{
    static long rename_counter_ = 0;
    rename_counter_ += 1;
    return prefix + "_" + to_string(rename_counter_);
}

int 
Semantic::renameVariables(Symbol::StackType &renames)
{
    bool pop_stack = false;

    switch (type_)
    {
    case Universal:
    {
        std::string unique_name = uniqueName("uvar");
        Symbol newsym(name_, unique_name);
        renames.push_front(newsym);
        name_ =  unique_name;
        pop_stack = true;
        break;
    }
    case Existential:
    {
        std::string unique_name = uniqueName("xvar");
        Symbol newsym(name_, unique_name);
        renames.push_front(newsym);
        name_ =  unique_name;
        pop_stack = true;
        break;
    }
    default:
    {
        pop_stack = false;
        if (name_ != "")
        {
            Symbol::StackTypeCIter cit    = renames.begin();
            Symbol::StackTypeCIter citend = renames.end();
            for ( ; cit!=citend; ++cit)
            {
                if (cit->name() == name_)
                {
                    name_ = cit->unique_name();
                    break;
                }
            }
        }
        if (arguments_.size() > 0)
        {
            ArgListTypeIter it    = arguments_.begin();
            ArgListTypeIter itend = arguments_.end();
            for ( ; it!=itend; ++it)
            {
                if ((*it)->renameVariables(renames) != OK)
                {
                    return NOTOK;
                }
            }
        }
        break;
    }
    }

    if ((left_ != NULL) && 
        (left_->renameVariables(renames) != OK))
    {
        return NOTOK;
    }
    if ((right_ != NULL) && 
        (right_->renameVariables(renames) != OK))
    {
        return NOTOK;
    }

    if (pop_stack)
    {
        renames.pop_front();
    }

    return OK;
}

int 
Semantic::skolemize(Symbol::StackType &uvars_scope,
                    SkolemMapType &skolem_fns)
{
    switch (type_)
    {
    case Universal:
    {
        // push universal variable into scope stack
        Symbol newsym(name_);
        uvars_scope.push_back(newsym);

        // traverse universal expression 
        MustBeTrue(right_ != NULL);
        if (right_->skolemize(uvars_scope, skolem_fns) != OK)
        {
            return NOTOK;
        }

        // pop universal variable from scope stack.
        uvars_scope.pop_back();

        return OK;
    }
    case Existential:
    {
        // create new skolem constants or functions
        if (uvars_scope.size() == 0)
        {
            // no universal variables in scope,
            // so make a new skolem constant.
            std::string skolem_const = uniqueName("skolem");

            // new semantic record for this constant.
            ExtUseCntPtr<Semantic> pconst;
            pconst = new Semantic(Constant, skolem_const);
            MustBeTrue(pconst != NULL);

            // save for future use
            skolem_fns.insert(SkolemMapType::value_type(name_, pconst));
        }
        else
        {
            // we have universal variables in scope,
            // new make a new skolem function dependent
            // on these universal variables in scope.
            std::string skolem_func = uniqueName("skolem");

            // new semantic record for this function
            ExtUseCntPtr<Semantic> pfunc;
            pfunc = new Semantic(Function, skolem_func);
            MustBeTrue(pfunc != NULL);

            // the function is dependent on the universal
            // variables currently in scope. add argument
            // records to the skolem function, one for each
            // universal quantifier variable in scope.
            Symbol::StackTypeCIter cit    = uvars_scope.begin();
            Symbol::StackTypeCIter citend = uvars_scope.end();
            for ( ; cit!=citend; ++cit)
            {
                ExtUseCntPtr<Semantic> parg;
                parg = new Semantic(Variable, cit->name());
                pfunc->arguments().push_back(parg);
            }
            pfunc->number_of_arguments(pfunc->arguments().size());

            // save for future use
            skolem_fns.insert(SkolemMapType::value_type(name_, pfunc));
        }

        // traverse existential expression 
        MustBeTrue(right_ != NULL);
        if (right_->skolemize(uvars_scope, skolem_fns) != OK)
        {
            return NOTOK;
        }

        // remove the existential record. move right to top,
        // except keep conclusion flag, set of support and whether
        // it is a query.
        number_of_arguments_ = right_->number_of_arguments_;
        type_ = right_->type_;
        name_ = right_->name_;
        value_ = right_->value_;
        arguments_.assign(right_->arguments_.begin(), 
                          right_->arguments_.end());
        ExtUseCntPtr<Semantic> ps;
        ps = right_->left_;
        left_ = ps;
        ps = right_->right_;
        right_ = ps;

        return OK;
    }
    case Variable:
    {
        SkolemMapTypeCIter cit =  skolem_fns.find(name_);
        if (cit != skolem_fns.end())
        {
            // overwrite the current variable record
            // with the skolem record. assignment and 
            // copy ctor are deep copies.
            *this = *(cit->second);
        }
        return OK;
    }
    default:
    {
        if ((left_ != NULL) && 
            (left_->skolemize(uvars_scope, skolem_fns) != OK))
        {
            return NOTOK;
        }
        if ((right_ != NULL) && 
            (right_->skolemize(uvars_scope, skolem_fns) != OK))
        {
            return NOTOK;
        }
        if (arguments_.size() > 0)
        {
            ArgListTypeIter it    = arguments_.begin();
            ArgListTypeIter itend = arguments_.end();
            for ( ; it!=itend; ++it)
            {
                if ((*it)->skolemize(uvars_scope, skolem_fns) != OK)
                {
                    return NOTOK;
                }
            }
        }
        return OK;
    }
    }
}

// replace (A && B) || C with (A || C) && (B || C)
//     OR
// replace A || (B && C) with (A || B) && (A || C)
int 
Semantic::distributionOrsOverAnds(bool &changed)
{
    if (type_ == Or)
    {
        if (left_->type_ == And)
        {
            ExtUseCntPtr<Semantic> pc;
            pc = right_;

            ExtUseCntPtr<Semantic> pdupc;
            pdupc = new Semantic(*pc);
            MustBeTrue(pdupc != NULL);

            ExtUseCntPtr<Semantic> pnewl;
            pnewl = new Semantic(Or, left_->left_, pc);
            MustBeTrue(pnewl != NULL);
        
            ExtUseCntPtr<Semantic> pnewr;
            pnewr = new Semantic(Or, left_->right_, pdupc);
            MustBeTrue(pnewr != NULL);
        
            type_ = And;
            left_ = pnewl;
            right_ = pnewr;
            changed = true;
        }
        if (right_->type_ == And)
        {
            ExtUseCntPtr<Semantic> pa;
            pa = left_;

            ExtUseCntPtr<Semantic> pdupa;
            pdupa = new Semantic(*pa);
            MustBeTrue(pdupa != NULL);

            ExtUseCntPtr<Semantic> pnewl;
            pnewl = new Semantic(Or, pa, right_->left_);
            MustBeTrue(pnewl != NULL);
        
            ExtUseCntPtr<Semantic> pnewr;
            pnewr = new Semantic(Or, pdupa, right_->right_);
            MustBeTrue(pnewr != NULL);
        
            type_ = And;
            left_ = pnewl;
            right_ = pnewr;
            changed = true;
        }
    }

    if ((left_ != NULL) && (left_->distributionOrsOverAnds(changed) != OK))
    {
        return NOTOK;
    }
    if ((right_ != NULL) && (right_->distributionOrsOverAnds(changed) != OK))
    {
        return NOTOK;
    }

    return OK;
}

// replace (A || B) && C with (A && C) || (B && C)
//     OR
// replace A && (B || C) with (A && B) || (A && C)
int 
Semantic::distributionAndsOverOrs(bool &changed)
{
    if (type_ == And)
    {
        if (left_->type_ == Or)
        {
            ExtUseCntPtr<Semantic> pc;
            pc = right_;

            ExtUseCntPtr<Semantic> pdupc;
            pdupc = new Semantic(*pc);
            MustBeTrue(pdupc != NULL);

            ExtUseCntPtr<Semantic> pnewl;
            pnewl = new Semantic(Or, left_->left_, pc);
            MustBeTrue(pnewl != NULL);
        
            ExtUseCntPtr<Semantic> pnewr;
            pnewr = new Semantic(Or, left_->right_, pdupc);
            MustBeTrue(pnewr != NULL);
        
            type_ = Or;
            left_ = pnewl;
            right_ = pnewr;
            changed = true;
        }
        if (right_->type_ == Or)
        {
            ExtUseCntPtr<Semantic> pa;
            pa = left_;

            ExtUseCntPtr<Semantic> pdupa;
            pdupa = new Semantic(*pa);
            MustBeTrue(pdupa != NULL);

            ExtUseCntPtr<Semantic> pnewl;
            pnewl = new Semantic(Or, pa, right_->left_);
            MustBeTrue(pnewl != NULL);
        
            ExtUseCntPtr<Semantic> pnewr;
            pnewr = new Semantic(Or, pdupa, right_->right_);
            MustBeTrue(pnewr != NULL);
        
            type_ = Or;
            left_ = pnewl;
            right_ = pnewr;
            changed = true;
        }
    }

    if ((left_ != NULL) && (left_->distributionAndsOverOrs(changed) != OK))
    {
        return NOTOK;
    }
    if ((right_ != NULL) && (right_->distributionAndsOverOrs(changed) != OK))
    {
        return NOTOK;
    }

    return OK;
}

int 
Semantic::removeAnds(std::list<ExtUseCntPtr<Semantic> > &clist,
                    bool conclusion, bool sos, bool query)
{
    switch (type_)
    {
    case Existential:
    case Universal:
    case Conditional:
    case Biconditional:
    {
        MustBeTrue(0);
        return OK;
    }
    case And:
    {
        if ((left_ != NULL) &&
            (left_->removeAnds(clist, conclusion, sos, query) != OK))
        {
            return NOTOK;
        }
        if ((right_ != NULL) &&
            (right_->removeAnds(clist, conclusion, sos, query) != OK))
        {
            return NOTOK;
        }
        return OK;
    }
    default:
    {
        ExtUseCntPtr<Semantic> pnew;
        pnew = new Semantic(*this);
        MustBeTrue(pnew != NULL);
        pnew->conclusion(conclusion);
        pnew->set_of_support(sos);
        pnew->query(query);
        clist.push_back(pnew);
        return OK;
    }
    }
}

int 
Semantic::renameAgain(Semantic::RenameMapType &names)
{
    if (type_ == Variable)
    {
        RenameMapTypeIter it = names.find(name_);
        if (it != names.end())
        {
            name_ = it->second;
        }
        else
        {
            std::string unique_name = uniqueName("rvar");
            names.insert(RenameMapType::value_type(name_, unique_name));
            name_ = unique_name;
        }
    }

    if ((left_ != NULL) && (left_->renameAgain(names) != OK))
    {
        return NOTOK;
    }
    if ((right_ != NULL) && (right_->renameAgain(names) != OK))
    {
        return NOTOK;
    }

    if (arguments_.size() > 0)
    {
        ArgListTypeIter it    = arguments_.begin();
        ArgListTypeIter itend = arguments_.end();
        for ( ; it!=itend; ++it)
        {
            if ((*it)->renameAgain(names) != OK)
            {
                return NOTOK;
            }
        }
    }
    return OK;
}

int 
Semantic::removeOrs(std::list<ExtUseCntPtr<Semantic> > &clist,
                    bool conclusion, bool sos, bool query)
{
    switch (type_)
    {
    case Existential:
    case Universal:
    case And:
    case Conditional:
    case Biconditional:
    {
        MustBeTrue(0);
        return OK;
    }
    case Or:
    {
        if ((left_ != NULL) &&
            (left_->removeOrs(clist, conclusion, sos, query) != OK))
        {
            return NOTOK;
        }
        if ((right_ != NULL) &&
            (right_->removeOrs(clist, conclusion, sos, query) != OK))
        {
            return NOTOK;
        }
        return OK;
    }
    default:
    {
        ExtUseCntPtr<Semantic> pnew;
        pnew = new Semantic(*this);
        MustBeTrue(pnew != NULL);
        pnew->conclusion(conclusion);
        pnew->set_of_support(sos);
        pnew->query(query);
        clist.push_back(pnew);
        return OK;
    }
    }
}

void 
Semantic::dumpData(std::ostream &) const
{
}


}

