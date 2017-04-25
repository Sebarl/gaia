#include "Tanh.h"
#include <math.h>
#include <stdio.h>

Tanh::Tanh(Random* rng){
    this->k = 2.0*rng->nextFloat();
}

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

void Tanh::mutate(Random* rng){
    this->k+= this->k * 0.1*rng->nextGaussian();
}

int Tanh::serialize(char* buff, int length)const{
    length+=sprintf(buff+length,"%u;%f;",2,this->k);
    return length;
}

Tanh* Tanh::clone(){
    return new Tanh(k);
}
