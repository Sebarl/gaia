#ifndef PLATFORMSPECIFIC_H
#define PLATFORMSPECIFIC_H
#include <time.h>
int getNumThreads();
unsigned long long getTimeSeed();

typedef timespec* tstamp;
tstamp getTimeStamp();
long getElapsedTimeMicroSeconds(tstamp startT,tstamp endT);

#endif
