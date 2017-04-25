#ifndef IDENTITY_H
#define IDENTITY_H

#include "ActivationFunction.h"


class Identity : public ActivationFunction
{
    public:
        Identity();
        virtual ~Identity();

        virtual float evaluate(float x);
        virtual float differentiate(float x);
        virtual void mutate(Random* rng);
        virtual int serialize(char* buff, int length)const;
        virtual Identity* clone();
};

#endif // IDENTITY_H
