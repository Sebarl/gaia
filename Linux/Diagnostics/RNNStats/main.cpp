#include "../../ANN/RNN.h"
#include "../../ANN/Identity.h"
#include "../../ANN/Sigmoid.h"
#include "../../ANN/CheapSigmoid.h"
#include "../../ANN/Tanh.h"
#include "../../ANN/Gaussian.h"
#include "../../ANN/ReLU.h"


RNN* deserializeChromosome(FILE* file){
	char c;
	for(unsigned int i=0;i<3;i++){
		while((c = fgetc(file)) != '\n');
	}
    for(unsigned int i=0;i<10;i++){
		fgetc(file);
	}
	char* ramPath = new char[100];
	int i=0;
	while((c = fgetc(file)) != ':'){
        	ramPath[i]=c;
        	i++;
	}
	ramPath[i]='\000';
	unsigned int prefixLength;
	unsigned int dummy;
	fscanf(file, "%u:", &prefixLength);
	for(unsigned int i=0;i<prefixLength;i++){
		fscanf(file,"%u;",&dummy);
	}
	//discard :
	fgetc(file);

	RNN* rnn = new RNN(file);
	printf("RAMPath: %s\n",ramPath);
	delete [] ramPath;
	return rnn;
}


int main(int argc, char *argv[]){
FILE* file = fopen(argv[1], "r");
	RNN* rnn;
	if(file != NULL){
		rnn = deserializeChromosome(file);
	}
	else{
		//error, cerrar fceux!
		return 1;
	}


	printf("Input neurons: %u\n\n", rnn->cantInputs);
	unsigned int cantIdentity=0;
	unsigned int cantSigmoid=0;
	unsigned int cantCheapSigmoid=0;
	unsigned int cantTanh=0;
	unsigned int cantGaussian=0;
	unsigned int cantReLU=0;
	for(unsigned int i=0;i<rnn->cantInputs;++i){
		if(dynamic_cast<Identity*>(rnn->inputNeurons[i]->actFun) != NULL){
			cantIdentity++;
		}
		else if(dynamic_cast<CheapSigmoid*>(rnn->inputNeurons[i]->actFun) != NULL){
			cantCheapSigmoid++;
		}
		else if(dynamic_cast<Sigmoid*>(rnn->inputNeurons[i]->actFun) != NULL){
			cantSigmoid++;
		}
		else if(dynamic_cast<Tanh*>(rnn->inputNeurons[i]->actFun) != NULL){
			cantTanh++;
		}
		else if(dynamic_cast<Gaussian*>(rnn->inputNeurons[i]->actFun) != NULL){
			cantGaussian++;
		}
		else if(dynamic_cast<ReLU*>(rnn->inputNeurons[i]->actFun) != NULL){
			cantReLU++;
		}
	}	
	printf("Identities: %u\nSigmoids: %u\nCheapSigmoids: %u\nTanhs: %u\nGaussians: %u\nReLUs: %u\n\n", cantIdentity, cantSigmoid, cantCheapSigmoid, cantTanh, cantGaussian, cantReLU);

	printf("Bias neurons: %u\n\n", rnn->cantBiases);

	cantIdentity=0;
	cantSigmoid=0;
	cantCheapSigmoid=0;
	cantTanh=0;
	cantGaussian=0;
	cantReLU=0;
	for(unsigned int i=0;i<rnn->cantBiases;++i){
		if(dynamic_cast<Identity*>(rnn->biasNeurons[i]->actFun) != NULL){
			cantIdentity++;
		}
		else if(dynamic_cast<CheapSigmoid*>(rnn->biasNeurons[i]->actFun) != NULL){
			cantCheapSigmoid++;
		}
		else if(dynamic_cast<Sigmoid*>(rnn->biasNeurons[i]->actFun) != NULL){
			cantSigmoid++;
		}
		else if(dynamic_cast<Tanh*>(rnn->biasNeurons[i]->actFun) != NULL){
			cantTanh++;
		}
		else if(dynamic_cast<Gaussian*>(rnn->biasNeurons[i]->actFun) != NULL){
			cantGaussian++;
		}
		else if(dynamic_cast<ReLU*>(rnn->biasNeurons[i]->actFun) != NULL){
			cantReLU++;
		}
	}	
	printf("Identities: %u\nSigmoids: %u\nCheapSigmoids: %u\nTanhs: %u\nGaussians: %u\nReLUs: %u\n\n", cantIdentity, cantSigmoid, cantCheapSigmoid, cantTanh, cantGaussian, cantReLU);

	printf("Hidden neurons: %u\n\n", rnn->cantHidden);

	cantIdentity=0;
	cantSigmoid=0;
	cantCheapSigmoid=0;
	cantTanh=0;
	cantGaussian=0;
	cantReLU=0;
	for(unsigned int i=0;i<rnn->cantHidden;++i){
		if(dynamic_cast<Identity*>(rnn->hiddenNeurons[i]->actFun) != NULL){
			cantIdentity++;
		}
		else if(dynamic_cast<CheapSigmoid*>(rnn->hiddenNeurons[i]->actFun) != NULL){
			cantCheapSigmoid++;
		}
		else if(dynamic_cast<Sigmoid*>(rnn->hiddenNeurons[i]->actFun) != NULL){
			cantSigmoid++;
		}
		else if(dynamic_cast<Tanh*>(rnn->hiddenNeurons[i]->actFun) != NULL){
			cantTanh++;
		}
		else if(dynamic_cast<Gaussian*>(rnn->hiddenNeurons[i]->actFun) != NULL){
			cantGaussian++;
		}
		else if(dynamic_cast<ReLU*>(rnn->hiddenNeurons[i]->actFun) != NULL){
			cantReLU++;
		}
	}	
	printf("Identities: %u\nSigmoids: %u\nCheapSigmoids: %u\nTanhs: %u\nGaussians: %u\nReLUs: %u\n\n", cantIdentity, cantSigmoid, cantCheapSigmoid, cantTanh, cantGaussian, cantReLU);

	printf("Output neurons: %u\n\n", rnn->cantOutputs);

	cantIdentity=0;
	cantSigmoid=0;
	cantCheapSigmoid=0;
	cantTanh=0;
	cantGaussian=0;
	cantReLU=0;
	for(unsigned int i=0;i<rnn->cantOutputs;++i){
		if(dynamic_cast<Identity*>(rnn->outputNeurons[i]->actFun) != NULL){
			cantIdentity++;
		}
		else if(dynamic_cast<CheapSigmoid*>(rnn->outputNeurons[i]->actFun) != NULL){
			cantCheapSigmoid++;
		}
		else if(dynamic_cast<Sigmoid*>(rnn->outputNeurons[i]->actFun) != NULL){
			cantSigmoid++;
		}
		else if(dynamic_cast<Tanh*>(rnn->outputNeurons[i]->actFun) != NULL){
			cantTanh++;
		}
		else if(dynamic_cast<Gaussian*>(rnn->outputNeurons[i]->actFun) != NULL){
			cantGaussian++;
		}
		else if(dynamic_cast<ReLU*>(rnn->outputNeurons[i]->actFun) != NULL){
			cantReLU++;
		}
	}	
	printf("Identities: %u\nSigmoids: %u\nCheapSigmoids: %u\nTanhs: %u\nGaussians: %u\nReLUs: %u\n\n", cantIdentity, cantSigmoid, cantCheapSigmoid, cantTanh, cantGaussian, cantReLU);
	return 0;
}
