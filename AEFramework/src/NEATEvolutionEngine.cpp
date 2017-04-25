#include <math.h>

#include "NEATEvolutionEngine.h"
#include "PlatformSpecific.h"
#include <stdio.h>
//#define DEBUG_TIME
//#define DEBUG_SPECIES

NEATEvolutionEngine::NEATEvolutionEngine(unsigned int populationSize, unsigned int eliteCount, NEATCandidateFactory* candidateFactory, NEATSelectionStrategy* intraspeciesSelectionStrategy, NEATSelectionStrategy* interspeciesSelectionStrategy, FitnessEvaluator<NEATGenome>* fitnessEvaluator, NEATParams params, Random* rng, unsigned int numThreads) {
    this->candidateFactory = candidateFactory;
    this->fitnessEvaluator = fitnessEvaluator;
    this->rng = rng;
    this->evolutionScheme = NULL;
    this->selectionStrategy = selectionStrategy;
    this->popData = NULL;
    this->populationSize = populationSize;
    this->eliteCount = eliteCount;
    this->numThreads = numThreads;
    this->nextSpeciesId = 0;
    if(this->numThreads > populationSize){
        this->numThreads = populationSize;
    }
	this->interspeciesSelectionStrategy = interspeciesSelectionStrategy;
	this->intraspeciesSelectionStrategy = intraspeciesSelectionStrategy;
	this->params = params;
	if(fitnessEvaluator->isNatural()){
		this->bestFitness = 0;
	} else {
		this->bestFitness = INFINITY;
	}
	this->innovationDB = new InnovationDB();
	candidateFactory->setInnovationDB(this->innovationDB);
    this->numThreads = numThreads;
    if(this->numThreads > populationSize){
        this->numThreads = populationSize;
    }
}

NEATEvolutionEngine::NEATEvolutionEngine(unsigned int populationSize, unsigned int eliteCount, NEATCandidateFactory* candidateFactory, NEATSelectionStrategy* selectionStrategy, FitnessEvaluator<NEATGenome>* fitnessEvaluator, NEATParams params, Random* rng, unsigned int numThreads) {
    this->candidateFactory = candidateFactory;
    this->fitnessEvaluator = fitnessEvaluator;
    this->rng = rng;
    this->evolutionScheme = NULL;
    this->selectionStrategy = NULL;
    this->popData = NULL;
    this->populationSize = populationSize;
    this->eliteCount = eliteCount;
    this->numThreads = numThreads;
    this->nextSpeciesId = 0;
    if(this->numThreads > populationSize){
        this->numThreads = populationSize;
    }
	this->interspeciesSelectionStrategy = selectionStrategy;
	this->intraspeciesSelectionStrategy = selectionStrategy;
	this->params = params;
	if(fitnessEvaluator->isNatural()){
		this->bestFitness = 0;
	} else {
		this->bestFitness = INFINITY;
	}
	this->innovationDB = new InnovationDB();
    candidateFactory->setInnovationDB(this->innovationDB);
    this->numThreads = numThreads;
    if(this->numThreads > populationSize){
        this->numThreads = populationSize;
    }
}

NEATEvolutionEngine::NEATEvolutionEngine(unsigned int populationSize, NEATCandidateFactory* candidateFactory, NEATSelectionStrategy* intraspeciesSelectionStrategy, NEATSelectionStrategy* interspeciesSelectionStrategy, FitnessEvaluator<NEATGenome>* fitnessEvaluator, NEATParams params, Random* rng, unsigned int numThreads) {

    this->candidateFactory = candidateFactory;
    this->fitnessEvaluator = fitnessEvaluator;
    this->rng = rng;
    this->evolutionScheme = NULL;
    this->selectionStrategy = NULL;
    this->popData = NULL;
    this->populationSize = populationSize;
    this->eliteCount = eliteCount;
    this->numThreads = numThreads;
    this->nextSpeciesId = 0;
    if(this->numThreads > populationSize){
        this->numThreads = populationSize;
    }
	this->interspeciesSelectionStrategy = interspeciesSelectionStrategy;
	this->intraspeciesSelectionStrategy = intraspeciesSelectionStrategy;
	this->params = params;
	if(fitnessEvaluator->isNatural()){
		this->bestFitness = 0;
	} else {
		this->bestFitness = INFINITY;
	}
	this->innovationDB = new InnovationDB();
    candidateFactory->setInnovationDB(this->innovationDB);
    this->numThreads = numThreads;
    if(this->numThreads > populationSize){
        this->numThreads = populationSize;
    }
}

