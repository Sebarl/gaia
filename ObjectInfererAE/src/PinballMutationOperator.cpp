#include "PinballMutationOperator.h"
//#include "Genome.h"
#include "Constants.h"
#include <math.h>

#define EPS 0.00000001
#define BYTES_TO_MUTATE 5

PinballMutationOperator::PinballMutationOperator(float probability){
    this->probability = probability;
}

PinballMutationOperator::~PinballMutationOperator(){}

std::vector<Solution*> PinballMutationOperator::apply(std::vector<Solution*> selectedCandidates, Random* rng){
    for(unsigned int i=0; i< selectedCandidates.size(); i++){
        if(rng->nextFloat() < probability){
            std::vector<float>* candidate = selectedCandidates[i]->getSolution();
            unsigned int cantIndices = candidate->size();
            if(cantIndices > BYTES_TO_MUTATE){
                cantIndices = BYTES_TO_MUTATE;
            }
            unsigned int indices[cantIndices];
            for(unsigned int j = 0; j < cantIndices; ++j){
                bool exists;
                do{
                    exists = false;
                    indices[j] = rng->nextInt(candidate->size());
                    for(unsigned int ind = 0; ind < j; ++ind){
                        if(indices[ind] == indices[j]){
                            exists = true;
                        }
                    }
                }while(exists);
            }
            for(unsigned int j = 0; j < cantIndices; ++j){
                float weight = (*candidate)[indices[j]];
                if(fabs(weight) < EPS) {
                    weight = INTERVAL_LENGTH*(rng->nextFloat()*0.0002 - 0.0001);
                } else {
                    if(rng->nextFloat() < 0.01){
                        weight = (rng->nextFloat()*2.0 - 1.0)*INTERVAL_LENGTH;
                    } else {
                        weight+=rng->nextGaussian()*0.1*weight;
                        if(weight<-INTERVAL_LENGTH){
                            weight=-INTERVAL_LENGTH;
                        }
                        else if(weight>INTERVAL_LENGTH){
                            weight=INTERVAL_LENGTH;
                        }
                    }
                }
				(*candidate)[indices[j]] = weight;
            }
        }

        /*
        for(unsigned int j=0; j<candidate->size();j++){
            if(rng->nextFloat() < probability){
                //mutate the candidate
                float weight = (*candidate)[j];
                if(fabs(weight) < EPS) {
                    weight = INTERVAL_LENGTH*(rng->nextFloat()*0.0002 - 0.0001);
                } else {
                    if(rng->nextFloat() < 0.01){
                        weight = (rng->nextFloat()*2.0 - 1.0)*INTERVAL_LENGTH;
                    } else {
                        weight+=rng->nextGaussian()*0.1*weight;
                        if(weight<-INTERVAL_LENGTH){
                            weight=-INTERVAL_LENGTH;
                        }
                        else if(weight>INTERVAL_LENGTH){
                            weight=INTERVAL_LENGTH;
                        }
                    }
                }
				(*candidate)[j] = weight;
            }
        }
        */
    }
    return selectedCandidates;
}
