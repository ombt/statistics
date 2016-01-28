// vector clock pair - one clock to track modifications and
// a second clock to track synchronizations.

#include "hdr/vectorclock.h"

// ctors and dtor
VectorClock::VectorClock():
    filenode_(), filename_(), clock_()
{
}

VectorClock::VectorClock(const QString &strvectorclock):
    filenode_(), filename_(), clock_()
{
    build(strvectorclock);
}

VectorClock::VectorClock(const QString &filenode, const QString &filename):
    filenode_(filenode), filename_(filename), clock_()
{
}

VectorClock::VectorClock(const VectorClock &src):
    filenode_(src.filenode_), filename_(src.filename_), clock_(src.clock_)
{
}

VectorClock::~VectorClock()
{
}

void
VectorClock::build(const QString &strvectorclock)
{
    filenode_ = "";
    filename_ = "";
    clock_.clear();

    QStringList strvclist(strvectorclock.split("^"));

    QStringList filenodenv(strvclist.takeFirst().split("="));
    filenode_ = filenodenv[1];

    QStringList filenamenv(strvclist.takeFirst().split("="));
    filename_ = filenamenv[1];

    while (!strvclist.empty())
    {
        QStringList clocknv(strvclist.takeFirst().split("="));
        if (clocknv.empty() || clocknv[0] == "" || clocknv[1] == "") continue;
        clock_[clocknv[0]] = clocknv[1].toInt();
    }
}

// assignment
VectorClock &
VectorClock::operator=(const QString &strvectorclock)
{
    build(strvectorclock);
    return(*this);
}

VectorClock &
VectorClock::operator=(const VectorClock &rhs)
{
    if (this != &rhs)
    {
        filenode_ = rhs.filenode_;
        filename_ = rhs.filename_;
        clock_ = rhs.clock_;
    }
    return(*this);
}

// compare to vector clocks. remember if the file name
// is not in the list, then assume the version is 0.
VectorClock::Results
VectorClock::compare(const VectorClock &vcA, const VectorClock &vcB)
{
    ClockTypeConstIt vcAcit = vcA.clock_.constBegin();
    ClockTypeConstIt vcAcitend = vcA.clock_.constEnd();
    ClockTypeConstIt vcBcit = vcB.clock_.constBegin();
    ClockTypeConstIt vcBcitend = vcB.clock_.constEnd();

    Results result = Unknown;

    while (vcAcit != vcAcitend && vcBcit != vcBcitend)
    {
        if (vcAcit.key() == vcBcit.key())
        {
            // both vectors have an entry
            if (vcAcit.value() == vcBcit.value())
            {
                if (result == Unknown)
                    result = AEqualB;
            }
            else if (vcAcit.value() < vcBcit.value())
            {
                if (result == Unknown || result == AEqualB)
                {
                    result = APrefixOfB;
                }
                else if (result == BPrefixOfA)
                {
                    // changed direction!
                    return(Conflict);
                }
            }
            else
            {
                if (result == Unknown || result == AEqualB)
                {
                    result = BPrefixOfA;
                }
                else if (result == APrefixOfB)
                {
                    // changed direction!
                    return(Conflict);
                }
            }
            ++vcAcit;
            ++vcBcit;
        }
        else if (vcAcit.key() < vcBcit.key())
        {
            // vc B is missing an entry. 
            // the default version is zero.
            if (vcAcit.value() == 0)
            {
                if (result == Unknown)
                    result = AEqualB;
            }
            else
            {
                if (result == Unknown || result == AEqualB)
                {
                    result = BPrefixOfA;
                }
                else if (result == APrefixOfB)
                {
                    // changed direction!
                    return(Conflict);
                }
            }
            ++vcAcit;
        }
        else
        {
            // vc A is missing an entry. 
            // the default version is zero.
            if (vcBcit.value() == 0)
            {
                if (result == Unknown)
                    result = AEqualB;
            }
            else
            {
                if (result == Unknown || result == AEqualB)
                {
                    result = APrefixOfB;
                }
                else if (result == BPrefixOfA)
                {
                    // changed direction!
                    return(Conflict);
                }
            }
            ++vcBcit;
        }
    }

    if (vcAcit != vcAcitend)
    {
        do {
            if (vcAcit.value() == 0)
            {
                if (result == Unknown)
                    result = AEqualB;
            }
            else
            {
                if (result == Unknown || result == AEqualB)
                {
                    result = BPrefixOfA;
                }
                else if (result == APrefixOfB)
                {
                    // changed direction!
                    return(Conflict);
                }
            }
            ++vcAcit;
        } while (vcAcit != vcAcitend);
    }
    else if (vcBcit != vcBcitend)
    {
        do {
            if (vcBcit.value() == 0)
            {
                if (result == Unknown)
                    result = AEqualB;
            }
            else
            {
                if (result == Unknown || result == AEqualB)
                {
                    result = APrefixOfB;
                }
                else if (result == BPrefixOfA)
                {
                    // changed direction!
                    return(Conflict);
                }
            }
            ++vcBcit;
        } while (vcBcit != vcBcitend);
    }

    return(result);
}

