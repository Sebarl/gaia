#include <stdlib.h>
#include <cstring>
#include "FCEUXEvaluator.h"
#include "Constants.h"
#include <stdio.h>
#include "NeuronType.h"
#include "XorShift128.h"
#include "GenericException.h"

FCEUXEvaluator::FCEUXEvaluator(Parameters* params, int islandIndex){
    this->params = params;
    this->islandIndex = islandIndex;
}

FCEUXEvaluator::~FCEUXEvaluator(){}

inline int FCEUXEvaluator::serializeChromosome(NEATGenome* sol, char* buff, Parameters* params){
    int length = 0;
    length+=sprintf(buff+length,"%s:",params->ramPath);
    length+=sprintf(buff+length,"%s:",params->weightsPath);
    length+=sprintf(buff+length,"%s",params->prefix);
	std::vector<NEATNeuron*>::iterator iter = sol->neurons.begin();

	//serialize input neurons


	length+=sprintf(buff+length,"%u:",params->config.numInputs);
	for(unsigned int i=0; i<params->config.numInputs;i++){
        length=(*iter)->getFunction()->serialize(buff,length);
        iter++;
	}

	//serialize biases, currently one

	length+=sprintf(buff+length,"%u:",1);
	length+=sprintf(buff+length,"%f;",1.0);
    length=(*iter)->getFunction()->serialize(buff,length);
    iter++;

    //serialize output neurons

	length+=sprintf(buff+length,"%u:",params->config.numOutputs);
	for(unsigned int i=0; i<params->config.numOutputs;i++){
        length=(*iter)->getFunction()->serialize(buff,length);
        iter++;
	}

    //serialize hidden neurons

    unsigned int cantHidden = sol->neurons.size() - (params->config.numInputs + params->config.numOutputs + 1);
	length+=sprintf(buff+length,"%u:",cantHidden);
	for(unsigned int i=0; i<cantHidden;i++){
        length=(*iter)->getFunction()->serialize(buff,length);
        iter++;
	}

    //serialize links

    length+=sprintf(buff+length,"%zu:",sol->links.size());
    unsigned int sourceNeuronType, sourceIndex, destNeuronType, destIndex;
    for(std::vector<NEATLink*>::iterator iter2 = sol->links.begin();iter2!=sol->links.end();iter2++){
        sourceIndex = (unsigned int)sol->getNeuronIndex((*iter2)->fromNeuron);
        destIndex = (unsigned int)sol->getNeuronIndex((*iter2)->toNeuron);
        switch(sol->neurons[sourceIndex]->type){
            case INPUT:
                 sourceNeuronType=0;
                break;
            case BIAS:
                sourceIndex=0;
                sourceNeuronType=1;
                break;
            case HIDDEN:
                sourceIndex-=params->config.numInputs+params->config.numOutputs+1;
                sourceNeuronType=2;
                break;
            case OUTPUT:
                sourceIndex-=params->config.numInputs+1;
                sourceNeuronType=3;
                break;
            default:
                throw GenericException("Unrecognized neuron type in source neuron");
        }
        switch(sol->neurons[destIndex]->type){
            case INPUT:
                 destNeuronType=0;
                break;
            case BIAS:
                destIndex=0;
                destNeuronType=1;
                break;
            case HIDDEN:
                destIndex-=params->config.numInputs+params->config.numOutputs+1;
                destNeuronType=2;
                break;
            case OUTPUT:
                destIndex-=params->config.numInputs+1;
                destNeuronType=3;
                break;
            default:
                throw GenericException("Unrecognized neuron type in destination neuron");
        }
        length+=sprintf(buff+length,"%f;%u;%u;%u;%u;",(*iter2)->weight,sourceNeuronType,sourceIndex,destNeuronType,destIndex);
    }
	return length;
}


double FCEUXEvaluator::getFitness(const int index, std::vector<NEATGenome*> population){

	char filename[50];
    NEATGenome* sol = population[index];
	sprintf(filename, "tempSols/%d%d%d.sol", index, this->islandIndex, this->params->evolutiveProcessID);
	char* serialChromosome;

	serialChromosome = new char[1000000];
	serializeChromosome(sol, serialChromosome, params);

	//saquemos de donde viene solution
	FILE* file = fopen(filename, "w");
	if(file != NULL){
		//escribimos el individuo al archivo
		fputs(serialChromosome, file);
		fclose(file);
	}
	else{
		printf("%s\n", "BOOOOOM!");
		//manejo de errores, que hacer si no puedo crear archivo
	}
    delete [] serialChromosome;
	//invocar a fceux y obtener el resultado
	char path[200];
	#if __GNUC__ < 5
    sprintf(path, "cd \"Fceux Headless/bin/\"; ./fceux -l %d -e ../../%s %s", this->params->frameCount, filename,this->params->ROM);
    #else
    sprintf(path, "cd \"../Linux/Fceux Headless/bin/\"; ./fceux -l %d -e ../../../ANNTrainerAE/%s %s", this->params->frameCount, filename,this->params->ROM);
    #endif // __GNUC__

	FILE *fp = popen(path,"r");
	double fitness;
//	char* buff = new char[500];
	//long unsigned int a =500;
	//getline(&buff,&a,fp);
	//printf("%s\n",path);
	//printf("%s\n",buff);
	fscanf(fp,"%lf",&fitness);
	//printf("%lf\n",fitness);
	//fitness=0;
	pclose(fp);
	if(fitness < 0){
        fitness = 1.0 + 1.0/(fitness-1.0);
	}
	else{
        fitness+=1.0;
	}
	return fitness;

}

bool FCEUXEvaluator::isNatural(){
    return true;
}
