#include "InputNeuron.h"

InputNeuron::InputNeuron(ActivationFunction* actFun):Neuron(actFun){
}

InputNeuron::~InputNeuron(){
}

void InputNeuron::setInput(float input){
	this->oldOutput=input;
}

void InputNeuron::propagate(){
	oldOutput = actFun->evaluate(oldOutput);
}

void InputNeuron::advanceTimestep(){
}

int InputNeuron::neuronType(){
		return 0;
}
