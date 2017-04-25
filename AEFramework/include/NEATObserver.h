#ifndef NEATOBSERVER_H
#define NEATOBSERVER_H

#include <EvolutionObserver.h>
#include <NEATGenome.h>

class NEATObserver: public EvolutionObserver<NEATGenome>
{
    private:
        char* msg;

    public:
        NEATObserver(char* msg){
            this->msg = msg;
        }
        virtual ~NEATObserver(){
            delete[] msg;
        }
        virtual void populationUpdate(PopulationData<NEATGenome>* data){
            printf("%s %d statistics:\n", this->msg, data->generationNumber);
            printf("\tBest fitness: %lf\n", data->bestCandidateFitness);
            printf("\tMean fitness: %lf\n", data->meanFitness);
            printf("\tFitness SD: %lf\n", data->fitnessStandardDeviation);
            printf("\tElapsed time: %ld\n", data->elapsedTime);
            printf("\n\n");
            NEATGenome* candidate = (NEATGenome*)data->bestCandidate;
            printf("\tHidden neuron count: %lu\n",candidate->neurons.size()-candidate->numInputs - 9);
            printf("\tLinks count:%lu\n",candidate->links.size());
        }
};

#endif // GENERATIONOBSERVER_H
