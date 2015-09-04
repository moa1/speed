#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	int a = atoi(argv[1]);
	
	switch(a) {
		case 0: printf("0"); break;
		case 1: printf("1"); break;
		case 2: printf("2"); break;
		case 3: printf("3"); break;
		case 4: printf("4"); break;
		default: printf("default: %i", a); break;
	}
	printf("\n");
	return 0;
}
