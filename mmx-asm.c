// What does COMISS/UCOMISS do?

#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>

int randint() {
	return random() * ((random()%2)==0?1:-1);
}

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

void sse_using_asm(void) {
//	int i0[4]={ 1001002003,2001002003,501002003,251002003};
//	int i1[4]={-1001002003,2091002003,500000000,251002003};
	int i0[4]={randint(),randint(),randint(),randint()};
	int i1[4]={randint(),randint(),randint(),randint()};
	
	for (int i=0; i<4; i++) {
		printf("i0[%i]:%i\ti1[%i]:%i\n", i, i0[i], i, i1[i]);
	}

	printf("----\n");
	__asm__(
		"movq %0,%%xmm0\n"
		"movq %1,%%xmm1\n"
		"pmaxsw %%xmm0, %%xmm1\n"
		"movq %%xmm0,%0\n"
		"movq %%xmm1,%1\n"
		: "=m"(i0), "=m"(i1)
		: "m"(i0), "m"(i1)
		: "0", "1"
	);

	for (int i=0; i<4; i++) {
		printf("i0[%i]:%i\ti1[%i]:%i\n", i, i0[i], i, i1[i]);
	}
}

typedef int v4si __attribute__((vector_size(16)));

typedef short int v4ssi __attribute__ ((vector_size (8)));

void mmx_using_intrinsics1(void) {
	v4ssi a = {randint(),randint(),randint(),randint()};
	v4ssi b = {randint(),randint(),randint(),randint()};
	v4ssi c;
	for (int i=0; i<4; i++) {
		printf("a[%i]:%i\tb[%i]:%i\tc[%i]:%i\n", i, a[i], i, b[i], i, c[i]);
	}
	printf("c = a <= b;\n");
	c = a <= b;
	for (int i=0; i<4; i++) {
		printf("a[%i]:%i\tb[%i]:%i\tc[%i]:%i\n", i, a[i], i, b[i], i, c[i]);
	}
	printf("v4si d = {c[0],c[1],c[2],c[3]};\n");
	v4si d = {c[0],c[1],c[2],c[3]};
	for (int i=0; i<4; i++) {
		printf("c[%i]:%i d[%i]:%i\n", i, c[i], i, d[i]);
	}

}

typedef int v2si __attribute__ ((vector_size (8)));

/*
// TODO: find out what the equivalent of __builtin_shufle is for clang.
void mmx_using_intrinsics2(void) {
	v2si a = {random(),random()};
	v2si b = {random(),random()};
	v2si ign = {(random()%2)==0?0:-1,(random()%2)==0?0:-1};
	v2si mask = {random()%8, random()%8};
	v2si c;
	for (int i=0; i<2; i++) {
		printf("a[%i]:%i\tb[%i]:%i\tign[%i]:%i\tc[%i]:%i\n", i, a[i], i, b[i], i, ign[i], i, c[i]);
	}
	printf("c = (a & ign) | (b & ~ign);\n");
	c = (a & ign) | (b & ~ign);
	for (int i=0; i<2; i++) {
		printf("a[%i]:%i\tb[%i]:%i\tign[%i]:%i\tc[%i]:%i\n", i, a[i], i, b[i], i, ign[i], i, c[i]);
	}
	printf("c = __builtin_shuffle(a, b, mask);\n");
	c = __builtin_shuffle(a, b, mask);
	for (int i=0; i<2; i++) {
		printf("a[%i]:%i\tb[%i]:%i\tmask[%i]:%i\tc[%i]:%i\n", i, a[i], i, b[i], i, mask[i], i, c[i]);
	}
}
*/

void mmx_using_intrinsics3(void) {
	v2si a = {randint(),randint()};
	v2si b = {16,16};
	for (int i=0; i<2; i++) {
		printf("a[%i]:%i\n", i, a[i]);
	}
	printf("a = a >> 16;\n");
	a = a >> b;
	for (int i=0; i<2; i++) {
		printf("a[%i]:%i\n", i, a[i]);
	}
}

int main(void) {

	//mmx_using_asm();
	//sse_using_asm();
	mmx_using_intrinsics1();
	//mmx_using_intrinsics2();
	//mmx_using_intrinsics3();
	
	return 0;
}
