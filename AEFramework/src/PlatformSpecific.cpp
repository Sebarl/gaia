#include "PlatformSpecific.h"

#ifdef _WIN32
#include <windows.h>
int getNumThreads(){
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );
    return sysinfo.dwNumberOfProcessors;
}

long getTimeSeed(){
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return ft.dwLowDateTime;
}

#endif // _WIN32

#ifdef linux
#include <unistd.h>
#include <time.h>

int getNumThreads(){
    return sysconf( _SC_NPROCESSORS_ONLN );
}

unsigned long long getTimeSeed(){
     unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

tstamp getTimeStamp(){
    struct timespec* tp = new timespec;
    clock_gettime(CLOCK_MONOTONIC, tp);
    return tp;
}
long getElapsedTimeMicroSeconds(tstamp startT,tstamp endT){
    return (endT->tv_sec - startT->tv_sec) * 1e6 + (endT->tv_nsec - startT->tv_nsec) / 1e3;;
}
#endif
