#ifndef GENERATIONOBSERVER_H
#define GENERATIONOBSERVER_H

#include <EvolutionObserver.h>

template <class T>
class GenerationObserver : public EvolutionObserver<T>
{
    private:
        char* msg;

    public:
        GenerationObserver(char* msg){
            this->msg = msg;
        }
        virtual ~GenerationObserver(){
            delete[] msg;
        }
        virtual void populationUpdate(PopulationData<T>* data){
            printf("%s %d statistics:\n", this->msg, data->generationNumber);
            printf("\tBest fitness: %lf\n", data->bestCandidateFitness);
            printf("\tMean fitness: %lf\n", data->meanFitness);
            printf("\tFitness SD: %lf\n", data->fitnessStandardDeviation);
            printf("\tElapsed time: %ld\n", data->elapsedTime);
            printf("\n\n");
        }
};

#endif // GENERATIONOBSERVER_H
