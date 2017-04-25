#ifndef RELU_H
#define RELU_H

#include "ActivationFunction.h"
#include "Random.h"

class ReLU : public ActivationFunction {
    public:
        ReLU();
        ReLU(float alpha);
        ReLU(Random* rng);
        virtual ~ReLU();

        virtual float evaluate(float x);
        virtual float differentiate(float x);
        virtual void mutate(Random* rng);
        virtual int serialize(char* buff, int length)const;
        virtual ReLU* clone();
    protected:
        float alpha;
};

#endif // RELU_H
