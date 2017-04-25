#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <ext/stdio_filebuf.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>
#include <algorithm>

#define uint8 unsigned char
#define CANT_REPETITIONS 100
#define INTERVAL_LENGTH 1
#define EQUAL_THRESHOLD 2000

//Dumbed down FM2 parser extracted from FCEUX

uint8 parseJoy(FILE* is)
{
	char buf[8];
	fread(buf,sizeof(char),8,is);
	uint8 joystate = 0;
	for(int i=0;i<8;i++)
	{
		joystate <<= 1;
		joystate |= ((buf[i]=='.'||buf[i]==' ')?0:1);
	}
	return joystate;
}

uint8 parse(FILE* is)
{
	uint8 res;
	//by the time we get in here, the initial pipe has already been extracted
	int dummy;
	//extract the commands
	fscanf(is,"%d",&dummy);

	//*is >> commands;
	fgetc(is); //eat the pipe

	//a special case: if fourscore is enabled, parse four gamepads

	res = parseJoy(is);
	fgetc(is); //eat the pipe

	parseJoy(is);
	fgetc(is); //eat the pipe


	//(no fcexp data is logged right now)
	fgetc(is); //eat the pipe

	//should be left at a newline
	return res;
}


uint8 parseBinary(FILE* is)
{
	fgetc(is);

	//check for eof
	if(feof(is)) return false;
	uint8 res = (uint8)fgetc(is);
	fgetc(is);
	return res;
}

void LoadFM2_binarychunk(std::vector<uint8> *keyPresses, FILE* fp)
{

	//find out how much remains in the file
	int curr = ftell(fp);
	fseek(fp,0,SEEK_END);
	int end = ftell(fp);
	int flen = end-curr;
	fseek(fp,curr,SEEK_SET);

	int numRecords = flen/3;

	for(int i=0;i<numRecords;i++)
	{
		keyPresses->push_back(parseBinary(fp));
	}
}

//yuck... another custom text parser.
bool LoadFM2(std::vector<uint8> *keyPresses, FILE* fp)
{
	// if there's no "binary" tag in the movie header, consider it as a movie in text format
	bool binaryFlag = false;

	std::ios::pos_type curr = ftell(fp);

	// first, look for an fcm signature
	char fcmbuf[3];
	fread(fcmbuf,sizeof(char),3,fp);
	fseek(fp,curr,SEEK_SET);
	if(!strncmp(fcmbuf,"FCM",3)) {
		printf("FCM File format is no longer supported. Please use Tools > Convert FCM");
		return false;
	}

	//movie must start with "version 3"
	char buf[9];
	curr = ftell(fp);
	fread(buf,sizeof(char),9,fp);
	fseek(fp,curr,SEEK_SET);
	if(ferror(fp)) return false;
	if(memcmp(buf,"version 3",9)) return false;

	std::string key,value;
	enum {
		NEWLINE, KEY, SEPARATOR, VALUE, RECORD, COMMENT, SUBTITLE
	} state = NEWLINE;
	bool bail = false;
	bool iswhitespace, isrecchar, isnewline;
	int c;
	int size;
	//long int size = ftell(fp);
	for(;;)
	{
		//if(size--<=0) goto bail;
		c = fgetc(fp);
		if(c == -1)
			goto bail;
		iswhitespace = (c==' '||c=='\t');
		isrecchar = (c=='|');
		isnewline = (c==10||c==13);
		if(isrecchar && binaryFlag)
		{
			LoadFM2_binarychunk(keyPresses, fp);
			return true;
		}
		switch(state)
		{
		case NEWLINE:
			if(isnewline) goto done;
			if(iswhitespace) goto done;
			if(isrecchar)
				goto dorecord;
			//must be a key
			key = "";
			value = "";
			goto dokey;
			break;
		case RECORD:
			{
				dorecord:
				int preparse = ftell(fp);
				keyPresses->push_back(parse(fp));
				int postparse = ftell(fp);
				size -= (postparse-preparse);
				state = NEWLINE;
				break;
			}

		case KEY:
			dokey: //dookie
			state = KEY;
			if(iswhitespace) goto doseparator;
			if(isnewline) goto commit;
			key += c;
			break;
		case SEPARATOR:
			doseparator:
			state = SEPARATOR;
			if(isnewline) goto commit;
			if(!iswhitespace) goto dovalue;
			break;
		case VALUE:
			dovalue:
			state = VALUE;
			if(isnewline) goto commit;
			value += c;
		}
		goto done;

		bail:
		bail = true;
		if(state == VALUE) goto commit;
		goto done;
		commit:
		if(key == "binary"){
			binaryFlag = atoi(key.c_str())!=0;
		}
		state = NEWLINE;
		done: ;
		if(bail) break;
	}

	return true;
}


//End of dumbed down FM2 parser



//inference functions

void findStaticAndErraticLocations(std::vector<uint8>* RAMs, int* objectives){
	double E, var, X, delta;
	for(int i=0;i<2048;i++){
		E = 0;
		var = 0;
		for(unsigned int j=1;j<=RAMs->size()/2048;j++){
			X = (double)(*RAMs)[i+2048*(j-1)];
			delta = X - E;
			E += delta/(double)j;
			var += delta*(X - E);

		}
		var /=(double)(RAMs->size()/2048-1);
		//magic number of extremely high caliber
		if(var < 0.17){
			objectives[i]=-1;
		}
		//erratic values test, experimiental, may be detrimental
		else if(var > 12100){
			objectives[i]=-2;
		}
	}
}

void findRandomLocations(std::vector<uint8>* RAMs, int* objectives){
	int* reps = new int[256];
	double max, sum;
	for(int i=0;i<2048;i++){
		if(objectives[i]>=0){
			for(int n=0;n<256;n++){
				reps[n]=0;
			}
			for(unsigned int j=1;j<=RAMs->size()/2048;j++){
				reps[(*RAMs)[i+2048*(j-1)]]++;

			}
			max = 0;
			sum = 0;
			for(int n=0;n<256;n++){
				sum += (double)reps[n];
				if(max < (double)reps[n]){
					max = (double) reps[n];
				}
			}

			//check for abnormal maximum
			if((max*100.0/sum)>95.0){
				objectives[i]=-3;
			}
		}
 	}
	delete [] reps;
}


void printMeMan(std::vector<uint8>* RAMs){
	printf("  ");
	for(int n=0;n<32;n++){
		printf("\t%02X",n);
	}
	for(int a=0;a<64;a++){
		printf("\t%02X",a*2);
		for(int n=0;n<32;n++){
			printf("\t%d", (*RAMs)[n+a*32+2048*(RAMs->size()/2048-1)]);
 		}
		printf("\n");
 	}
}
//end of inference functions


