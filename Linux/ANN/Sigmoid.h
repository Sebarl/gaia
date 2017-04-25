#ifndef SIGMOID_H
#define SIGMOID_H

#include "ActivationFunction.h"


class Sigmoid : public ActivationFunction {
    public:
        Sigmoid();
        Sigmoid(float k);
        virtual ~Sigmoid();

        virtual float evaluate(float x);
        virtual float differentiate(float x);
    protected:
        float k;
        float minusK;
};

#endif // SIGMOID_H
