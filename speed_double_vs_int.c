#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
	clock_t start;
	{
	start = clock();
	double a = 0;
	double b = 1.5;
	for (int i=0; i<1000000000; i++) {
		a += b;
	}
	printf("double time:%f\n", (double)(clock()-start)/CLOCKS_PER_SEC);
	}

	{
	start = clock();
	float a = 0;
	float b = 1.5;
	for (int i=0; i<1000000000; i++) {
		a += b;
	}
	printf("float time:%f\n", (double)(clock()-start)/CLOCKS_PER_SEC);
	}
	
	{
	start = clock();
	int a = 0;
	int b = 15;
	for (int i=0; i<1000000000; i++) {
		a += b;
	}
	printf("int time:%f\n", (double)(clock()-start)/CLOCKS_PER_SEC);
	}
}
