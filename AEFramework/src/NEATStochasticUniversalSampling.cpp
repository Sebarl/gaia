#include "NEATStochasticUniversalSampling.h"

#include <math.h>

double NEATStochasticUniversalSampling::getAdjustedFitness(double rawFitness, bool naturalFitness){
    if (naturalFitness){
        return rawFitness;
    }
    else{
        // If standardised fitness is zero we have found the best possible
        // solution.  The evolutionary algorithm should not be continuing
        // after finding it.
        return rawFitness == 0.0 ? INFINITY : 1.0 / rawFitness;
    }
}

NEATStochasticUniversalSampling::NEATStochasticUniversalSampling(){}

NEATStochasticUniversalSampling::~NEATStochasticUniversalSampling(){}

std::vector<EvaluatedCandidate<NEATGenome>*> NEATStochasticUniversalSampling::select(std::vector<EvaluatedCandidate<NEATGenome>*> population, bool naturalFitnessScores, int selectionSize, Random* rng){

    // Calculate the sum of all fitness values.
    double aggregateFitness = 0;
    for (unsigned int i = 0; i<population.size(); i++){
        aggregateFitness += getAdjustedFitness(population[i]->fitness, naturalFitnessScores);
    }

    std::vector<EvaluatedCandidate<NEATGenome>*> selection;
    // Pick a random offset between 0 and 1 as the starting point for selection.
    double startOffset = rng->nextDouble();
    double cumulativeExpectation = 0;
    int index = 0;
    for (unsigned int i = 0; i<population.size(); i++){
        // Calculate the number of times this candidate is expected to
        // be selected on average and add it to the cumulative total
        // of expected frequencies.
        cumulativeExpectation += getAdjustedFitness(population[i]->fitness, naturalFitnessScores) / aggregateFitness * selectionSize;

        // If f is the expected frequency, the candidate will be selected at
        // least as often as floor(f) and at most as often as ceil(f). The
        // actual count depends on the random starting offset.
        while (cumulativeExpectation > startOffset + index){
            selection.push_back(population[i]);
            index++;
        }
    }
    return selection;
}
