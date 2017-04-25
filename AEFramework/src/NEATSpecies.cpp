#include "NEATSpecies.h"

NEATSpecies::NEATSpecies(const NEATGenome* representative, bool naturalFitness, int id){
	this->naturalFitness = naturalFitness;
	this->representative = representative->clone();
	this->age = 0;
	this->generationsWithoutImprovement = 0;
	this->totalFitness = 0;
	this->id = id;
}

NEATSpecies::~NEATSpecies(){
    delete this->representative;
}

unsigned int NEATSpecies::size() {
	return this->members.size();
}

double NEATSpecies::getAdjustedFitness(unsigned int youngBonusThreshold, unsigned int oldPenaltyThreshold, double youngBonus, double oldPenalty){
	if(this->age < youngBonusThreshold) {
		return this->totalFitness * youngBonus;
	} else if(this->age > oldPenaltyThreshold) {
		return this->totalFitness * oldPenalty;
	} else {
		return this->totalFitness;
	}
}

void NEATSpecies::addMember(EvaluatedCandidate<NEATGenome>* newMember){
	members.push_back(newMember);
	this->totalFitness += newMember->fitness;
}

void NEATSpecies::purge(){
	this->members.clear();
	this->totalFitness = 0;
	this->age++;
	this->generationsWithoutImprovement++;
}

NEATGenome* NEATSpecies::getRepresentative(){
	return this->representative;
}

EvaluatedCandidate<NEATGenome>* NEATSpecies::getLeader(){
    return this->members[0];
}

std::vector<EvaluatedCandidate<NEATGenome>*>& NEATSpecies::getMembers(){
	return this->members;
}
