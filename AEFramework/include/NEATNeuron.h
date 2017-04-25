#ifndef NEATNEURON_H
#define NEATNEURON_H

#include <iostream>

#include "NeuronType.h"
#include "ActivationFunction.h"
#include "Random.h"

class NEATNeuron {
    protected:
        ActivationFunction* function;
    public:
        int id;

        NeuronType type;

        bool recurrent;

        float xCoord, yCoord;



        NEATNeuron(int id, NeuronType type, ActivationFunction* function, float xCoord, float yCoord);
        NEATNeuron(int id, NeuronType type, bool recurrent, ActivationFunction* function, float xCoord, float yCoord);
        NEATNeuron(int id, NeuronType type, float xCoord, float yCoord, Random* rng);
        NEATNeuron(int id, NeuronType type, bool recurrent, float xCoord, float yCoord, Random* rng);
        virtual ~NEATNeuron();

        NEATNeuron* clone();
        void resetActivationFunction(Random* rng);
        void mutateActivationFunction(Random* rng);
        const ActivationFunction* getFunction();

    //overload << for streaming
    friend std::ostream& operator <<(std::ostream &os, const NEATNeuron &neuron) {
        os << "Neuron: " << neuron.id << "\n"
           << "\tType: " << neuron.type << "\n"
           << "\tRecurrent: " << neuron.recurrent << "\n"
           << "\tActivation function: " << neuron.function << "\n"
           << "\tCoords: " << neuron.xCoord << " , " << neuron.yCoord << "\n";
        return os;
    }
};

#endif // NEATNEURON_H
