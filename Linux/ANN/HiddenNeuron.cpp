#include "HiddenNeuron.h"

HiddenNeuron::HiddenNeuron(ActivationFunction* actFun):Neuron(actFun){
	this->output=0.0;
}
HiddenNeuron::~HiddenNeuron(){
}

void HiddenNeuron::setInput(float input){
	this->output+=input;
}

void HiddenNeuron::propagate(){
	output = actFun->evaluate(output);
}

void HiddenNeuron::advanceTimestep(){
	oldOutput = output;
	output=0.0;
}

int HiddenNeuron::neuronType(){
		return 2;
}
