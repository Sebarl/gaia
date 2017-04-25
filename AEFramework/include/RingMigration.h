#ifndef RINGMIGRATION_H
#define RINGMIGRATION_H

#include "MigrationOperator.h"
#include "EvolutionUtils.h"

template <class T>
class RingMigration : public MigrationOperator<T>
{
    public:
        RingMigration(){}
        virtual ~RingMigration(){}
        virtual void migrate(std::vector<std::vector<EvaluatedCandidate<T>*> > &islandPopulations, unsigned int migrationCount, Random* rng){

           if(islandPopulations.size() == 1){
                return;
            }
            int* immigrantIndexes = new int[migrationCount];
            int* emigrantIndexes = new int[migrationCount];
            EvaluatedCandidate<T>** tempCandidates = new EvaluatedCandidate<T>*[migrationCount];

            getMigrationIndexes(immigrantIndexes, migrationCount, islandPopulations[0].size(), rng);
            getMigrationIndexes(emigrantIndexes, migrationCount, islandPopulations[islandPopulations.size()-1].size(), rng);

             for (unsigned int i = 0; i < migrationCount; i++){
                tempCandidates[i] = islandPopulations[0][immigrantIndexes[i]];
                islandPopulations[0][immigrantIndexes[i]] = islandPopulations[islandPopulations.size()-1][emigrantIndexes[i]];
             }

            for(unsigned int j = islandPopulations.size()-1;j>1; j--){
                int* temp = immigrantIndexes;
                immigrantIndexes=emigrantIndexes;
                emigrantIndexes = temp;
                getMigrationIndexes(emigrantIndexes, migrationCount, islandPopulations[j-1].size(), rng);

                for (unsigned int i = 0; i < migrationCount; i++){
                    islandPopulations[j][immigrantIndexes[i]] = islandPopulations[j-1][emigrantIndexes[i]];
                }
            }
            delete[] immigrantIndexes;
            for (unsigned int i = 0; i < migrationCount; i++){
                islandPopulations[1][emigrantIndexes[i]] = tempCandidates[i];
            }
            delete[] tempCandidates;
            delete[] emigrantIndexes;
        }

    private:
        void getMigrationIndexes(int* indexes, int migrationCount, int populationSize, Random* rng){
            int i = 0;
            while(i < migrationCount){
                int index = rng->nextInt(populationSize);
                bool validIndex = true;
                for(int j = 0; j < i; j++){
                    if(indexes[j] == index){
                        validIndex = false;
                        break;
                    }
                }
                if(validIndex){
                    indexes[i++] = index;
                }
            }
        }

};

#endif // RINGMIGRATION_H
