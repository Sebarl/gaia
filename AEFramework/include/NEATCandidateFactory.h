#ifndef NEATCANDIDATEFACTORY_H
#define NEATCANDIDATEFACTORY_H

#include "NEATGenome.h"
#include "Random.h"
#include "InnovationDB.h"
#include "CandidateFactory.h"

class NEATCandidateFactory : public CandidateFactory<NEATGenome>{
    protected:
    	InnovationDB* innovationDB;
    	unsigned int numInputs;
    	unsigned int numOutputs;
    	bool useOtherActivationFunctions;
        ActivationFunctionType outputFunctionType;

        ActivationFunction* getNewOuputActivationFunction();

    public:
        NEATCandidateFactory(unsigned int numInputs, unsigned int numOutputs, ActivationFunctionType outputFunctionType, bool useOtherActivationFunctions);
        virtual ~NEATCandidateFactory();
		virtual std::vector<NEATGenome*> generateInitialPopulation(unsigned int populationSize,  Random* rng);
        virtual std::vector<NEATGenome*> generateInitialPopulation(unsigned int populationSize, std::vector<NEATGenome*> seedCandidates, Random* rng);
        virtual NEATGenome* generateRandomCandidate(Random* rng);
        virtual void setInnovationDB(InnovationDB* iDB);
};

#endif // NEATCANDIDATEFACTORY_H
