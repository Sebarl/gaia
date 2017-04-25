#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <omp.h>
#include <unistd.h>

#include "Parameters.h"
#include "FinalStatistics.h"
#include "Solution.h"
#include "PinballFactory.h"
#include "EvolutionaryOperator.h"
#include "PinballCrossoverOperator.h"
#include "PinballMutationOperator.h"
#include "EvolutionPipeline.h"
#include "Constants.h"
#include "PinballEvaluator.h"
#include "SimpleEvolutionEngine.h"
#include "XorShift128.h"
#include "StochasticUniversalSampling.h"
#include "GenerationCount.h"
#include "GenerationObserver.h"
#include "ParallelEvolutionEngine.h"
#include "IslandsEvolutionEngine.h"
#include "RingMigration.h"
#include "AverageCrossoverOperator.h"

//#include "MicParallelEvolutionEngine.h"
#include <ext/stdio_filebuf.h>
#include <dirent.h>

Parameters loadParams(char* configFilePath, int evolutiveProcessID){
    Parameters p;
    std::string line;
    ifstream configFile(configFilePath);
    if (configFile.is_open()){
        #define MAXIUM_COMMENT_LENGTH 100
        char* buff = new char[MAXIUM_COMMENT_LENGTH];
        configFile >> p.generations;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        configFile >> p.populationSize;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
		configFile >> p.numSeeds;
		configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        configFile >> p.crossoverProbability;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        configFile >> p.mutationProbability;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        configFile >> p.elitismPercentage;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        configFile >> p.enableIslands;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        configFile >> p.islandCount;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        configFile >> p.epochLength;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        configFile >> p.migrationCount;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        configFile >> p.frameCount;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        configFile >> p.instance;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        configFile >> p.islandThreads;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        configFile >> p.islandEngineThreads;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        configFile >> p.enableMic;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        p.evolutiveProcessID = evolutiveProcessID;
        configFile.getline(buff, MAXIUM_COMMENT_LENGTH);
        configFile.close();
        delete[] buff;
    }
    else{
        std::cout << "Unable to open config file";
        exit(-1);
    }
    return p;
}


#define BYTES_TO_MUTATE 5
#define EPS 0.00000001
std::vector<Solution*> generateSeedCandidates(char* lexOrderingsPath, Parameters& params, Random* rng) {

    int* RAMMap = new int[params.RAMCount];
	std::vector<Solution*> seeds(params.numSeeds, NULL);
    FILE* file = fopen(lexOrderingsPath,"r");
    unsigned int location;
    float weight;

    //Read initial candidate from file
    std::vector<float>* sol = new std::vector<float>(params.RAMCount, 0.0);
    float* weights = new float[2048];
    bool* inUse = (bool*)calloc(2048, sizeof(bool));
    for(int i = 0; i < params.RAMCount; i++){
        fscanf(file, "%u:%f\n", &location, &weight);
        weights[location] = weight;
        inUse[location] = true;
    }
    unsigned int ind = 0;
    for(unsigned int i = 0; i < 2048; i++){
        if(inUse[i]){
            (*sol)[ind] = weights[i];
            RAMMap[ind] = i;
            ind++;
        }
    }
    delete [] weights;
    free(inUse);
    params.RAMMap = RAMMap;

    if(params.numSeeds > 0){
        seeds[0] = new Solution(sol);
        std::vector<float>* newSol;

        bool* mutate = (bool*)calloc(params.RAMCount, sizeof(bool));
        unsigned int cantIndices = params.RAMCount;
        if(cantIndices > BYTES_TO_MUTATE){
            cantIndices = BYTES_TO_MUTATE;
        }
        unsigned int indices[cantIndices];
        for(unsigned int i = 1; i < params.numSeeds; i++){
            newSol = new std::vector<float>(params.RAMCount, 0.0);

            for(unsigned int j = 0; j < cantIndices; ++j){
                bool exists;
                do{
                    exists = false;
                    indices[j] = rng->nextInt(params.RAMCount);
                    for(unsigned int ind = 0; !exists && ind < j; ++ind){
                        exists = indices[ind] == indices[j];
                    }
                }while(exists);
            }
            for(unsigned int j = 0; j < cantIndices; ++j){
                mutate[indices[j]] = true;
            }
            for(int j = 0; j < params.RAMCount; j++){
                weight = (*sol)[j];
                if(mutate[j]) {
                    if(fabs(weight) < EPS) {
                        weight = INTERVAL_LENGTH*(rng->nextFloat()*0.0002 - 0.0001);
                    } else {
                        if(rng->nextFloat() < 0.01){
                            weight = (rng->nextFloat()*2.0 - 1.0)*INTERVAL_LENGTH;
                        } else {
                            weight+=rng->nextGaussian()*0.1*weight;
                            if(weight<-INTERVAL_LENGTH){
                                weight=-INTERVAL_LENGTH;
                            }
                            else if(weight>INTERVAL_LENGTH){
                                weight=INTERVAL_LENGTH;
                            }
                        }
                    }
                }
                (*newSol)[j] = weight;
            }
            seeds[i] = new Solution(newSol);

            for(unsigned int j = 0; j < cantIndices; ++j){
                mutate[indices[j]] = false;
            }

/*

            for(int j = 0; j < params.RAMCount; j++){
                if(rng->nextFloat() < 0.05) {
                    weight = (*sol)[j]*(1 + 0.1*rng->nextGaussian());
                    if(weight<-INTERVAL_LENGTH){
                        weight=-INTERVAL_LENGTH;
                    }
                    else if(weight>INTERVAL_LENGTH){
                        weight=INTERVAL_LENGTH;
                    }
                } else {
                    weight = (*sol)[j];
                }
                (*newSol)[j] = weight;
            }
            seeds[i] = new Solution(newSol);
*/
        }
    }
	return seeds;
}