NEATEvolutionEngine::NEATEvolutionEngine(unsigned int populationSize, NEATCandidateFactory* candidateFactory, NEATSelectionStrategy* intraspeciesSelectionStrategy, NEATSelectionStrategy* interspeciesSelectionStrategy, FitnessEvaluator<NEATGenome>* fitnessEvaluator, NEATParams params, Random* rng) {
    this->candidateFactory = candidateFactory;
    this->fitnessEvaluator = fitnessEvaluator;
    this->rng = rng;
    this->evolutionScheme = NULL;
    this->selectionStrategy = NULL;
    this->popData = NULL;
    this->populationSize = populationSize;
    this->eliteCount = eliteCount;
    this->numThreads = numThreads;
    this->nextSpeciesId = 0;
    if(this->numThreads > populationSize){
        this->numThreads = populationSize;
    }
	this->interspeciesSelectionStrategy = interspeciesSelectionStrategy;
	this->intraspeciesSelectionStrategy = intraspeciesSelectionStrategy;
	this->params = params;
	if(fitnessEvaluator->isNatural()){
		this->bestFitness = 0;
	} else {
		this->bestFitness = INFINITY;
	}
	this->innovationDB = new InnovationDB();
    candidateFactory->setInnovationDB(this->innovationDB);
    this->numThreads = numThreads;
    if(this->numThreads > populationSize){
        this->numThreads = populationSize;
    }
}

NEATEvolutionEngine::~NEATEvolutionEngine(){
	if(this->interspeciesSelectionStrategy != this->intraspeciesSelectionStrategy){
		delete this->interspeciesSelectionStrategy;
	}
	delete this->intraspeciesSelectionStrategy;
	std::list<NEATSpecies*>::iterator curSpecies = this->species.begin();
	while (curSpecies != this->species.end()) {
       delete (*curSpecies);
       curSpecies++;
	}
	delete innovationDB;
}

NEATGenome* NEATEvolutionEngine::mate(EvaluatedCandidate<NEATGenome>* mum, EvaluatedCandidate<NEATGenome>* dad){
	NEATGenome* stronger = mum->candidate;
    NEATGenome* weaker = dad->candidate;
    if(mum->fitness == dad->fitness) {
        int mumLength = stronger->numLinks();
        int dadLength = weaker->numLinks();
        if((dadLength < mumLength) ||
          ((mumLength == dadLength) && (this->rng->nextBool())) ) {
            NEATGenome* temp = stronger;
            stronger = weaker;
            weaker = temp;
        }
    } else if( ( this->fitnessEvaluator->isNatural() && dad->fitness > mum->fitness) ||
    		   (!this->fitnessEvaluator->isNatural() && dad->fitness < mum->fitness)) {
        NEATGenome* temp = stronger;
        stronger = weaker;
        weaker = temp;
    }

    std::vector<NEATNeuron*> babyNeurons;
    std::vector<NEATLink*> babyLinks;

    std::vector<NEATLink*>::iterator curStrongerLink = stronger->links.begin();
    std::vector<NEATLink*>::iterator curWeakerLink = weaker->links.begin();

    while(curStrongerLink != stronger->links.end() && curWeakerLink != weaker->links.end()) {
    	if((*curStrongerLink)->innovationId < (*curWeakerLink)->innovationId){
    		// Add disjoint genes from stronger parent
    		babyLinks.push_back((*curStrongerLink)->clone());
    		curStrongerLink++;
    	} else if((*curStrongerLink)->innovationId == (*curWeakerLink)->innovationId){
    		// Add genes with same innovationId from either parent
    		if(rng->nextBool()){
                babyLinks.push_back((*curStrongerLink)->clone());
	        } else {
	            babyLinks.push_back((*curWeakerLink)->clone());
	        }
	        // Disable the link if it was disabled in either parent with a probability of disabledLinkProbability
	        if(!(*curStrongerLink)->enabled){
                if((*curWeakerLink)->enabled && rng->nextFloat() < this->params.disabledLinkProbability){
                    babyLinks[babyLinks.size() - 1]->enabled = false;
                }
	        } else if(!(*curWeakerLink)->enabled && rng->nextFloat() < this->params.disabledLinkProbability){
                babyLinks[babyLinks.size() - 1]->enabled = false;
	        }
	        curStrongerLink++;
	        curWeakerLink++;
    	} else {
    		// Skip disjoint genes from weaker parent
    		curWeakerLink++;
    	}
    }
    // Add excess genes from stronger parent
    while(curStrongerLink != stronger->links.end()){
    	babyLinks.push_back((*curStrongerLink)->clone());
    	curStrongerLink++;
    }

	std::vector<NEATNeuron*>::iterator curStrongerNeuron = stronger->neurons.begin();
	std::vector<NEATNeuron*>::iterator curWeakerNeuron = weaker->neurons.begin();

    while(curStrongerNeuron != stronger->neurons.end() && curWeakerNeuron != weaker->neurons.end()) {
    	if((*curStrongerNeuron)->id < (*curWeakerNeuron)->id){
    		// Add disjoint neurons from stronger parent
    		babyNeurons.push_back((*curStrongerNeuron)->clone());
    		curStrongerNeuron++;
    	} else if((*curStrongerNeuron)->id == (*curWeakerNeuron)->id){
    		// Add neurons with same id from either parent
    		if(rng->nextBool()){
                babyNeurons.push_back((*curStrongerNeuron)->clone());
	        } else {
	            babyNeurons.push_back((*curWeakerNeuron)->clone());
	        }

	        curStrongerNeuron++;
	        curWeakerNeuron++;
    	} else {
    		// Skip disjoint neurons from weaker parent
    		curWeakerNeuron++;
    	}
    }

    while(curStrongerNeuron != stronger->neurons.end()){
    	babyNeurons.push_back((*curStrongerNeuron)->clone());
    	curStrongerNeuron++;
    }

    return new NEATGenome(this->params.numInputs, babyNeurons, babyLinks);
}

