#include <stdio.h>
#include <stdlib.h>
#include <features.h>

// the purpose of this file is to test the speed difference between a predicted and a missed if-branch, but it doesn't work yet.

// this function is not a good test case since I think it measures mainly the idiv-instruction, not the if.
int f1(int divisor) {
	int a = 0;
	for (int i = 0; i < 10000000; i++) {
		//if (__glibc_unlikely((i%divisor)==0)) {
		if ((i%divisor)==0) {
			a += 5;
		} else {
			a += 7;
		}
	}
	return a;
}

// this function is not a good idea since I don't even know how many times I should expect the if-branch to be taken an how often the else-branch.
int f2(int divisor) {
	int a = 0;
	int b = 0;
	for (int i = 0; i < 10000000; i++) {
		b += i;
		if ((b&divisor)==divisor) {
			a += 5;
		} else {
			a += 7;
		}
	}
	return a;
}

int f3_list(int* list, int n) {
	int a = 0;
	for (int i=0; i<n; i++) {
		if (list[i]==0) {
			a += 5;
		} else {
			a += 7;
		}
	}
	return a;
}

// f3 doesn't find a difference between which==0 and which==1, probably because it doesn't measure branch prediction failure, but memory fetch speed.
int f3(int which) {
	int n=10000000;
	int* list = malloc(sizeof(int)*n);
	if (list==NULL) {printf("no memory\n");exit(1);}
	if (which==0) {
		for (int i=0; i<n; i++) {
			list[i] = 0;
		}
	} else if (which==1) {
		for (int i=0; i<n; i++) {
			list[i] = i;
		}
	} else {
		printf("unknown which\n");
		exit(1);
	}
	return f3_list(list, n);
}

int f4(int x) {
	// try with x==n**2, where n is an int >= 0. e.g. x==1 and x==255. 1/(x+1) iterations of the loop will go into the if-branch, and x/(x+1) into the else-branch.
	int a = 0;
	int n=1000000000;
	for (int i=0; i<n; i++) {
		if ((i&x)==0) {
			a += 5;
		} else {
			a += 7;
		}
	}
	return a;
}

int main(int argc, char** argv) {
//	int div = atoi(argv[1]);
//	return f1(div);
	int which = atoi(argv[1]);
	return f4(which);
}

