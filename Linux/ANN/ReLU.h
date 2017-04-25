#ifndef RELU_H
#define RELU_H

#include "ActivationFunction.h"

class ReLU : public ActivationFunction {
    public:
        ReLU();
        ReLU(float alpha);
        virtual ~ReLU();

        virtual float evaluate(float x);
        virtual float differentiate(float x);
    protected:
        float alpha;
};

#endif // RELU_H
