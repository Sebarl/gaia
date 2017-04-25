#ifndef POPULATIONDATA_H
#define POPULATIONDATA_H

template <class T>
class PopulationData{
    public:
        T* bestCandidate;
        double bestCandidateFitness;
        double meanFitness;
        double fitnessStandardDeviation;
        bool naturalFitness;
        int populationSize;
        int eliteCount;
        int generationNumber;
        long elapsedTime;
        int bestCandidateFirstGeneration;
        PopulationData(){}
        virtual ~PopulationData(){
            delete bestCandidate;
        }
};

#endif // POPULATIONDATA_H
