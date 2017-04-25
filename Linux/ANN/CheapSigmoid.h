#ifndef CHEAPSIGMOID_H
#define CHEAPSIGMOID_H

#include "Sigmoid.h"


class CheapSigmoid : public Sigmoid {
    public:
        CheapSigmoid();
        CheapSigmoid(float k);
        virtual ~CheapSigmoid();

        float evaluate(float x);
        float differentiate(float x);
};

#endif // CHEAPSIGMOID_H
