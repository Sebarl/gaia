#ifndef ACTIVATIONFUNCTION_H
#define ACTIVATIONFUNCTION_H


class ActivationFunction{
    public:
        ActivationFunction(){}
        virtual ~ActivationFunction(){}

        virtual float evaluate(float x)=0;
        virtual float differentiate(float x)=0;
};

#endif // ACTIVATIONFUNCTION_H
