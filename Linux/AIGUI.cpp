#include "AIGUI.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include "ANN/RNNGUI.h"

//definicion de teclas;
const uint8 right = 128;
const uint8 left = 64;
const uint8 down = 32;
const uint8 up = 16;
const uint8 start = 8;
const uint8 Select = 4;
const uint8 keyB = 2;
const uint8 keyA = 1;

RNN* rnn;
unsigned int RAMCount = 0;
std::vector<int> RAMMask;
char* ramPath;
uint8* prefix;
unsigned int prefixLength;
float* MaskedRAM;
unsigned int frames = 0;

uint8 runAI(uint8* RAM){
	uint8 keypress = 0;
	if(frames>=prefixLength){
		for(unsigned int i=0;i<RAMCount;i++){
			MaskedRAM[i]=RAM[RAMMask[i]];
		}
		float* results = rnn->propagate(MaskedRAM);
	/*
		for(unsigned int i=0;i<8;i++){
			printf("%d ",results[i]);
		}
		printf("\n");
	*/
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
		printf("%d\n",keypress);
		fflush(0);
		delete [] results;
	}
	else{
		keypress=prefix[frames];
		frames++;
		printf("%d\n",keypress);
		fflush(0);
	}
	return keypress;
}

void printStatistics(){
	std::cout << "Finished";
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
		MaskedRAM = new float[RAMCount];
    }
    else{
        std::cout << "Unable to open RAMMap file: " << path;
        exit(-1);
    }
}

int parseInt2(FILE* file, char delim){
    char buff[11];
    char c;
    int i=0;
	while((c = fgetc(file)) != delim){
        	buff[i]=c;
        	i++;
	}
	int acc=0;
	int j;
	for(j=0;j<i-1;j++){
        acc=(acc+buff[j]-'0')*10;
	}
	acc+=buff[j]-'0';
	return acc;
}

/*
float atof2(char* num)
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


float parseFloat2(FILE* file, char delim){
    char buff[10];
    char c;
    int i=0;
    buff[9]='\000';
	while((c = fgetc(file)) != delim){
        	buff[i]=c;
        	i++;
	}
	return (float) atof2(buff);
}
*/

RNN* deserializeChromosome(FILE* file){
	char c;
	for(unsigned int i=0;i<3;i++){
		while((c = fgetc(file)) != '\n');
	}
    for(unsigned int i=0;i<10;i++){
		fgetc(file);
	}
	ramPath = new char[100];
	int i=0;
	while((c = fgetc(file)) != ':'){
        	ramPath[i]=c;
        	i++;
	}
	ramPath[i]='\000';
	prefixLength=parseInt2(file, ':');
	prefix = new uint8[prefixLength];
	for(unsigned int i=0;i<prefixLength;i++){
		prefix[i]=parseInt2(file,';');
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
		fclose(file);
		return 1;
	}
	else{
		//error, cerrar fceux!
		return 0;
	}
}

