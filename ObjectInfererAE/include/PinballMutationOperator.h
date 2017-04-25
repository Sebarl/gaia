#ifndef PINBALLMUTATIONOPERATOR_H
#define PINBALLMUTATIONOPERATOR_H

#include "Solution.h"
#include "EvolutionaryOperator.h"

class PinballMutationOperator : public EvolutionaryOperator<Solution>{
    private:
        float probability;
    public:
        PinballMutationOperator(float probability);
        virtual ~PinballMutationOperator();
        virtual std::vector<Solution*> apply(std::vector<Solution*> selectedCandidates, Random* rng);
};

#endif // MUTATIONOPERATOR_H
