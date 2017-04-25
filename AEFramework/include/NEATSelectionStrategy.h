#ifndef NEAT_SELECTION_STRATEGY_H
#define NEAT_SELECTION_STRATEGY_H

#include <vector>
#include "Random.h"
#include "NEATGenome.h"
#include "EvaluatedCandidate.h"

class NEATSelectionStrategy{

    /**
        Select the specified number of candidates from the population.
        Implementations may assume that the population is sorted in descending
        order according to fitness (so the fittest individual is the first item
        in the list).
     */
    public:
        NEATSelectionStrategy(){};
        virtual ~NEATSelectionStrategy(){};
        virtual std::vector<EvaluatedCandidate<NEATGenome>*> select(std::vector<EvaluatedCandidate<NEATGenome>*> population, bool naturalFitnessScores, int selectionSize, Random* rng)=0;
};

#endif // NEAT_SELECTION_STRATEGY_H
