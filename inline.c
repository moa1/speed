#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define LOOKUP(x,y,hm,h) {int x1 = (int)floorf(x);int y1 = (int)floorf(y); h=(hm)[x1][y1];}


float __attribute__((__noinline__)) sum_inline(float** hm, int hw, int hh, float sum) {
	for (int x=0; x<hw; x++) {
		for (int y=0; y<hh; y++) {
			float h;
			LOOKUP(x,y,hm,h);
			sum += h;
		}
	}
	return sum;
}

float __attribute__((__noinline__)) lookup_function(float x, float y, float** hm) {
	float h;
	LOOKUP(x,y,hm,h);
	return h;
}

float __attribute__((__noinline__)) sum_function(float** hm, int hw, int hh, float sum) {
	for (int x=0; x<hw; x++) {
		for (int y=0; y<hh; y++) {
			float h = lookup_function(x,y,hm);
			sum += h;
		}
	}
	return sum;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Syntax: %s REPEATS\n",argv[0]);
		exit(1);
	}
	int repeats=atoi(argv[1]);
	printf("repeats:%i\n",repeats);

	int hw = 234;
	int hh = 256;
	float** hm = malloc(sizeof(float*) * hw);
	for (int x=0; x<hw; x++) {
		hm[x] = malloc(sizeof(float) * hh);
	}
	srand(repeats);
	for (int x=0; x<hw; x++) {
		for (int y=0; y<hh; y++) {
			hm[x][y] = random()%10;
		}
	}

	float s_inline=0.0;
	clock_t inline_start = clock();
	for (int i=0; i<repeats; i++) {
		s_inline+=sum_inline(hm,hw,hh,s_inline);
	}
	clock_t inline_stop = clock();
	float inline_seconds = ((float)inline_stop-inline_start)/CLOCKS_PER_SEC;
	printf("s:%f inline:%f\n",s_inline,inline_seconds);

	float s_function=0.0;
	clock_t function_start = clock();
	for (int i=0; i<repeats; i++) {
		s_function+=sum_function(hm,hw,hh,s_function);
	}
	clock_t function_stop = clock();
	float function_seconds = ((float)function_stop-function_start)/CLOCKS_PER_SEC;
	printf("s:%f function:%f\n",s_function,function_seconds);
}
