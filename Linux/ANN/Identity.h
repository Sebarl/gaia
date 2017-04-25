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
};

#endif // IDENTITY_H
