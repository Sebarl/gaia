#ifndef FCEUX_EVALUATOR_H
#define FCEUX_EVALUATOR_H

#include "Parameters.h"
#include "FitnessEvaluator.h"
#include "NEATGenome.h"

class XOREvaluator : public FitnessEvaluator<NEATGenome>{
    public:
        XOREvaluator(Parameters* params, int islandIndex);
        ~XOREvaluator();
        double getFitness(const int index, std::vector<NEATGenome*> population);
        bool isNatural();

    private:
        int islandIndex;
        Parameters* params;

        int serializeChromosome(NEATGenome* sol, char* buff, Parameters* params);
};

#endif
