#ifndef TERMINATION_CONDITION_H
#define TERMINATION_CONDITION_H

#include "PopulationData.h"

template <class T>
class TerminationCondition
{
    /**
        Indicates if a termination criterion has been reached over the population state.
     */
    public:
        TerminationCondition(){}
        virtual ~TerminationCondition(){}
        virtual bool shouldTerminate(PopulationData<T>* populationData)=0;
        virtual TerminationCondition* clone()=0;
};

#endif
