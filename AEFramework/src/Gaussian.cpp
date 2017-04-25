#include "Gaussian.h"
#include <math.h>
#include <stdio.h>

Gaussian::Gaussian(Random* rng){
    this->m = rng->nextFloat();
    this->s = rng->nextFloat() + 1.01;
}

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

void Gaussian::mutate(Random* rng){
    this->s+= this->s * 0.1*rng->nextGaussian();
    if(this-> s <= 0) {
        this-> s = 0.0001;
    }
    this->m+= this->m * 0.1*rng->nextGaussian();
}

int Gaussian::serialize(char* buff, int length)const{
    length+=sprintf(buff+length,"%u;%f;%f;",4,this->m, this->s);
    return length;
}

Gaussian* Gaussian::clone(){
    return new Gaussian(m, s);
}
