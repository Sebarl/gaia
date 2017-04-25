#ifndef TANH_H
#define TANH_H

#include "ActivationFunction.h"
#include "Random.h"

class Tanh : public ActivationFunction {
    public:
        Tanh();
        Tanh(float k);
        Tanh(Random* rng);
        virtual ~Tanh();

        virtual float evaluate(float x);
        virtual float differentiate(float x);
        virtual void mutate(Random* rng);
        virtual int serialize(char* buff, int length)const;
        virtual Tanh* clone();
    protected:
        float k;
};

#endif // TANH_H