FinalStatistics<Solution>* evolveSolutions(Parameters &params, char* lexOrderingsPath){
    //ENTREGA LAS ISLAS PIBE!

    Random* rng = new XorShift128();
    std::vector<Solution*> seedCandidates = generateSeedCandidates(lexOrderingsPath, params, rng);

    EvolutionEngine<Solution>* engine;

    if(params.enableIslands){
        std::vector<EvolutionEngine<Solution>*> islands;
        EvolutionEngine<Solution>* island;

        for(int i =0; i <params.islandCount;i++){
            PinballFactory* factory = new PinballFactory(&params);
            std::vector<EvolutionaryOperator<Solution>*> operators;
            operators.push_back(new PinballCrossoverOperator(1,params.crossoverProbability));
            //operators.push_back(new AverageCrossoverOperator(params.crossoverProbability));
            operators.push_back(new PinballMutationOperator(params.mutationProbability));
            EvolutionaryOperator<Solution>* pipeline = new EvolutionPipeline<Solution>(operators);

            //if(params.enableMic){
                //FitnessEvaluator<Solution>* pe = new MicPinballEvaluator(&params,i);
//                    island = new MicParallelEvolutionEngine<Solution>(params.populationSize, params.elitismPercentage, factory, pipeline, new StochasticUniversalSampling<Solution>(), new XorShift128(), params.islandThreads, i, &params);
            //}else{
                FitnessEvaluator<Solution>* pe = new PinballEvaluator(&params,i);
                island = new ParallelEvolutionEngine<Solution>(params.populationSize, params.elitismPercentage, factory, pipeline, pe, new StochasticUniversalSampling<Solution>(), new XorShift128(), params.islandThreads);
            //}
            //char* algo = new char[40];
            //sprintf(algo, "Island %d:\nGeneration", i);
            //island->addEvolutionObserver(new GenerationObserver<Solution>(algo));
            //char* algo= new char[40];
            //sprintf(algo, "Generation:");
            //island->addEvolutionObserver(new GenerationObserver<Solution>(algo));
            islands.push_back(island);
        }

        engine = new IslandsEvolutionEngine<Solution>(islands, params.epochLength, params.migrationCount, new RingMigration<Solution>(), new XorShift128(), true, params.islandEngineThreads);

    }
    else{


        PinballFactory* factory = new PinballFactory(&params);
        std::vector<EvolutionaryOperator<Solution>*> operators;
        operators.push_back(new PinballCrossoverOperator(1,params.crossoverProbability));
        //operators.push_back(new AverageCrossoverOperator(params.crossoverProbability));
        operators.push_back(new PinballMutationOperator(params.mutationProbability));
        EvolutionaryOperator<Solution>* pipeline = new EvolutionPipeline<Solution>(operators);
//            if(params.enableMic){
            //FitnessEvaluator<Solution>* pe = new MicPinballEvaluator(&params,i);
//                engine = new MicParallelEvolutionEngine<Solution>(params.populationSize, params.elitismPercentage, factory, pipeline, new StochasticUniversalSampling<Solution>(), new XorShift128(), params.islandThreads,0, &params);
//            }else{
            FitnessEvaluator<Solution>* pe = new PinballEvaluator(&params,0);
            engine = new ParallelEvolutionEngine<Solution>(params.populationSize, params.elitismPercentage, factory, pipeline, pe, new StochasticUniversalSampling<Solution>(), new XorShift128());
//            }


    }
    std::vector<TerminationCondition<Solution>*> tc;
    tc.push_back(new GenerationCount<Solution>(params.generations));

    /*
    char* algo= new char[40];
    sprintf(algo, "Epoch");
    engine->addEvolutionObserver(new GenerationObserver<Solution>(algo));
    */

    FinalStatistics<Solution>* finalResult = engine->evolve(seedCandidates, tc);
    delete engine;
    return finalResult;
}

