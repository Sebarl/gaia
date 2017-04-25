#ifndef INNOVATIONDB_H
#define INNOVATIONDB_H

/***********************************************
 *  Class that keeps track of all innovations
 *  throughout the evolutionary process.
 ***********************************************/

#include <vector>

#include "Innovations.h"
#include "NeuronType.h"
#include "NEATNeuron.h"

class InnovationDB {
	private:
		std::vector<LinkInnovation> links;
		std::vector<NeuronInnovation> neurons;
		int nextNeuronId;

		int getNewNeuronId();
		int reserveNeuronSlot();
		int reserveLinkSlot();

	public:
		InnovationDB();
		virtual ~InnovationDB();

		// Initializes this InnovationDB with all the links between (INPUT + BIAS) neurons and OUTPUT neurons.
		// There are numInput INPUT neurons, one BIAS neuron and numOutputs OUTPUT neurons
		void initialize(unsigned int numInputs, unsigned int numOutputs);

		// Checks to see if this innovation has already occurred, and returns
		// the neuron id. If the check fails, returns a negative value
		int checkNeuronInnovation(int neuronIn, int neuronOut);

		// Checks to see if this innovation has already occurred, and returns
		// its innovation id. If the check fails, returns a negative value
		int checkLinkInnovation(int neuronIn, int neuronOut);

		// Adds a NEURON innovation and returns the neuron ID.
		// If the innovation already exists, returns the neuron ID
		// associated with the innovation
		int createNeuronInnovation(int neuronIn, int neuronOut);

		// Adds a LINK innovation and returns its ID
		// If the innovation already exists, returns its ID
		int createLinkInnovation(int neuronIn, int neuronOut);
};

#endif // INNOVATIONDB_H
