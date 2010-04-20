#ifndef __GET_TIME_H__
#define __GET_TIME_H__
#include <sys/time.h>

struct PreciseTime {
    int sec;
    int usec;
    
    PreciseTime(int sec, int usec)
        :sec(sec), usec(usec) {
            while (usec > 1000000) {
                sec++;
                usec -= 1000000;
            }
        }
    
    static PreciseTime getTime() {
        struct timeval tv;
        gettimeofday(&tv, 0);
        return PreciseTime(tv.tv_sec, tv.tv_usec);
    }

    PreciseTime operator+(const PreciseTime& other) const {
        int s = sec + other.sec;
        int us = usec + other.usec;
        while (us > 1000000) {
            s++;
            us -= 1000000;
        }
        return PreciseTime(s, us);
    }

    PreciseTime operator-(const PreciseTime& other) const {
        int s = sec - other.sec;
        int us = usec - other.usec;
        while (us < 0) {
            us += 1000000;
            s--;
        }
        return PreciseTime(s, us);
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

    bool operator>(const PreciseTime& other) const {
        if (sec > other.sec) {
            return true;
        } else if (sec < other.sec) {
            return false;
        } else {
            return usec > other.usec;
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
