#ifndef MIGRATIONOPERATOR_H
#define MIGRATIONOPERATOR_H

#include "EvaluatedCandidate.h"
#include <vector>
#include "Random.h"

template <class T>
class MigrationOperator
{
    public:
        MigrationOperator(){}
        virtual ~MigrationOperator(){}
        virtual void migrate(std::vector<std::vector<EvaluatedCandidate<T>*> > &islandPopulations, unsigned int migrationCount, Random* rng)=0;
};

#endif // MIGRATIONOPERATOR_H
