#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "NEATParams.h"
#include "ActivationFunction.h"

struct Parameters{
    int generations;
    int populationSize;
    int eliteCount;
    int frameCount;
    int evolutiveProcessID;
    int numThreads;
    char* ramPath;
    char* prefix;
    char* weightsPath;
    char* ROM;
    NEATParams config;
    ActivationFunctionType outputFunctionType;

    ~Parameters() {
        delete [] prefix;
    }
};

#endif // PARAMETERS_H
