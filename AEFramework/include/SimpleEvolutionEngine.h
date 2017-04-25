#ifndef SIMPLE_ENGINE_H
#define SIMPLE_ENGINE_H

#include <vector>
#include "EvolutionEngine.h"
#include "EvolutionObserver.h"
#include "Random.h"
#include "CandidateFactory.h"
#include "FitnessEvaluator.h"
#include "TerminationCondition.h"
#include <exception>
#include "GenericException.h"
#include <cmath>
#include "PopulationData.h"
#include "EvolutionUtils.h"
#include "EvolutionaryOperator.h"
#include "SelectionStrategy.h"


template <class T>
class SimpleEvolutionEngine : public EvolutionEngine<T>{

    protected:
        Random* rng;
        CandidateFactory<T>* candidateFactory;
        FitnessEvaluator<T>* fitnessEvaluator;
        EvolutionaryOperator<T>* evolutionScheme;
        SelectionStrategy<T>* selectionStrategy;
        unsigned int eliteCount;


    /**
        Creates a new evolution engine by specifying the various components required by
        an evolutionary algorithm.
     */
    public:

        void setEliteCount(int eliteCount){
            this->eliteCount=eliteCount;
        }

        int getEliteCount(){
            return this->eliteCount;
        }

        SimpleEvolutionEngine(){}

        SimpleEvolutionEngine(unsigned int populationSize, unsigned int eliteCount, CandidateFactory<T>* candidateFactory, EvolutionaryOperator<T>* evolutionScheme, FitnessEvaluator<T>* fitnessEvaluator, SelectionStrategy<T>* selectionStrategy, Random* rng){
            this->candidateFactory = candidateFactory;
            this->fitnessEvaluator = fitnessEvaluator;
            this->rng = rng;
            this->evolutionScheme = evolutionScheme;
            this->selectionStrategy = selectionStrategy;
            this->popData = NULL;
            this->populationSize = populationSize;
            this->eliteCount = eliteCount;
        }

        virtual ~SimpleEvolutionEngine(){
            delete this->rng;
            delete this->candidateFactory;
            delete this->fitnessEvaluator;
            if(this->evolutionScheme != NULL){
                delete this->evolutionScheme;
            }
            if(this->selectionStrategy != NULL){
                delete this->selectionStrategy;
            }
        }


        virtual FinalStatistics<T>*  evolve(std::vector<TerminationCondition<T>*> terminationConditions){
            std::vector<T*> a;
            return evolve(a, terminationConditions);
        }

        virtual FinalStatistics<T>*  evolve(std::vector<T*> seedCandidates, std::vector<TerminationCondition<T>*> terminationConditions){
            FinalStatistics<T>* fs = evolvePopulation(seedCandidates, terminationConditions);
            for(unsigned int i=0; i<fs->finalPopulation.size();i++){
                delete fs->finalPopulation[i];
            }
            fs->finalPopulation.clear();
            return fs;
        }

        virtual FinalStatistics<T>*  evolvePopulation(std::vector<TerminationCondition<T>*> terminationConditions){
            std::vector<T*> a;
            return evolvePopulation(a, terminationConditions);
        }

