#include "AI.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>

//Dumbed down FM2 parser extracted from FCEUX


#define MOVIE_VERSION           3

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
	(uint8)fgetc(is);

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

	//the amount todo is the min of the limiting size we received and the remaining contents of the file
	int todo = flen;

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






//static parameters definition


//variables declaration

std::vector<uint8> *keyPresses;
uint8* RAMs;
int frameCount = 0;

void HELP(char* c){
	FILE* file = fopen("HELP", "a");
	if(file != NULL){
		fprintf(file, c);
		fclose(file);
	}
}

//RAM analysis
#define BLAH 2048
int arrCount = 0;
inline void saveFrame(uint8* RAM){
	memcpy(&RAMs[BLAH*frameCount],RAM,sizeof(uint8)*BLAH);
	arrCount++;
}

bool firstKeyPress = false;
uint8 runAI(uint8* RAM, bool& exitNow){
	if(frameCount>=keyPresses->size()){
		exitNow = true;
		return 0;
	}
	if(firstKeyPress){
		saveFrame(RAM);
	}
	if(!firstKeyPress && ((*keyPresses)[frameCount]>0)){
		firstKeyPress = true;	
	}

	return (*keyPresses)[frameCount++];
}



void printStatistics(){

	for(int i=0; i<arrCount;i++){
		for(int j=0;j<BLAH;j++){
			printf("%d ", RAMs[j+BLAH*i]);
		}
	}
	
	delete [] RAMs;	
	delete keyPresses;
}


int initializeParams(std::string s){
	FILE* file = fopen(s.c_str(), "r");
		if(file != NULL){
			keyPresses = new std::vector<uint8>();
			LoadFM2(keyPresses, file);
			fclose(file);
			RAMs = new uint8[BLAH*keyPresses->size()];
			return 1;
		}
		else{
			//error, cerrar fceux!
			return 0;
		}
}


