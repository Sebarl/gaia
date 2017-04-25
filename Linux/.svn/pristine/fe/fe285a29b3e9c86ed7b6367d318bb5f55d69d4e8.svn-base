#include "AI.h"
#include <cmath>
#include <iostream>
#include <fstream>

//static parameters definition

#define CANT_POINTS 20
#define START_RADIUS 2

//variables declaration

bool exitEarly = false;

//definicion de teclas;
const uint8 right = 128;
const uint8 left = 64;
const uint8 down = 32;
const uint8 up = 16;
const uint8 start = 8;
const uint8 Select = 4;
const uint8 keyB = 2;
const uint8 keyA = 1;

//parameters declaration

int keyStartDelay = 0; //Instancia a cargar
//int keyPressCoolValue = 5; //Sticky keys (cantidad de frames que componen un keypress)
int coolOffTime = 50; //numero de frames para esperar si nos trancamos
int startTime = 100; //cantida de frames que mantenemos el piston abajo para tirar la pelota
int stuckTolerance = 180;//cantidad de frames que esperamos para decidir que nos trancamos

//variables declaration

Point* points[CANT_POINTS];
bool inCooloff = false; //estamos en cooloff? (Para destrancarse)
int startCount; // Tiempo a mantener bajo el piston para dispararlo
int coolOff = 0; //Cantidad de frames para estar en cooloff
uint8 keyPress = 0, oldKeyPress = 0; //Keypresses, tanto la que apretamos antes como la que vamos a mandar
int keyPressCoolOff = 0; //Timer de sticky keys
int repeatedFrames = 0; //Cantidad de frames que llevamos trancados
uint8 oldX = 0; //Xviejo, para ver si nos trancamos
uint8 oldY = 0; //Yviejo, para ver si nos trancamos
uint8* RAM2; // copia del puntero de ram anterior, asi cuando terminamos de ejecutar podemos imprimir el puntaje y las bolas
/*
int f(uint8* RAM, double A, double B, double C, double D, double E, double F, double G, double H, double I, double J, double K, double L, double M, double N, double O, double P){
	return A*RAM[0x0151] //vidas pinball
		+ B*RAM[0x0015] //valor menos significativo del eje x de la bola
		+ C*RAM[0x0016] //valor mas significativo del eje x de la bola
		+ D*RAM[0x0017] //valor menos significativo del eje y de la bola
		+ E*RAM[0x0018] //valor mas significativo del eje y de la 
		+ F*RAM[0x0019] //velocidad en el eje x de la bola
		+ G*RAM[0x001A] //velocidad en el eje y de la bola
		+ H*RAM[0x0100] //digito mas significativo puntaje
		+ I*RAM[0x0101] //2do digito mas significativo puntaje
		+ J*RAM[0x0102] //3er digito mas significativo puntaje
		+ K*RAM[0x0103] //4to digito mas significativo puntaje
		+ L*RAM[0x0104] //5to digito mas significativo puntaje
		+ M*RAM[0x0105] //6to digito mas significativo puntaje
		+ N*RAM[0x00BF] //Pantalla actual: 0 - arriba, 1 - abajo, 2 - bonus
		+ O*RAM[0x0003] //Boolean menu: 1 - true, 0 - false
		+ P*RAM[0x02A3] //Mario's face in bonus stage
		+ 0 * RAM[0x02B3] //Princess x axis
		+ 0 * RAM[0x011C] //Princess y logical axis
		;
}
*/

void HELP(char* c){
FILE* file = fopen("HELP", "a");
if(file != NULL){
	fprintf(file, c);
	fclose(file);
}
}

inline bool insidePoint(uint8 xcenter, uint8 ycenter, uint8 x, uint8 y, uint8 radius){
	int diffX = x-xcenter;
	int diffY = y-ycenter;	
	return (diffX*diffX+diffY*diffY<=radius*radius);
}
/*
inline bool inVelocityRange(uint8 velBallX, uint8 velBallY ,bool goUp, bool goRight){
	return (velBallX>>7 != goRight)&&(velBallY>>7 == goUp);
}*/

