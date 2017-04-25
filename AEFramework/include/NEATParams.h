#ifndef NEATPARAMS_H
#define NEATPARAMS_H

typedef struct {
    unsigned int numInputs;
    unsigned int numOutputs;
    float crossoverProbability;
    float disabledLinkProbability;
	float addNeuronProbability;
	unsigned int numTriesToFindLink;
	float addLinkProbability;
	float addLoopProbability;
	unsigned int numTriesToAddLink;
	unsigned int numTriesToFindLoopNeuron;
	float mutateWeightProbability;
	float replaceWeightProbability;
	float mutateActivationFunctionProbability;
	float replaceActivationFunctionProbability;
	float compatibilityThreshold;
	float cantCompatibilityLinks;
	float cMatched, cDisjoint, cExcess;
	int maxPermittedNeurons;
	unsigned int allowedGenerationsWithoutImprovement;
	unsigned int youngBonusThreshold;
	unsigned int oldPenaltyThreshold;
	double youngBonus;
	double oldPenalty;
	unsigned int elitismThreshold;
	bool useOtherActivationFunctions;
} NEATParams;

#endif // NEATPARAMS_H