void NEATEvolutionEngine::mutate(NEATGenome* network) {
	// Add a new neuron if it not reached the maximum permitted quantity
	if ((this->params.maxPermittedNeurons == -1 || network->numNeurons() < (unsigned int)(this->params.maxPermittedNeurons))
		&& this->rng->nextFloat() < this->params.addNeuronProbability){
		network->addNeuron(this->innovationDB, this->params.numTriesToFindLink, this->rng);
	}

	// Add a link between two unlinked neurons
	if (this->rng->nextFloat() < this->params.addLinkProbability){
		if(rng->nextFloat() < this->params.addLoopProbability){
            network->addLoop(this->innovationDB, this->params.numTriesToFindLoopNeuron, this->rng);
		} else {
            network->addLink(this->innovationDB, this->params.numTriesToAddLink, this->rng);
		}
	}

	// Mutate the link weights
	network->mutateWeights(this->params.mutateWeightProbability, this->params.replaceWeightProbability, this->rng);

	// Mutate the neurons activation functions
    network->mutateActivationFunctions(this->params.mutateActivationFunctionProbability, this->params.replaceActivationFunctionProbability, this->rng, this->params.useOtherActivationFunctions);
}

// DO NOT PARALLELIZE THIS FUNCTION!!! InnovationDB is not Thread-safe!
std::vector<NEATGenome*> NEATEvolutionEngine::applyEvolutionScheme(std::vector<EvaluatedCandidate<NEATGenome>*> selectedCandidates){
	std::vector<NEATGenome*> result;
    unsigned int numCandidates = selectedCandidates.size();
    EvaluatedCandidate<NEATGenome> *mum, *dad;
    NEATGenome* baby;
    for (unsigned int i = 0; i < numCandidates; i+=2) {
        mum = selectedCandidates[i];
        dad = selectedCandidates[i+1];
        // Randomly decide (according to the current cross-over probability)
        // whether to perform cross-over for these 2 parents.
        if (rng->nextFloat() < this->params.crossoverProbability){
            baby = mate(mum, dad);
        } else {
            // If there is no cross-over to perform, just add the first
            // parent to the results unaltered.
            baby = mum->candidate->clone();
        }
        //baby->check();
        mutate(baby);
        result.push_back(baby);
    }
    return result;
}

