#include "Tanh.h"
#include <math.h>

Tanh::Tanh(){
    this->k = 1.0;
}

Tanh::Tanh(float k){
    this->k = k;
}

Tanh::~Tanh(){}

float Tanh::evaluate(float x) {
    return tanh(k*x);
}

float Tanh::differentiate(float x) {
	float cosh2 = cosh(k*x);
	cosh2*= cosh2;
	float denom = cosh(2*k*x) + 1;
	denom*= denom;
	return (4*k*cosh2)/denom;
}
