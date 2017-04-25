#ifndef EVOLUTIONARY_ENGINE_H
#define EVOLUTIONARY_ENGINE_H

#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "TerminationCondition.h"
#include "EvaluatedCandidate.h"
#include "EvolutionObserver.h"
#include "FinalStatistics.h"
#include "GenericException.h"

template <class T>
class EvolutionEngine{
    public:

        EvolutionEngine(){}

        virtual ~EvolutionEngine(){
            for(unsigned int i=0; i<observers.size();i++){
                delete observers[i];
            }
            for(unsigned int i=0; i<satisfiedTerminationConditions.size();i++){
                delete satisfiedTerminationConditions[i];
            }
        }

        /**
            Execute evolution until one of the termination conditions is met
        */
        virtual FinalStatistics<T>*  evolve(std::vector<TerminationCondition<T>*> terminationConditions) =0;


    /**
     * Execute the evolutionary algorithm until one of the termination conditions is met,
     * then return the fittest candidate from the final generation.  To return the
     * entire population rather than just the fittest candidate, use the
     * {@link #evolvePopulation(int, int, Collection, TerminationCondition[])}
     * method instead.
     * @param populationSize The number of candidate solutions present in the population
     * at any point in time.
     * @param eliteCount The number of candidates preserved via elitism.  In elitism, a
     * sub-set of the population with the best fitness scores are preserved unchanged in
     * the subsequent generation.  Candidate solutions that are preserved unchanged through
     * elitism remain eligible for selection for breeding the remainder of the next generation.
     * This value must be non-negative and less than the population size.  A value of zero
     * means that no elitism will be applied.
     * @param seedCandidates A set of candidates to seed the population with.  The size of
     * this collection must be no greater than the specified population size.
     * @param conditions One or more conditions that may cause the evolution to terminate.
     * @return The fittest solution found by the evolutionary process.
     * @see #evolve(int,int,TerminationCondition[])
     */

    /**
       Evolve population until termination condition is met, using the candidates provided as seedCandidates
       to initialize the population. This collection size must not exceed that of populationSize.
    */

    virtual FinalStatistics<T>*  evolve(std::vector<T*> seedCandidates, std::vector<TerminationCondition<T>*> terminationConditions) =0;


    /**
     Evolves population until termination condition is met, returning the entire final population.
     */
    virtual FinalStatistics<T>*  evolvePopulation(std::vector<TerminationCondition<T>*> terminationConditions)=0;


   /**
       Evolve population until termination condition is met, using the candidates provided as seedCandidates
       to initialize the population. This collection size must not exceed that of populationSize. Returns the entire final population.
    */
    virtual FinalStatistics<T>* evolvePopulation(std::vector<T*> seedCandidates, std::vector<TerminationCondition<T>*> terminationConditions) =0;


    /**

    */


    /**

    */

    /**
        Adds a listener to receive status updates on the evolution progress.
    */

    virtual void addEvolutionObserver(EvolutionObserver<T>* observer){
        observers.push_back(observer);
    }
    /**
        Removes an evolution progress listener.
    */
    virtual void removeEvolutionObserver(EvolutionObserver<T>* observer){
        for(unsigned int i=0;i<observers.size();i++){
            if(observers[i] == observer){
                observers.erase(observers.begin() + i);
                break;
            }
        }
    }

    /**
        Returns a list of satisfied termination conditions by the actual state of the population.
     */
     virtual const std::vector<TerminationCondition<T>*> getSatisfiedTerminationConditions(){
        if (satisfiedTerminationConditions.empty()){
            throw GenericException("EvolutionEngine has not terminated.");
        }
        //Make a clean copy of the satisified conditions vector in order to
        //enable the engine to handle resource deallocation as it sees fit.
        std::vector<TerminationCondition<T>*> result;
        for(unsigned int i = 0; i<satisfiedTerminationConditions.size();i++){
            result.push_back(satisfiedTerminationConditions[i]->clone());
        }
        return result;
    }

    void setPopulationSize(unsigned int populationSize){
        this->populationSize=populationSize;
    }

    unsigned int getPopulationSize(){
        return this->populationSize;
    }

    protected:

        /**
            Each implementation of the EvolutionEngine interface must handle multiple calls
            to its evolve methods while maintaining PopulationData in a coherent state.
            Check SimpleEvolutionEngine's evolvePopulation implementation for an example of
            how this could be achieved.
        */

        PopulationData<T>* popData;
        std::vector<EvolutionObserver<T>*> observers;
        unsigned int populationSize;
        std::vector<TerminationCondition<T>*> satisfiedTerminationConditions;

        void getPopulationData(std::vector<EvaluatedCandidate<T>*> evaluatedPopulation, bool isNatural, unsigned int eliteCount, int currentGenerationIndex, long startTime){
            if(popData->bestCandidate != NULL){
                delete popData->bestCandidate;
            }
            popData->bestCandidate = evaluatedPopulation[0]->candidate->clone();
            if(isNatural){
                if(popData->bestCandidateFitness < evaluatedPopulation[0]->fitness){
                    popData->bestCandidateFirstGeneration = currentGenerationIndex;
                }
            }
            else{
                if(popData->bestCandidateFitness > evaluatedPopulation[0]->fitness){
                    popData->bestCandidateFirstGeneration = currentGenerationIndex;
                }

            }
            popData->bestCandidateFitness=evaluatedPopulation[0]->fitness;
            double mean = 0;
            for(unsigned int i=0;i<evaluatedPopulation.size();i++){
                mean+=evaluatedPopulation[i]->fitness;
            }
            mean /= (double)evaluatedPopulation.size();
            popData->meanFitness = mean;
            double SD = 0;
            double diff;
            for(unsigned int i=0;i<evaluatedPopulation.size();i++){
                diff = evaluatedPopulation[i]->fitness - mean;
                SD += diff*diff;
            }
            popData->fitnessStandardDeviation = sqrt(SD);
            popData->naturalFitness=isNatural;
            popData->populationSize=evaluatedPopulation.size();
            popData->eliteCount=eliteCount;
            popData->generationNumber=currentGenerationIndex;
            popData->elapsedTime=difftime(time(0),startTime);

        }

         virtual void notifyPopulationChange(PopulationData<T>* data){
            for (unsigned int i = 0; i<observers.size(); i++){
                observers[i]->populationUpdate(data);
            }
        }
};

#endif