uint8 runAI(uint8* RAM, bool& exitNow){
	RAM2=RAM;
	//stuck check
	oldKeyPress = keyPress;
	keyPress = 0;
	if (RAM[0x00BF] < 2){ //estamos jugando al pinball?
		//Chequeo para ver si estamos trancados		
		if (!(std::abs(RAM[0x0016] - 0xE1) < 10 && std::abs(RAM[0x0018] - 0x9A) < 10) && !inCooloff && std::abs(RAM[0x0016] - oldX) <5 && std::abs(RAM[0x0018] - oldY) <5){
			repeatedFrames++;
		}
		else{ //nos destrancamos
			repeatedFrames = 0;
		}
		if (repeatedFrames >= stuckTolerance){ //Detectar que nos trancamos
			repeatedFrames = 0;
			coolOff = coolOffTime;
			inCooloff = true;
		}
		else if (keyPressCoolOff > 0){ //Sticky keys, mantener una tecla apretada mas de un frame, emula ser un humano
			keyPressCoolOff--;
			keyPress = oldKeyPress;
		}
		else if (inCooloff){ //Tener cooloff, timer para esperar para destrancarnos
			coolOff--;
			if (coolOff <= 0){
				inCooloff = false;
			}
		}
		else{
			int i =0;
			while(i < CANT_POINTS){ //chequeo de la bola			
				if(points[i]->stage == RAM[0x00BF] && //Chequea que estemos en el stage correcto
				insidePoint(points[i]->x,points[i]->y,RAM[0x0016],RAM[0x0018],points[i]->radius) //&& //Chequea que la bola este en un punto
				//inVelocityRange(RAM[0x0019], RAM[0x001A],points[i]->goUp, points[i]->goRight) //Chequea que estemos en el sentido correcto
				){
					keyPress |= points[i]->action;
					break;
				}
				i++;							
			}
			if (i == CANT_POINTS ||(std::abs(RAM[0x0016] - 0xE1) < START_RADIUS && std::abs(RAM[0x0018] - 0x9A) < START_RADIUS)){
				//inicio
				if (std::abs(RAM[0x0016] - 0xE1) < START_RADIUS && std::abs(RAM[0x0018] - 0x9A) < START_RADIUS && startCount > 0){
					keyPress = keyPress | keyB;
					//keyPressCoolOff = keyPressCoolValue;
					startCount--;
				}
				else if (std::abs(RAM[0x0016] - 0xE1) < START_RADIUS && std::abs(RAM[0x0018] - 0x9A) < START_RADIUS && startCount == 0){
					startCount = startTime;
				}
				//si estamos en el menu
				else if (RAM[0x0003] == 1){
					if(!exitEarly){
						keyStartDelay--;
						if (keyStartDelay<1){
							keyPress = keyPress | start;
							exitEarly = true;	
						}		
					}
					else{
						exitNow = true;
					}	
				}
			}

		}
	}
	//AI que juega al bonus stage (hardcodeada, no evoluciona)
	else{ //no, jugar al bonus stage!
		if (RAM[0x011C] == 0 || (0x02B3<0x70 || 0xBF>0x02B3)){//princess is still up, follow ball
			if (RAM[0x02A3] < RAM[0x0016]){
				keyPress = keyPress | right;
			}
			else if (RAM[0x02A3] > RAM[0x0016]){
				keyPress = keyPress | left;
			}
		}
		else{ //princess falling, follow princess
			if (RAM[0x02A3] < RAM[0x02B3]){
				keyPress = keyPress | right;
			}
			else if (RAM[0x02A3] > RAM[0x02B3]){
				keyPress = keyPress | left;
			}
		}
	}
	oldX = RAM[0x0016];
	oldY = RAM[0x0018];
	return keyPress;
}

void printStatistics(){
	std::cout << (int)RAM2[0x010E] << (int)RAM2[0x010F] << (int)RAM2[0x0110] << (int)RAM2[0x0111] << (int)RAM2[0x0112] << (int)RAM2[0x0113] << " " << (int)RAM2[0x0151]<<"\n";
}

//funcion de des-serializacion de puntos

Point* deserializePoint(FILE* file){
 	Point* p  = (Point*)_mm_malloc(sizeof(Point),64);
	fscanf (file, "%d", &p->x);
	fscanf (file, "%d", &p->y);
	fscanf (file, "%d", &p->radius);
	fscanf (file, "%d", &p->action);
	fscanf (file, "%d", &p->stage);
	return p;
}


int initializeParams(std::string s){
FILE* file = fopen(s.c_str(), "r");
if(file != NULL){
	for(int i=0;i<CANT_POINTS;i++)
		points[i] = deserializePoint(file);
	fscanf (file, "%d", &keyStartDelay);
	//fscanf (file, "%d", &keyPressCoolValue);
	fscanf (file, "%d", &coolOffTime);
	fscanf (file, "%d", &startTime);
	fscanf (file, "%d", &stuckTolerance);	
	fclose(file);
	startCount=startTime;
	return 1;
}
else{
	//error, cerrar fceux!
	return 0;
}
}

