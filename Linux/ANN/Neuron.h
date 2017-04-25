#ifndef NEURON_H
#define NEURON_H
#include "ActivationFunction.h"
#include "Link.h"
#include <vector>
class Neuron {

public:
	std::vector<Link*> outboundLinks;
	int cantLinks;
	bool visited;
	float oldOutput;
	ActivationFunction* actFun;
	Neuron(ActivationFunction* actFun);
	virtual ~Neuron();
	virtual void propagate()=0;
	virtual void setInput(float input)=0;
	virtual void advanceTimestep()=0;
	virtual int neuronType()=0;

};

#endif
