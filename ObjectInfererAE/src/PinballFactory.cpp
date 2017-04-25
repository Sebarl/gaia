#include "PinballFactory.h"
#include <math.h>
#include "Constants.h"

PinballFactory::PinballFactory(Parameters* params){
    this->params = params;
}

PinballFactory::~PinballFactory(){}

Solution* PinballFactory::generateRandomCandidate(Random* rng){
        std::vector<float>* sol = new std::vector<float>;
        for(int i=0; i<params->RAMCount;i++){
            sol->push_back((rng->nextFloat()*2.0 - 1.0)*INTERVAL_LENGTH);
        }
        return new Solution(sol);
}
