#ifndef FINALSTATISTICS_H
#define FINALSTATISTICS_H

#include "PopulationData.h"
#include <vector>
#include "EvaluatedCandidate.h"

template <class T>
class FinalStatistics {
    public:
        PopulationData<T> *data;
        std::vector<EvaluatedCandidate<T>*> finalPopulation;
        FinalStatistics(){}
        virtual ~FinalStatistics(){
            delete data;
            for(unsigned int i=0; i<finalPopulation.size();i++){
                delete finalPopulation[i];
            }
        }
};

#endif // FINALSTATISTICS_H
