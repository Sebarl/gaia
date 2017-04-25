#include "Identity.h"

Identity::Identity(){}

Identity::~Identity(){}

float Identity::evaluate(float x){
    return x;
}

float Identity::differentiate(float x){
    return 1;
}