std::vector<uint8>* obtainRAMs(char* filename, char* rom){
        //invocar a fceux y obtener el resultado
	std::vector<uint8>* input = new std::vector<uint8>();
	int data;
    char path[200];
    sprintf(path, "cd \"../Fceux Headless/bin/\"; ./fceux -a %s %s", filename, rom);
    FILE *fp = popen(path,"r");
	int MAHCRAP = fileno(fp);
	__gnu_cxx::stdio_filebuf<char> makeFilesGreatAgain(MAHCRAP, std::ios::in );// 1 CRAP
	std::istream instr(&makeFilesGreatAgain);
	while(instr >> data){
		input->push_back((uint8) data);
	}
	pclose(fp);
	return input;
}


typedef struct {
    int* locations;
    int sign;
    int length;
} lexicographicOrdering;


lexicographicOrdering* findRandomOrder(std::vector<int> candidates, std::vector<uint8>* bestVideo, int sign, int step){
    //generar un orden aleatorio, y despues sacamos de atras porque somos vagos
    std::vector<int> lexOrd;
    std::random_shuffle(candidates.begin(), candidates.end());
    while(!candidates.empty()){
        lexOrd.push_back(candidates.back());
        candidates.pop_back();
        bool validOrdering = true;
        bool tight = false;
        for(unsigned int i = 0; i < bestVideo->size()/2048 - step; i+=step){
            unsigned int indexRAM = 0;
            while( (indexRAM < lexOrd.size()) && (*bestVideo)[i*2048 + lexOrd[indexRAM]] == (*bestVideo)[(i+step)*2048 + lexOrd[indexRAM]] ) {
                indexRAM++;
            }
            if((indexRAM < lexOrd.size()) && (*bestVideo)[i*2048 + lexOrd[indexRAM]]*sign > (*bestVideo)[(i+step)*2048 + lexOrd[indexRAM]]*sign) { //el valor decrece en el siguiente frame
                validOrdering = false;
                break;
            }
            tight |= indexRAM == lexOrd.size()-1;
        }
        if(!validOrdering || !tight){
            lexOrd.pop_back();
        }
    }
    if(lexOrd.size()>1){
        lexicographicOrdering* lOrd = new lexicographicOrdering();
        lOrd->length=lexOrd.size();
        lOrd->locations = new int[lOrd->length];
        for(int i=0;i<lOrd->length;++i){
            lOrd->locations[i] = lexOrd[i];
        }
        lOrd->sign=sign;
        return lOrd;
    }
    else{
        return NULL;
    }
}

//nasty hack hackinson
inline bool inOrdering(int* ord, int value, int length){
    for(int i=0;i<length;++i){
        if(ord[i] == value){
            return true;
        }
    }
    return false;
}

std::vector<int> dropLexOrd(std::vector<int>& candidates, lexicographicOrdering* lexOrd){
    std::vector<int> newCands;
    int lexCount=0;
    while(!candidates.empty()){
        if(lexCount < lexOrd->length && !inOrdering(lexOrd->locations,candidates.back(),lexOrd->length)){
            newCands.push_back(candidates.back());
        }
        candidates.pop_back();
    }
    return newCands;
}



void llsq(std::vector<double>& video, double &a, double &b){
	double bot, top, xbar, ybar;
	unsigned int n = video.size();

	if (n == 1){
		a = 0.0;
		b = video[0];
		return;
	}

	xbar = 0.0;
	ybar = 0.0;
	for (unsigned int i = 0; i < n; i++){
		xbar = xbar + i;
		ybar = ybar + video[i];
	}
	xbar = xbar / (double)n;
	ybar = ybar / (double)n;

	top = 0.0;
	bot = 0.0;
	for (unsigned int i = 0; i < n; i++){
		top = top + (i - xbar) * (video[i] - ybar);
		bot = bot + (i - xbar) * (i - xbar);
	}
	a = top / bot;
	b = ybar - a * xbar;
	return;
}

inline double evalLexOrd(std::vector<uint8>* video, unsigned int frame, int* locations, unsigned int cantLocations, unsigned int* maxValues) {
	double value = (*video)[2048*frame + locations[cantLocations-1]];
	double weight = 1.0;
	double maxVal = maxValues[locations[cantLocations-1]];
	for(int i = cantLocations - 2; i >= 0; --i){
		value+= (*video)[2048*frame + locations[i]] * (maxVal + 1.0) * weight;
		maxVal=maxValues[locations[i]];
		weight*=maxVal;
	}
	return value;
}

inline double* evalLexOrdWeight(double weight, int* locations, unsigned int cantLocations, unsigned int* maxValues) {
	double* values = new double[cantLocations];
	values[cantLocations-1] = 1;
	for(int i = cantLocations - 1; i > 0; i--){
        values[i-1] = weight*(maxValues[locations[i]] + 1) * values[i];
	}

	/* adjust weights down */
/*
	if(fabs(values[0]) > (double)INTERVAL_LENGTH) {

//        printf("El peso del mas significativo es: %lf\n", values[0]);

        double adjustValue = fabs((double)INTERVAL_LENGTH/values[0]);

//        printf("El valor de ajuste es: %lf\n", adjustValue);

        for(unsigned int i = 0; i < cantLocations; i++){
//            printf("\tAjustando peso %d: %lf ---> ", i, values[i]);
            values[i] *= adjustValue;
//            printf("%lf\n", values[i]);
        }
//        printf("\n");
	}
*/
	return values;
}

