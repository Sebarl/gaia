#include "NEATGenome.h"

#include <cmath>
#include <stdio.h>

NEATGenome::NEATGenome(unsigned int numInputs) {
    this->numInputs = numInputs;
}

NEATGenome::NEATGenome(unsigned int numInputs, std::vector<NEATNeuron*> neurons, std::vector<NEATLink*> links) {
	this->numInputs = numInputs;
	this->links = links;
	this->neurons = neurons;
}

NEATGenome::~NEATGenome() {
	for (unsigned int i = 0; i < this->neurons.size(); i++){
		delete this->neurons[i];
	}
	this->neurons.clear();
	for (unsigned int i = 0; i < this->links.size(); i++){
		delete this->links[i];
	}
	this->links.clear();


	// NO POINTERS
	// YES POINTERS
}
/*
NEATGenome::NEATGenome(const NEATGenome& g) {

}

NEATGenome& NEATGenome::operator=(const NEATGenome& g) {
	//self assignment guard
	if (this != &g) {
        this->numInputs = g.numInputs;
		for(unsigned int i=0; i<g.neurons.size();i++){
            this->neurons.push_back(g.neurons[i]);
        }
        for(unsigned int i=0; i<g.links.size();i++){
                this->links.push_back(g.links[i]);
        }
		this->depth    = g.depth;
	}
	return *this;
}
*/
NEATGenome* NEATGenome::clone() const{
    std::vector<NEATNeuron*> copyNeurons;
    std::vector<NEATLink*> copyLinks;
	for(unsigned int i=0; i<this->neurons.size();i++){
            copyNeurons.push_back(this->neurons[i]->clone());
	}
	for(unsigned int i=0; i<this->links.size();i++){
            copyLinks.push_back(this->links[i]->clone());
	}

    NEATGenome* ng = new NEATGenome(this->numInputs,copyNeurons,copyLinks);
    ng->depth = this->depth;
    return ng;
}

int NEATGenome::getNeuronIndex(int neuronId) {
	for (unsigned int i = 0; i < neurons.size(); i++) {
		if (neurons[i]->id == neuronId) {
			return i;
		}
	}
	return -1;
}

int NEATGenome::getLinkIndex(int neuronIn, int neuronOut) {
	for (unsigned int i = 0; i < links.size(); i++) {
		if (links[i]->fromNeuron == neuronIn && links[i]->toNeuron == neuronOut) {
			return i;
		}
	}
	return -1;
}

unsigned int NEATGenome::numLinks() const{
	return this->links.size();
}

unsigned int NEATGenome::numNeurons() const{
	return this->neurons.size();
}

