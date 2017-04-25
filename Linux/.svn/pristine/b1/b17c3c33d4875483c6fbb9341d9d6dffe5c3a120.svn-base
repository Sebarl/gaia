#include "AI.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>




//static parameters definition


//variables declaration

uint8* RAMs;
int frameCount = 0;

void HELP(char* c){
	FILE* file = fopen("HELP", "a");
	if(file != NULL){
		fprintf(file, c);
		fclose(file);
	}
}

//RAM analysis
#define BLAH 2048
int arrCount = 0;

uint8 runAI(uint8* RAM, bool& exitNow){
	if(frameCount>=arrCount){
		exitNow = true;
		return 0;
	}
		memcpy(RAM, &RAMs[BLAH*frameCount], sizeof(uint8)*BLAH);
	frameCount++;
	return 8;
}


void printStatistics(){
	
	delete [] RAMs;	
}


int initializeParams(std::string s){
	std::ifstream in(s.c_str());
		in >> arrCount;
		unsigned int blah;
		RAMs = new uint8[BLAH*arrCount];	
		for(unsigned long long i=0; i <BLAH*arrCount;i++){
			in >> blah;
			RAMs[i] = blah;
		}
		in.close();
}


