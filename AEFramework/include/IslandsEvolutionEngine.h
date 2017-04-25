#ifndef ISLANDSEVOLUTIONENGINE_H
#define ISLANDSEVOLUTIONENGINE_H

#include <EvolutionEngine.h>
#include "GenerationCount.h"
#include "MigrationOperator.h"
#include "GenericException.h"
#include "EvolutionUtils.h"
#include "PlatformSpecific.h"
#include <omp.h>
#include <time.h>

template <class T>
class IslandsEvolutionEngine : public EvolutionEngine<T>
{
    protected:
        std::vector<EvolutionEngine<T>*> islands;
        unsigned int epochLength;
        unsigned int migrationCount;
        MigrationOperator<T>* migration;
        bool isNatural;
        unsigned int numThreads;
        Random* rng;

    public:

        IslandsEvolutionEngine(std::vector<EvolutionEngine<T>*> islands, unsigned int epochLength, unsigned int migrationCount, MigrationOperator<T>* migration, Random* rng, bool isNaturalFitness, unsigned int numThreads){
            this->islands = islands;
            this->populationSize=0;
            unsigned int mini = islands[0]->getPopulationSize();
            unsigned int popSize = 0;
            for(unsigned int i=0; i<islands.size(); i++){
                popSize=islands[i]->getPopulationSize();
                this->populationSize+=popSize;
                if(popSize<mini){
                    mini=popSize;
                }
            }
            if(migrationCount>=mini){
                throw new GenericException("Invalid migration count. The number of individuals to migrate must be smaller than the population of the smallest island.");
            }
            this->epochLength = epochLength;
            this->migrationCount = migrationCount;
            this->migration = migration;
            this->rng = rng;
            this->isNatural = isNaturalFitness;
            this->numThreads = numThreads;
            if(this->numThreads > islands.size()){
                this->numThreads = islands.size();
            }
            this->popData = NULL;
        }


        IslandsEvolutionEngine(std::vector<EvolutionEngine<T>*> islands, unsigned int epochLength, unsigned int migrationCount, MigrationOperator<T>* migration, Random* rng, bool isNaturalFitness){
            this->islands = islands;
            this->populationSize=0;
            unsigned int mini = islands[0]->getPopulationSize();
            unsigned int popSize = 0;
            for(unsigned int i=0; i<islands.size(); i++){
                popSize=islands[i]->getPopulationSize();
                this->populationSize+=popSize;
                if(popSize<mini){
                    mini=popSize;
                }
            }
            if(migrationCount>=mini){
                throw new GenericException("Invalid migration count. The number of individuals to migrate must be smaller than the population of the smallest island.");
            }
            this->epochLength = epochLength;
            this->migrationCount = migrationCount;
            this->migration = migration;
            this->rng = rng;
            this->isNatural = isNaturalFitness;
            this->numThreads = getNumThreads();
            if(this->numThreads > islands.size()){
                this->numThreads = islands.size();
            }
            this->popData = NULL;
        }

        virtual ~IslandsEvolutionEngine(){
            for(unsigned int i=0; i<this->islands.size();i++){
                delete this->islands[i];
            }
            islands.clear();
            delete this->migration;
            delete this->rng;
        }

        virtual FinalStatistics<T>* evolve(std::vector<TerminationCondition<T>*> terminationConditions){
            std::vector<T*> a;
            return evolve(a, terminationConditions);
        }

        virtual FinalStatistics<T>* evolve(std::vector<T*> seedCandidates, std::vector<TerminationCondition<T>*> terminationConditions){
            FinalStatistics<T>* fs = evolvePopulation(seedCandidates, terminationConditions);
            for(unsigned int i=0; i<fs->finalPopulation.size();i++){
                delete fs->finalPopulation[i];
            }
            fs->finalPopulation.clear();
            return fs;
        }

        virtual FinalStatistics<T>* evolvePopulation(std::vector<TerminationCondition<T>*> terminationConditions){
            std::vector<T*> a;
            return evolvePopulation(a, terminationConditions);
        }

        /**
            Assigns all candidates contained in seedCandidates in the provided order to each of this engine
            islands. If there aren't enough candidates, the islands will be responsible for generating the
            missing candidates using its own candidate factories.
        */

