#ifndef PINBALLFACTORY_H
#define PINBALLFACTORY_H

#include <CandidateFactory.h>
#include "Solution.h"
#include "Random.h"
#include "Parameters.h"

class PinballFactory : public CandidateFactory<Solution>
{
    public:
        PinballFactory(Parameters* params);
        virtual ~PinballFactory();
        virtual Solution* generateRandomCandidate(Random* rng);
    private:
        Parameters* params;
};

#endif // PINBALLFACTORY_H
