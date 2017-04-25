#ifndef _AI_GUI
#define _AI_GUI
#include <string>
#ifdef __GNUC__
#include <xmmintrin.h>
#endif
typedef unsigned char uint8;

extern "C" uint8 runAI(uint8* RAM);
extern "C" void printStatistics();
extern "C" int initializeParams(std::string s);

#endif
