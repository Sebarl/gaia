#include "InnovationDB.h"
#include "Sigmoid.h"

InnovationDB::InnovationDB() {
	this->nextNeuronId = 0;
}

InnovationDB::~InnovationDB(){}

void InnovationDB::initialize(unsigned int numInputs, unsigned int numOutputs){
	this->neurons.clear();
	this->links.clear();
	this->nextNeuronId = numInputs + numOutputs + 1;

	// Create the link genes:
	// connect each input and bias neuron to each output neuron

	for (unsigned int i = 0; i < numInputs + 1; i++) {
		for (unsigned int j = 0; j < numOutputs; j++) {
			LinkInnovation newInnovation;
			newInnovation.neuronIn = i;
			newInnovation.neuronIn = j;
			links.push_back(newInnovation);
		}
	}

}


// TODO: use a more efficient collection than vector
int InnovationDB::checkNeuronInnovation(int neuronIn, int neuronOut){
	for(unsigned int i = 0; i < neurons.size(); i++){
		if(neurons[i].neuronIn == neuronIn && neurons[i].neuronOut == neuronOut) {
			return neurons[i].neuronId;
		}
	}
	return -1;
}

int InnovationDB::checkLinkInnovation(int neuronIn, int neuronOut){
	for(unsigned int i = 0; i < links.size(); i++){
		if(links[i].neuronIn == neuronIn && links[i].neuronOut == neuronOut) {
			return i;
		}
	}
	return -1;
}

int InnovationDB::createNeuronInnovation(int neuronIn, int neuronOut){
	NeuronInnovation newInnovation;
	newInnovation.neuronIn = neuronIn;
	newInnovation.neuronOut = neuronOut;
	newInnovation.neuronId = this->nextNeuronId++;
	this->neurons.push_back(newInnovation);
	return newInnovation.neuronId;
}

int InnovationDB::createLinkInnovation(int neuronIn, int neuronOut){
	LinkInnovation newInnovation;
	newInnovation.neuronIn = neuronIn;
	newInnovation.neuronOut = neuronOut;
	this->links.push_back(newInnovation);
	return this->links.size() - 1;
}
