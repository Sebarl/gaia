#include "Sigmoid.h"
#include <math.h>
#include <stdio.h>

Sigmoid::Sigmoid(Random* rng){
    this->k = 2.0*rng->nextFloat() - 1.0;
    this->minusK = -k;
}

Sigmoid::Sigmoid(){
    this->k = 1.0;
    this->minusK = -1.0;
}

Sigmoid::Sigmoid(float k){
    this->k = k;
    this->minusK = -k;
}

Sigmoid::~Sigmoid(){}

float Sigmoid::evaluate(float x) {
    return 1/(1 + exp(minusK * x));
}

float Sigmoid::differentiate(float x) {
   float evaluated = evaluate(x);
   return k*evaluated*(1-evaluated);
}

void Sigmoid::mutate(Random* rng){
    this->k+= this->k * 0.3*rng->nextGaussian();
    this->minusK = -this->k;
}

int Sigmoid::serialize(char* buff, int length)const{
    length+=sprintf(buff+length,"%u;%f;",1,this->k);
    return length;
}

Sigmoid* Sigmoid::clone(){
    return new Sigmoid(k);
}
