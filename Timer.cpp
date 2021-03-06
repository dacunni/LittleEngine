#include <stdlib.h>
#include "Timer.h"

double timeAsDouble( const struct timeval & tm)
{
    return (double) tm.tv_sec + (double) tm.tv_usec * 1.0e-6;
}

double timeNowAsDouble()
{
    struct timeval tm = {};
    gettimeofday( &tm, NULL);
    return timeAsDouble( tm);
}

void Timer::start()
{
    gettimeofday( &start_time, NULL);
    running = true;
    valid = true;
}

void Timer::stop()
{
    gettimeofday( &end_time, NULL);
    running = false;
}

double Timer::elapsed()
{
    if( !valid) {
        return 0.0;
    }
    
    if( running) {
        gettimeofday( &end_time, NULL);
    }
    
    return timeAsDouble( end_time) - timeAsDouble( start_time);
}
