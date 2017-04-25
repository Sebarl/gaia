#include "AI.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include "ANN/RNN.h"

//definicion de teclas;
const uint8 right = 128;
const uint8 left = 64;
const uint8 down = 32;
const uint8 up = 16;
const uint8 start = 8;
const uint8 Select = 4;
const uint8 keyB = 2;
const uint8 keyA = 1;

typedef struct{
	int address;
	float weight;
}weight;

RNN* rnn;
int RAMCount = 0;
std::vector<int> RAMMask;
weight* fitnessWeights;
char* ramPath;
char* weightPath;
uint8* prefix;
unsigned int prefixLength;
float* MaskedRAM;
unsigned int frames = 0;
float shift;
double fitness = 0.0;
unsigned int weightsCount;
void HELP(char* c){
FILE* file = fopen("HELP", "a");
	if(file != NULL){
		fprintf(file, c);
		fclose(file);
	}
}


uint8 runAI(uint8* RAM, bool& exitNow){
	uint8 keypress = 0;
    float value;
	int index = 0;
	if(frames>=prefixLength){
		for(unsigned int i=0;i<RAMCount;i++){
            value=RAM[RAMMask[i]];
			MaskedRAM[i]=value;
			if(fitnessWeights[index].address == RAMMask[i]){
	            fitness+=value*fitnessWeights[index].weight;
				++index;
			}
		}
		fitness-= shift;
		float* results = rnn->propagate(MaskedRAM);
		if(results[0] > 0.5){
			keypress |= keyA;
		}
		if(results[1] > 0.5){
			keypress |= keyB;
		}
		if(results[2] > 0.5){
			keypress |= Select;
		}
		if(results[3] > 0.5){
			keypress |= start;
		}
		if(results[4] > 0.5){
			keypress |= up;
		}
		if(results[5] > 0.5){
			keypress |= down;
		}
		if(results[6] > 0.5){
			keypress |= left;
		}
		if(results[7] > 0.5){
			keypress |= right;
		}
		delete [] results;
	}
	else{
		keypress=prefix[frames];
		frames++;
	}
	return keypress;
}

void printStatistics(){
	delete [] fitnessWeights;
	printf("%lf\n",fitness);
}

//funcion de des-serializacion de puntos

void loadRAMMap(char* path){
    std::string line;
    std::ifstream RAMMapFile(path);
    if (RAMMapFile.is_open()){
        char elem;
        //ignore first line
        RAMMapFile.ignore(9999,'\n');
        for(unsigned int i=0; i< 64;i++){
                RAMMapFile.ignore(255,'|');
                for(unsigned int j=0; j<31;j++){
                    RAMMapFile >> elem;
                    if(elem != 'S' && elem != 'E' && elem != 'L' && elem != 'I'){
                        RAMMask.push_back(32*i+j);
                    }
                    RAMMapFile.ignore(255,'|');
                }
				RAMMapFile >> elem;
                if(elem != 'S' && elem != 'E' && elem != 'L' && elem != 'I'){
                    RAMMask.push_back(32*i+31);
                }
        }
        RAMMapFile.close();
        RAMCount = RAMMask.size();
    }
    else{
        std::cout << "Unable to open RAMMap file: " << path;
        exit(-1);
    }
}

void loadFitnessWeights(char* path){
    std::string line;
    std::ifstream FitnessWeightsFile(path);
    if (FitnessWeightsFile.is_open()){
        //ignore first, second and third lines
        FitnessWeightsFile.ignore(9999,'\n');
        FitnessWeightsFile.ignore(9999,'\n');
        FitnessWeightsFile.ignore(9999,'\n');
        FitnessWeightsFile.ignore(10,' ');
		unsigned int addr;        
		float data;
		FitnessWeightsFile >> shift;
        FitnessWeightsFile >> weightsCount;
		fitnessWeights = new weight[weightsCount];
        for(int i=0;i<weightsCount;i++){
                FitnessWeightsFile >> fitnessWeights[i].address;
		        FitnessWeightsFile.ignore(1,':');
                FitnessWeightsFile >> fitnessWeights[i].weight;
        }
	MaskedRAM = new float[RAMCount];
    }
    else{
        std::cout << "Unable to open FitnessWeights file: " << path;
        exit(-1);
    }
}

RNN* deserializeChromosome(FILE* file){
	ramPath = new char[100];
	weightPath = new char[100];
	char c;
	int i=0;
	while((c = fgetc(file)) != ':'){
        	ramPath[i]=c;
        	i++;
	}
	ramPath[i]='\000';
	i=0;
	while((c = fgetc(file)) != ':'){
        	weightPath[i]=c;
        	i++;
	}
	weightPath[i]='\000';

	fscanf(file, "%u:", &prefixLength);
	prefix = new uint8[prefixLength];
	for(unsigned int i=0;i<prefixLength;i++){
		fscanf(file,"%u;",&prefix[i]);
	}
	//discard :
	fgetc(file);

	RNN* rnn = new RNN(file);
	return rnn;
}


int initializeParams(std::string s){
FILE* file = fopen(s.c_str(), "r");
	if(file != NULL){
		rnn = deserializeChromosome(file);
		loadRAMMap(ramPath);
		loadFitnessWeights(weightPath);
		fclose(file);
		return 1;
	}
	else{
		//error, cerrar fceux!
		return 0;
	}
}

