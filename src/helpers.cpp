#include "helpers.h"
#include <time.h>
#include <errno.h>

namespace helpers
{
    timer::timer()
        : m_valid(false)
    {
    }

    timer::~timer()
    {
        if (m_valid)
        {
            abort();
        }
    }

    uint64_t timer::ts2nSec(struct timespec *ts)
    {
        uint64_t value = ts->tv_sec * 1000000000 + ts->tv_nsec;
        return value;
    }

    void timer::nsec2Ts(timespec *ts, uint64_t nsec)
    {
        ts->tv_sec = nsec / 1000000000;
        ts->tv_nsec = nsec % 1000000000;
    }

    bool timer::initialize(uint32_t nsPeriod)
    {
        if (m_valid)
        {
            // already initialized
            return false;
        }

        m_nsPeriod = nsPeriod;

        struct timespec currentTime_ts;
        clock_gettime(CLOCK_MONOTONIC, &currentTime_ts);
        m_nextWakeupNs = ts2nSec(&currentTime_ts);

        calcNextWakeup(0);

        m_valid = true;

        return true;
    }

    bool timer::abort()
    {
        return true;
    }

    void timer::calcNextWakeup(uint32_t numOverflows)
    {
        m_nextWakeupNs = m_nextWakeupNs + m_nsPeriod * (numOverflows + 1);
    }

    int timer::wait()
    {
        if (!m_valid)
        {
            return -1;
        }
        struct timespec nextWakup_ts;
        nsec2Ts(&nextWakup_ts, m_nextWakeupNs);

        while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &nextWakup_ts, NULL) == EINTR)
            ;
        struct timespec currentTime_ts;
        clock_gettime(CLOCK_MONOTONIC, &currentTime_ts);

        int64_t currentTimeNs = ts2nSec(&currentTime_ts);

        int64_t diffTime = currentTimeNs - m_nextWakeupNs;
        if (diffTime < 0)
        {
            diffTime = diffTime * -1;
        }
        uint32_t overflows = diffTime / m_nsPeriod;

        calcNextWakeup(overflows);

        return overflows;
    }
}