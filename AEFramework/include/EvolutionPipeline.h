#ifndef EVOLUTIONPIPELINE_H
#define EVOLUTIONPIPELINE_H

#include <EvolutionaryOperator.h>

template <class T>
class EvolutionPipeline : public EvolutionaryOperator<T>
{
    private:
        std::vector<EvolutionaryOperator<T>*> pipeline;

    public:
        EvolutionPipeline(std::vector<EvolutionaryOperator<T>*> pipeline){
            this->pipeline = pipeline;
        }

        virtual ~EvolutionPipeline(){
            for(unsigned int i = 0; i < pipeline.size(); i++){
                delete pipeline[i];
            }
        }

        std::vector<T*> apply(std::vector<T*> selectedCandidates, Random* rng){
            std::vector<T*> tempVector = selectedCandidates;
            for(unsigned int i = 0; i < pipeline.size(); i++){
                tempVector = pipeline[i]->apply(tempVector, rng);
            }
            return tempVector;
        }
};

#endif // EVOLUTIONPIPELINE_H
