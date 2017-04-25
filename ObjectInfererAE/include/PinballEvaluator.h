#ifndef PINBALL_EVALUATOR_H
#define PINBALL_EVALUATOR_H

#include "Parameters.h"
#include "FitnessEvaluator.h"
#include "Solution.h"

class PinballEvaluator : public FitnessEvaluator<Solution>{
    public:
        PinballEvaluator(Parameters* params, int islandIndex);
        ~PinballEvaluator();
        double getFitness(const int index, vector<Solution*> population);
        bool isNatural();
		float* evalFrames(const float* video, unsigned int cantFrames, std::vector<float>* weights);

    private:
        int islandIndex;
        Parameters* params;
};

#endif
