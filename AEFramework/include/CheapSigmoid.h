#ifndef CHEAPSIGMOID_H
#define CHEAPSIGMOID_H

#include "Sigmoid.h"
#include "Random.h"

class CheapSigmoid : public Sigmoid {
    public:
        CheapSigmoid();
        CheapSigmoid(float k);
        CheapSigmoid(Random* rng);
        virtual ~CheapSigmoid();

        virtual float evaluate(float x);
        virtual float differentiate(float x);
        virtual int serialize(char* buff, int length)const;
        virtual CheapSigmoid* clone();
};

#endif // CHEAPSIGMOID_H