        virtual FinalStatistics<T>* evolvePopulation(std::vector<T*> seedCandidates, std::vector<TerminationCondition<T>*> terminationConditions){

            if (eliteCount < 0 || eliteCount >= this->populationSize){
                throw GenericException("Elite count must be non-negative and less than population size.");
            }
            if (terminationConditions.size() == 0){
                throw GenericException("At least one TerminationCondition must be specified.");
            }

            //termination conditions reset

            if(!this->satisfiedTerminationConditions.empty()){
                for(unsigned int i = 0; i<this->satisfiedTerminationConditions.size();i++){
                    delete this->satisfiedTerminationConditions[i];
                }
                this->satisfiedTerminationConditions.clear();
            }

            //PopulationData reset, in order to ensure a coherent state.

            this->popData = new PopulationData<T>;
            this->popData->bestCandidateFirstGeneration=0;
            this->popData->bestCandidate = NULL;
            if(this->fitnessEvaluator->isNatural()){
                this->popData->bestCandidateFitness=0;
            }
            else{
                this->popData->bestCandidateFitness=INFINITY;
            }

            int currentGenerationIndex = 0;
            long startTime = time(0);

            std::vector<T*> population = candidateFactory->generateInitialPopulation(this->populationSize, seedCandidates, rng);

            // Calculate the fitness scores for each member of the initial population.
            std::vector<EvaluatedCandidate<T>*> evaluatedPopulation = evaluatePopulation(population);
            sortEvaluatedPopulation(evaluatedPopulation, fitnessEvaluator->isNatural());
            this->getPopulationData(evaluatedPopulation, fitnessEvaluator->isNatural(), eliteCount, currentGenerationIndex,startTime);

            // Notify observers of the state of the population.
            this->notifyPopulationChange(this->popData);

            std::vector<TerminationCondition<T>*> satisfiedConditions = shouldContinue(this->popData,terminationConditions);
            while (satisfiedConditions.empty()){
                ++currentGenerationIndex;
                evaluatedPopulation = nextEvolutionStep(evaluatedPopulation, eliteCount, rng);
                sortEvaluatedPopulation(evaluatedPopulation, fitnessEvaluator->isNatural());
                this->getPopulationData(evaluatedPopulation, fitnessEvaluator->isNatural(), eliteCount, currentGenerationIndex, startTime);

                // Notify observers of the state of the population.
                this->notifyPopulationChange(this->popData);
                satisfiedConditions = shouldContinue(this->popData, terminationConditions);
            }

            this->satisfiedTerminationConditions = satisfiedConditions;

            //limpieza de las termination conditions que no me interesan
            typename std::vector<TerminationCondition<T>*>::iterator iter;
            for(unsigned int i = 0; i<satisfiedConditions.size(); i++){
                iter = std::find(terminationConditions.begin(), terminationConditions.end(), satisfiedConditions[i]);
                if( iter != terminationConditions.end()) {

                       terminationConditions.erase(iter);

                }
            }
            for(unsigned int i = 0; i<terminationConditions.size(); i++){
                delete terminationConditions[i];
            }
            FinalStatistics<T>* fs = new FinalStatistics<T>;
            fs->data = this->popData;
            fs->finalPopulation = evaluatedPopulation;
            this->popData = NULL;
            return fs;
        }



    protected:
        virtual std::vector<EvaluatedCandidate<T>*> nextEvolutionStep(std::vector<EvaluatedCandidate<T>*> evaluatedPopulation, int eliteCount, Random* rng){

            // First perform any elitist selection.
            std::vector<T*> elite;
            for(int i = 0; i < eliteCount;i++){
                elite.push_back(evaluatedPopulation[i]->candidate->clone());
            }

            // Then select candidates that will be operated on to create the evolved
            // portion of the next generation.
            std::vector<T*> population = selectionStrategy->select(evaluatedPopulation,
                                                       fitnessEvaluator->isNatural(),
                                                       evaluatedPopulation.size() - eliteCount,
                                                       rng);
            for(unsigned int i = 0; i<evaluatedPopulation.size();i++){
                delete evaluatedPopulation[i];
            }
            evaluatedPopulation.clear();
            population = evolutionScheme->apply(population, rng);

             for(int i = 0; i < eliteCount;i++){
                population.push_back(elite[i]);
            }

            return evaluatePopulation(population);
        }

        virtual std::vector<EvaluatedCandidate<T>*> evaluatePopulation(std::vector<T*> population){
            std::vector<EvaluatedCandidate<T>*> evaluatedPopulation;
            EvaluatedCandidate<T>* ec;
            for (unsigned int i = 0; i< population.size(); i++){
                    ec = new EvaluatedCandidate<T>;
                    ec->candidate = population[i];
                    ec->fitness = fitnessEvaluator->getFitness(i, population);
                    evaluatedPopulation.push_back(ec);
                }
            return evaluatedPopulation;
        }



};

#endif
