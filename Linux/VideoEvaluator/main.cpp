#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <omp.h>
#include <ext/stdio_filebuf.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "GenericException.h"

#define uint8 unsigned char 

float shift;
unsigned int goodVidsCount;
unsigned int badVidsCount;
char** goodVids2;
char** badVids2;

int RAMCount = 0;
std::vector<int> RAMMask;
std::vector<float> fitnessWeights;

std::vector<uint8>* calculateIndividualFitness(int index, char* vidFile, char* ROM, int framesToDrop){
	//invocar a fceux y obtener el resultado
	char path[200];
	#if __GNUC__ < 5
    sprintf(path, "cd \"Fceux Headless/bin/\"; ./fceux -a ../../%s %s", vidFile, ROM);
    #else
    sprintf(path, "cd \"../Fceux Headless/bin/\"; ./fceux -a ../../%s %s", vidFile, ROM);
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

void loadRAMMap(char* path){
    std::string line;
    std::ifstream RAMMapFile(path);
    if (RAMMapFile.is_open()){
        char elem;
        //ignore first line
        RAMMapFile.ignore(9999,'\n');
        for(unsigned int i=0; i< 64;i++){
                RAMMapFile.ignore(255,'|');
                for(unsigned int j=0; j<31;j++){
                    RAMMapFile >> elem;
                    if(elem != 'S' && elem != 'E' && elem != 'L' && elem != 'I'){
                        RAMMask.push_back(32*i+j);
                    }
                    RAMMapFile.ignore(255,'|');
                }
				RAMMapFile >> elem;
                if(elem != 'S' && elem != 'E' && elem != 'L' && elem != 'I'){
                    RAMMask.push_back(32*i+31);
                }
        }
        RAMMapFile.close();
        RAMCount = RAMMask.size();
    }
    else{
        std::cout << "Unable to open RAMMap file: " << path;
        exit(-1);
    }
}

std::vector<float> loadFitnessWeights(char* path){
    std::vector<float> fWeights;
    std::string line;
    std::ifstream FitnessWeightsFile(path);
    if (FitnessWeightsFile.is_open()){
        //ignore first, second and third lines
        FitnessWeightsFile.ignore(9999,'\n');
        FitnessWeightsFile.ignore(9999,'\n');
        FitnessWeightsFile.ignore(9999,'\n');
        FitnessWeightsFile.ignore(10,' ');
        float data;
		FitnessWeightsFile >> shift;
        for(int i=0;i<RAMCount;i++){
                FitnessWeightsFile >> data;
                fWeights.push_back(data);
        }
    }
    else{
        std::cout << "Unable to open FitnessWeights file: " << path;
        exit(-1);
    }
    return fWeights;
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

void loadVideos(char* path){

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

    goodVidsCount = goodVids.size();
    badVidsCount = badVids.size();
    goodVids2 = new char*[goodVids.size()];
    badVids2 = new char*[badVids.size()];

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
    for(unsigned int i=0;i<goodVidsCount;i++){
        goodVids2[i]=new char[1000];
        strcpy(goodVids2[i],goodPath2);
        strcat(goodVids2[i],goodVids[i]);
    }
    for(unsigned int i=0;i<badVidsCount;i++){
        badVids2[i]=new char[1000];
        strcpy(badVids2[i],badPath2);
        strcat(badVids2[i],badVids[i]);
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

    std::vector<float>* avgVideo = new std::vector<float>(minLength/10*2048, 0.0f);
    for(unsigned int i = 0; i < cantGames; i++){
        for(unsigned int j = 0; j < minLength/10; ++j){
            for(unsigned int k = 0; k < 2048; k++){
                (*avgVideo)[j*2048 + k] += (float)gamesPointer[i][20480*j + k];
            }
        }
    }
    for(unsigned int j = 0; j < minLength/10*2048; j++){
        (*avgVideo)[j]/= (float)cantGames;
    }
    delete [] gamesPointer;
    return avgVideo;
}

std::vector<float> otherWeights;

int main(int argc, char** argv){
        if(argc == 7){
            otherWeights = loadFitnessWeights(argv[6]);
        }
       	loadRAMMap(argv[1]);
		fitnessWeights = loadFitnessWeights(argv[2]);
        loadVideos(argv[3]);
        char* ROM = argv[5];
        int* framesToDrop = new int[goodVidsCount + badVidsCount];
        FILE* fileinsqui = fopen(argv[4],"r");
        if(fileinsqui){
            for(unsigned int i=0;i<goodVidsCount + badVidsCount;++i){
                fscanf(fileinsqui,"%d\n",&framesToDrop[i]);
            }
        }
        else{
            throw(new GenericException("No pude abrir el archivo de prefijos!\n"));
        }
        std::vector<uint8>** gameFrames = new std::vector<uint8>*[goodVidsCount + badVidsCount];
        omp_set_num_threads(sysconf( _SC_NPROCESSORS_ONLN ));
        #pragma omp parallel shared(gameFrames, framesToDrop)
        {
            #pragma omp for
            for(unsigned int i=0;i<goodVidsCount + badVidsCount;i++){
                if(i < goodVidsCount) {
                    gameFrames[i]=calculateIndividualFitness(i, goodVids2[i], ROM, framesToDrop[i]);
                } else {
                    gameFrames[i]=calculateIndividualFitness(i, badVids2[i-goodVidsCount], ROM, framesToDrop[i]);
                }
            }
        }
        
        double* fitness = new double[goodVidsCount + badVidsCount];
        double value;
        std::vector<float>* avgGood;
        std::vector<float>* avgBad;
        if(argc == 7){
            for(unsigned int i=0; i<goodVidsCount;++i){
                printf("Video %s\n",goodVids2[i]);
                fitness[i] = 0.0;
                for(unsigned int j=0; j<gameFrames[i]->size()/2048;++j){
                    value = 0;            
                    for(unsigned int k=0;k<RAMCount;++k){
                        value += (*gameFrames[i])[j*2048+RAMMask[k]]*fitnessWeights[k] - (*gameFrames[i])[j*2048+RAMMask[k]]*otherWeights[k];    
                    }
				    value-= shift;
                    fitness[i] += value;
                    printf("%u,%lf\n",j+framesToDrop[i], value);
                }
                printf("El video %s tuvo %lf fitness\n",goodVids2[i],fitness[i]);
            }
            
            for(unsigned int i=goodVidsCount; i<goodVidsCount+badVidsCount;++i){
                printf("Video %s\n",badVids2[i-goodVidsCount]);
                fitness[i] = 0.0;
                for(unsigned int j=0; j<gameFrames[i]->size()/2048;++j){            
                    value=0;                
                    for(unsigned int k=0;k<RAMCount;++k){
                        value += (*gameFrames[i])[j*2048+RAMMask[k]]*fitnessWeights[k] - (*gameFrames[i])[j*2048+RAMMask[k]]*otherWeights[k];
                    }
				    value-= shift;
                    fitness[i] += value;
                    printf("%u,%lf\n",j+framesToDrop[i], value);
                }
                printf("El video %s tuvo %lf fitness\n",badVids2[i-goodVidsCount],fitness[i]);
            }
            delete [] framesToDrop;
            std::vector<uint8>** goodVideos = new std::vector<uint8>*[goodVidsCount];
            for(unsigned int i = 0; i < goodVidsCount; ++i){
                goodVideos[i] = gameFrames[i];
            }
            std::vector<uint8>** badVideos = new std::vector<uint8>*[badVidsCount];
            for(unsigned int i = 0; i < badVidsCount; ++i){
                badVideos[i] = gameFrames[goodVidsCount + i];
            }
     
            avgGood = averageVideos(goodVideos,goodVidsCount);
            avgBad = averageVideos(badVideos,badVidsCount);

            delete [] goodVideos;
            delete [] badVideos;

            printf("Valor de promedio de videos buenos:\n");
            for(unsigned int i=0; i<avgGood->size()/2048;++i){
			    value = 0.0;
                for(unsigned int k=0;k<RAMCount;++k){
                    value += (*avgGood)[i*2048+RAMMask[k]]*fitnessWeights[k] - (*avgGood)[i*2048+RAMMask[k]]*otherWeights[k];    
                }
			    value-= shift;
                printf("%u,%f\n",i*10,value);
            }

            printf("\nValor de promedio de videos malos:\n");
            for(unsigned int i=0; i<avgBad->size()/2048;++i){
			    value = 0.0;
                for(unsigned int k=0;k<RAMCount;++k){
                    value += (*avgBad)[i*2048+RAMMask[k]]*fitnessWeights[k] - (*avgBad)[i*2048+RAMMask[k]]*otherWeights[k];    
                }
			    value-= shift;
                printf("%u,%f\n",i*10,value);
            }

        }
        else{
            for(unsigned int i=0; i<goodVidsCount;++i){
                printf("Video %s\n",goodVids2[i]);
                fitness[i] = 0.0;
                for(unsigned int j=0; j<gameFrames[i]->size()/2048;++j){
                    value = 0;            
                    for(unsigned int k=0;k<RAMCount;++k){
                        value += (*gameFrames[i])[j*2048+RAMMask[k]]*fitnessWeights[k];    
                    }
				    value-= shift;
                    fitness[i] += value;
                    printf("%u,%lf\n",j+framesToDrop[i], value);
                }
                printf("El video %s tuvo %lf fitness\n",goodVids2[i],fitness[i]);
            }
            
            for(unsigned int i=goodVidsCount; i<goodVidsCount+badVidsCount;++i){
                printf("Video %s\n",badVids2[i-goodVidsCount]);
                fitness[i] = 0.0;
                for(unsigned int j=0; j<gameFrames[i]->size()/2048;++j){            
                    value=0;                
                    for(unsigned int k=0;k<RAMCount;++k){
                        value += (*gameFrames[i])[j*2048+RAMMask[k]]*fitnessWeights[k];
                    }
				    value-= shift;
                    fitness[i] += value;
                    printf("%u,%lf\n",j+framesToDrop[i], value);
                }
                printf("El video %s tuvo %lf fitness\n",badVids2[i-goodVidsCount],fitness[i]);
            }
            delete [] framesToDrop;
            std::vector<uint8>** goodVideos = new std::vector<uint8>*[goodVidsCount];
            for(unsigned int i = 0; i < goodVidsCount; ++i){
                goodVideos[i] = gameFrames[i];
            }
            std::vector<uint8>** badVideos = new std::vector<uint8>*[badVidsCount];
            for(unsigned int i = 0; i < badVidsCount; ++i){
                badVideos[i] = gameFrames[goodVidsCount + i];
            }
     
            avgGood = averageVideos(goodVideos,goodVidsCount);
            avgBad = averageVideos(badVideos,badVidsCount);

            delete [] goodVideos;
            delete [] badVideos;

            printf("Valor de promedio de videos buenos:\n");
            for(unsigned int i=0; i<avgGood->size()/2048;++i){
			    value = 0.0;
                for(unsigned int k=0;k<RAMCount;++k){
                    value += (*avgGood)[i*2048+RAMMask[k]]*fitnessWeights[k];    
                }
			    value-= shift;
                printf("%u,%f\n",i*10,value);
            }

            printf("\nValor de promedio de videos malos:\n");
            for(unsigned int i=0; i<avgBad->size()/2048;++i){
			    value = 0.0;
                for(unsigned int k=0;k<RAMCount;++k){
                    value += (*avgBad)[i*2048+RAMMask[k]]*fitnessWeights[k];    
                }
			    value-= shift;
                printf("%u,%f\n",i*10,value);
            }
        }
        delete avgGood;
        delete avgBad;
        delete [] fitness;
        for(unsigned int i=0;i<goodVidsCount;++i){
            delete [] goodVids2[i];
            delete gameFrames[i];
        }
        for(unsigned int i=0;i<badVidsCount;++i){
            delete [] badVids2[i];
            delete gameFrames[i+goodVidsCount];
        }
        delete [] gameFrames;
        delete [] goodVids2;
        delete [] badVids2;
        return 0;
}
