// What does COMISS/UCOMISS do?

#include <stdio.h>
#include <stdlib.h>

void mmx_using_asm(void) {
	short int i0[4]={0,2,3,4};
	//short int i1[4]={-1,3,3,0};
	short int i1[4]={-1,2,3,4};
	
	for (int i=0; i<4; i++) {
		printf("i0[%i]:%i\ti1[%i]:%i\n", i, i0[i], i, i1[i]);
	}

	printf("----\n");
	__asm__(
		"movq %0,%%mm0\n"
		"movq %1,%%mm1\n"
		//"paddsw %%mm0, %%mm1\n"
		//"pcmpgtw %%mm0, %%mm1\n"
		"psadbw %%mm0, %%mm1\n"
		"movq %%mm0,%0\n"
		"movq %%mm1,%1\n"
		: "=m"(i0), "=m"(i1)
		: "m"(i0), "m"(i1)
		: "0", "1"
	);

	for (int i=0; i<4; i++) {
		printf("i0[%i]:%i\ti1[%i]:%i\n", i, i0[i], i, i1[i]);
	}
}

typedef short int v8ssi __attribute__ ((vector_size (16)));

void mmx_using_intrinsics1(void) {
	v8ssi a = {0,1,2,3,4,5,6,7};
	v8ssi b = {0,1,2,3,-4,-5,12,14};
	v8ssi c;
	for (int i=0; i<8; i++) {
		printf("a[%i]:%i\tb[%i]:%i\tc[%i]:%i\n", i, a[i], i, b[i], i, c[i]);
	}
	printf("c = a <= b;\n");
	c = a <= b;
	for (int i=0; i<8; i++) {
		printf("a[%i]:%i\tb[%i]:%i\tc[%i]:%i\n", i, a[i], i, b[i], i, c[i]);
	}
}

typedef int v4si __attribute__ ((vector_size (16)));

void mmx_using_intrinsics2(void) {
	v4si a = {random(),random(),random(),random()};
	v4si b = {random(),random(),random(),random()};
	v4si ign = {(random()%2)==0?0:-1,(random()%2)==0?0:-1,(random()%2)==0?0:-1,(random()%2)==0?0:-1};
	v4si mask = {random()%8, random()%8, random()%8, random()%8};
	v4si c;
	for (int i=0; i<4; i++) {
		printf("a[%i]:%i\tb[%i]:%i\tign[%i]:%i\tc[%i]:%i\n", i, a[i], i, b[i], i, ign[i], i, c[i]);
	}
	printf("c = (a & ign) | (b & ~ign);\n");
	c = (a & ign) | (b & ~ign);
	for (int i=0; i<4; i++) {
		printf("a[%i]:%i\tb[%i]:%i\tign[%i]:%i\tc[%i]:%i\n", i, a[i], i, b[i], i, ign[i], i, c[i]);
	}
	printf("c = __builtin_shuffle(a, b, mask);\n");
	c = __builtin_shuffle(a, b, mask);
	for (int i=0; i<4; i++) {
		printf("a[%i]:%i\tb[%i]:%i\tmask[%i]:%i\tc[%i]:%i\n", i, a[i], i, b[i], i, mask[i], i, c[i]);
	}
}

void mmx_using_intrinsics3(void) {
	int randint() {
		return random() * ((random()%2)==0?1:-1);
	}
	v4si a = {randint(),randint(),randint(),randint()};
	for (int i=0; i<4; i++) {
		printf("a[%i]:%i\n", i, a[i]);
	}
	printf("a = a >> 16;\n");
	a = a >> 16;
	for (int i=0; i<4; i++) {
		printf("a[%i]:%i\n", i, a[i]);
	}
}

int main(void) {

	mmx_using_intrinsics3();
	
	return 0;
}
