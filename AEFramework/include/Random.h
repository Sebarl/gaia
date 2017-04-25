#ifndef RANDOM_INTERFACE_H
#define RANDOM_INTERFACE_H

class Random{
    public:
        Random(){}
        virtual ~Random(){}

        /**
            Return next int
        */

        virtual unsigned int nextInt()=0;

        /**
            Return next int from 0 (inclusive) to n (exclusive)
        */

        virtual unsigned int nextInt(unsigned int n)=0;

        /**
            Random float between 0.0 and 1.0
        */

        virtual float nextFloat()=0;

        /**
            Random double between 0.0 and 1.0
        */

        virtual double nextDouble()=0;

        /**
            Random value from Normal distribution with mean 0 and standard deviation 1.0
        */

        virtual double nextGaussian()=0;

        virtual bool nextBool()=0;
};

#endif
