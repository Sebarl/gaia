#ifndef INNOVATIONS_H
#define INNOVATIONS_H

/*******************************************************
 *  Defines the Innovations registers.
 *  These structures are used to define an InnovationDB
 *******************************************************/

#include "NeuronType.h"

typedef struct {
	// The ids of the two neurons this link connects
	int neuronIn, neuronOut;
} LinkInnovation;

typedef struct {
	// The ids of the two neurons this neuron is between
	int neuronIn, neuronOut;

	int neuronId;
	float xCoord, yCoord;
} NeuronInnovation;

#endif // INNOVATIONS_H
