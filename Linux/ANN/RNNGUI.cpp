#include "RNNGUI.h"
#include "Identity.h"
#include "Sigmoid.h"
#include "CheapSigmoid.h"
#include "Tanh.h"
#include "ReLU.h"
#include "Gaussian.h"
#include <cstdio>
#include <stdlib.h>

float atof(char* num)
 {
     if (!num || !*num)
         return 0;
     float integerPart = 0;
     float fractionPart = 0;
     int divisorForFraction = 1;
     int sign = 1;
     bool inFraction = false;

     if (*num == '-')
     {
         ++num;
         sign = -1;
     }
     else if (*num == '+')
     {
         ++num;
     }
     while (*num != '\0')
     {
         if (*num >= '0' && *num <= '9')
         {
             if (inFraction)
             {

                 fractionPart = fractionPart*10 + (*num - '0');
                 divisorForFraction *= 10;
             }
             else
             {
                 integerPart = integerPart*10 + (*num - '0');
             }
         }
         else if (*num == '.')
         {
             if (inFraction)
                 return sign * (integerPart + fractionPart/divisorForFraction);
             else
                 inFraction = true;
         }
         else
         {
             return sign * (integerPart + fractionPart/divisorForFraction);
         }
         ++num;
     }
     return sign * (integerPart + fractionPart/divisorForFraction);
 }


inline float parseFloat(FILE* file, char delim){
    char buff[10];
    char c;
    int i=0;
    buff[9]='\000';
	while((c = fgetc(file)) != delim){
        	buff[i]=c;
        	i++;
	}
	return (float) atof(buff);
}


inline ActivationFunction* parseFunction(FILE* specification){
	ActivationFunction* actFun;
	unsigned int funId;
	float param, param2;
	fscanf(specification,"%u;",&funId);
	switch(funId){
		case 0:
			//identity
			actFun = new Identity();
			break;
		case 1:
			//sigmoid
			//fscanf(specification,"%f;",&param);
			param=parseFloat(specification,';');
			actFun = new Sigmoid(param);
			break;
		case 2:
			//tanh
//			fscanf(specification,"%f;",&param);
			param=parseFloat(specification,';');
			actFun = new Tanh(param);
			break;
		case 3:
			//cheap sigmoid
//			fscanf(specification,"%f;",&param);
			param=parseFloat(specification,';');
			actFun = new CheapSigmoid(param);
			break;
		case 4:
			//cheap gaussian
			param=parseFloat(specification,';');
			param2=parseFloat(specification,';');
			actFun = new Gaussian(param, param2);
			break;
		case 5:
			//cheap relu
			param=parseFloat(specification,';');
			actFun = new ReLU(param);
			break;
        default:
            printf("La funcion de activacion es invalida: %u\n",funId);
	    	exit(418);
	}
	return actFun;
}

