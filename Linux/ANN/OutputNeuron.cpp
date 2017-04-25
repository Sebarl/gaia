#include "OutputNeuron.h"

OutputNeuron::OutputNeuron(ActivationFunction* actFun):Neuron(actFun){
	this->output=0.0;
}
OutputNeuron::~OutputNeuron(){
}

void OutputNeuron::setInput(float input){
	this->output+=input;
}

void OutputNeuron::propagate(){
	output = actFun->evaluate(output);
}

void OutputNeuron::advanceTimestep(){
	oldOutput = output;
	output=0.0;
}

int OutputNeuron::neuronType(){
		return 3;
}
