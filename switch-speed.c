#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	int a = atoi(argv[1]);
	int b = 0;
	int i;

	for (i=0; i<a; i++) {
		switch(a) {
			case 0: b += 1; break;
			case 1: b += 2; break;
			case 2: b += 3; break;
			case 3: b += 4; break;
			case 4: b += 5; break;
			default: b = 0; break;
		}
	}

	return 0;
}
