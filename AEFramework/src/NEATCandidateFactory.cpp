#include "NEATCandidateFactory.h"
#include "Identity.h"
#include "Sigmoid.h"
#include "CheapSigmoid.h"
#include "Tanh.h"
#include "Gaussian.h"
#include "ReLU.h"

NEATCandidateFactory::NEATCandidateFactory(unsigned int numInputs, unsigned int numOutputs, ActivationFunctionType outputFunctionType, bool useOtherActivationFunctions){
	this->numInputs = numInputs;
	this->numOutputs = numOutputs;
	this->outputFunctionType = outputFunctionType;
	this->useOtherActivationFunctions=useOtherActivationFunctions;
}

NEATCandidateFactory::~NEATCandidateFactory(){}

std::vector<NEATGenome*> NEATCandidateFactory::generateInitialPopulation(unsigned int populationSize, Random* rng){
	std::vector<NEATGenome*> population;
	for (unsigned int i = 0; i < populationSize; i++){
		population.push_back(generateRandomCandidate(rng));
	}
	innovationDB->initialize(numInputs, numOutputs);
	return population;
}

void NEATCandidateFactory::setInnovationDB(InnovationDB* iDB){
    this->innovationDB = iDB;
}

std::vector<NEATGenome*> NEATCandidateFactory::generateInitialPopulation(unsigned int populationSize, std::vector<NEATGenome*> seedCandidates, Random* rng){
	// Seeds are not supported as the database could be left in an inconsistent state.

	/*
	if (seedCandidates.size() > populationSize){
        throw GenericException("Too many seed candidates for specified population size.");
    }
    std::vector<NEATGenome*> population = generateInitialPopulation(populationSize - seedCandidates.size(), rng);
    for (unsigned int i = 0; i < seedCandidates.size(); i++){
        population.push_back(seedCandidates[i]);
        innovationDB.update(seedCandidates[i]->neurons, seedCandidates[i]->links);
    }
    return population;
    */

    return generateInitialPopulation(populationSize, rng);
}

ActivationFunction* NEATCandidateFactory::getNewOuputActivationFunction(){
    switch(this->outputFunctionType){
        case IDENTITY:
            return new Identity();
            break;
        case SIGMOID:
            return new Sigmoid();
            break;
        case TANH:
            return new Tanh();
            break;
        case CHEAPSIGMOID:
            return new CheapSigmoid();
            break;
        case GAUSSIAN:
            return new Gaussian();
            break;
        case RELU:
            return new ReLU();
            break;
        default:
            return new Tanh();
            break;
    }
}

NEATGenome* NEATCandidateFactory::generateRandomCandidate(Random* rng){
	std::vector<NEATNeuron*> neurons;
    std::vector<NEATLink*> links;

	// Create the input neurons
	double inputRowSlice = 0.8/(double)(numInputs);
	unsigned int i = 0;
	for (i = 0; i < numInputs; i++) {
        if(useOtherActivationFunctions){
            neurons.push_back(new NEATNeuron(i, INPUT, 0, 0.1 + i*inputRowSlice, rng));
        }
        else{
            neurons.push_back(new NEATNeuron(i, INPUT, new Sigmoid(rng), 0, 0.1 + i*inputRowSlice));
        }
	}

	// Create the bias
	neurons.push_back(new NEATNeuron(numInputs, BIAS, new Identity(), 0, 0.9));

	// Create the output neurons
	double outputRowSlice = 1/(double)(numOutputs+1);

	for (i = 1; i <= numOutputs; i++) {
        if(useOtherActivationFunctions){
            neurons.push_back(new NEATNeuron(i+numInputs, OUTPUT, getNewOuputActivationFunction(), 1, i*outputRowSlice));
        }
        else{
            neurons.push_back(new NEATNeuron(i+numInputs, OUTPUT, new Sigmoid(rng), 1, i*outputRowSlice));
        }
	}

	// Create the link genes, connect each input neuron to each output neuron and
	// assign a random weight -1 < w < 1
	for (i = 0; i < numInputs + 1; i++) {
		for (unsigned int j = 0; j < numOutputs; j++) {
			links.push_back(
				new NEATLink(neurons[i]->id,
                         neurons[numInputs + j + 1]->id,
						 2.0*rng->nextFloat() - 1.0,
						 true,
						 numOutputs*i + j)
				);
		}
	}
	return new NEATGenome(numInputs, neurons, links);
}
