#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void shuffle(int* a, int n) {
	for (int i=n-1;i>=0;i--) {
	  int r=random()%(i+1);
	  int temp=a[i];
	  a[i]=a[r];
	  a[r]=temp;
	}
}

int inner_loop(int* a, int* b, int n) {
	int sum=0;
	for (int i=0;i<n;i++) {
		sum += a[b[i]];
	}
	return sum;
}

void cache_test(int n,int repeats) {
	int *a=malloc(n*sizeof(int));
	for (int i=0;i<n;i++) {
		a[i]=random();
	}

	int *b=malloc(n*sizeof(int));
	for (int i=0;i<n;i++) {
		b[i]=i;
	}

	shuffle(b,n);

	int sum=0;
	clock_t start = clock();
	for (int r=0;r<repeats;r++) {
		sum += inner_loop(a,b,n);
	}
	clock_t stop = clock();
	float seconds = ((float)stop-start)/CLOCKS_PER_SEC;
	printf("MiB/seconds:%f n:%i seconds:%f (repeats:%i sum:%i)\n",(float)n*repeats*sizeof(int)/seconds/1024/1024,n,seconds,repeats,sum);

	free(b);
	free(a);
}

int main(int argc,char** argv) {
	if (argc < 2){
		printf("%s REPEATS\n",argv[0]);
		return 1;
	}

	int repeats=atoi(argv[1]);

	for (int n=1;n<=1024*1024*16;n*=2) {
		cache_test(n,(int)((float)repeats/n));
	}

	return 0;
}
