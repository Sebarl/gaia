#ifndef AVERAGECROSSOVEROPERATOR_H
#define AVERAGECROSSOVEROPERATOR_H

#include "Solution.h"
#include "AbstractCrossover.h"

class AverageCrossoverOperator : public EvolutionaryOperator<Solution>{
    public:
        AverageCrossoverOperator(float crossoverProbability);
        virtual ~AverageCrossoverOperator();

	    virtual std::vector<Solution*> apply(std::vector<Solution*> selectedCandidates, Random* rng);

	private:
        float probability;

		void mate(Solution* parent1, Solution* parent2, Random* rng);

};

#endif // AVERAGEOPERATOR_H
