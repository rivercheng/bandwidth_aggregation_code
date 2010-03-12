#ifndef __GET_TIME_H__
#define __GET_TIME_H__
#include <sys/time.h>
struct PreciseTime {
    unsigned int sec;
    unsigned int usec;
    PreciseTime(unsigned int sec, unsigned int usec)
        :sec(sec), usec(usec) {};
};

PreciseTime getTime() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return PreciseTime(tv.tv_sec, tv.tv_usec);
}
#endif
