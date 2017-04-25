#ifndef CANDIDATE_FACTORY_H
#define CANDIDATE_FACTORY_H

#include <vector>
#include "Random.h"
#include "GenericException.h"

template <class T>
class CandidateFactory{
    public:
        CandidateFactory(){}
        virtual ~CandidateFactory(){}

        /**
            Creates an initial population of candidates.
         */
        virtual std::vector<T*> generateInitialPopulation(unsigned int populationSize, Random* rng){
            std::vector<T*> population;
            for (unsigned int i = 0; i < populationSize; i++){
                population.push_back(generateRandomCandidate(rng));
            }
            return population;
        }

        /**
            Creates an initial population of candidates using seedCandidates as part of it.
         */
        virtual std::vector<T*> generateInitialPopulation(unsigned int populationSize, std::vector<T*> seedCandidates, Random* rng){
            if (seedCandidates.size() > populationSize){
                throw GenericException("Too many seed candidates for specified population size.");
            }
            std::vector<T*> population;
            for (unsigned int i = 0; i < seedCandidates.size(); i++){
                population.push_back(seedCandidates[i]);
            }
            for (unsigned int i = seedCandidates.size(); i < populationSize; i++){
                population.push_back(generateRandomCandidate(rng));
            }
            return population;
        }

        /**
         * Randomly create a single candidate solution.
         */
        virtual T* generateRandomCandidate(Random* rng) =0;
};

#endif
