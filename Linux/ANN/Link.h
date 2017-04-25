#ifndef LINK_H
#define LINK_H
#include "Neuron.h"
class Neuron;
typedef struct Link{
	Neuron* neuron;
	float weight;
}Link;

#endif
