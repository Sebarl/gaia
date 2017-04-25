#include "ReLU.h"
#include <math.h>

ReLU::ReLU(){
    this->alpha = 0.0;
}

ReLU::ReLU(float alpha){
    this->alpha = alpha;
}

ReLU::~ReLU(){}

float ReLU::evaluate(float x) {
    return (x<0)? alpha*x : x;
}

float ReLU::differentiate(float x) {
	return (x<0)? alpha : 1.0;
}

