#ifndef OUTPUT_H
#define OUTPUT_H
#include "Neuron.h"

class OutputNeuron :public Neuron{

public:
	float output;
	OutputNeuron(ActivationFunction* actFun);
	virtual ~OutputNeuron();
	void setInput(float input);
	void propagate();
	void advanceTimestep();
	virtual int neuronType();
};

#endif
