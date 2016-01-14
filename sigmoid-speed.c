#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//This function can be sped up (and made inaccurate) by compiling with "-ffast-math".
float sigmoid(float x) {
	return 1.0/(1+exp(-x));
}

int main(void) {
	clock_t t0 = clock();	

	float x;
	float y=0;
	for (x=-100.0;x<100.0;x+=0.1) {
		y += sigmoid(x);
	}

	float time=((float)clock()-t0)/CLOCKS_PER_SEC;
	printf("elapsed time: %f\n", time);
	printf("y=%f\n",y);

	return 0;
}