void NEATGenome::addNeuron(InnovationDB* innovationDB, unsigned int numTriesToFindLink, Random* rng){
    //check();
	unsigned int i = 0;
	unsigned int linkIndex;
	unsigned int fromNeuronId;
	unsigned int toNeuronId;
	int fromNeuronIndex;
	int toNeuronIndex;
	while(i < numTriesToFindLink) {
		linkIndex = rng->nextInt(this->links.size());
		if( this->links[linkIndex]->enabled &&
		   !this->links[linkIndex]->recurrent){
			fromNeuronId = this->links[linkIndex]->fromNeuron;
			fromNeuronIndex = getNeuronIndex(fromNeuronId);
			if(this->neurons[fromNeuronIndex]->type != BIAS){
				break;
			}
		}
		i++;
	}
	// If not suitable link was found, do not mutate de genome
	if(i == numTriesToFindLink){
		return;
	}

	// Disable the selected link
	this->links[linkIndex]->enabled = false;

	// Get id and index of toNeuron
	toNeuronId = this->links[linkIndex]->toNeuron;
	toNeuronIndex = getNeuronIndex(toNeuronId);

	// Calculate the position of the new neuron
	float newDepth = (this->neurons[fromNeuronIndex]->yCoord + this->neurons[toNeuronIndex]->yCoord) / 2.0;
	float newWidth = (this->neurons[fromNeuronIndex]->xCoord + this->neurons[toNeuronIndex]->xCoord) / 2.0;

	int newNeuronId = innovationDB->checkNeuronInnovation(fromNeuronId, toNeuronId);
	if(newNeuronId < 0 || getNeuronIndex(newNeuronId) >= 0){
		// If this neuron was not previously created by another individual,
		// or this network already has this neuron, then create a new innovation
		newNeuronId = innovationDB->createNeuronInnovation(fromNeuronId, toNeuronId);

		// Add the new neuron. It is already sorted as the neuronId is new thus, greater
		this->neurons.push_back(new NEATNeuron(newNeuronId, HIDDEN, newWidth, newDepth, rng));

		unsigned int firstLinkInnovationId = innovationDB->createLinkInnovation(fromNeuronId, newNeuronId);
		unsigned int secondLinkInnovationId = innovationDB->createLinkInnovation(newNeuronId, toNeuronId);

		this->links.push_back(new NEATLink(fromNeuronId, newNeuronId, 1.0, true, firstLinkInnovationId));
		this->links.push_back(new NEATLink(newNeuronId, toNeuronId, this->links[linkIndex]->weight, true, secondLinkInnovationId));

	} else {
		// Add the new neuron.
		this->neurons.push_back(new NEATNeuron(newNeuronId, HIDDEN, newWidth, newDepth, rng));

		// Since the neuron id is not new, the new neuron may not be sorted
		// Move it to the left until its id is greater or we reach the beginning of the vector
		i = this->neurons.size() - 1;
		while(i > 0 && this->neurons[i]->id < this->neurons[i-1]->id){
			std::swap(this->neurons[i], this->neurons[i-1]);
			i--;
		}

		// Since this innovation has already been created, the associated link innovations
		// must be present in the InnovationDB
		unsigned int firstLinkInnovationId = innovationDB->checkLinkInnovation(fromNeuronId, newNeuronId);
		unsigned int secondLinkInnovationId = innovationDB->checkLinkInnovation(newNeuronId, toNeuronId);

		this->links.push_back(new NEATLink(fromNeuronId, newNeuronId, 1.0, true, firstLinkInnovationId));

		// As before, perform an insert sort
		i = this->links.size() - 1;
		while(i > 0 && this->links[i]->innovationId < this->links[i-1]->innovationId){
			std::swap(this->links[i], this->links[i-1]);
			i--;
		}

		this->links.push_back(new NEATLink(newNeuronId, toNeuronId, this->links[linkIndex]->weight, true, secondLinkInnovationId));
		i = this->links.size() - 1;
		while(i > 0 && this->links[i]->innovationId < this->links[i-1]->innovationId){
			std::swap(this->links[i], this->links[i-1]);
			i--;
		}
	}
	//check();
}

void NEATGenome::addLink(InnovationDB* innovationDB, unsigned int numTriesToAddLink, Random* rng){
    //check();
	unsigned int numNeurons = this->numNeurons();
	unsigned int fromIndex = rng->nextInt(numNeurons);
	unsigned int toIndex, fromId, toId;
	while(numTriesToAddLink--){
		toIndex = (this->numInputs + 1) + rng->nextInt(numNeurons - this->numInputs - 1); // Not an input or bias neuron
		fromId = this->neurons[fromIndex]->id;
		toId = this->neurons[toIndex]->id;
		if(fromId != toId && getLinkIndex(fromId, toId) < 0){
			// If not the same neuron, and the network does not have this link already
			createLink(fromIndex, fromId, toIndex, toId, 2.0*rng->nextFloat() - 1.0, innovationDB);
			return;
		}
	}
	//printf("Warning: Failed to find two suitable neurons to add a link\n");
	//check();
}

void NEATGenome::addLoop(InnovationDB* innovationDB, unsigned int numTriesToFindLoopNeuron, Random* rng){
    //check();
	unsigned int index;
	while(numTriesToFindLoopNeuron--){
		index = (this->numInputs + 1) + rng->nextInt(this->numNeurons() - this->numInputs - 1); // Not an input or bias neuron
		if(!this->neurons[index]->recurrent){
			this->neurons[index]->recurrent = true;
			unsigned int neuronId = this->neurons[index]->id;
			createLink(index, neuronId, index, neuronId, 2.0*rng->nextFloat() - 1.0, innovationDB);
			return;
		}
	}
	//printf("Warning: Failed to find a suitable neuron to add a loop\n");
	//check();
}

void NEATGenome::createLink(unsigned int fromIndex, unsigned int fromId, unsigned int toIndex, unsigned int toId, float weight, InnovationDB* innovationDB){
	// Check if innovation already exists
	//check();
	int innovationId = innovationDB->checkLinkInnovation(fromId, toId);

	bool recurrent = this->neurons[fromIndex]->yCoord > this->neurons[toIndex]->yCoord;

	if (innovationId < 0){
		// We need to create a new innovation
		innovationId = innovationDB->createLinkInnovation(fromId, toId);
	}

	// Create the new link
	this->links.push_back(new NEATLink(fromId, toId, weight, recurrent, true, innovationId));
	//check();
}

