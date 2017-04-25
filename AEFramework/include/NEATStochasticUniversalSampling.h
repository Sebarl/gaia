#ifndef NEATSTOCHASTICUNIVERSALSAMPLING_H
#define NEATSTOCHASTICUNIVERSALSAMPLING_H

#include <vector>
#include "NEATSelectionStrategy.h"

class NEATStochasticUniversalSampling : public NEATSelectionStrategy{
    private:
         double getAdjustedFitness(double rawFitness, bool naturalFitness);

    public:
        NEATStochasticUniversalSampling();
        virtual ~NEATStochasticUniversalSampling();

        std::vector<EvaluatedCandidate<NEATGenome>*> select(std::vector<EvaluatedCandidate<NEATGenome>*> population, bool naturalFitnessScores, int selectionSize, Random* rng);
};

#endif // NEATSTOCHASTICUNIVERSALSAMPLING_H
