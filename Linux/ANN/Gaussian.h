#ifndef GAUSSIAN_H
#define GAUSSIAN_H

#include "ActivationFunction.h"
#include <math.h>

class Gaussian : public ActivationFunction {
    public:
        Gaussian();
        Gaussian(float m, float s);
        virtual ~Gaussian();

        virtual float evaluate(float x);
        virtual float differentiate(float x);
    protected:
        static const float inv_sqrt2pi = 0.398942280401432677939946059934381868475858631164934657665;

        float m;
        float s;
};

#endif // GAUSSIAN_H
