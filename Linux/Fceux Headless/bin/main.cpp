#include "omp.h"
#include <stdlib.h>
int main(){
	#pragma omp parallel for
	for(int i=0; i<8;i++){
		system("./fceux -l 4000 Pinball.nes");
	}
	return 0;
}
