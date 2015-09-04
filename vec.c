#include <stdio.h>
#include <stdlib.h>

int main(void) {
	char a[8];
	char b[8];

	for (int i=0;i<8;i++) {	
		a[i] = random() % 256;
		b[i] = random() % 256;
	}

	for (int i=0;i<8;i++) {
		printf("a[%i]=%i b[%i]=%i\n",i,a[i],i,b[i]);
	}

	printf("adding\n");
	for (int i=0;i<8;i++) {
		a[i] += b[i];
	}

	for (int i=0;i<8;i++) {
		printf("a[%i]=%i b[%i]=%i\n",i,a[i],i,b[i]);
	}

	return 0;
}