std::vector<uint8>* calculateIndividualFitness(int index, char* vidFile, Parameters* params, int framesToDrop){
	//invocar a fceux y obtener el resultado
	char path[200];
	#if __GNUC__ < 5
    sprintf(path, "cd \"Fceux Headless/bin/\"; ./fceux -a ../../%s %s", vidFile,params->ROM);
    #else
    sprintf(path, "cd \"../Linux/Fceux Headless/bin/\"; ./fceux -a ../../%s %s", vidFile,params->ROM);
    #endif // __GNUC__

	FILE *fp = popen(path,"r");
	std::vector<uint8>* game = new std::vector<uint8>();
	int MAHCRAP = fileno(fp);
	__gnu_cxx::stdio_filebuf<char> makeFilesGreatAgain(MAHCRAP, std::ios::in );// 1 CRAP
	std::istream instr(&makeFilesGreatAgain);
	int data;
	while(instr >> data){
		game->push_back(data);
	}
	pclose(fp);
    //drop frames from video
    game->erase(game->begin(),game->begin() + framesToDrop*2048);
    return game;
}

char* copyStr(char* str){
    char* newStr = new char[strlen(str)+1];
    strcpy(newStr, str);
    return newStr;
}

bool checkSubstring(char* str, char* sub){
    unsigned int length = strlen(sub);
    bool equal = true;
    for(unsigned int i=0;i<length;++i){
        if(str[i]!=sub[i]){
            equal=false;
            break;
        }
    }
    return equal;
}

