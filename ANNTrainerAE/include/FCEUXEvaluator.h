#ifndef FCEUX_EVALUATOR_H
#define FCEUX_EVALUATOR_H

#include "Parameters.h"
#include "FitnessEvaluator.h"
#include "NEATGenome.h"

class FCEUXEvaluator : public FitnessEvaluator<NEATGenome>{
    public:
        FCEUXEvaluator(Parameters* params, int islandIndex);
        ~FCEUXEvaluator();
        double getFitness(const int index, std::vector<NEATGenome*> population);
        bool isNatural();

    private:
        int islandIndex;
        Parameters* params;

        int serializeChromosome(NEATGenome* sol, char* buff, Parameters* params);
};

#endif
