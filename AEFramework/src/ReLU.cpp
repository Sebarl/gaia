#include "ReLU.h"
#include <math.h>
#include <stdio.h>

ReLU::ReLU(Random* rng){
    this->alpha = 0.5*(rng->nextFloat() + 1.0);
}

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

void ReLU::mutate(Random* rng){
    if(this->alpha < 0) {
        this->alpha = 0.001;
    }
    this->alpha+= this->alpha * 0.1*rng->nextGaussian();
}

int ReLU::serialize(char* buff, int length)const{
    length+=sprintf(buff+length,"%u;%f;",5,this->alpha);
    return length;
}

ReLU* ReLU::clone(){
    return new ReLU(alpha);
}
