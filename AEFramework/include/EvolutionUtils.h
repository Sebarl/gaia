#ifndef EVOLUTION_UTILS_H
#define EVOLUTION_UTILS_H

#include <vector>
#include "PopulationData.h"
#include "Random.h"
#include "EvaluatedCandidate.h"
#include "TerminationCondition.h"
#include <algorithm>
#include <math.h>

template<class T>
bool natural(EvaluatedCandidate<T>* i,EvaluatedCandidate<T>* j) { return (i->fitness>j->fitness); }

template<class T>
bool notNatural(EvaluatedCandidate<T>* i,EvaluatedCandidate<T>* j) { return (i->fitness<j->fitness); }

template<class T>
void sortEvaluatedPopulation(std::vector<EvaluatedCandidate<T>*> &evaluatedPopulation, bool isNatural){
    if(isNatural){
        std::sort (evaluatedPopulation.begin(), evaluatedPopulation.end(), natural<T>);
    }
    else{
        std::sort (evaluatedPopulation.begin(), evaluatedPopulation.end(), notNatural<T>);
    }
}



template<class T>
std::vector<TerminationCondition<T>*> shouldContinue(PopulationData<T>* data, std::vector<TerminationCondition<T>*> conditions){
    std::vector<TerminationCondition<T>*> result;
    for(unsigned int i=0; i< conditions.size(); i++){
        if(conditions[i]->shouldTerminate(data)){
            result.push_back(conditions[i]);
        }
    }
    return result;
}

template<class T>
void shufflePopulation(std::vector<T*> &population, Random* rng){
    unsigned int j;
    T* temp;
    for(unsigned int i=population.size();i>1;i--){
        j = rng->nextInt(i);
        temp = population[j];
        population[j] = population[i-1];
        population[i-1] = temp;
    }
}

#endif
