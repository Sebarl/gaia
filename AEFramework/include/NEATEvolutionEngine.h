#ifndef NEATEVOLUTIONENGINE_H
#define NEATEVOLUTIONENGINE_H

#include <vector>

#include "ParallelEvolutionEngine.h"
#include "NEATSelectionStrategy.h"
#include "NEATGenome.h"
#include "NEATSpecies.h"
#include "NEATParams.h"
#include "InnovationDB.h"
#include "NEATCandidateFactory.h"
#include <list>

class NEATEvolutionEngine : public ParallelEvolutionEngine<NEATGenome> {
    protected:
        NEATSelectionStrategy* intraspeciesSelectionStrategy;
        NEATSelectionStrategy* interspeciesSelectionStrategy;
        std::list<NEATSpecies*> species;
        NEATParams params;
        double bestFitness;
        EvaluatedCandidate<NEATGenome>* bestIndividual;
        InnovationDB* innovationDB;
        int nextSpeciesId;

        NEATGenome* mate(EvaluatedCandidate<NEATGenome>* mum, EvaluatedCandidate<NEATGenome>* dad);
        void mutate(NEATGenome* network);
        std::vector<NEATGenome*> applyEvolutionScheme(std::vector<EvaluatedCandidate<NEATGenome>*> selectedCandidates);
        virtual std::vector<EvaluatedCandidate<NEATGenome>*> nextEvolutionStep(std::vector<EvaluatedCandidate<NEATGenome>*> evaluatedPopulation, int eliteCount, Random* rng);
    public:
        NEATEvolutionEngine(unsigned int populationSize,
                            unsigned int eliteCount,
                            NEATCandidateFactory* candidateFactory,
                            NEATSelectionStrategy* intraspeciesSelectionStrategy,
                            NEATSelectionStrategy* interspeciesSelectionStrategy,
                            FitnessEvaluator<NEATGenome>* fitnessEvaluator,
                            NEATParams params,
                            Random* rng,
                            unsigned int numThreads);

        NEATEvolutionEngine(unsigned int populationSize,
                            unsigned int eliteCount,
                            NEATCandidateFactory* candidateFactory,
                            NEATSelectionStrategy* selectionStrategy,
                            FitnessEvaluator<NEATGenome>* fitnessEvaluator,
                            NEATParams params,
                            Random* rng,
                            unsigned int numThreads);

        NEATEvolutionEngine(unsigned int populationSize,
                            NEATCandidateFactory* candidateFactory,
                            NEATSelectionStrategy* intraspeciesSelectionStrategy,
                            NEATSelectionStrategy* interspeciesSelectionStrategy,
                            FitnessEvaluator<NEATGenome>* fitnessEvaluator,
                            NEATParams params,
                            Random* rng,
                            unsigned int numThreads);

        NEATEvolutionEngine(unsigned int populationSize,
                            NEATCandidateFactory* candidateFactory,
                            NEATSelectionStrategy* intraspeciesSelectionStrategy,
                            NEATSelectionStrategy* interspeciesSelectionStrategy,
                            FitnessEvaluator<NEATGenome>* fitnessEvaluator,
                            NEATParams params,
                            Random* rng);

        virtual ~NEATEvolutionEngine();


};

#endif // NEATEVOLUTIONENGINE_H
