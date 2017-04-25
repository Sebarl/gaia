#include "Neuron.h"

Neuron::Neuron(ActivationFunction* actFun){
	visited=false;
	this->oldOutput=0.0;
	this->actFun=actFun;
}

Neuron::~Neuron(){
	for(unsigned int i=0;i<outboundLinks.size();++i){
        	delete outboundLinks[i];
	}
	outboundLinks.clear();
	delete actFun;
}
