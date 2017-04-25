#ifndef ACTIVATIONFUNCTION_H
#define ACTIVATIONFUNCTION_H

#include "Random.h"

enum ActivationFunctionType{IDENTITY, SIGMOID, TANH, CHEAPSIGMOID, GAUSSIAN, RELU};

class ActivationFunction{
    public:
        ActivationFunction(){}
        virtual ~ActivationFunction(){}

        virtual float evaluate(float x)=0;
        virtual float differentiate(float x)=0;
        virtual void mutate(Random* rng)=0;
        virtual int serialize(char* buff, int length)const=0;
        virtual ActivationFunction* clone()=0;
};

#endif // ACTIVATIONFUNCTION_H
