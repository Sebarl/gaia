#include "CheapSigmoid.h"
#include <math.h>

CheapSigmoid::CheapSigmoid() : Sigmoid(){}

CheapSigmoid::CheapSigmoid(float k) : Sigmoid(k){}

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
