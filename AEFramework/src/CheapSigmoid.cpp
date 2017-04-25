#include "CheapSigmoid.h"
#include <math.h>
#include <stdio.h>

CheapSigmoid::CheapSigmoid() : Sigmoid(){}

CheapSigmoid::CheapSigmoid(float k) : Sigmoid(k){}

CheapSigmoid::CheapSigmoid(Random* rng) : Sigmoid(rng){}

CheapSigmoid::~CheapSigmoid(){}

float CheapSigmoid::evaluate(float x) {
    float v = minusK*x;
    return v/(1+fabs(v));
}

float CheapSigmoid::differentiate(float x) {
   float denom = fabs(minusK*x)+1;
   denom*= denom;
   return minusK/denom;
}

int CheapSigmoid::serialize(char* buff, int length)const{
    length+=sprintf(buff+length,"%u;%f;",3,this->k);
    return length;
}

CheapSigmoid* CheapSigmoid::clone(){
    return new CheapSigmoid(k);
}