RNN::RNN(FILE* specification){

	//parse configuration file

	//load inputs
	fscanf(specification,"%u:",&cantInputs);
	float bias;
	inputNeurons = new InputNeuron*[cantInputs];
	for(unsigned int i=0;i<cantInputs;i++){
		inputNeurons[i] =  new InputNeuron(parseFunction(specification));
	}

	//load biases
	fscanf(specification,"%u:",&cantBiases);

	biasNeurons = new BiasNeuron*[cantBiases];

	for(unsigned int i=0;i<cantBiases;i++){
//		fscanf(specification,"%f;",&bias);
		bias=parseFloat(specification,';');
		biasNeurons[i] =  new BiasNeuron(bias, parseFunction(specification));
	}

	//load outputs
	fscanf(specification,"%u:",&cantOutputs);
	outputNeurons = new OutputNeuron*[cantOutputs];
	for(unsigned int i=0;i<cantOutputs;i++){
		outputNeurons[i] =  new OutputNeuron(parseFunction(specification));
	}

	//load hidden
	fscanf(specification,"%u:",&cantHidden);
	hiddenNeurons = new HiddenNeuron*[cantHidden];
	for(unsigned int i=0;i<cantHidden;i++){
		hiddenNeurons[i] =  new HiddenNeuron(parseFunction(specification));
	}



	//load links with weights
	unsigned int cantLinks;
	unsigned int sourceNeuronType, destNeuronType;
	unsigned int sourceIndex, destIndex;
	fscanf(specification,"%u:",&cantLinks);
	for(unsigned int i=0;i<cantLinks;i++){
		Link* link = new Link();
			link->weight=parseFloat(specification,';');
		fscanf(specification,"%u;%u;%u;%u;",&sourceNeuronType,&sourceIndex,&destNeuronType,&destIndex);

		switch(destNeuronType){
			case 0: //input
				link->neuron=inputNeurons[destIndex];
				break;
			case 1: //biases
				link->neuron=biasNeurons[destIndex];
				break;
			case 2: //hidden
				link->neuron=hiddenNeurons[destIndex];
				break;
			case 3: //output
				link->neuron=outputNeurons[destIndex];
				break;
		}

		switch(sourceNeuronType){
			case 0: //input
				inputNeurons[sourceIndex]->outboundLinks.push_back(link);
				break;
			case 1: //biases
				biasNeurons[sourceIndex]->outboundLinks.push_back(link);
				break;
			case 2: //hidden
				hiddenNeurons[sourceIndex]->outboundLinks.push_back(link);
				break;
			case 3: //output
				outputNeurons[sourceIndex]->outboundLinks.push_back(link);
				break;
		}
	}

	neuronList = new std::list<Neuron*>();
	secondList = new std::list<Neuron*>();
	std::list<Neuron*> toEmptyList;
	depth=0;
	unsigned int candidateDepth;
	unsigned int cantOutputsSeen;
	for(unsigned int i=0;i<cantInputs;i++){
		candidateDepth=0;
		cantOutputsSeen=0;
		neuronList->push_back(inputNeurons[i]);
		//BFS traversal over graph to propagate inputs throughout the network

		//toggle valid flag indicator from previous invocation, to prevent the need of resetting neuron flags
		//declare queue for BFS

		Neuron* actNeuron;
		Neuron* neighbour;
		//fill queue with input neurons while setting their input values
		while(!neuronList->empty() && cantOutputsSeen < cantOutputs){
			for (std::list<Neuron*>::iterator it=neuronList->begin(); it != neuronList->end(); ++it){
			    actNeuron = *it;
				for(unsigned int i=0; i<actNeuron->outboundLinks.size();i++){
					neighbour=actNeuron->outboundLinks[i]->neuron;
					if(!neighbour->visited){
					    neighbour->visited = true;
                        toEmptyList.push_back(neighbour);
					    secondList->push_back(neighbour);
						if(neighbour->neuronType() == 3){
							cantOutputsSeen++;
						}
					}
				}
			}
			neuronList->clear();
			std::list<Neuron*>* temp;
			temp = neuronList;
			neuronList=secondList;
			secondList=temp;
			candidateDepth++;
		}
		for (std::list<Neuron*>::iterator it=toEmptyList.begin(); it != toEmptyList.end(); ++it){
			(*it)->visited=false;
		}
		secondList->clear();
		neuronList->clear();
		toEmptyList.clear();
		if(candidateDepth>depth){
			depth=candidateDepth;
		}
	}

	for(unsigned int i=0;i<cantBiases;i++){
		candidateDepth=0;
		cantOutputsSeen=0;
		neuronList->push_back(biasNeurons[i]);
		//BFS traversal over graph to propagate inputs throughout the network

		//toggle valid flag indicator from previous invocation, to prevent the need of resetting neuron flags
		//declare queue for BFS

		Neuron* actNeuron;
		Neuron* neighbour;
		//fill queue with input neurons while setting their input values
		while(!neuronList->empty() && cantOutputsSeen < cantOutputs){
			for (std::list<Neuron*>::iterator it=neuronList->begin(); it != neuronList->end(); ++it){
			    actNeuron = *it;
				for(unsigned int i=0; i<actNeuron->outboundLinks.size();i++){
					neighbour=actNeuron->outboundLinks[i]->neuron;
					if(!neighbour->visited){
					    neighbour->visited = true;
					    toEmptyList.push_back(neighbour);
					    secondList->push_back(neighbour);
						if(neighbour->neuronType() == 3){
							cantOutputsSeen++;
						}
					}
				}
			}
			neuronList->clear();
			std::list<Neuron*>* temp;
			temp = neuronList;
			neuronList=secondList;
			secondList=temp;
			candidateDepth++;
		}
		for (std::list<Neuron*>::iterator it=toEmptyList.begin(); it != toEmptyList.end(); ++it){
			(*it)->visited=false;
		}
		secondList->clear();
		neuronList->clear();
		toEmptyList.clear();
		if(candidateDepth>depth){
			depth=candidateDepth;
		}
	}


}

RNN::~RNN(){
	for(unsigned int i=0;i<cantInputs;i++){
		delete inputNeurons[i];
	}
	for(unsigned int i=0;i<cantBiases;i++){
		delete biasNeurons[i];
	}
	for(unsigned int i=0;i<cantHidden;i++){
		delete hiddenNeurons[i];
	}
	for(unsigned int i=0;i<cantOutputs;i++){
		delete outputNeurons[i];
	}
	delete [] inputNeurons;
	delete [] hiddenNeurons;
	delete [] outputNeurons;
	delete [] biasNeurons;
	delete neuronList;
	delete secondList;
}

float* RNN::propagate(float* inputs){


	//BFS traversal over graph to propagate inputs throughout the network

	//toggle valid flag indicator from previous invocation, to prevent the need of resetting neuron flags
	//declare queue for BFS

	Neuron* actNeuron;
	Neuron* neighbour;
	//fill queue with input neurons while setting their input values
	for(unsigned int i=0;i<cantInputs;i++){
		neuronList->push_back(inputNeurons[i]);
		inputNeurons[i]->setInput(inputs[i]);
	}
	for(unsigned int i=0;i<cantBiases;i++){
		neuronList->push_back(biasNeurons[i]);
	}
	for(unsigned int i=0;i<depth+1;i++){
		for (std::list<Neuron*>::iterator it=neuronList->begin(); it != neuronList->end(); ++it){
		    actNeuron = *it;
			actNeuron->propagate();
			for(unsigned int i=0; i<actNeuron->outboundLinks.size();i++){
		        neighbour=actNeuron->outboundLinks[i]->neuron;
		        neighbour->setInput(actNeuron->outboundLinks[i]->weight*actNeuron->oldOutput);
		        if(!neighbour->visited){
		            neighbour->visited = true;
		            secondList->push_back(neighbour);
		        }
			}
		}
		for (std::list<Neuron*>::iterator it=neuronList->begin(); it != neuronList->end(); ++it){
			actNeuron=(*it);
			actNeuron->advanceTimestep();
			actNeuron->visited=false;
		}
		neuronList->clear();
		std::list<Neuron*>* temp;
		temp = neuronList;
		neuronList=secondList;
		secondList=temp;
	}
	//obtain outputs
	float* outputs = new float[cantOutputs];
	for(unsigned int i=0; i<cantOutputs; i++){
		outputs[i] = outputNeurons[i]->oldOutput;
	}
	return outputs;
}
