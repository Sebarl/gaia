#ifndef EVOLUTION_OBSERVER_H
#define EVOLUTION_OBSERVER_H

#include "PopulationData.h"

template <class T>
class EvolutionObserver
{
    /**
     Invoked at the end of each generation to monitor the population state.
     */
    public:
        EvolutionObserver(){}
        virtual ~EvolutionObserver(){}
        virtual void populationUpdate(PopulationData<T>* data)=0;
};

#endif
