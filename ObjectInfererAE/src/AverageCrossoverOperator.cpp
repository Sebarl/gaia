#include "AverageCrossoverOperator.h"
#include <stdlib.h>
#include <string.h>

AverageCrossoverOperator::AverageCrossoverOperator(float crossoverProbability){
    if (crossoverProbability >1 || crossoverProbability < 0){
        throw GenericException("Crossover probability must be between 0.0 and 1.0.");
    }
    this->probability = crossoverProbability;
}

AverageCrossoverOperator::~AverageCrossoverOperator(){}

void AverageCrossoverOperator::mate(Solution* parent1, Solution* parent2, Random* rng){
    std::vector<Solution*> result;
    std::vector<float>* p1 = parent1->getSolution();
    std::vector<float>* p2 = parent2->getSolution();
	
	float newP1;
	for(unsigned int i = 0; i < p1->size(); i++){
		newP1 = 0.75*(*p1)[i] + 0.25*(*p2)[i];
		(*p2)[i] = 0.25*(*p1)[i] + 0.75*(*p2)[i];
		(*p1)[i] = newP1;
	}
}

 /**
 * Applies the cross-over operation to the selected candidates.  Pairs of
 * candidates are chosen randomly and subjected to cross-over to produce
 * a pair of offspring candidates.
 * @param selectedCandidates The evolved individuals that have survived to
 * be eligible to reproduce.
 * @param rng A source of randomness used to determine the location of
 * cross-over points.
 * @return The combined set of evolved offspring generated by applying
 * cross-over to the the selected candidates.
 */
std::vector<Solution*> AverageCrossoverOperator::apply(std::vector<Solution*> selectedCandidates, Random* rng){
    // Shuffle the collection before applying each operation so that the
    // evolution is not influenced by any ordering artifacts from previous
    // operations.

    shufflePopulation(selectedCandidates, rng);

    std::vector<Solution*> result;

    unsigned int i=0;

    while (i<selectedCandidates.size()){

        Solution* parent1 = selectedCandidates[i++];

        if (i<selectedCandidates.size()){
            Solution* parent2 = selectedCandidates[i++];
            // Randomly decide (according to the current cross-over probability)
            // whether to perform cross-over for these 2 parents.

            if (rng->nextFloat() < probability){
                mate(parent1, parent2, rng);

            }
            // If there is no cross-over to perform, just add the parents to the
            // results unaltered.
            result.push_back(parent1);
            result.push_back(parent2);

        }
        else {
            // If we have an odd number of selected candidates, we can't pair up
            // the last one so just leave it unmodified.
            result.push_back(parent1);
        }
    }
    return result;
}