void loadVideos(Parameters& params, char* path){

    char* goodPath = new char[1000];
    char* badPath = new char[1000];

    strcpy(goodPath, path);
    strcat(goodPath, "/Good/");

    strcpy(badPath, path);
    strcat(badPath, "/Bad/");

    std::vector<char*> goodVids;
    std::vector<char*> badVids;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (goodPath)) != NULL) {
      while ((ent = readdir (dir)) != NULL) {
        if(ent->d_type == DT_REG){
            goodVids.push_back(copyStr(ent->d_name));
        }
      }
      closedir (dir);
    } else {
      /* could not open directory */
      printf("Couldn't open good videos directory\n");
      exit(418);
    }

    if ((dir = opendir (badPath)) != NULL) {
      while ((ent = readdir (dir)) != NULL) {
        if(ent->d_type == DT_REG){
            badVids.push_back(copyStr(ent->d_name));
        }
      }
      closedir (dir);
    } else {
      /* could not open directory */
      printf("Couldn't open good videos directory\n");
      exit(418);
    }

    params.goodVidsCount = goodVids.size();
    params.badVidsCount = badVids.size();
    params.goodVids = new char*[goodVids.size()];
    params.badVids = new char*[badVids.size()];
    unsigned int i=0;
    while(!checkSubstring(&goodPath[i],"TestMovies")){
        i++;
    }
    char* goodPath2 = new char[1000];
    strcpy(goodPath2,&goodPath[i]);
    char* badPath2 = new char[1000];
    strcpy(badPath2,&badPath[i]);
    delete [] goodPath;
    delete [] badPath;
    for(unsigned int i=0;i<params.goodVidsCount;i++){
        params.goodVids[i]=new char[1000];
        strcpy(params.goodVids[i],goodPath2);
        strcat(params.goodVids[i],goodVids[i]);
    }
    for(unsigned int i=0;i<params.badVidsCount;i++){
        params.badVids[i]=new char[1000];
        strcpy(params.badVids[i],badPath2);
        strcat(params.badVids[i],badVids[i]);
    }

    for(unsigned int i=0;i<goodVids.size();++i){
        delete [] goodVids[i];
    }

    for(unsigned int i=0;i<badVids.size();++i){
        delete [] badVids[i];
    }

    delete [] goodPath2;
    delete [] badPath2;

}

std::vector<float>* averageVideos(std::vector<uint8>** games, unsigned int cantGames){
    uint8** gamesPointer = new uint8*[cantGames];
    unsigned int minLength = games[0]->size()/2048;
    gamesPointer[0] = games[0]->data();
    for(unsigned int i = 1; i < cantGames; i++){
        gamesPointer[i]= games[i]->data();
        if(minLength>games[i]->size()/2048){
            minLength=games[i]->size()/2048;
        }
    }

    std::vector<float>* avgVideo = new std::vector<float>(minLength/10*2048, 0);
    for(unsigned int i = 0; i < cantGames; i++){
        for(unsigned int j = 0; j < minLength/10; ++j){
            for(unsigned int k = 0; k < 2048; k++){
                (*avgVideo)[j*2048 + k] += gamesPointer[i][20480*j + k];
            }
        }
    }
    for(unsigned int j = 0; j < minLength/10*2048; j++){
        (*avgVideo)[j]/= (float)cantGames;
    }
    delete [] gamesPointer;
    return avgVideo;
}

inline double evalRAM(const float* video, const Parameters& params, std::vector<float>* weights) {
    double sum = 0.0;
    int index = 0;
    for(int j = 0; j < 2048 && index < params.RAMCount; j++){
        if(params.RAMMap[index] == j){
            sum+= video[j] * (*weights)[index];
            index++;
        }
    }
    return sum;
}

float* evalFrames(const float* video, unsigned int cantFrames, const Parameters& params, std::vector<float>* weights){
	float* frames = new float[cantFrames];
	for(unsigned int i = 0; i < cantFrames; ++i){
		frames[i] = evalRAM(&video[2048*i], params, weights);
	}
	return frames;
}


float calculateShift(Solution* result, const Parameters& params) {
    std::vector<float>* sol = result->getSolution();
    unsigned int goodLength = params.avgGoodVideo->size() / 2048;

    float* goodVideo = evalFrames(params.avgGoodVideo->data(), goodLength, params, sol);
    float min = goodVideo[0];
    for(unsigned int i = 1; i < goodLength; ++i){
        if(min > goodVideo[i]){
            min = goodVideo[i];
        }
    }
    return min;
}

