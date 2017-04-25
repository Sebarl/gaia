#include "BiasNeuron.h"

BiasNeuron::BiasNeuron(float bias, ActivationFunction* actFun):Neuron(actFun){
	this->bias=bias;
}
BiasNeuron::~BiasNeuron(){
}

void BiasNeuron::setInput(float input){
	this->bias=input;
}

void BiasNeuron::propagate(){
	oldOutput = actFun->evaluate(bias);
}

void BiasNeuron::advanceTimestep(){
}

int BiasNeuron::neuronType(){
		return 1;
}
