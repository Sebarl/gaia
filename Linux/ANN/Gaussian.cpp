#include "Gaussian.h"
#include <math.h>

Gaussian::Gaussian(){
    this->m = 0.0;
    this->s = 1.0;
}

Gaussian::Gaussian(float m, float s){
    this->m = m;
    this->s = s;
}

Gaussian::~Gaussian(){}

float Gaussian::evaluate(float x) {
    float modx2 = (x-m)/s;
    modx2*= modx2;
    return inv_sqrt2pi*exp(-0.5*modx2) / s;
}

float Gaussian::differentiate(float x) {
	float g = evaluate(x);
	return (m-x)/(s*s) * g;
}
