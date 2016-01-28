//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_NANO_SEC_TIME_H
#define __OMBT_NANO_SEC_TIME_H

// take nano-second times and basic math

// os headers
#include <time.h>
#include <string.h>
#include <assert.h>

namespace ombt {

// high-res timer
class NanoSecTime
{
public:
    // data types
    enum TimerType
    {
        TT_Realtime = CLOCK_REALTIME,
        TT_Monotonic = CLOCK_MONOTONIC,
        TT_Process_Cputime_Id = CLOCK_PROCESS_CPUTIME_ID,
        TT_Thread_Cputime_Id = CLOCK_THREAD_CPUTIME_ID,
    };

public:
    // ctors and dtor
    NanoSecTime(TimerType tt = TT_Monotonic);
    NanoSecTime(long sec, long nsec, TimerType tt = TT_Monotonic);
    NanoSecTime(const NanoSecTime &src);
    ~NanoSecTime();

    // assignment
    NanoSecTime &operator=(const NanoSecTime &rhs);
    NanoSecTime &set(long sec, long nsec, TimerType tt = TT_Monotonic);

    // snapshot for reuse
    void snapshot(TimerType tt = TT_Monotonic);

    // stop-watch mode
    void start(TimerType tt = TT_Monotonic);
    void stop(TimerType tt = TT_Monotonic);

    // basic arithmetic
    NanoSecTime &operator+=(const NanoSecTime &rhs);
    NanoSecTime &operator-=(const NanoSecTime &rhs);
    NanoSecTime operator+(const NanoSecTime &rhs) const;
    NanoSecTime operator-(const NanoSecTime &rhs) const;

    // comparison
    bool operator<(const NanoSecTime &rhs) const;
    bool operator<=(const NanoSecTime &rhs) const;
    bool operator>(const NanoSecTime &rhs) const;
    bool operator>=(const NanoSecTime &rhs) const;
    bool operator==(const NanoSecTime &rhs) const;
    bool operator!=(const NanoSecTime &rhs) const;

    // output
    friend std::ostream &operator<<(std::ostream &os, const NanoSecTime &o);

public:
    // exposed data
    TimerType _timer_type;
    long      _seconds;
    long      _nanoseconds;

private:
    // internal data
    clockid_t       _id;
    struct timespec _tp;
};

}

#endif