double lexOrdWeight(std::vector<double>& goodVideo, std::vector<double>& badVideo){
	unsigned int goodLength = goodVideo.size();
	unsigned int badLength = badVideo.size();
	double avgGood = 0.0;
	double avgBad = 0.0;
	double a,b, extendedValue;
	if(goodLength > badLength) {
		for(unsigned int i = 0; i < badLength; i++){
			avgGood += (goodVideo[i] - avgGood)/((double)i+1.0);
			avgBad += (badVideo[i] - avgBad)/((double)i+1.0);
		}
		llsq(badVideo, a, b);
		extendedValue = a*badLength + b;
		for(unsigned int i = badLength; i < goodLength; i++){
			avgGood += (goodVideo[i] - avgGood)/((double)i+1.0);
			avgBad += (extendedValue - avgBad)/((double)i+1.0);
		}
	} else {
		for(unsigned int i = 0; i < goodLength; i++){
			avgGood += (goodVideo[i] - avgGood)/((double)i+1.0);
			avgBad += (badVideo[i] - avgBad)/((double)i+1.0);
		}
		llsq(goodVideo, a, b);
		extendedValue = a*goodLength + b;
		for(unsigned int i = goodLength; i < badLength; i++){
			avgGood += (extendedValue - avgGood)/((double)i+1.0);
			avgBad += (badVideo[i] - avgBad)/((double)i+1.0);
		}
	}

	if(avgGood == 0){
        return (avgBad == 0)? 0 : -INFINITY;
	}

	if(avgBad == 0){
        return (avgGood == 0)? 0 : INFINITY;
	}


/*
	if(avgGood == 0){
        return (avgBad == 0)? 0 : -INTERVAL_LENGTH;
	}

	if(avgBad == 0){
        return (avgGood == 0)? 0 : INTERVAL_LENGTH;
	}
*/
	double weight;
	if(avgGood > avgBad) {
		weight = avgGood/avgBad -1.0;
	} else {
		weight = 1.0 - avgBad/avgGood;
	}

	return weight;

	//return INTERVAL_LENGTH*tanh(0.5*weight);
}

std::vector<double> averageFunctions(std::vector<uint8>** games, int cantGames, int* lexOrd, int lexCant, unsigned int* maxValues){
	unsigned int minLength = games[0]->size();
	for(int i=1;i<cantGames;++i){
		if(minLength>games[i]->size()){
			minLength=games[i]->size();
		}
	}

	std::vector<double> values(minLength/2048, 0);
	double val;
	for(unsigned int i=0;i<minLength/2048;++i){
		val=0;
		for(int j=0;j<cantGames;++j){
			val+=evalLexOrd(games[j],i,lexOrd,lexCant, maxValues);
		}
		val/=(double)cantGames;
		values.push_back(val);
	}
	return values;
}

inline lexicographicOrdering* mergeLex(lexicographicOrdering* L1, lexicographicOrdering* L2){
    if(L1->sign != L2->sign){
        return NULL;
    }
    else{
        lexicographicOrdering* newLex = new lexicographicOrdering();
        newLex->length=L1->length+L2->length;
        newLex->locations=new int[newLex->length];
        for(unsigned int i=0; i<L1->length;++i){
            newLex->locations[i]=L1->locations[i];
        }
        for(unsigned int i=0; i<L2->length;++i){
            newLex->locations[L1->length+i]=L2->locations[i];
        }
        newLex->sign = L1->sign;
        return newLex;
    }
}

inline bool isOrdering(lexicographicOrdering& lexOrd, std::vector<uint8>* bestVideo){
        bool validOrdering = true;
        bool* tightArr = (bool*)calloc(lexOrd.length,sizeof(bool));
        for(unsigned int i = 0; i < bestVideo->size()/2048 - 1; ++i){
            unsigned int indexRAM = 0;
            while( (indexRAM < lexOrd.length) && (*bestVideo)[i*2048 + lexOrd.locations[indexRAM]] == (*bestVideo)[(i+1)*2048 + lexOrd.locations[indexRAM]] ) {
                indexRAM++;
            }
            if((indexRAM < lexOrd.length) && (*bestVideo)[i*2048 + lexOrd.locations[indexRAM]]*lexOrd.sign > (*bestVideo)[(i+1)*2048 + lexOrd.locations[indexRAM]]*lexOrd.sign ) { //el valor decrece en el siguiente frame
                validOrdering = false;
                break;
            }
            else if((indexRAM < lexOrd.length)){
                tightArr[indexRAM] =  true;
            }
        }
        bool tight = true;
        for(unsigned int i=0;i<lexOrd.length;++i){
            if(!tightArr[i]){
                tight = false;
                break;
            }
        }
        free(tightArr);
        if(!validOrdering || !tight){
            return false;
        }
        else{
            return true;
        }
}

inline int mergeLexOrds(std::vector<lexicographicOrdering*>* lexOrders, std::vector<uint8>* bestVideo){
    int cantMerged = 0;
    if(lexOrders->size() > 1){
        std::vector<lexicographicOrdering*> candidates;
        for(unsigned int i=0;i<lexOrders->size();++i){
            candidates.push_back((*lexOrders)[i]);
        }
        int current = 1;
        lexicographicOrdering* newCand;
        while(candidates.size()>1){
            newCand = mergeLex(candidates[0], candidates[current]);
            //se chequea una sola vez, cuando se llama abajo si no fue null aca no puede ser alla
            if (newCand == NULL){
                current++;
                if(current >= candidates.size()){
                    current = 1;
                    candidates.erase(candidates.begin());
                }
                continue;
            }
            if(isOrdering(*newCand, bestVideo)){
                ++cantMerged;
                lexOrders->erase(std::find(lexOrders->begin(), lexOrders->end(), candidates[0]));
                lexOrders->erase(std::find(lexOrders->begin(), lexOrders->end(), candidates[current]));
                delete candidates[0];
                delete candidates[current];
                candidates.erase(candidates.begin()+current);
                candidates.erase(candidates.begin());
                lexOrders->push_back(newCand);
                candidates.push_back(newCand);
                current = 1;
                continue;
            }
            else{
                delete newCand;
            }
            newCand = mergeLex(candidates[current],candidates[0]);
            if(isOrdering(*newCand, bestVideo)){
                ++cantMerged;
                lexOrders->erase(std::find(lexOrders->begin(), lexOrders->end(), candidates[0]));
                lexOrders->erase(std::find(lexOrders->begin(), lexOrders->end(), candidates[current]));
                delete candidates[0];
                delete candidates[current];
                candidates.erase(candidates.begin()+current);
                candidates.erase(candidates.begin());
                lexOrders->push_back(newCand);
                candidates.push_back(newCand);
                current = 1;
                continue;
            }
            else{
                delete newCand;
            }
            current++;
            if(current >= candidates.size()){
               current = 1;
               candidates.erase(candidates.begin());
            }
        }
    }
    return cantMerged;
}

