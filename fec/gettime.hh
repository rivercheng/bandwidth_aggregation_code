#ifndef __GET_TIME_H__
#define __GET_TIME_H__
#include <sys/time.h>
struct PreciseTime {
    unsigned int sec;
    unsigned int usec;
    PreciseTime(unsigned int sec, unsigned int usec)
        :sec(sec), usec(usec) {};
    static PreciseTime getTime() {
        struct timeval tv;
        gettimeofday(&tv, 0);
        return PreciseTime(tv.tv_sec, tv.tv_usec);
    }
    bool operator<(const PreciseTime& other) const {
        if (sec < other.sec) {
            return true;
        } else if (sec > other.sec) {
            return false;
        } else {
            return usec < other.usec;
        }
    }

    bool operator<=(const PreciseTime& other) const {
        if (sec < other.sec) {
            return true;
        } else if (sec > other.sec) {
            return false;
        } else {
            return usec <= other.usec;
        }
    }
    bool operator==(const PreciseTime& other) const {
        return (sec == other.sec) && (usec == other.usec);
    }
};

#endif
