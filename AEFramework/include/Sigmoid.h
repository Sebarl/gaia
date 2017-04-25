#ifndef SIGMOID_H
#define SIGMOID_H

#include "ActivationFunction.h"
#include "Random.h"

class Sigmoid : public ActivationFunction {
    public:
        Sigmoid();
        Sigmoid(float k);
        Sigmoid(Random* rng);
        virtual ~Sigmoid();

        virtual float evaluate(float x);
        virtual float differentiate(float x);
        virtual void mutate(Random* rng);
        virtual int serialize(char* buff, int length)const;
        virtual Sigmoid* clone();
    protected:
        float k;
        float minusK;
};

#endif // SIGMOID_H
