#include "Identity.h"
#include <stdio.h>

Identity::Identity(){}

Identity::~Identity(){}

float Identity::evaluate(float x){
    return x;
}

float Identity::differentiate(float x){
    return 1;
}

void Identity::mutate(Random* rng){}

int Identity::serialize(char* buff, int length)const{
    length+=sprintf(buff+length,"%u;",0);
    return length;
}

Identity* Identity::clone(){
    return new Identity(); //OH YEAH!
}
