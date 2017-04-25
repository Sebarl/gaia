#ifndef TANH_H
#define TANH_H

#include "ActivationFunction.h"


class Tanh : public ActivationFunction {
    public:
        Tanh();
        Tanh(float k);
        virtual ~Tanh();

        virtual float evaluate(float x);
        virtual float differentiate(float x);
    protected:
        float k;
};

#endif // TANH_H
