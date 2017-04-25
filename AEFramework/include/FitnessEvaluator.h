#ifndef FITNESS_EVALUATOR_H
#define FITNESS_EVALUATOR_H

#include <vector>

template <class T>
class FitnessEvaluator{
    public:
        FitnessEvaluator(){};
        virtual ~FitnessEvaluator(){};
        virtual double getFitness(const int index, std::vector<T*> population) =0;
        virtual bool isNatural() =0;
};

#endif
