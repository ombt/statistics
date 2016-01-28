#ifndef __VECTOR_CLOCK_H
#define __VECTOR_CLOCK_H

// vector clock pair - one clock to track modifications and
// a second clock to track synchronizations.

#include <QString>
#include <QMap>
#include <QHash>
#include <QStringList>

// forward declarations
class VectorClockPair;

class VectorClock
{
public:
    enum Results
    {
        AEqualB = 0,
        APrefixOfB = 1,
        BPrefixOfA = 2,
        Conflict = 3,
        Unknown = 4,
    };

    // ctors and dtor
    VectorClock();
    VectorClock(const QString &strvectorclock);
    VectorClock(const QString &filenode, const QString &filename);
    VectorClock(const VectorClock &src);
    ~VectorClock();

    // init function
    void build(const QString &strvectorclock);

    // assignment
    VectorClock &operator=(const QString &strvectorclock);
    VectorClock &operator=(const VectorClock &rhs);

    // vector clocks are partial orderings, strict less-than
    // or greater-than relations may *not* exist between two clocks, ie,
    // collision occurred. in the following functions, *this is vcA.
    inline bool operator==(const VectorClock &vcB) const
        { return(compare(*this, vcB) == AEqualB); }
    inline bool operator!=(const VectorClock &vcB) const
        { return(compare(*this, vcB) != AEqualB); }
    inline bool operator<(const VectorClock &vcB) const
        { return(compare(*this, vcB) == APrefixOfB); }
    inline bool operator>(const VectorClock &vcB) const
        { return(compare(*this, vcB) == BPrefixOfA); }
    inline bool operator<=(const VectorClock &vcB) const
    {
        Results result = compare(*this, vcB);
        return(result == APrefixOfB || result == AEqualB);
    }
    inline bool operator>=(const VectorClock &vcB) const
    {
        Results result = compare(*this, vcB);
        return(result == BPrefixOfA || result == AEqualB);
    }

    // maintain vector
    const QString &getFileNode() const;
    const QString &getFileName() const;
    void incrementClock();
    void incrementClock(const QString &filenode);
    int getValue() const;
    int getValue(const QString &filenode) const;

    // misc
    const QString toString() const;

private:
    // internal use only
    static Results compare(const VectorClock &vcA, const VectorClock &vcB);

private:
    typedef QMap<QString, int> ClockType;
    typedef ClockType::iterator ClockTypeIt;
    typedef ClockType::const_iterator ClockTypeConstIt;

    QString filenode_;
    QString filename_;
    ClockType clock_; // QString filenode, int counter
};

class VectorClockPair
{
public:
    // ctors and dtor
    VectorClockPair();
    VectorClockPair(const QString &filenode, const QString &filename);
    VectorClockPair(const VectorClockPair &src);
    ~VectorClockPair();

    // assignment
    VectorClockPair &operator=(const VectorClockPair &rhs);

    // maintain vector pair
    inline const QString &getFileNode() const
        { return(filenode_); }
    inline const QString &getFileName() const
        { return(filename_); }
    inline void incrModClock()
        { modifications_.incrementClock(filenode_); }
    inline void incrModClock(const QString &filenode)
        { modifications_.incrementClock(filenode); }
    inline void incrSyncClock()
        { synchronizations_.incrementClock(filenode_); }
    inline void incrSyncClock(const QString &filenode)
        { synchronizations_.incrementClock(filenode); }
    inline int getModValue() const
        { return(modifications_.getValue(filenode_)); }
    inline int getModValue(const QString &filenode) const
        { return(modifications_.getValue(filenode)); }
    inline int getSyncValue() const
        { return(synchronizations_.getValue(filenode_)); }
    inline int getSyncValue(const QString &filenode) const
        { return(synchronizations_.getValue(filenode)); }

    // comparisons
    inline bool operator==(const VectorClockPair &rhs) const
        { return(filename_ == rhs.filename_); }
    inline bool operator!=(const VectorClockPair &rhs) const
        { return(filename_ != rhs.filename_); }
    inline bool operator<(const VectorClockPair &rhs) const
        { return(filename_ < rhs.filename_); }
    inline bool operator>(const VectorClockPair &rhs) const
        { return(filename_ > rhs.filename_); }
    inline bool operator<=(const VectorClockPair &rhs) const
        { return(filename_ <= rhs.filename_); }
    inline bool operator>=(const VectorClockPair &rhs) const
        { return(filename_ >= rhs.filename_); }

private:
    QString filenode_;
    QString filename_;
    VectorClock modifications_;
    VectorClock synchronizations_;
};

class VectorClockPairs
{
public:
    // ctors and dtor
    VectorClockPairs();
    VectorClockPairs(const VectorClockPairs &src);
    ~VectorClockPairs();

    // assignment
    VectorClockPairs &operator=(const VectorClockPairs &rhs);

    // maintain container
    void insert(const QString &filename, const VectorClockPair &pair);
    void retrieve(const QString &filename, VectorClockPair &pair, bool &found) const;
    void update(const QString &filename, const VectorClockPair &pair, bool &found);
    void remove(const QString &filename, bool &found);

private:
    typedef QHash<QString, VectorClockPair> VectorClockPairsType;
    typedef VectorClockPairsType::const_iterator VectorClockPairsTypeConstIt;
    typedef VectorClockPairsType::iterator VectorClockPairsTypeIt;
    VectorClockPairsType pairs_;
};

#endif
