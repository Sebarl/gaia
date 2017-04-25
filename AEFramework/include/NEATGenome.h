#ifndef NEATGENOME_H
#define NEATGENOME_H

/*******************************************************
 *  NEAT genome definition. These are the evolutionary
 *  process individuals.
 *******************************************************/

#include <vector>
#include <iostream>

#include "NEATNeuron.h"
#include "NEATLink.h"
#include "InnovationDB.h"

class NEATGenome {
    protected:
        // Creates a link between the neurons of id fromId and toId,
        // located at indices fromIndex and toIndex of the neurons vector.
        // Creates the innovation in the database in case it does not already exist
        void createLink(unsigned int fromIndex, unsigned int fromId, unsigned int toIndex, unsigned int toId, float weight, InnovationDB* innovationDB);

    public:

        // Finds the position of a link by its index in links vector
        // Returns a negative value if the search fails
        int getLinkIndex(int neuronIn, int neuronOut);

        // Finds the position of a neuron by its index in neurons vector
        // Returns a negative value if the search fails
        int getNeuronIndex(int neuronId);

        // The neurons of the neural network
        std::vector<NEATNeuron*> neurons;

        // The links of the neural network
        std::vector<NEATLink*> links;

        // Number of INPUT neurons of this network
        unsigned int numInputs;

        // Depth of the neural network
        unsigned int depth;

        // Creates an empty NEATGenome
        NEATGenome(unsigned int numInputs);

        // Creates a NEATGenome from a vector of NEATLinks, a vector of NEATNeurons
        NEATGenome(unsigned int numInputs, std::vector<NEATNeuron*> neurons, std::vector<NEATLink*> genes);

        ~NEATGenome();

        // Copy constructor
        NEATGenome(const NEATGenome& g);

        // Assignment operator
        NEATGenome& operator=(const NEATGenome& g);

        NEATGenome* clone() const;

        // Returns the number of links
        unsigned int numLinks() const;

        // Returns the number of neurons
        unsigned int numNeurons() const;

        // Add a neuron
        void addNeuron(InnovationDB* innovationDB, unsigned int numTriesToFindLink, Random* rng);

        // Add a normal link (forward or recurrent)
        void addLink(InnovationDB* innovationDB, unsigned int numTriesToAddLink, Random* rng);

        // Add a loop link
        void addLoop(InnovationDB* innovationDB, unsigned int numTriesToFindLoopNeuron, Random* rng);

        // Iterates through the links and mutates the connection weights
        // according to mutationProbability.
        // If a link is selected for mutation, there is a probability of
        // newWeightProbability that the weight is uniformely selected between -1 and 1
        void mutateWeights(float mutationProbability, float newWeightProbability, Random* rng);

        // Iterates through the neurons and mutates its activations function
        // parammeters according to mutationProbability.
        // If a neuron is selected for mutation, there is a probability of
        // newFunctionProbability that the function is uniformely
        // selected between the avaliable default ones
        void mutateActivationFunctions(float mutationProbability, float newFunctionProbability, Random* rng, bool useOtherActivationFunctions);

        // Calculates the compatibility score between this NEATGenome and another NEATGenome
        float getCompatibilityScore(NEATGenome* other, float cMatched, float cDisjoint, float cExcess);

        //bool write(std::ostream &file);

        // Create a NEATGenome from a data file
        //bool createFromFile(const char* fileName);

        bool check();

};

#endif // NEATGENOME_H
