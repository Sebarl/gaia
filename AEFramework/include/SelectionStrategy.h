#ifndef SELECTION_STRATEGY_H
#define SELECTION_STRATEGY_H

#include <vector>
#include "Random.h"

template <class T>
class SelectionStrategy{

    /**
        Select the specified number of candidates from the population.
        Implementations may assume that the population is sorted in descending
        order according to fitness (so the fittest individual is the first item
        in the list).
     */
    public:
        SelectionStrategy(){};
        virtual ~SelectionStrategy(){};
        virtual std::vector<T*> select(std::vector<EvaluatedCandidate<T>*> population, bool naturalFitnessScores, int selectionSize, Random* rng)=0;
};

#endif