void NEATGenome::mutateWeights(float mutationProbability, float newWeightProbability, Random* rng){
    //check();
	for (std::vector<NEATLink*>::iterator curLink = this->links.begin(); curLink != this->links.end(); curLink++){
		if(rng->nextFloat() < mutationProbability){
			if(rng->nextFloat() < newWeightProbability){
				(*curLink)->resetWeight(rng);
			} else {
				(*curLink)->mutateWeight(rng);
			}
		}
	}
	//check();
}

void NEATGenome::mutateActivationFunctions(float mutationProbability, float newFunctionProbability, Random* rng, bool useOtherActivationFunctions){
	//check();
	for (std::vector<NEATNeuron*>::iterator curNeuron = this->neurons.begin(); curNeuron != this->neurons.end(); curNeuron++){
		if(rng->nextFloat() < mutationProbability){

			if(useOtherActivationFunctions && (*curNeuron)->type != OUTPUT && rng->nextFloat() < newFunctionProbability){
            // Do not replace the activation function of an OUTPUT neuron.
            // Only mutate its parammeters
				(*curNeuron)->resetActivationFunction(rng);
			} else {
				(*curNeuron)->mutateActivationFunction(rng);
			}
		}
	}
	//check();
}

float NEATGenome::getCompatibilityScore(NEATGenome* other, float cMatched, float cDisjoint, float cExcess){
    // Count the number of disjoint, excees and match genes
	unsigned int numDisjoint = 0;
	unsigned int numExcess   = 0;
	unsigned int numMatched  = 0;

	// This records the summed difference of weights in matched genes
	float weightDifference = 0;

    std::vector<NEATLink*>::iterator otherLink = other->links.begin();
    std::vector<NEATLink*>::iterator thisLink = this->links.begin();

    unsigned int thisInnovId;
    unsigned int otherInnovId;
    while(thisLink != this->links.end() && otherLink != other->links.end()) {
    	thisInnovId  = (*thisLink)->innovationId;
    	otherInnovId = (*otherLink)->innovationId;
    	if(otherInnovId < thisInnovId){
    		numDisjoint++;
    		otherLink++;
    	} else if(thisInnovId > otherInnovId){
    		numDisjoint++;
    		thisLink++;
    	} else { // Same innovationId
    		weightDifference += std::fabs((*thisLink)->weight - (*otherLink)->weight);
    		numMatched++;
    		thisLink++;
    		otherLink++;
    	}
    }

    while(thisLink != this->links.end()){
        numExcess++;
		thisLink++;
    }

    while(otherLink != other->links.end()){
		numExcess++;
		otherLink++;
    }

	// Get the length of the longest genome
	unsigned int longest = std::max(this->numLinks(), other->numLinks());
    //printf("\tWeightsDifferences: %.4f\n\tnumMatched: %u\n\tnumDisjoint: %u\n\tnumExcess: %u\n", weightDifference,numMatched,numDisjoint,numExcess);
	return cMatched*weightDifference/(float)numMatched + (cDisjoint*numDisjoint + cExcess*numExcess)/(float)longest;
}

bool NEATGenome::check(){

    //test non-existing neurons

    for(unsigned int link=0;link<links.size();link++){
        if(getNeuronIndex(links[link]->fromNeuron) == -1 || getNeuronIndex(links[link]->toNeuron) == -1){
            printf("Nos falta una neurona che!\n");
            return false;
        }
    }

    //test for multigraph

    for(unsigned int link=0;link<links.size();link++){
        for(unsigned int link2=link+1;link2<links.size();link2++){
            if(links[link]->fromNeuron == links[link2]->fromNeuron && links[link]->toNeuron == links[link2]->toNeuron){
                printf("Catastrofico guachin, nos volvimos un multigrafo!\n");
                return false;
            }
        }
    }

    //test link order

    for(unsigned int link=0;link<links.size()-1;link++){
        if(links[link]->innovationId > links[link+1]->innovationId){
            printf("OH NOES! No estoy ordenado lince mistico de las praderas patagonicas!\n");
            return false;
        }
    }

    //test neuron order

    for(unsigned int neuron=0;neuron<neurons.size()-1;neuron++){
        if(neurons[neuron]->id > neurons[neuron+1]->id){
            printf("ALAS! Ordenar neuronas es mas complicado de lo que parece!\n");
            return false;
        }
    }

    //test neuron uniqueness

    for(unsigned int neuron=0;neuron<neurons.size()-1;neuron++){
        if(neurons[neuron]->id == neurons[neuron+1]->id){
            printf("ALAS! Es la guerra de los clones!\n");
            return false;
        }
    }

    return true;
}
