#include <stdlib.h>
#include <cstring>
#include "XOREvaluator.h"
#include "Constants.h"
#include <stdio.h>
#include "NeuronType.h"
#include "../../Linux/ANN/RNN.h"
#include "XorShift128.h"

XOREvaluator::XOREvaluator(Parameters* params, int islandIndex){
    this->params = params;
    this->islandIndex = islandIndex;
}

XOREvaluator::~XOREvaluator(){}

inline int XOREvaluator::serializeChromosome(NEATGenome* sol, char* buff, Parameters* params){
    int length = 0;
	std::vector<NEATNeuron*>::iterator iter = sol->neurons.begin();

	//serialize input neurons

	length+=sprintf(buff+length,"%u:",params->config.numInputs);
	for(unsigned int i=0; i<params->config.numInputs;i++){
        length=(*iter)->getFunction()->serialize(buff,length);
        iter++;
	}

	//serialize biases, currently one

	length+=sprintf(buff+length,"%u:",1);
	length+=sprintf(buff+length,"%f;",1.0);
    length=(*iter)->getFunction()->serialize(buff,length);
    iter++;

    //serialize output neurons

	length+=sprintf(buff+length,"%u:",params->config.numOutputs);
	for(unsigned int i=0; i<params->config.numOutputs;i++){
        length=(*iter)->getFunction()->serialize(buff,length);
        iter++;
	}

    //serialize hidden neurons

    unsigned int cantHidden = sol->neurons.size() - (params->config.numInputs + params->config.numOutputs + 1);
	length+=sprintf(buff+length,"%u:",cantHidden);
	for(unsigned int i=0; i<cantHidden;i++){
        length=(*iter)->getFunction()->serialize(buff,length);
        iter++;
	}

    //serialize links

    length+=sprintf(buff+length,"%zu:",sol->links.size());
    unsigned int sourceNeuronType, sourceIndex, destNeuronType, destIndex;
    for(std::vector<NEATLink*>::iterator iter2 = sol->links.begin();iter2!=sol->links.end();iter2++){
        sourceIndex=sol->getNeuronIndex((*iter2)->fromNeuron);
        destIndex =sol->getNeuronIndex((*iter2)->toNeuron);
        switch(sol->neurons[sourceIndex]->type){
            case INPUT:
                 sourceNeuronType=0;
                break;
            case BIAS:
                sourceIndex=0;
                sourceNeuronType=1;
                break;
            case HIDDEN:
                sourceIndex-=params->config.numInputs+params->config.numOutputs+1;
                sourceNeuronType=2;
                break;
            case OUTPUT:
                sourceIndex-=params->config.numInputs+1;
                sourceNeuronType=3;
                break;
        }
        switch(sol->neurons[destIndex]->type){
            case INPUT:
                 destNeuronType=0;
                break;
            case BIAS:
                destIndex=0;
                destNeuronType=1;
                break;
            case HIDDEN:
                destIndex-=params->config.numInputs+params->config.numOutputs+1;
                destNeuronType=2;
                break;
            case OUTPUT:
                destIndex-=params->config.numInputs+1;
                destNeuronType=3;
                break;
        }
        length+=sprintf(buff+length,"%f;%u;%u;%u;%u;",(*iter2)->weight,sourceNeuronType,sourceIndex,destNeuronType,destIndex);
    }
	return length;
}


double XOREvaluator::getFitness(const int index, std::vector<NEATGenome*> population){

	char* serialChromosome;

	serialChromosome = new char[1000000];
	serializeChromosome(population[index], serialChromosome, params);
    RNN* rnn = new RNN(serialChromosome);
    delete [] serialChromosome;
	float* inputs = new float [2];
	double fitness = 1.0;

	inputs[0] = 0.0;
	inputs[1] = 0.0;
	float* sol = rnn->propagate(inputs);
	if(sol[0] < 0.5){
        fitness+=1.0;
	}

	inputs[0] = 1.0;
	inputs[1] = 0.0;
	sol = rnn->propagate(inputs);
	if(sol[0] >= 0.5){
        fitness+=1.0;
	}

	inputs[0] = 0.0;
	inputs[1] = 1.0;
	sol = rnn->propagate(inputs);
	if(sol[0] >= 0.5){
        fitness+=1.0;
	}

	inputs[0] = 1.0;
	inputs[1] = 1.0;
	sol = rnn->propagate(inputs);
	if(sol[0] < 0.5){
        fitness+=1.0;
	}
    delete rnn;
    return fitness;

}

bool XOREvaluator::isNatural(){
    return true;
}
