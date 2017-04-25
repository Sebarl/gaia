#ifndef PINBALLCROSSOVEROPERATOR_H
#define PINBALLCROSSOVEROPERATOR_H

#include "Solution.h"
#include "AbstractCrossover.h"

class PinballCrossoverOperator : public AbstractCrossover<Solution>{
    public:
        PinballCrossoverOperator(unsigned int crossoverPoints);
        PinballCrossoverOperator(unsigned int crossoverPoints, float crossoverProbability);
        virtual ~PinballCrossoverOperator();
        virtual void mate(Solution* parent1, Solution* parent2, Random* rng);
};

#endif // CROSSOVEROPERATOR_H
