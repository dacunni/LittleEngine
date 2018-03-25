#ifndef __Timer__
#define __Timer__

#include <sys/time.h>

class Timer {
public:
    Timer() : running(false), valid(false) {}
    ~Timer() {}
    
    void start();
    void stop();
    double elapsed();
    
protected:
    
    struct timeval start_time;
    struct timeval end_time;
    bool running;
    bool valid;
};

double timeAsDouble( const struct timeval & tm );
double timeNowAsDouble();

#endif