        virtual FinalStatistics<T>* evolvePopulation(std::vector<T*> seedCandidates, std::vector<TerminationCondition<T>*> terminationConditions){

             //termination conditions reset

            if(!this->satisfiedTerminationConditions.empty()){
                for(unsigned int i = 0; i<this->satisfiedTerminationConditions.size();i++){
                    delete this->satisfiedTerminationConditions[i];
                }
                this->satisfiedTerminationConditions.clear();
            }

            this->popData = new PopulationData<T>;
            this->popData->bestCandidate = NULL;
            this->popData->bestCandidateFirstGeneration=0;
            if(this->isNatural){
                this->popData->bestCandidateFitness=0;
            }
            else{
                this->popData->bestCandidateFitness=INFINITY;
            }

            int currentGenerationIndex = 0;
            long startTime = time(0);


            unsigned int cantSeeds[islands.size()];
            unsigned int cS = seedCandidates.size()/islands.size();
            for(unsigned int i = 0; i < islands.size(); ++i){
                cantSeeds[i] = cS;
            }
            for(unsigned int i = 0; i < seedCandidates.size()%islands.size(); ++i){
                cantSeeds[i]++;
            }

            for(unsigned int i = 0; i < islands.size() - 1; ++i){
                if(cantSeeds[i] > islands[i]->getPopulationSize()){
                    unsigned int diff = cantSeeds[i] - islands[i]->getPopulationSize();
                    cantSeeds[i]-=diff;
                    cantSeeds[i+1]+=diff;
                }
            }

            if(cantSeeds[islands.size()-1] > islands[islands.size()-1]->getPopulationSize()){
                throw GenericException("Too many seed candidates for the islands");
            }

            std::vector<T*>* seeds = new std::vector<T*>[islands.size()];
            unsigned int index = 0;
            for(unsigned int i=0; i<islands.size(); i++){
                seeds[i]=std::vector<T*>(seedCandidates.begin()+index, seedCandidates.begin()+index+cantSeeds[i]);
                index+=cantSeeds[i];
            }

/*
            std::vector<T*>* seeds = new std::vector<T*>[islands.size()];
            unsigned int lower = 0;
            for(unsigned int i=0; i<islands.size(); i++){
                if(lower+islands[i]->getPopulationSize() < seedCandidates.size()){
                    seeds[i]=std::vector<T*>(seedCandidates.begin()+lower, seedCandidates.begin()+lower+islands[i]->getPopulationSize());
                }
                else{
                    seeds[i]=std::vector<T*>(seedCandidates.begin()+lower, seedCandidates.begin()+seedCandidates.size());
                    break;
                }
                lower+=islands[i]->getPopulationSize();
            }
*/
            FinalStatistics<T>** results = new FinalStatistics<T>*[islands.size()];
            std::vector<EvolutionEngine<T>*> engines = islands;
            std::vector<TerminationCondition<T>*> satisfiedConditions;
            std::vector<std::vector<EvaluatedCandidate<T>*> > islandsPopulations;
            omp_set_dynamic(0);     // Explicitly disable dynamic teams
            omp_set_num_threads(this->numThreads);
            omp_set_nested(1);
            std::vector<std::vector<TerminationCondition<T>*> > tc;
            for(unsigned int i=0; i<islands.size(); i++){
                std::vector<TerminationCondition<T>*> cond;
                cond.push_back(new GenerationCount<T>(this->epochLength));
                tc.push_back(cond);
            }

            #pragma omp parallel shared(results, engines, seeds, tc)
            {
                #pragma omp for
                for(unsigned int i=0; i<engines.size(); i++){
                    results[i] = engines[i]->evolvePopulation(seeds[i],tc[i]);
                 }
            }

            for(unsigned int j = 0; j<islands.size();j++){
                islandsPopulations.push_back(results[j]->finalPopulation);
                results[j]->finalPopulation.clear();
            }

            migration->migrate(islandsPopulations , migrationCount, rng);
            std::vector<EvaluatedCandidate<T>*> mergedPopulation;

            for(unsigned int i=0; i<islandsPopulations.size();i++){
                mergedPopulation.insert(mergedPopulation.end(), islandsPopulations[i].begin(), islandsPopulations[i].end());
            }
            sortEvaluatedPopulation(mergedPopulation,this->isNatural);
            this->getPopulationData(mergedPopulation, this->isNatural, 0, currentGenerationIndex, startTime);
            this->notifyPopulationChange(this->popData);
            satisfiedConditions = shouldContinue(this->popData,terminationConditions);


            while (satisfiedConditions.empty()){
                ++currentGenerationIndex;
                for(unsigned int i = 0; i<islandsPopulations.size();i++){
                    seeds[i].clear();
                    for(unsigned int j = 0; j<islandsPopulations[i].size();j++){
                        seeds[i].push_back(islandsPopulations[i][j]->candidate);
                    }
                }

                omp_set_dynamic(0);     // Explicitly disable dynamic teams
                omp_set_num_threads(this->numThreads);
                omp_set_nested(1);
                tc.clear();
                unsigned int i;
                for(i=0; i<islands.size(); i++){
                    std::vector<TerminationCondition<T>*> cond;
                    cond.push_back(new GenerationCount<T>(this->epochLength));
                    tc.push_back(cond);
                }

                //clear older finalStatistics
                for(unsigned int j = 0; j<engines.size();j++){
                    delete results[j];
                }

                #pragma omp parallel shared(results, engines, seeds, tc)
                {
                    #pragma omp for
                    for(unsigned int i=0; i<engines.size(); i++){
                        results[i] = engines[i]->evolvePopulation(seeds[i],tc[i]);
                     }
                }
                unsigned int quantPops = islandsPopulations.size();
                for(unsigned int i = 0; i<quantPops;i++){
                    islandsPopulations.erase(islandsPopulations.begin());
                }
                for(unsigned int j = 0; j<islands.size();j++){
                    islandsPopulations.push_back(results[j]->finalPopulation);
                    results[j]->finalPopulation.clear();
                }

                migration->migrate(islandsPopulations , migrationCount, rng);

                mergedPopulation.clear();
                for(unsigned int i=0; i<islandsPopulations.size();i++){
                    mergedPopulation.insert(mergedPopulation.end(), islandsPopulations[i].begin(), islandsPopulations[i].end());
                }
                sortEvaluatedPopulation(mergedPopulation,this->isNatural);
                this->getPopulationData(mergedPopulation, this->isNatural, 0, currentGenerationIndex, startTime);
                this->notifyPopulationChange(this->popData);
                satisfiedConditions = shouldContinue(this->popData,terminationConditions);
            }


            //clear older finalStatistics
            for(unsigned int j = 0; j<engines.size();j++){
                delete results[j];
            }
            delete[] results;
            delete[] seeds;
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
            this->satisfiedTerminationConditions = satisfiedConditions;
            FinalStatistics<T>* fs = new FinalStatistics<T>;
            fs->data = this->popData;
            fs->finalPopulation = mergedPopulation;
            this->popData=NULL;
            return fs;
        }
};

#endif // ISLANDSEVOLUTIONENGINE_H
