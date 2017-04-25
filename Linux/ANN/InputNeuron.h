#ifndef INPUT_H
#define INPUT_H
#include "Neuron.h"

class InputNeuron :public Neuron{

public:
	InputNeuron(ActivationFunction* actFun);
	virtual ~InputNeuron();
	void setInput(float input);
	void propagate();
	void advanceTimestep();
	virtual int neuronType();
};

#endif
