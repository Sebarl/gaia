#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>

#include "Parameters.h"
#include "FinalStatistics.h"
#include "EvolutionaryOperator.h"
#include "Constants.h"
#include "XorShift128.h"
#include "GenerationCount.h"
#include "GenerationObserver.h"
#include "NEATCandidateFactory.h"
#include "NEATSelectionStrategy.h"
#include "NEATEvolutionEngine.h"
#include "NEATParams.h"
#include "NEATStochasticUniversalSampling.h"
#include "FCEUXEvaluator.h"
#include "NEATObserver.h"

Parameters loadParams(char* configFilePath, int evolutiveProcessID){
    Parameters p;
    std::string line;
    std::ifstream configFile(configFilePath);
    if (configFile.is_open()){
        #define MAXIMUM_COMMENT_LENGTH 100
        char* buff = new char[MAXIMUM_COMMENT_LENGTH];
        configFile >> p.generations;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.populationSize;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.eliteCount;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.elitismThreshold;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

/*
        configFile >> p.config.numInputs;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);
*/

        configFile >> p.config.numOutputs;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        int outputFunctionType;
        configFile >> outputFunctionType;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        switch(outputFunctionType){
            case 0:
                p.outputFunctionType = IDENTITY;
                break;
            case 1:
                p.outputFunctionType = SIGMOID;
                break;
            case 2:
                p.outputFunctionType = TANH;
                break;
            case 3:
                p.outputFunctionType = CHEAPSIGMOID;
                break;
            case 4:
                p.outputFunctionType = GAUSSIAN;
                break;
            case 5:
                p.outputFunctionType = RELU;
                break;
            default:
                p.outputFunctionType = SIGMOID;
                break;
        }

        configFile >> p.config.crossoverProbability;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.disabledLinkProbability;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.addNeuronProbability;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.numTriesToFindLink;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.addLinkProbability;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.numTriesToAddLink;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.addLoopProbability;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.numTriesToFindLoopNeuron;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.mutateWeightProbability;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.replaceWeightProbability;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.mutateActivationFunctionProbability;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.replaceActivationFunctionProbability;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);


        configFile >> p.config.cantCompatibilityLinks;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);
/*
        configFile >> p.config.cMatched;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.cDisjoint;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.cExcess;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);
*/
        configFile >> p.config.maxPermittedNeurons;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.allowedGenerationsWithoutImprovement;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.youngBonusThreshold;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.oldPenaltyThreshold;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.youngBonus;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.oldPenalty;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.frameCount;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.numThreads;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        configFile >> p.config.useOtherActivationFunctions;
        configFile.getline(buff, MAXIMUM_COMMENT_LENGTH);

        p.evolutiveProcessID = evolutiveProcessID;

        configFile.close();
        delete[] buff;
    }
    else{
        std::cout << "Unable to open config file";
        exit(-1);
    }
    return p;
}

FinalStatistics<NEATGenome>* evolveSolutions(Parameters &params){
    NEATEvolutionEngine* engine = new NEATEvolutionEngine(params.populationSize,
                                                          params.eliteCount,
                                                          new NEATCandidateFactory(params.config.numInputs, params.config.numOutputs, params.outputFunctionType, params.config.useOtherActivationFunctions),
                                                          new NEATStochasticUniversalSampling(),//NEATSelectionStrategy* intraspeciesSelectionStrategy,
                                                          //, NEATSelectionStrategy* interspeciesSelectionStrategy,
                                                          new FCEUXEvaluator(&params,0),//FitnessEvaluator<NEATGenome>* fitnessEvaluator,
                                                          params.config,
                                                          new XorShift128(),
                                                          params.numThreads);
        /*
        char* algo= new char[40];
        sprintf(algo, "Observer canchero!");
        engine->addEvolutionObserver(new NEATObserver(algo));
        */

        std::vector<TerminationCondition<NEATGenome>*> tc;
        tc.push_back(new GenerationCount<NEATGenome>(params.generations));

        FinalStatistics<NEATGenome>* finalResult = engine->evolve(tc);
        delete engine;
        return finalResult;
}


