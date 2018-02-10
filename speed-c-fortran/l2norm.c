// https://gcc.gnu.org/onlinedocs/gfortran/CPU_005fTIME.html

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv) {
	int size = 1000000;
	float *numbers;
	float sum;
	int repeats = 100;
	clock_t start,finish;

	printf("C version\n");
	printf("argc = %i\n", argc);
	numbers = malloc(sizeof(float) * size);
	
	for (int i=0; i<size; i++) {
		numbers[i] = ((float)i+1)/size + argc - 1;
	}

	printf("numbers[20000] = %f\n", numbers[20000]);

	start = clock();
	sum = 0.0;
	for (int j=0; j<repeats; j++) {
		for (int i=0; i<size; i++) {
			numbers[i] = numbers[i] * numbers[i];
			sum = sum + numbers[i];
		}
	}
	finish = clock();
	free(numbers);

	printf("Sum = %f.\n", sum);
	printf("Time = %f seconds.\n", (float)(finish-start)/CLOCKS_PER_SEC);
	return 0;
}