int main(int argc, char** argv){
	unsigned int gameCount = argc - 3;
	unsigned int cantGood = (unsigned int)atoi(argv[argc-1]);
	std::vector<uint8> prefix;
	std::vector<uint8>** videos = new std::vector<uint8>*[gameCount];


	std::vector<uint8>** games = new std::vector<uint8>*[gameCount];
	omp_set_dynamic(0);     // Explicitly disable dynamic teams
    omp_set_num_threads(gameCount);
	#pragma omp parallel shared(videos, argv)
    {
		#pragma omp for
		for(unsigned int i = 0; i< gameCount; i++){
			games[i] = obtainRAMs(argv[i+2],argv[1]);
			FILE* file = fopen(&(argv[i+2][3]),"r");
			std::vector<uint8>* keypresses = new std::vector<uint8>();
			LoadFM2(keypresses, file);
			fclose(file);
			videos[i]=keypresses;
		}
	}

	unsigned int minVideo = 0;
	unsigned int minLength = videos[0]->size();
	for(unsigned int i = 1; i < gameCount; i++) {
		if(videos[i]->size() < minLength) {
			minLength = videos[i]->size();
			minVideo = i;
		}
	}

	//generate prefix file
	std::vector<uint8>* video=videos[minVideo]; //take second video, we need at least one good and one bad video, so this specific video must exist
	uint8 lastKey=0;

	//populate prefix with entire video
	for(unsigned int j=0;j<video->size();j++){
		if((*video)[j] != 0 && (*video)[j]!=lastKey){
			lastKey=(*video)[j];
			prefix.push_back(lastKey);
		}
		lastKey=(*video)[j];
	}
	bool consumeKey;
	int keyCount;
	for(unsigned int i=1; i<gameCount;i++){
		if(i != minVideo){
		    consumeKey = false;
			video=videos[i];
			lastKey=0;
			keyCount=0;
			for(unsigned int j=0;j<video->size();j++){
				if(consumeKey == true){
		            if((*video)[j]== lastKey){
		            }
		            else if((*video)[j] == 0){
		                consumeKey = false;
		                lastKey=0;
		            }
		            else{
		                if(keyCount<prefix.size()){
		                    if(prefix[keyCount] == (*video)[j]){
		                        keyCount++; //consumed one key from prefix
		                    }
		                    else{
		                        //found new minimal prefix
		                        prefix.erase(prefix.begin()+keyCount,prefix.end());
		                        break;
		                    }
		                    lastKey=(*video)[j];
		                }
		                else{
		                    //exhausted prefix
		                    break;
		                }
		            }
		        }
		        else{
		            if(keyCount<prefix.size()){
		                if((*video)[j] != 0 && (*video)[j]!=lastKey){
		                    if(prefix[keyCount] == (*video)[j]){
		                        keyCount++; //consumed one key from prefix
		                        consumeKey=true;
		                    }
		                    else{
		                        //found new minimal prefix
		                        prefix.erase(prefix.begin()+keyCount,prefix.end());
		                        break;
		                    }
		                }
		                lastKey=(*video)[j];
		            }
		            else{
		                //exhausted prefix
		                break;
		            }
		        }
			}
			if(prefix.size()>keyCount){
                    //video too short, found new minimal prefix
                    prefix.erase(prefix.begin()+keyCount,prefix.end());
			}
		}
	}


	//print prefix of first video, which is a good video by call convention (see EvolutionaryPipeline.sh)

	video=videos[0];
	lastKey=0;
	keyCount=0;
	consumeKey = false;
	std::vector<uint8> finalPrefix;
	FILE* prefixFile = fopen("prefix","w");
	if(prefixFile != NULL){
		for(unsigned int j=0;j<video->size();j++){
			if(consumeKey == true){
				if((*video)[j]== lastKey){
					finalPrefix.push_back((*video)[j]);
				}
				else if((*video)[j] == 0){
					finalPrefix.push_back((*video)[j]);
					consumeKey = false;
					lastKey=0;
				}
				else{
					if(keyCount<prefix.size()){
						finalPrefix.push_back((*video)[j]);
						if(prefix[keyCount] == (*video)[j]){
							keyCount++; //consumed one key from prefix
						}
						else{
							//found new minimal prefix
							prefix.erase(prefix.begin()+keyCount,prefix.end());
							finalPrefix.erase(finalPrefix.end()-1,finalPrefix.end());
							break;
						}
						lastKey=(*video)[j];
					}
					else{
						//exhausted prefix
						break;
					}
				}
			}
			else{
				if(keyCount<prefix.size()){
					finalPrefix.push_back((*video)[j]);
					if((*video)[j] != 0 && (*video)[j]!=lastKey){
						if(prefix[keyCount] == (*video)[j]){
							keyCount++; //consumed one key from prefix
							consumeKey=true;
						}
						else{
							//found new minimal prefix
							prefix.erase(prefix.begin()+keyCount,prefix.end());
							finalPrefix.erase(finalPrefix.end()-1,finalPrefix.end());
							break;
						}
					}
					lastKey=(*video)[j];
				}
				else{
					//exhausted prefix
					break;
				}
			}
		}

        if(prefix.size()>keyCount){
                //video too short, found new minimal prefix
                prefix.erase(prefix.begin()+keyCount,prefix.end());
        }
		//drop 0's from end of prefix

		unsigned int eraseFirst;
		for(int i=finalPrefix.size()-1;i>0;--i){
			if(finalPrefix[i]!=0){
				eraseFirst=i;
				break;
			}
		}

		if(finalPrefix.size()>0){
			finalPrefix.erase(finalPrefix.begin()+eraseFirst+1,finalPrefix.end());
			fprintf(prefixFile, "%u:",finalPrefix.size());
			for(unsigned int iter=0;iter<finalPrefix.size();++iter){
				fprintf(prefixFile, "%u;", finalPrefix[iter]);
			}
		}
		fprintf(prefixFile, ":");
	}
	else{
		printf("Error, can't create prefix file!\n");
		exit(418);
	}
	fclose(prefixFile);

	if(keyCount < prefix.size()){
        //last key was not part of the prefix
        keyCount--;
	}
	FILE* otroFileMas = fopen("framesToDrop.drop","w");
	if(!otroFileMas){
        printf("No pude abrir el archivo de drops\n");
        exit(-6562);
	}
	int framesToDrop;
	unsigned int prefixPointer;
	std::vector<uint8>* game;
	for(unsigned int i=0;i<gameCount;++i){
		video = videos[i];
		game = games[i];
		framesToDrop = 0;
		prefixPointer = 0;
		consumeKey = false;
		while(prefixPointer<keyCount){
           if(consumeKey == true){
				if((*video)[framesToDrop] == lastKey){
				}
				else if((*video)[framesToDrop] == 0){
					consumeKey = false;
					lastKey=0;
				}
				else{
                    if(prefix[prefixPointer] == (*video)[framesToDrop]){
                        prefixPointer++; //consumed one key from prefix
                    }
                    lastKey=(*video)[framesToDrop];
				}
				framesToDrop++;
			}
			else{
                if((*video)[framesToDrop] != 0 && (*video)[framesToDrop]!=lastKey){
                    if(prefix[prefixPointer] == (*video)[framesToDrop]){
                        prefixPointer++; //consumed one key from prefix
                        consumeKey=true;
                    }
                }
                lastKey=(*video)[framesToDrop];
                framesToDrop++;
			}
		}

		//drop frames from video
		game->erase(game->begin(),game->begin() + (framesToDrop-2)*2048);
		fprintf(otroFileMas,"%d\n",framesToDrop);
	}
    fclose(otroFileMas);
	//delete videos, not needed anymore

	for(unsigned int i=0;i<gameCount;++i){
		delete videos[i];
	}
	delete [] videos;

	int** objectives = new int*[gameCount];
	for(unsigned int j=0;j<gameCount;j++){
		objectives[j]=new int[2048];
		for(int i=0;i<2048;i++){
			objectives[j][i]=0;
		}
	}

	//INFER MY FRIEND, INFER!
	for(unsigned int runs = 0; runs< gameCount;runs++){
		findStaticAndErraticLocations(games[runs], objectives[runs]);
		findRandomLocations(games[runs], objectives[runs]);
	}

    //generate output with analysis results
	std::vector<int> unmaskedRAM;
    int* RAMMask = new int[2048];

	int partialJudgement=0;
	for(int a=0;a<64;a++){
		for(int n=0;n<32;n++){
			partialJudgement=objectives[0][n+a*32];
			for(unsigned int j=1; j<gameCount;j++){
				if(partialJudgement == objectives[j][n+a*32]){

				}
				else if(partialJudgement != 0 && objectives[j][n+a*32] != 0){
					partialJudgement = -2;
				}
				else{
					partialJudgement = 0;
				}
			}
			RAMMask[n+a*32] = partialJudgement;
			if(partialJudgement >= 0){
                unmaskedRAM.push_back(n+a*32);
			}
 		}
 	}

	//find longest good video

	std::vector<uint8>* bestVideo = games[0];

	for(unsigned int i =1; i< cantGood; ++i){
	    if(games[i]->size() > bestVideo->size()){
	        bestVideo = games[i];
	    }
	}

	std::vector<lexicographicOrdering*> lexOrders;

    int startIndex = 0;
    int endIndex = 0;
    int startIndexRAM;
    int endIndexRAM;
    int unmaskedSize = (int)unmaskedRAM.size();
    bool validOrdering;
    bool tight;
    std::vector<int> newUnmaskedRAM;

    //crecientes hacia la derecha
    while(startIndex < unmaskedSize){
        validOrdering = true;
        startIndexRAM = unmaskedRAM[startIndex];
        tight = true;
        while(validOrdering && tight) {
            tight = false;
            endIndex++;
            if(endIndex >= unmaskedSize || unmaskedRAM[endIndex - 1] + 1 != unmaskedRAM[endIndex]) {
                break;
            }
            endIndexRAM = unmaskedRAM[endIndex];
            for(unsigned int i = 0; i < bestVideo->size()/2048 - 1; ++i){
                int indexRAM = startIndexRAM;
                while( (indexRAM <= endIndexRAM) && (*bestVideo)[i*2048 + indexRAM] == (*bestVideo)[i*2048 + 2048 + indexRAM] ) {
                    indexRAM++;
                }
                if(indexRAM <= endIndexRAM && (*bestVideo)[i*2048 + indexRAM] > (*bestVideo)[i*2048 + 2048 + indexRAM]) { //el valor decrece en el siguiente frame
                    validOrdering = false;
                    break;
                }
                else if(indexRAM == endIndexRAM){
                    tight = true;
                }
            }
        }

        int orderingLength = endIndex - startIndex;
        if(orderingLength > 1) {
            lexicographicOrdering* lo = new lexicographicOrdering;
            lo->sign = 1;
            lo->locations = new int[orderingLength];
            lo->length = orderingLength;
            for(int i = 0; i < orderingLength; i++) {
                lo->locations[i] = startIndexRAM + i;
            }
            lexOrders.push_back(lo);
        } else {
            newUnmaskedRAM.push_back(startIndexRAM);
        }
        startIndex = endIndex;
    }

    unmaskedRAM.clear();
    unmaskedRAM = newUnmaskedRAM;
    newUnmaskedRAM.clear();


    // decrecientes hacia la derecha
    startIndex = 0;
    endIndex = 0;
    unmaskedSize = unmaskedRAM.size();

    while(startIndex < unmaskedSize){
        validOrdering = true;
        startIndexRAM = unmaskedRAM[startIndex];
        tight=true;
        while(validOrdering && tight) {
            tight = false;
            endIndex++;
            if(endIndex >= unmaskedSize || unmaskedRAM[endIndex - 1] + 1 != unmaskedRAM[endIndex]) {
                break;
            }
            endIndexRAM = unmaskedRAM[endIndex];
            for(unsigned int i = 0; i < bestVideo->size()/2048 - 1; ++i){
                int indexRAM = startIndexRAM;
                while( (indexRAM <= endIndexRAM) && (*bestVideo)[i*2048 + indexRAM] == (*bestVideo)[i*2048 + 2048 + indexRAM] ) {
                    indexRAM++;
                }
                if(indexRAM <= endIndexRAM && (*bestVideo)[i*2048 + indexRAM] < (*bestVideo)[i*2048 + 2048 + indexRAM]) { //el valor crece en el siguiente frame
                    validOrdering = false;
                    break;
                }
                else if(indexRAM == endIndexRAM){
                    tight = true;
                }
            }
        }

        int orderingLength = endIndex - startIndex;
        if(orderingLength > 1) {
            lexicographicOrdering* lo = new lexicographicOrdering;
            lo->sign = -1;
            lo->locations = new int[orderingLength];
            lo->length = orderingLength;
            for(int i = 0; i < orderingLength; i++) {
                lo->locations[i] = startIndexRAM + i;
            }
            lexOrders.push_back(lo);
        } else {
            newUnmaskedRAM.push_back(startIndexRAM);
        }
        startIndex = endIndex;
    }

    unmaskedRAM.clear();
    unmaskedRAM = newUnmaskedRAM;
    newUnmaskedRAM.clear();

    // crecientes hacia la izquierda
    unmaskedSize = unmaskedRAM.size();
    startIndex = unmaskedSize - 1;
    endIndex = startIndex;

    while(startIndex >= 0){
        validOrdering = true;
        tight = true;
        startIndexRAM = unmaskedRAM[startIndex];
        //crecientes hacia la derecha
        while(validOrdering && tight) {
            tight = false;
            endIndex--;
            if((endIndex < 0) || (unmaskedRAM[endIndex + 1] - 1 != unmaskedRAM[endIndex])) {
                break;
            }
            endIndexRAM = unmaskedRAM[endIndex];
            for(unsigned int i = 0; i < bestVideo->size()/2048 - 1; ++i){
                int indexRAM = startIndexRAM;
                while( (indexRAM >= endIndexRAM) && (*bestVideo)[i*2048 + indexRAM] == (*bestVideo)[i*2048 + 2048 + indexRAM] ) {
                    indexRAM--;
                }
                if(indexRAM >= endIndexRAM && (*bestVideo)[i*2048 + indexRAM] > (*bestVideo)[i*2048 + 2048 + indexRAM]) { //el valor decrece en el siguiente frame
                    validOrdering = false;
                    break;
                }
                else if(indexRAM == endIndexRAM){
                    tight = true;
                }
            }
        }

        int orderingLength = startIndex - endIndex;
        if(orderingLength > 1) {
            lexicographicOrdering* lo = new lexicographicOrdering;
            lo->sign = 1;
            lo->locations = new int[orderingLength];
            lo->length = orderingLength;
            for(int i = 0; i < orderingLength; i++) {
                lo->locations[i] = startIndexRAM - i;
            }
            lexOrders.push_back(lo);
        } else {
            newUnmaskedRAM.push_back(startIndexRAM);
        }
        startIndex = endIndex;
    }

    unmaskedRAM.clear();
    unmaskedRAM = newUnmaskedRAM;
    newUnmaskedRAM.clear();


    // decrecientes hacia la izquierda
    unmaskedSize = unmaskedRAM.size();
    startIndex = unmaskedSize - 1;
    endIndex = startIndex;

    while(startIndex >= 0){
        validOrdering = true;
        startIndexRAM = unmaskedRAM[startIndex];
        tight = true;
        //crecientes hacia la derecha
        while(validOrdering && tight) {
            tight = false;
            endIndex--;
            if((endIndex < 0) || (unmaskedRAM[endIndex + 1] - 1 != unmaskedRAM[endIndex])) {
                break;
            }
            endIndexRAM = unmaskedRAM[endIndex];
            for(unsigned int i = 0; i < bestVideo->size()/2048 - 1; ++i){
                int indexRAM = startIndexRAM;
                while( (indexRAM >= endIndexRAM) && (*bestVideo)[i*2048 + indexRAM] == (*bestVideo)[i*2048 + 2048 + indexRAM] ) {
                    indexRAM--;
                }
                if(indexRAM >= endIndexRAM && (*bestVideo)[i*2048 + indexRAM] < (*bestVideo)[i*2048 + 2048 + indexRAM]) { //el valor crece en el siguiente frame
                    validOrdering = false;
                    break;
                }
                else if(indexRAM == endIndexRAM){
                    tight = true;
                }
            }
        }

        int orderingLength = startIndex - endIndex;
        if(orderingLength > 1) {
            lexicographicOrdering* lo = new lexicographicOrdering;
            lo->sign = -1;
            lo->locations = new int[orderingLength];
            lo->length = orderingLength;
            for(int i = 0; i < orderingLength; i++) {
                lo->locations[i] = startIndexRAM - i;
            }
            lexOrders.push_back(lo);
        } else {
            newUnmaskedRAM.push_back(startIndexRAM);
        }
        startIndex = endIndex;
    }

    unmaskedRAM.clear();
    unmaskedRAM = newUnmaskedRAM;
    newUnmaskedRAM.clear();



     //generate random lexOrderings
    lexicographicOrdering* candidateLex;
    for(unsigned int i=0;i<CANT_REPETITIONS;++i){
        candidateLex = findRandomOrder(unmaskedRAM,bestVideo,1,1);
        if(candidateLex!=NULL){
            lexOrders.push_back(candidateLex);
            unmaskedRAM=dropLexOrd(unmaskedRAM,candidateLex);
        }
    }

    for(unsigned int i=0;i<CANT_REPETITIONS;++i){
        candidateLex = findRandomOrder(unmaskedRAM,bestVideo,-1,1);
        if(candidateLex!=NULL){
            lexOrders.push_back(candidateLex);
            unmaskedRAM=dropLexOrd(unmaskedRAM,candidateLex);
        }
    }

    //int cantMerged = mergeLexOrds(&lexOrders, bestVideo);

    for(unsigned int i=0;i<CANT_REPETITIONS;++i){
        candidateLex = findRandomOrder(unmaskedRAM,bestVideo,1,100);
        if(candidateLex!=NULL){
            lexOrders.push_back(candidateLex);
            unmaskedRAM=dropLexOrd(unmaskedRAM,candidateLex);
        }
    }

    for(unsigned int i=0;i<CANT_REPETITIONS;++i){
        candidateLex = findRandomOrder(unmaskedRAM,bestVideo,1,250);
        if(candidateLex!=NULL){
            lexOrders.push_back(candidateLex);
            unmaskedRAM=dropLexOrd(unmaskedRAM,candidateLex);
        }
    }

    for(unsigned int i=0;i<CANT_REPETITIONS;++i){
        candidateLex = findRandomOrder(unmaskedRAM,bestVideo,1,1000);
        if(candidateLex!=NULL){
            lexOrders.push_back(candidateLex);
            unmaskedRAM=dropLexOrd(unmaskedRAM,candidateLex);
        }
    }



    for(unsigned int i=0;i<CANT_REPETITIONS;++i){
        candidateLex = findRandomOrder(unmaskedRAM,bestVideo,-1,100);
        if(candidateLex!=NULL){
            lexOrders.push_back(candidateLex);
            unmaskedRAM=dropLexOrd(unmaskedRAM,candidateLex);
        }
    }

    for(unsigned int i=0;i<CANT_REPETITIONS;++i){
        candidateLex = findRandomOrder(unmaskedRAM,bestVideo,-1,250);
        if(candidateLex!=NULL){
            lexOrders.push_back(candidateLex);
            unmaskedRAM=dropLexOrd(unmaskedRAM,candidateLex);
        }
    }

    for(unsigned int i=0;i<CANT_REPETITIONS;++i){
        candidateLex = findRandomOrder(unmaskedRAM,bestVideo,-1,1000);
        if(candidateLex!=NULL){
            lexOrders.push_back(candidateLex);
            unmaskedRAM=dropLexOrd(unmaskedRAM,candidateLex);
        }
    }

	//generate new mask

    int lexCount = 1;

    int cantIncreasing = 0;
    int cantDecreasing = 0;

    for(unsigned int i=0; i < lexOrders.size(); ++i){
        for(int j = 0; j < lexOrders[i]->length; ++j) {
            RAMMask[lexOrders[i]->locations[j]] = lexCount;
        }
        if(lexOrders[i]->sign > 0){
            ++cantIncreasing;
        }
        else{
            ++cantDecreasing;
        }
        ++lexCount;
    }



    if(cantGood > 1){
        // buscar bytes independientes del juego

        unsigned int* framesEqual = (unsigned int*)calloc(2048,sizeof(unsigned int));
        unsigned int candidatesRemaining = 0;
        bool* mask = new bool[2048];
        for(unsigned int i=0; i<2048;++i){
            if(RAMMask[i] == 0){
                ++candidatesRemaining;
                mask[i] = true;
            }
            else{
                mask[i] = false;
            }
        }

        unsigned int frameCount = 0;
        uint8 ramValue;

        unsigned int minSize = games[0]->size();
        for(unsigned int i=1;i<cantGood;++i){
            if(minSize > games[i]->size()){
                minSize = games[i]->size();
            }
        }

        while(frameCount < minSize/2048 && candidatesRemaining > 0){
            for(unsigned int i = 0; i<2048;++i){
                if(mask[i]){
                    ramValue = (*games[0])[frameCount*2048 + i];
                    for(unsigned int j=1;j<cantGood;++j){
                        if((*games[j])[frameCount*2048 + i] != ramValue){
                            mask[i] = false;
                            --candidatesRemaining;
                            break;
                        }
                    }
                    if(mask[i]){
                        ++framesEqual[i];
                    }
                }
            }
            ++frameCount;
        }

        for(unsigned int i=0; i<2048;++i){
    //        if(framesEqual[i]>=EQUAL_THRESHOLD){
            if(framesEqual[i]>=minSize/2048 - 10){
                RAMMask[i]=-4;
            }
        }
        delete [] mask;
        free(framesEqual);
    }

    std::vector<uint8>** goodVideos = new std::vector<uint8>*[cantGood];
	std::vector<uint8>** badVideos = new std::vector<uint8>*[gameCount - cantGood];
	for(unsigned int i = 0; i < cantGood; i++){
		goodVideos[i] = games[i];
	}
	for(unsigned int i = cantGood; i < gameCount; i++){
		badVideos[i-cantGood] = games[i];
	}

	unsigned int* maxValues = (unsigned int*)calloc(2048, sizeof(unsigned int));
	for(unsigned int g=0;g<gameCount;++g){
        for(unsigned int frame = 0; frame < games[g]->size()/2048; frame++){
            for(unsigned int i = 0; i < 2048; i++){
                if(maxValues[i]	< (*games[g])[2048*frame + i]) {
                    maxValues[i] = (*games[g])[2048*frame + i];
                }
            }
        }
	}

	FILE* stage2Weights = fopen("stage2.weights", "w");
/*
	for(unsigned int i = 0; i < lexOrders.size(); i++){
        std::vector<double> avgGood = averageFunctions(goodVideos, cantGood, lexOrders[i]->locations, lexOrders[i]->length, maxValues);
        std::vector<double> avgBad = averageFunctions(badVideos, gameCount - cantGood, lexOrders[i]->locations, lexOrders[i]->length, maxValues);
		double* weights = evalLexOrdWeight(lexOrdWeight(avgGood, avgBad), lexOrders[i]->locations, lexOrders[i]->length,maxValues);
		for(int j = 0; j < lexOrders[i]->length; j++){
			fprintf(stage2Weights,"%u:%.15lf\n", lexOrders[i]->locations[j], weights[j]);
		}
		delete [] weights;
	}

    int dummy;
	for(unsigned int i = 0; i < 2048; i++){
		if(RAMMask[i] == 0) {
            dummy = i;
            std::vector<double> avgGood = averageFunctions(goodVideos, cantGood, &dummy, 1, maxValues);
            std::vector<double> avgBad = averageFunctions(badVideos, gameCount - cantGood, &dummy, 1, maxValues);
			double weight = lexOrdWeight(avgGood, avgBad);
			fprintf(stage2Weights,"%u:%.15lf\n", i, weight);
		}
	}
*/

//double maxWeightAbs = 0;
double* allWeights = (double*)calloc(2048, sizeof(double));
bool* scaleDown = (bool*)calloc(2048, sizeof(bool));
for(unsigned int i = 0; i < lexOrders.size(); i++){
        std::vector<double> avgGood = averageFunctions(goodVideos, cantGood, lexOrders[i]->locations, lexOrders[i]->length, maxValues);
        std::vector<double> avgBad = averageFunctions(badVideos, gameCount - cantGood, lexOrders[i]->locations, lexOrders[i]->length, maxValues);

		double weight = lexOrdWeight(avgGood, avgBad);
		if(weight == INFINITY){
            for(int l = 0; l < lexOrders[i]->length; l++) {
				allWeights[lexOrders[i]->locations[l]] = INTERVAL_LENGTH;
			}
        } else if (weight == -INFINITY){
			for(int l = 0; l < lexOrders[i]->length; l++) {
				allWeights[lexOrders[i]->locations[l]] = -INTERVAL_LENGTH;
			}
		} else {
			for(int l = 0; l < lexOrders[i]->length; l++) {
				scaleDown[lexOrders[i]->locations[l]] = true;
			}

            double* weights = evalLexOrdWeight(weight, lexOrders[i]->locations, lexOrders[i]->length,maxValues);
            for(int l = 0; l < lexOrders[i]->length; l++) {
                allWeights[lexOrders[i]->locations[l]] = weights[l];
            }
            delete [] weights;
        }
	}

	//Ya no se calculan mas los pesos para estos muchachos, porque generan rudio y nos arruinan todo

	/*
	int dummy;
	for(unsigned int i = 0; i < 2048; i++){
		if(RAMMask[i] == 0) {
            dummy = i;
            std::vector<double> avgGood = averageFunctions(goodVideos, cantGood, &dummy, 1, maxValues);
            std::vector<double> avgBad = averageFunctions(badVideos, gameCount - cantGood, &dummy, 1, maxValues);

			double weight = lexOrdWeight(avgGood, avgBad);
            if(weight == INFINITY){
                weight = INTERVAL_LENGTH;
            } else if (weight == -INFINITY){
                weight = -INTERVAL_LENGTH;
			} else {
				scaleDown[i] = true;
                allWeights[i] = weight;
			}
		}
	}

	*/
/*
	double mean = 0.0, sigma = 0.0, n = 0;
	for(unsigned int i = 0; i < lexOrders.size(); ++i){
		for(int l = 0; l < lexOrders[i]->length; l++) {
			n+=1;
			double x = allWeights[lexOrders[i]->locations[l]];
			double delta = x - mean;
			mean+= delta/n;
			sigma+= delta*(x - mean);
		}
	}
	for(unsigned int i = 0; i < 2048; i++){
		if(RAMMask[i] == 0) {
		    n+=1;
			double x = allWeights[i];
			double delta = x - mean;
			mean+= delta/n;
			sigma+= delta*(x - mean);
		}
	}
	sigma/=(n-1);

	double threshold = mean + 3*sigma;
    for(unsigned int i = 0; i < 2048; i++){
		if(scaleDown[i]){
		    double v = fabs(allWeights[i]);
		    if(v > threshold){
	            scaleDown[i] = false;
		        if(allWeights[i] >= 0) {
		            allWeights[i] = INTERVAL_LENGTH;
		        } else {
		            allWeights[i] = -INTERVAL_LENGTH;
		        }
		    } else if (v > maxWeightAbs){
	            maxWeightAbs = v;
	        }
        }
    }
*/
	for(unsigned int i = 0; i < 2048; i++){
		if(scaleDown[i]){
			//allWeights[i]*=(double)INTERVAL_LENGTH/maxWeightAbs;
			allWeights[i] = INTERVAL_LENGTH*tanh(0.01*allWeights[i]);
		}
	}

	for(unsigned int i = 0; i < lexOrders.size(); i++){
        for(int j = 0; j < lexOrders[i]->length; j++){
            fprintf(stage2Weights,"%u:%.15lf\n", lexOrders[i]->locations[j], allWeights[lexOrders[i]->locations[j]]);
        }
	}
	/*
	for(unsigned int i = 0; i < 2048; i++){
		if(RAMMask[i] == 0) {
			fprintf(stage2Weights,"%u:%.15lf\n", i, allWeights[i]);
		}
	}
	*/
	fclose(stage2Weights);

	delete [] allWeights;
	delete [] scaleDown;

    delete [] goodVideos;
    delete [] badVideos;
	free(maxValues);

    //print updated RAMMask
    int stat = 0;
	int erratic = 0;
	int lowchange = 0;
	int independent = 0;
    FILE* stage2Mask = fopen("stage2.mask", "w");
	fprintf(stage2Mask,"  ");
	for(int n=0;n<32;n++){
		fprintf(stage2Mask,"|%03X",n);
	}
	fprintf(stage2Mask,"\n");
	for(int a=0;a<64;a++){
		fprintf(stage2Mask,"%02X",a*2);
		for(int n=0;n<32;n++){
			if(RAMMask[n+a*32] == -1){
				fprintf(stage2Mask,"| S ");
				stat++;
			}
			else if(RAMMask[n+a*32] == -2){
				fprintf(stage2Mask,"| E ");
				erratic++;
			}
			else if(RAMMask[n+a*32] == -3){
				fprintf(stage2Mask,"| L ");
				lowchange++;
			}
			else if(RAMMask[n+a*32] == -4){
				fprintf(stage2Mask,"| I ");
				independent++;
			}
			else{
                if(RAMMask[n+a*32] > 0){
                    fprintf(stage2Mask,"|%03d", RAMMask[n+a*32]*lexOrders[RAMMask[n+a*32] - 1]->sign);
                }
                else{
                    fprintf(stage2Mask,"|%03d", RAMMask[n+a*32]);
                }
			}
 		}
		fprintf(stage2Mask,"\n");
 	}
	fprintf(stage2Mask,"\nTotal number of static ram locations: %d.\nTotal number of dynamic ram locations: %d.\nTotal number of low changing ram locations: %d.\nTotal number of independent ram locations: %d.\nPercentage of ignored ram: %lf.\nAmount of increasing lexicographic orderings: %d\nAmount of decreasing lexicographic orderings: %d"/*\nQuantity of merged lexicographic orderings: %d"*/, stat, erratic, lowchange, independent, (double)(stat + erratic + lowchange + independent)*100.0/2048.0,cantIncreasing,cantDecreasing);//,cantMerged);
    fprintf(stage2Mask,"\n%d",2048-stat-erratic-lowchange-independent);
    fclose(stage2Mask);
    /*
    for(unsigned int i = 0; i < lexOrders.size(); i++){
        for(int j = 0; j < lexOrders[i]->length; j++) {
            printf("%x ", lexOrders[i]->locations[j]);
        }
        if(lexOrders[i]->sign == 1) {
            printf("( creciente )");
        } else {
            printf("( decreciente )");
        }
        printf("\n\n");
    }
    */

    stat = 0;
	erratic = 0;
	lowchange = 0;
	independent = 0;
	printf("  ");
	for(int n=0;n<32;n++){
		printf("|%02X",n);
	}
	printf("\n");
	for(int a=0;a<64;a++){
		printf("%02X",a*2);
		for(int n=0;n<32;n++){

			if(RAMMask[n+a*32] == -1){
				printf("|S ");
				stat++;
			}
			else if(RAMMask[n+a*32] == -2){
				printf("|E ");
				erratic++;
			}
			else if(RAMMask[n+a*32] == -3){
				printf("|L ");
				lowchange++;
			}
			else if(RAMMask[n+a*32] == -4){
				printf("|I ");
				independent++;
			}
			else{
				printf("|0 ");
			}
 		}
		printf("\n");
 	}
	printf("\nTotal number of static ram locations: %d.\nTotal number of dynamic ram locations: %d.\nTotal number of low changing ram locations: %d.\nTotal number of independent ram locations: %d.\nPercentage of ignored ram: %lf.", stat, erratic, lowchange, independent, (double)(stat + erratic + lowchange + independent)*100.0/2048.0);
    printf("\n%d",2048-stat-erratic-lowchange-independent);
	//teardown
	for(unsigned int i=0; i < gameCount; i++){
		delete games[i];
	}
 	delete [] games;
	for(unsigned int j=0;j<gameCount;j++){
		delete [] objectives[j];
	}
	delete [] objectives;
	delete [] RAMMask;
	return 0;
}
