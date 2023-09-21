#include <time.h>
#include <inttypes.h>

namespace helpers
{
   class timer
    {
    public:
        timer();
        virtual ~timer();

        bool initialize(uint32_t usPeriod);
        bool abort();
        int wait();

    protected:
        uint64_t ts2nSec(struct timespec*);
        void nsec2Ts(struct timespec*, uint64_t nsec);
        uint64_t m_nextWakeupNs;
        uint64_t m_nsPeriod;
        void calcNextWakeup(uint32_t numOverflows);
        bool m_valid;

    };

}