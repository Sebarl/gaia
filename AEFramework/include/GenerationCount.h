#ifndef GENERATIONCOUNT_H
#define GENERATIONCOUNT_H

#include <TerminationCondition.h>
#include "GenericException.h"
#include "PopulationData.h"

template <class T>
class GenerationCount : public TerminationCondition<T>{
    private:
        int generationCount;
    public:
        GenerationCount(int generationCount){
            if(generationCount<=0){
                throw GenericException("Generation count must be positive");
            }
            this->generationCount = generationCount;
        }

        virtual ~GenerationCount(){}

        virtual bool shouldTerminate(PopulationData<T>* populationData){
            return populationData->generationNumber + 1 >= generationCount;
        }

        virtual TerminationCondition<T>* clone(){
            return new GenerationCount(generationCount);
        }

};

#endif // GENERATIONCOUNT_H