// maintain vector
const QString &
VectorClock::getFileNode() const
{
    return(filenode_);
}

const QString &
VectorClock::getFileName() const
{
    return(filename_);
}

void 
VectorClock::incrementClock()
{
    incrementClock(filenode_);
}

void 
VectorClock::incrementClock(const QString &filenode)
{
    ClockTypeIt it = clock_.find(filenode);
    if (it != clock_.end())
    {
        clock_[filenode] += 1;
    }
    else
    {
        clock_[filenode] = 1;
    }
}

int 
VectorClock::getValue() const
{
    return(getValue(filenode_));
}

int 
VectorClock::getValue(const QString &filenode) const
{
    ClockTypeConstIt cit = clock_.find(filenode);
    if (cit != clock_.end())
    {
        return(cit.value());
    }
    else
    {
        return(0);
    }
}

// misc
const QString
VectorClock::toString() const
{
    QString output(QString("NODE=%1^NAME=%2^").arg(filenode_).arg(filename_));
    ClockTypeConstIt cit = clock_.constBegin();
    ClockTypeConstIt citend = clock_.constEnd();
    for ( ; cit != citend; ++cit)
    {
        output.append(QString("%1=%2^").arg(cit.key()).arg(cit.value()));
    }
    return(output);
}

// vector clock pair 
VectorClockPair::VectorClockPair():
   filenode_(), filename_(), modifications_(), synchronizations_()
{
}

VectorClockPair::VectorClockPair(
    const QString &filenode, const QString &filename):
   filenode_(filenode), filename_(filename), 
   modifications_(), synchronizations_()
{
}

VectorClockPair::VectorClockPair(const VectorClockPair &src):
   filenode_(src.filenode_), filename_(src.filename_), 
   modifications_(src.modifications_), 
   synchronizations_(src.synchronizations_)
{
}

VectorClockPair::~VectorClockPair()
{
}

VectorClockPair &
VectorClockPair::operator=(const VectorClockPair &rhs)
{
    if (this != &rhs)
    {
        filenode_ = rhs.filenode_;
        filename_ = rhs.filename_;
        modifications_ = rhs.modifications_;
        synchronizations_ = rhs.synchronizations_;
    }
    return(*this);
}

// vector clock pairs repository
VectorClockPairs::VectorClockPairs():
    pairs_()
{
}

VectorClockPairs::VectorClockPairs(const VectorClockPairs &src):
    pairs_(src.pairs_)
{
}

VectorClockPairs::~VectorClockPairs()
{
}

VectorClockPairs &
VectorClockPairs::operator=(const VectorClockPairs &rhs)
{
    if (this != &rhs)
    {
        pairs_ = rhs.pairs_;
    }
    return(*this);
}

void
VectorClockPairs::insert(const QString &filename, const VectorClockPair &vcpair)
{
    pairs_[filename] = vcpair;
}

void
VectorClockPairs::retrieve(const QString &filename, VectorClockPair &vcpair, bool &found) const
{
    found = false;
    VectorClockPairsTypeConstIt cit = pairs_.find(filename);
    if (cit != pairs_.end())
    {
        vcpair = pairs_[filename];
        found = true;
    }
}

void
VectorClockPairs::update(const QString &filename, const VectorClockPair &vcpair, bool &found)
{
    found = false;
    VectorClockPairsTypeIt it = pairs_.find(filename);
    if (it != pairs_.end())
    {
        pairs_[filename] = vcpair;
        found = true;
    }
}

void
VectorClockPairs::remove(const QString &filename, bool &found)
{
    found = false;
    VectorClockPairsTypeIt it = pairs_.find(filename);
    if (it != pairs_.end())
    {
        pairs_.erase(it);
        found = true;
    }
}

