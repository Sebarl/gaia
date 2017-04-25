#ifndef _AI
#define _AI
#include <string>
#ifdef __GNUC__
#include <xmmintrin.h>
#endif
typedef unsigned char uint8;

extern "C" uint8 runAI(uint8* RAM, bool& exitNow);
extern "C" void printStatistics();
extern "C" int initializeParams(std::string s);

#endif
