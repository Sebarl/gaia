#include "NEATNeuron.h"
#include "Sigmoid.h"
#include "Tanh.h"
#include "Identity.h"
#include "CheapSigmoid.h"
#include "Gaussian.h"
#include "ReLU.h"

NEATNeuron::NEATNeuron(int id, NeuronType type, ActivationFunction* function, float xCoord, float yCoord) {
    this->id = id;
    this->type = type;
    this->recurrent = false;
    this->xCoord = xCoord;
    this->yCoord = yCoord;
    this->function = function;
}

NEATNeuron::NEATNeuron(int id, NeuronType type, bool recurrent, ActivationFunction* function, float xCoord, float yCoord) {
    this->id = id;
    this->type = type;
    this->recurrent = recurrent;
    this->xCoord = xCoord;
    this->yCoord = yCoord;
    this->function = function;
}

NEATNeuron::NEATNeuron(int id, NeuronType type, float xCoord, float yCoord, Random* rng) {
    this->id = id;
    this->type = type;
    this->recurrent = false;
    this->xCoord = xCoord;
    this->yCoord = yCoord;
    int rnd = rng->nextInt(6);
	ActivationFunction* f;
	switch(rnd){
		case 0: f = new Sigmoid(rng);
				break;
		case 1: f = new Identity();
				break;
		case 2: f = new Tanh(rng);
				break;
		case 3: f = new CheapSigmoid(rng);
				break;
        case 4: f = new Gaussian(rng);
                break;
        case 5: f = new ReLU(rng);
                break;
        default:
            f = new Tanh();
            break;
	}
    this->function = f;
}

NEATNeuron::NEATNeuron(int id, NeuronType type, bool recurrent, float xCoord, float yCoord, Random* rng) {
    this->id = id;
    this->type = type;
    this->recurrent = recurrent;
    this->xCoord = xCoord;
    this->yCoord = yCoord;
    int rnd = rng->nextInt(6);
	ActivationFunction* f;
	switch(rnd){
		case 0: f = new Sigmoid(rng);
				break;
		case 1: f = new Identity();
				break;
		case 2: f = new Tanh(rng);
				break;
		case 3: f = new CheapSigmoid(rng);
				break;
        case 4: f = new Gaussian(rng);
                break;
        case 5: f = new ReLU(rng);
                break;
        default:
            f = new Tanh();
            break;
	}
    this->function = f;
}

NEATNeuron::~NEATNeuron(){
    delete function;
}

void NEATNeuron::resetActivationFunction(Random* rng){
    int rnd = rng->nextInt(6);
	ActivationFunction* f;
	switch(rnd){
		case 0: f = new Sigmoid(rng);
				break;
		case 1: f = new Identity();
				break;
		case 2: f = new Tanh();
				break;
		case 3: f = new CheapSigmoid();
				break;
        case 4: f = new Gaussian(rng);
                break;
        case 5: f = new ReLU(rng);
                break;
        default:
            f = new Tanh();
            break;
	}
	delete this->function;
    this->function = f;
}

void NEATNeuron::mutateActivationFunction(Random* rng){
    this->function->mutate(rng);
}

NEATNeuron* NEATNeuron::clone(){
    return new NEATNeuron(id, type,recurrent,function->clone(),xCoord,yCoord);
}

const ActivationFunction* NEATNeuron::getFunction(){
    return function;
}
