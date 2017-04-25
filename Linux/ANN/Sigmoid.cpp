#include "Sigmoid.h"
#include <math.h>

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
