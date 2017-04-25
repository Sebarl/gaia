#ifndef RNN_H
#define RNN_H
#include "BiasNeuron.h"
#include "InputNeuron.h"
#include "OutputNeuron.h"
#include "HiddenNeuron.h"
#include <stdio.h>
#include <list>


class RNN {

public:
	InputNeuron** inputNeurons;
	unsigned int cantInputs;
	HiddenNeuron** hiddenNeurons;
	unsigned int cantHidden;
	OutputNeuron** outputNeurons;
	unsigned int cantOutputs;
	BiasNeuron** biasNeurons;
	unsigned int cantBiases;
	unsigned int depth;
	std::list<Neuron*>* neuronList;
	std::list<Neuron*>* secondList;


	RNN(FILE* specification);
	RNN(char* specification);
	virtual ~RNN();
	float* propagate(float* inputs);
};

#endif
