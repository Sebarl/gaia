#ifndef PARALLELEVOLUTIONENGINE_H
#define PARALLELEVOLUTIONENGINE_H

#include <SimpleEvolutionEngine.h>
#include "PlatformSpecific.h"
#include <omp.h>

template <class T>
class ParallelEvolutionEngine : public SimpleEvolutionEngine<T>
{
    protected:
        unsigned int numThreads;
    public:

        ParallelEvolutionEngine(){}
        ParallelEvolutionEngine(unsigned int populationSize,unsigned int eliteCount, CandidateFactory<T>* candidateFactory, EvolutionaryOperator<T>* evolutionScheme, FitnessEvaluator<T>* fitnessEvaluator, SelectionStrategy<T>* selectionStrategy, Random* rng, unsigned int numThreads)
        {
             this->candidateFactory = candidateFactory;
            this->fitnessEvaluator = fitnessEvaluator;
            this->rng = rng;
            this->evolutionScheme = evolutionScheme;
            this->selectionStrategy = selectionStrategy;
            this->popData = NULL;
            this->populationSize = populationSize;
            this->eliteCount = eliteCount;
            this->numThreads = numThreads;
            if(this->numThreads > populationSize){
                this->numThreads = populationSize;
            }
        }

        ParallelEvolutionEngine(unsigned int populationSize,unsigned int eliteCount, CandidateFactory<T>* candidateFactory, EvolutionaryOperator<T>* evolutionScheme, FitnessEvaluator<T>* fitnessEvaluator, SelectionStrategy<T>* selectionStrategy, Random* rng)
    {
             this->candidateFactory = candidateFactory;
            this->fitnessEvaluator = fitnessEvaluator;
            this->rng = rng;
            this->evolutionScheme = evolutionScheme;
            this->selectionStrategy = selectionStrategy;
            this->popData = NULL;
            this->populationSize = populationSize;
            this->eliteCount = eliteCount;
            this->numThreads = getNumThreads();
            if(this->numThreads > populationSize){
                this->numThreads = populationSize;
            }
        }

        virtual ~ParallelEvolutionEngine(){
        }

        virtual std::vector<EvaluatedCandidate<T>*> evaluatePopulation(std::vector<T*> population){

            EvaluatedCandidate<T>** resultArray = new EvaluatedCandidate<T>*[population.size()];
            omp_set_dynamic(0);     // Explicitly disable dynamic teams
            omp_set_num_threads(this->numThreads);
            #pragma omp parallel shared(resultArray, population)
            {
                EvaluatedCandidate<T>* ec;
                #pragma omp for
                for (unsigned int i = 0; i< population.size(); i++){
                    ec = new EvaluatedCandidate<T>;
                    ec->candidate = population[i];
                    ec->fitness = this->fitnessEvaluator->getFitness(i, population);
                    resultArray[i]=ec;
                }
            }
            std::vector<EvaluatedCandidate<T>*> evaluatedPopulation(resultArray, resultArray + population.size());
            delete[] resultArray;
            return evaluatedPopulation;
        }
};

#endif // PARALLELEVOLUTIONENGINE_H
