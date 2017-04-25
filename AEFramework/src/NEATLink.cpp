#include "NEATLink.h"

NEATLink::NEATLink(int in, int out, float weight, bool enabled, int innovationId) {
    this->fromNeuron = in;
    this->toNeuron = out;
    this->weight = weight;
    this->recurrent = false;
    this->enabled = enabled;
    this->innovationId = innovationId;
}

NEATLink::NEATLink(int in, int out, float weight, bool recurrent, bool enabled, int innovationId) {
    this->fromNeuron = in;;
    this->toNeuron = out;
    this->weight = weight;
    this->recurrent = recurrent;
    this->enabled = enabled;
    this->innovationId = innovationId;
}

NEATLink::~NEATLink(){}

void NEATLink::resetWeight(Random* rng){
    this->weight = 2.0*rng->nextFloat() - 1.0;
}

void NEATLink::mutateWeight(Random* rng){
    this->weight+= this->weight * 0.3*rng->nextGaussian();
}

NEATLink* NEATLink::clone(){
    return new NEATLink(fromNeuron,toNeuron,weight,recurrent,enabled,innovationId);
}
