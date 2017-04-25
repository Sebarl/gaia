#include "PinballCrossoverOperator.h"
#include <stdlib.h>
#include <string.h>

PinballCrossoverOperator::PinballCrossoverOperator(unsigned int crossoverPoints){
    if (crossoverPoints == 0){
        throw GenericException("Number of crossover points must be positive.");
    }
    this->crossoverPoints = crossoverPoints;
    this->probability = 1.0;
}

PinballCrossoverOperator::PinballCrossoverOperator(unsigned int crossoverPoints, float crossoverProbability){
     if (crossoverPoints == 0){
        throw GenericException("Number of crossover points must be positive.");
     }
     if (crossoverProbability >1 || crossoverProbability < 0){
        throw GenericException("Crossover probability must be between 0.0 and 1.0.");
     }
    this->crossoverPoints = crossoverPoints;
    this->probability = crossoverProbability;
}

PinballCrossoverOperator::~PinballCrossoverOperator(){}

inline void swap(float* g1, float* g2, unsigned int lower, unsigned int upper){
    int length = upper - lower + 1;
    int start=sizeof(float)*lower;
    void* temp = malloc(length*sizeof(float));
    memcpy(temp, g1+start, sizeof(float)*length);
    memcpy(g1+start, g2+start, sizeof(float)*length);
    memcpy(g2+start, temp, sizeof(float)*length);
    free(temp);
}

void PinballCrossoverOperator::mate(Solution* parent1, Solution* parent2, Random* rng){
    std::vector<Solution*> result;
    std::vector<float>* p1 = parent1->getSolution();
    std::vector<float>* p2 = parent2->getSolution();
    float* ap1 = &(*p1)[0];
    float* ap2 = &(*p2)[0];
    if(crossoverPoints>=p1->size()-1){
        float temp;
        for(unsigned int i=0; i<p1->size(); i+=2){
            temp = ap1[i];
            ap1[i] = ap2[i];
            ap2[i]=temp;
        }
    }
    else{
        int* arr = new int[p1->size()-1];
        int j;
        for(unsigned int i = 1; i < p1->size();i++){
            j = rng->nextInt(i);
            arr[i-1] = arr[j];
            arr[j] = i;
        }
        for(unsigned int i = 0; i<crossoverPoints;i++){
            swap(ap1,ap2, 0, arr[i]);
        }
        delete [] arr;
    }
}