std::vector<EvaluatedCandidate<NEATGenome>*> NEATEvolutionEngine::nextEvolutionStep(std::vector<EvaluatedCandidate<NEATGenome>*> evaluatedPopulation, int eliteCountTrucho, Random* rng){
    #ifdef DEBUG_TIME
    tstamp start, endT;
    #endif // DEBUG_TIME
	std::list<NEATSpecies*>::iterator curSpecies = this->species.begin();
	while (curSpecies != this->species.end()) {
		(*curSpecies)->purge();

		// Kill species with no improvement
		// (if not the species which contains the best genome found so far) ?????????????? Maybe apply real elitism
		if ((*curSpecies)->generationsWithoutImprovement > this->params.allowedGenerationsWithoutImprovement) {
			// Deletes the current species and points to next one (or species.end() if deletes the last one)
			delete (*curSpecies);
			curSpecies = this->species.erase(curSpecies);
		} else {
			curSpecies++;
		}
	}

	// Update best fitness so far
	this->bestFitness = evaluatedPopulation[0]->fitness;
    this->bestIndividual = evaluatedPopulation[0];
	// Speciate population
	float compatibilityScore;

	//printf("Speciating\n\n");

    bool speciated;
	for(unsigned int i = 0; i < this->populationSize; i++) {
//		printf("Searching suitable species...\n");
		speciated=false;
		for (curSpecies = this->species.begin(); curSpecies != this->species.end(); curSpecies++) {
			NEATGenome* representative = (*curSpecies)->getRepresentative();
            compatibilityScore = evaluatedPopulation[i]->candidate->getCompatibilityScore(representative, this->params.cMatched, this->params.cDisjoint, this->params.cExcess);
            //printf("Lince mistico de las praderas taringueras, mi compatibility es: %f\n", compatibilityScore);
            if(compatibilityScore <= this->params.compatibilityThreshold) {
                //evaluatedPopulation was sorted, thus the individuals in this species are also sorted
                (*curSpecies)->addMember(evaluatedPopulation[i]);
                //printf("%lu guachin!\n",evaluatedPopulation[i]->candidate->neurons.size()-evaluatedPopulation[i]->candidate->numInputs -9);
                speciated = true;
          //      printf("FOUND\n");
             //   printf("New size: %d\n", (*curSpecies)->size());
                break;
            }
		}
		// No suitable species found. Create a new one
		if(!speciated) {
           // printf("No suitable species found. Creating one.\n");
            NEATSpecies* newSpecies = new NEATSpecies(evaluatedPopulation[i]->candidate, this->fitnessEvaluator->isNatural(), this->nextSpeciesId);
            this->nextSpeciesId++;
			this->species.push_back(newSpecies);
			newSpecies->addMember(evaluatedPopulation[i]);
           // printf("Now we have %u species\n", (unsigned int)this->species.size());
		}

	}

	curSpecies = this->species.begin();
	while(curSpecies != this->species.end()){
		if ((*curSpecies)->size() == 0) {
			//resulta que nadie fue compatible con esta especie, asi que morimos! (nadie nos quiere)
			curSpecies=species.erase(curSpecies);
		} else{
            curSpecies++;
		}
	}
	#ifdef DEBUG_SPECIES
    printf("\n#Especies: %lu\n\n",this->species.size());
    curSpecies = this->species.begin();
	while(curSpecies != this->species.end()){
        printf("Especie %d: %u (%u)\n", (*curSpecies)->id, (*curSpecies)->size(), (*curSpecies)->age);
        curSpecies++;
	}
    #endif
	double totalAdjustedFitness = 0.0;
	for(curSpecies = this->species.begin(); curSpecies != this->species.end(); curSpecies++){
		totalAdjustedFitness+= (*curSpecies)->getAdjustedFitness(this->params.youngBonusThreshold, this->params.oldPenaltyThreshold, this->params.youngBonus, this->params.oldPenalty);
	}

	std::vector<EvaluatedCandidate<NEATGenome>*> newPopulation;
	std::vector<EvaluatedCandidate<NEATGenome>*> eliteIndividuals;

	// Intraspecies selection
	unsigned int numSpawned = 0;
	unsigned int speciesEliteCount;
	curSpecies = this->species.begin();
	while(curSpecies != this->species.end()){
		unsigned int numSpawn = (int)floor((*curSpecies)->getAdjustedFitness(this->params.youngBonusThreshold,
                                                                          this->params.oldPenaltyThreshold,
                                                                          this->params.youngBonus,
                                                                          this->params.oldPenalty)
                                            * populationSize / totalAdjustedFitness);
		if(numSpawn == 0){
            delete (*curSpecies);
			curSpecies=this->species.erase(curSpecies);
		} else {
			std::vector<EvaluatedCandidate<NEATGenome>* >& speciesIndividuals = (*curSpecies)->getMembers();
			unsigned int speciesSize = speciesIndividuals.size();
			if(speciesSize < this->params.elitismThreshold){
                bool hasElite = false;
                for(unsigned int i=0;i<speciesSize;++i){
                    if(speciesIndividuals[i] == this->bestIndividual){
                        hasElite = true;
                        break;
                    }
                }
                if(hasElite){
                    speciesEliteCount=1;
                }
                else{
                    speciesEliteCount=0;
                }
			}
			else{
                speciesEliteCount = std::min(speciesSize,eliteCount);
			}
			if(numSpawn > speciesEliteCount){
				for (unsigned int i = 0; i < speciesEliteCount; i++){
					eliteIndividuals.push_back(speciesIndividuals[i]->clone());
				}
				// Selects without copying individuals
				#ifdef DEBUG_TIME
                start = getTimeStamp();
                #endif // DEBUG_TIME
				std::vector<EvaluatedCandidate<NEATGenome>*> selectedIndividuals = intraspeciesSelectionStrategy->select(speciesIndividuals,
																						 			 					 fitnessEvaluator->isNatural(),
																						 			 					 2*(numSpawn - speciesEliteCount),
																						 			 					 rng);
                #ifdef DEBUG_TIME
                endT = getTimeStamp();
                printf("La seleccion intraespecie llevo %ld microsegundos\n",getElapsedTimeMicroSeconds(start,endT));
                delete start;
                delete endT;
                #endif // DEBUG_TIME
				shufflePopulation(selectedIndividuals, rng);
				newPopulation.insert(newPopulation.end(), selectedIndividuals.begin(), selectedIndividuals.end());
			} else {
				for (unsigned int i = 0; i < numSpawn; i++){
					eliteIndividuals.push_back(speciesIndividuals[i]->clone());
				}
			}
			numSpawned+= numSpawn;
			curSpecies++;
		}
	}

	// Interspecies selection
	if(numSpawned < this->populationSize){
		#ifdef DEBUG_TIME
        start = getTimeStamp();
        #endif // DEBUG_TIME
		std::vector<EvaluatedCandidate<NEATGenome>*> selectedIndividuals = interspeciesSelectionStrategy->select(evaluatedPopulation,
											   												 					 fitnessEvaluator->isNatural(),
											   												 					 2*(this->populationSize - numSpawned),
											   												 					 rng);
        #ifdef DEBUG_TIME
        endT = getTimeStamp();
        printf("La seleccion interespecie llevo %ld microsegundos\n",getElapsedTimeMicroSeconds(start,endT));
        delete start;
        delete endT;
        #endif // DEBUG_TIME
        shufflePopulation(selectedIndividuals, rng);
		newPopulation.insert(newPopulation.end(), selectedIndividuals.begin(), selectedIndividuals.end());
	}

    // Apply evolution scheme to new population and evaluate new individuals.
    // The evolution scheme is applied over all population because the crossover operator
    // works with the individuals in pairs, so there is no unwanted interspacies mating

	#ifdef DEBUG_TIME
	start = getTimeStamp();
	std::vector<NEATGenome*> pop = applyEvolutionScheme(newPopulation);
	endT = getTimeStamp();
    printf("El pipeline de crossover y mutacion de una generacion llevo %ld microsegundos\n",getElapsedTimeMicroSeconds(start,endT));
    delete start;
    delete endT;
    start = getTimeStamp();
    std::vector<EvaluatedCandidate<NEATGenome>*> newEvaluatedPopulation = evaluatePopulation(pop);
    endT = getTimeStamp();
    printf("La evaluacion de una generacion llevo %ld microsegundos\n",getElapsedTimeMicroSeconds(start,endT));
	delete start;
    delete endT;
	#else
	std::vector<EvaluatedCandidate<NEATGenome>*> newEvaluatedPopulation = evaluatePopulation(applyEvolutionScheme(newPopulation));
	#endif // DEBUG_TIME

	// Add elite individuals
	newEvaluatedPopulation.insert(newEvaluatedPopulation.end(), eliteIndividuals.begin(), eliteIndividuals.end());

	// Delete old population
	for(unsigned int i = 0; i < evaluatedPopulation.size(); i++){
		delete evaluatedPopulation[i];
	}
	evaluatedPopulation.clear();

	return newEvaluatedPopulation;
}
