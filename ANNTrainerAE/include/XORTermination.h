#ifndef XORTERMINATION_H
#define XORTERMINATION_H

#include <TerminationCondition.h>
#include "GenericException.h"
#include "PopulationData.h"

template <class T>
class XORTermination : public TerminationCondition<T>{
    public:
        XORTermination(){
        }

        virtual ~XORTermination(){}

        virtual bool shouldTerminate(PopulationData<T>* populationData){
            return populationData->bestCandidateFitness >= 5.0;
        }

        virtual TerminationCondition<T>* clone(){
            return new XORTermination();
        }

};

#endif // GENERATIONCOUNT_H
