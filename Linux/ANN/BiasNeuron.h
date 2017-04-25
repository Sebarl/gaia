#ifndef BIAS_H
#define BIAS_H
#include "Neuron.h"
#include "ActivationFunction.h"

class BiasNeuron:public Neuron {

public:
	float bias;
	BiasNeuron(float bias, ActivationFunction* actFun);
	virtual ~BiasNeuron();
	void propagate();
	void setInput(float input);
	void advanceTimestep();
	virtual int neuronType();
};

#endif
