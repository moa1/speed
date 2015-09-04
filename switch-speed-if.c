#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	int a = atoi(argv[1]);
	int b = 0;
	int i;

	for (i=0; i<a; i++) {
		if (a == 0) {
			b+= 1;
		} else if (a == 1) {
			b += 2;
		} else if (a == 2) {
			b += 3;
		} else if (a == 3) {
			b += 4;
		} else if (a == 4) {
			b += 5;
		} else {
			b = 0;
		}
	}

	return 0;
}