int main(int argc, char *argv[]){
    char* configFilePath = argv[1];
    int evolutiveProcessID = atoi(argv[2]);
    char* weightsPath = argv[3];
    char* framesToDropPath = argv[4];
    char* videosPath = argv[5];
    std::vector<uint8>** gameFrames;
    try {
        Parameters params = loadParams(configFilePath, evolutiveProcessID);
        loadVideos(params,videosPath);
        int* framesToDrop = new int[params.goodVidsCount + params.badVidsCount];
        FILE* fileinsqui = fopen(framesToDropPath,"r");
        if(fileinsqui){
            for(unsigned int i=0;i<params.goodVidsCount + params.badVidsCount;++i){
                fscanf(fileinsqui,"%d\n",&framesToDrop[i]);
            }
        }
        else{
            throw(new GenericException("No pude abrir el archivo de prefijos!\n"));
        }
        gameFrames = new std::vector<uint8>* [params.goodVidsCount + params.badVidsCount];
        params.RAMCount = atoi(argv[6]);
        params.ROM =  argv[7];
        omp_set_num_threads(sysconf( _SC_NPROCESSORS_ONLN ));
        #pragma omp parallel shared(params, gameFrames, framesToDrop)
        {
            #pragma omp for
            for(unsigned int i=0;i<params.goodVidsCount + params.badVidsCount;i++){
                if(i < params.goodVidsCount) {
                    gameFrames[i]=calculateIndividualFitness(i, params.goodVids[i], &params, framesToDrop[i]);
                } else {
                    gameFrames[i]=calculateIndividualFitness(i,params.badVids[i-params.goodVidsCount], &params, framesToDrop[i]);
                }
            }
        }
        delete [] framesToDrop;

        std::vector<uint8>** goodVideos = new std::vector<uint8>*[params.goodVidsCount];
        for(unsigned int i = 0; i < params.goodVidsCount; i++){
            goodVideos[i] = gameFrames[i];
        }
        params.avgGoodVideo = averageVideos(goodVideos, params.goodVidsCount);
        std::vector<uint8>** badVideos = new std::vector<uint8>*[params.badVidsCount];
        for(unsigned int i = 0; i < params.badVidsCount; i++){
            badVideos[i] = gameFrames[params.goodVidsCount + i];
        }
        params.avgBadVideo = averageVideos(badVideos, params.badVidsCount);

        delete [] goodVideos;
        delete [] badVideos;
        for(unsigned int i = 0; i < params.goodVidsCount + params.badVidsCount; i++){
            delete gameFrames[i];
        }
        delete [] gameFrames;

        FinalStatistics<Solution>* finalResult = evolveSolutions(params, weightsPath);
        Solution* result = finalResult->data->bestCandidate;

        float shift = calculateShift(result, params);

        printf("Best solution: Mask: %d\n\n", params.RAMCount);
        printf("Fitness: %lf\nSolution: %lf ", finalResult->data->bestCandidateFitness, shift);
        result->print(params.RAMMap);
        printf("\nElapsed time: %ld \n", finalResult->data->elapsedTime);
        printf("Mean fitness: %lf \n", finalResult->data->meanFitness);
        printf("Fitness standard deviation: %lf \n", finalResult->data->fitnessStandardDeviation);
        printf("First generation to find best candidate: %d \n", finalResult->data->bestCandidateFirstGeneration);
        char path[100];
        sprintf(path, "%dsol.txt", params.evolutiveProcessID);


        ofstream solutionFile(path, std::ofstream::out);
        solutionFile << "Instance: " << params.instance << endl
                   << "Solution: ";

        result->print(solutionFile, params.RAMMap);

        solutionFile << endl
                   << "Fitness: " << finalResult->data->bestCandidateFitness << endl
                   << "Elapsed time: " << finalResult->data->elapsedTime << "s" << endl
                   << "Mean fitness: " << finalResult->data->meanFitness << endl
                   << "Fitness standard deviation: " << finalResult->data->fitnessStandardDeviation << endl
                   << "First generation to find best candidate: " << finalResult->data->bestCandidateFirstGeneration + 1 << endl;

        solutionFile.close();

        delete finalResult;

    } catch (exception ex) {
        std::cout << "An exception occurred. Code: " << ex.what() << endl;
    }
    return 0;
}
