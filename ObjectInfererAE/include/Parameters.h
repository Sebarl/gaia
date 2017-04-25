#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <vector>

#define uint8 unsigned char

struct Parameters{
    int generations;
    int populationSize;
    unsigned int numSeeds;
    double crossoverProbability;
    double mutationProbability;
    int elitismPercentage;
    int islandCount;
    int epochLength;
    int migrationCount;
    int frameCount;
    int instance;
    int evolutiveProcessID;
    int islandThreads;
    int islandEngineThreads;
    bool enableIslands;
    bool enableMic;
    int RAMCount;
    int* RAMMap;
    char* ROM;
    unsigned int goodVidsCount;
    unsigned int badVidsCount;
    char** goodVids;
    char** badVids;
    std::vector<float>* avgGoodVideo;
    std::vector<float>* avgBadVideo;
    ~Parameters(){
        delete [] RAMMap;
        for(unsigned int i=0; i<goodVidsCount;i++){
            delete [] goodVids[i];
        }
        for(unsigned int i=0; i<badVidsCount;i++){
            delete [] badVids[i];
        }

        delete [] goodVids;
        delete [] badVids;
        delete avgGoodVideo;
        delete avgBadVideo;
    }
};

#endif // PARAMETERS_H