inline int serializeChromosome(NEATGenome* sol, char* buff, Parameters* params){
    int length = 0;
    length+=sprintf(buff+length,"%s:",params->ramPath);
    //length+=sprintf(buff+length,"%s:",params->weightsPath);
    length+=sprintf(buff+length,"%s",params->prefix);
	std::vector<NEATNeuron*>::iterator iter = sol->neurons.begin();

	//serialize input neurons


	length+=sprintf(buff+length,"%u:",params->config.numInputs);
	for(unsigned int i=0; i<params->config.numInputs;i++){
        length=(*iter)->getFunction()->serialize(buff,length);
        iter++;
	}

	//serialize biases, currently one

	length+=sprintf(buff+length,"%u:",1);
	length+=sprintf(buff+length,"%f;",1.0);
    length=(*iter)->getFunction()->serialize(buff,length);
    iter++;

    //serialize output neurons

	length+=sprintf(buff+length,"%u:",params->config.numOutputs);
	for(unsigned int i=0; i<params->config.numOutputs;i++){
        length=(*iter)->getFunction()->serialize(buff,length);
        iter++;
	}

    //serialize hidden neurons

    unsigned int cantHidden = sol->neurons.size() - (params->config.numInputs + params->config.numOutputs + 1);
	length+=sprintf(buff+length,"%u:",cantHidden);
	for(unsigned int i=0; i<cantHidden;i++){
        length=(*iter)->getFunction()->serialize(buff,length);
        iter++;
	}

    //serialize links

    length+=sprintf(buff+length,"%zu:",sol->links.size());
    unsigned int sourceNeuronType, sourceIndex, destNeuronType, destIndex;
    for(std::vector<NEATLink*>::iterator iter2 = sol->links.begin();iter2!=sol->links.end();iter2++){
        sourceIndex=sol->getNeuronIndex((*iter2)->fromNeuron);
        destIndex =sol->getNeuronIndex((*iter2)->toNeuron);
        switch(sol->neurons[sourceIndex]->type){
            case INPUT:
                 sourceNeuronType=0;
                break;
            case BIAS:
                sourceIndex=0;
                sourceNeuronType=1;
                break;
            case HIDDEN:
                sourceIndex-=params->config.numInputs+params->config.numOutputs+1;
                sourceNeuronType=2;
                break;
            case OUTPUT:
                sourceIndex-=params->config.numInputs+1;
                sourceNeuronType=3;
                break;
            default:
                throw GenericException("Unrecognized neuron type in source neuron");
        }
        switch(sol->neurons[destIndex]->type){
            case INPUT:
                 destNeuronType=0;
                break;
            case BIAS:
                destIndex=0;
                destNeuronType=1;
                break;
            case HIDDEN:
                destIndex-=params->config.numInputs+params->config.numOutputs+1;
                destNeuronType=2;
                break;
            case OUTPUT:
                destIndex-=params->config.numInputs+1;
                destNeuronType=3;
                break;
            default:
                throw GenericException("Unrecognized neuron type in destination neuron");
        }
        length+=sprintf(buff+length,"%f;%u;%u;%u;%u;",(*iter2)->weight,sourceNeuronType,sourceIndex,destNeuronType,destIndex);
    }
	return length;
}

int main(int argc, char *argv[]){

    char* configFilePath = argv[1];
    int evolutiveProcessID = atoi(argv[2]);

    try {
        Parameters params = loadParams(configFilePath, evolutiveProcessID);
        params.config.numInputs = atoi(argv[3]);
        params.ramPath=argv[4];
        FILE* file = fopen(argv[5],"r");
        params.prefix = new char[10000];
        fscanf(file,"%s",params.prefix);
        fclose(file);
        params.weightsPath=argv[6];
        params.ROM=argv[7];

        params.config.cMatched = 0.6;
        params.config.cExcess =  params.config.cDisjoint = (params.config.numInputs + 1)*8.0*3.0/(params.config.cantCompatibilityLinks);
        params.config.compatibilityThreshold=3.4;

        //loadRAMMap(ramMapPath,params);
        //loadFitnessWeights(fitnessWeightsPath,params);
        //printf("%d %s %s %s",params.RAMCount,params.ramPath,params.weightsPath,params.ROM);
        FinalStatistics<NEATGenome>* finalResult = evolveSolutions(params);
        //NEATGenome* result = finalResult->data->bestCandidate;

        printf("Best solution: \n\n");
        printf("Fitness: %lf\nSolution: ",finalResult->data->bestCandidateFitness);
        char* serialChromosome = new char[1000000];
        serializeChromosome(finalResult->data->bestCandidate, serialChromosome, &params);
        printf("%s",serialChromosome);

        printf("\nElapsed time: %ld \n", finalResult->data->elapsedTime);
        printf("Mean fitness: %lf \n", finalResult->data->meanFitness);
        printf("Fitness standard deviation: %lf \n", finalResult->data->fitnessStandardDeviation);
        printf("First generation to find best candidate: %d \n", finalResult->data->bestCandidateFirstGeneration);

        char path[100];
        sprintf(path, "%dsol.txt", params.evolutiveProcessID);

        std::ofstream solutionFile(path, std::ofstream::out);
        solutionFile << "Solution: " << serialChromosome;
        delete [] serialChromosome;
        //result->print(solutionFile, params.instance);

        solutionFile << std::endl
                   << "Fitness: " << finalResult->data->bestCandidateFitness << std::endl
                   << "Elapsed time: " << finalResult->data->elapsedTime << "s" << std::endl
                   << "Mean fitness: " << finalResult->data->meanFitness << std::endl
                   << "Fitness standard deviation: " << finalResult->data->fitnessStandardDeviation << std::endl
                   << "First generation to find best candidate: " << finalResult->data->bestCandidateFirstGeneration + 1 << std::endl;

        solutionFile.close();

        delete finalResult;

    } catch (std::exception ex) {
        std::cout << "An exception occurred. Code: " << ex.what() << std::endl;
    }
    return 0;
}
