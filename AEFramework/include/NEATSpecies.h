#ifndef NEATSPECIES_H
#define NEATSPECIES_H

#include <vector>

#include "NEATGenome.h"
#include "EvaluatedCandidate.h"

class NEATSpecies {
	protected:
		NEATGenome* representative;
		double totalFitness;
		std::vector<EvaluatedCandidate<NEATGenome>*> members;
		bool naturalFitness;
	public:

        int id;

        unsigned int age;
		double leaderFitness;
		unsigned int generationsWithoutImprovement;

		NEATSpecies(const NEATGenome* representative, bool naturalFitness, int id);
		virtual ~NEATSpecies();

		unsigned int size();
		void addMember(EvaluatedCandidate<NEATGenome>* newMember);
		void purge();
		NEATGenome* getRepresentative();
		EvaluatedCandidate<NEATGenome>* getLeader();
		double getAdjustedFitness(unsigned int youngBonusThreshold, unsigned int oldPenaltyThreshold, double youngBonus, double oldPenalty);
		std::vector<EvaluatedCandidate<NEATGenome>*>& getMembers();
};

#endif // NEATSPECIES_H
