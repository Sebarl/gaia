#ifndef HIDDEN_H
#define HIDDEN_H
#include "Neuron.h"

class HiddenNeuron :public Neuron{

public:
	float output;
	HiddenNeuron(ActivationFunction* actFun);
	virtual ~HiddenNeuron();
	void setInput(float input);
	void propagate();
	void advanceTimestep();
	virtual int neuronType();
};

#endif
