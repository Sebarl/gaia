#include <stdlib.h>
#include <cstring>
#include "PinballEvaluator.h"
#include "Constants.h"
#include <stdio.h>

PinballEvaluator::PinballEvaluator(Parameters* params, int islandIndex){
    this->params = params;
    this->islandIndex = islandIndex;
}

PinballEvaluator::~PinballEvaluator(){}

inline double evalRAM(const float* video, Parameters* params, std::vector<float>* weights) {
    double sum = 0.0;
    int index = 0;
    for(int j = 0; j < 2048 && index < params->RAMCount; j++){
        if(params->RAMMap[index] == j){
            sum+= video[j] * (*weights)[index];
            index++;
        }
    }
    return sum;
}

void llsq(float* video, unsigned int cantFrames, float &a, float &b){
	float bot, top, xbar, ybar;
	if (cantFrames == 1){
		a = 0.0;
		b = video[0];
		return;
	}
	xbar = 0.0;
	ybar = 0.0;
	for (unsigned int i = 0; i < cantFrames; i++){
		xbar = xbar + i;
		ybar = ybar + video[i];
	}
	xbar = xbar / (float)cantFrames;
	ybar = ybar / (float)cantFrames;

	top = 0.0;
	bot = 0.0;
	for (unsigned int i = 0; i < cantFrames; i++){
		top = top + (i - xbar) * (video[i] - ybar);
		bot = bot + (i - xbar) * (i - xbar);
	}
	a = top / bot;
	b = ybar - a * xbar;
	return;
}

float* PinballEvaluator::evalFrames(const float* video, unsigned int cantFrames, std::vector<float>* weights){
	float* frames = new float[cantFrames];
	for(unsigned int i = 0; i < cantFrames; ++i){
		frames[i] = evalRAM(&video[2048*i], params, weights);
	}
	return frames;
}

double PinballEvaluator::getFitness(const int index, std::vector<Solution*> population){
	Solution* s = population[index];
	std::vector<float>* sol = s->getSolution();

    unsigned int goodLength = params->avgGoodVideo->size() / 2048;
	unsigned int badLength = params->avgBadVideo->size() / 2048;
	float sum;
	unsigned int cantIncr = 0;
	unsigned int cantDecr = 0;
	unsigned int cantEqual = 0;

	float* goodVideo = evalFrames(params->avgGoodVideo->data(), goodLength, sol);
	float* badVideo = evalFrames(params->avgBadVideo->data(), badLength, sol);

	float a,b, extendedValue;
	sum = goodVideo[0] - badVideo[0];
	if(goodLength > badLength) {
		for(unsigned int i = 1; i < badLength; ++i){
			sum += goodVideo[i] - badVideo[i];
			if(goodVideo[i-1] < goodVideo[i]){
                cantIncr++;
			}
			else if(goodVideo[i-1] > goodVideo[i]){
                cantDecr++;
			}
			else{
                cantEqual++;
			}
		}
		llsq(badVideo, badLength, a, b);
		extendedValue = a*badLength + b;
		for(unsigned int i = badLength; i < goodLength; ++i){
			sum += goodVideo[i] - extendedValue;
			if(goodVideo[i-1] < goodVideo[i]){
                cantIncr++;
			}
			else if(goodVideo[i-1] > goodVideo[i]){
                cantDecr++;
			}
			else{
                cantEqual++;
			}
		}
	} else {
		for(unsigned int i = 1; i < goodLength; ++i){
			sum += goodVideo[i] - badVideo[i];
			if(goodVideo[i-1] < goodVideo[i]){
                cantIncr++;
			}
			else if(goodVideo[i-1] > goodVideo[i]){
                cantDecr++;
			}
			else{
                cantEqual++;
			}
		}
		llsq(goodVideo, goodLength, a, b);
		extendedValue = a*goodLength + b;
		for(unsigned int i = goodLength; i < badLength; ++i){
			sum += extendedValue - badVideo[i];
		}
	}

	delete [] goodVideo;
	delete [] badVideo;

	float eqf = cantEqual * EQUAL_FACTOR;
    double fitness = sum * (cantIncr + eqf)/(double)(goodLength-1);

	if(fitness < 0){
        fitness = 1.0 + 1.0/(fitness-1.0);
	}
	else{
        fitness+=1.0;
	}
	return fitness;
}

bool PinballEvaluator::isNatural(){
    return true;
}
