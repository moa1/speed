#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include <stdint.h> // for [u]intXX_t types only
#define __STDC_FORMAT_MACROS // for PRI[iu]XX strings
#include <inttypes.h> // for types + printf support

char* bin(uint64_t x, int bits) {
	char* ret = malloc(sizeof(char)*(bits+2+1));
	ret[0]='0';
	ret[1]='b';
	for (int i=bits-1; i>=0; i--) {
		uint64_t a=(uint64_t)1<<i;
		if ((a&x)!=0) {
			ret[bits-1-i+2] = '1';
		} else {
			ret[bits-1-i+2] = '0';
		}
	}
	ret[bits+2] = '\0';
	return ret;
}

#define NUMBERHELP "NUMBER may be given either as a decimal, or as a number of any base, specified\n"\
"by NUMBER,BASE, where NUMBER is a number in base BASE, and BASE is a decimal\n"\
"A negative BASE specifies that NUMBER should be taken in reverse.\n"\
"For example, a0,-16 is 10 decimal, and a0,16 is 160 decimal.\n"

char* strrevcond(const char* s, int revert) {
	int l = strchrnul(s, ',') - s;
	char* m = malloc(sizeof(char) * (l + 1));
	if (revert) {
		for (int i = 0; i < l; i++) {
			m[l-1-i] = s[i];
		}
	} else {
		strcpy(m, s);
	}
	m[l] = '\0';
	printf("m=%s\n", m);
	return m;
}

long int strtolformat(const char* s) {
	int base = 10;
	// Find a comma: If present, the number afterwards specifies the base.
	const char* comma = strchr(s,',');
    if (comma != NULL) {
		base = strtol(comma+1, NULL, 10);
	}
	char* m = strrevcond(s, base < 0);
	if (base < 0) base = -base;
	long int result = strtol(m, NULL, base);
	free(m);
	return result;
}

unsigned long int strtoulformat(const char* s) {
	int base = 10;
	// Find a comma: If present, the number afterwards specifies the base.
	const char* comma = strchr(s,',');
    if (comma != NULL) {
		base = strtol(comma+1, NULL, 10);
	}
	char* m = strrevcond(s, base < 0);
	if (base < 0) base = -base;
	unsigned long int result = strtoul(m, NULL, base);
	free(m);
	return result;
}

int mult(int argc, char** argv) {
	if (argc < 3) {
		fprintf(stderr, "mult NUMBER1 NUMBER2\nPrint the product NUMBER1*NUMBER2 in 8, 16, and 32 bits.\n"NUMBERHELP);
		return 1;
	}
	printf("mult\n");
	{
		int8_t as = strtolformat(argv[1]);
		int8_t bs = strtolformat(argv[2]);
		int16_t cs = (int16_t)as * bs;
		printf("as:% 4"PRIi8"(%s) bs:% 4"PRIi8"(%s) cs:% 6"PRIi16"(%s)\n",as,bin(as,8),bs,bin(bs,8),cs,bin(cs,16));
		uint8_t au = strtoulformat(argv[1]);
		uint8_t bu = strtoulformat(argv[2]);
		uint16_t cu = (uint16_t)au * bu;
		printf("au:% 4"PRIu8"(%s) bu:% 4"PRIu8"(%s) cu:% 6"PRIu16"(%s)\n",au,bin(au,8),bu,bin(bu,8),cu,bin(cu,16));
	}
	{ 
		int16_t as = strtolformat(argv[1]);
		int16_t bs = strtolformat(argv[2]);
		int32_t cs = (int32_t)as * bs;
		printf("as:% 6"PRIi16"(%s) bs:% 6"PRIi16"(%s) cs:% 11"PRIi32"(%s)\n",as,bin(as,16),bs,bin(bs,16),cs,bin(cs,32));
		uint16_t au = strtoulformat(argv[1]);
		uint16_t bu = strtoulformat(argv[2]);
		uint32_t cu = (uint32_t)au * bu;
		printf("au:% 6"PRIu16"(%s) bu:% 6"PRIu16"(%s) cu:% 11"PRIu32"(%s)\n",au,bin(au,16),bu,bin(bu,16),cu,bin(cu,32));
	}
	{
		int32_t as = strtolformat(argv[1]);
		int32_t bs = strtolformat(argv[2]);
		int64_t cs = (int64_t)as * bs; // TODO: why is cs==294745296, when as==-2000111000 and bs=2?
		printf("as:% 11"PRIi32"(%s) bs:% 11"PRIi32"(%s) cs:% 20"PRIi64"(%s)\n",as,bin(as,32),bs,bin(bs,32),cs,bin(cs,64));
		uint32_t au = strtoulformat(argv[1]);
		uint32_t bu = strtoulformat(argv[2]);
		uint64_t cu = (uint64_t)au * bu; // TODO: why is cs==294745296, when as==-2000111000 and bs=2?
		printf("au:% 11"PRIu32"(%s) bu:% 11"PRIu32"(%s) cu:% 20"PRIu64"(%s)\n",au,bin(au,32),bu,bin(bu,32),cu,bin(cu,64));
	}

	return 0;
}

int sub(int argc, char** argv) {
	if (argc < 3) {
		fprintf(stderr, "sub NUMBER1 NUMBER2\nPrint the difference NUMBER1-NUMBER2 in 8, 16, and 32 bits.\n"NUMBERHELP);
		return 1;
	}
	printf("sub\n");
	{
		int8_t as = strtolformat(argv[1]);
		int8_t bs = strtolformat(argv[2]);
		int16_t cs = (int16_t)as - bs;
		printf("as:% 4"PRIi8"(%s) bs:% 4"PRIi8"(%s) cs:% 6"PRIi16"(%s)\n",as,bin(as,8),bs,bin(bs,8),cs,bin(cs,16));
		uint8_t au = strtoulformat(argv[1]);
		uint8_t bu = strtoulformat(argv[2]);
		uint16_t cu = (uint16_t)au - bu;
		printf("au:% 4"PRIu8"(%s) bu:% 4"PRIu8"(%s) cu:% 6"PRIu16"(%s)\n",au,bin(au,8),bu,bin(bu,8),cu,bin(cu,16));
	}
	{ 
		int16_t as = strtolformat(argv[1]);
		int16_t bs = strtolformat(argv[2]);
		int32_t cs = (int32_t)as - bs;
		printf("as:% 6"PRIi16"(%s) bs:% 6"PRIi16"(%s) cs:% 11"PRIi32"(%s)\n",as,bin(as,16),bs,bin(bs,16),cs,bin(cs,32));
		uint16_t au = strtoulformat(argv[1]);
		uint16_t bu = strtoulformat(argv[2]);
		uint32_t cu = (uint32_t)au - bu;
		printf("au:% 6"PRIu16"(%s) bu:% 6"PRIu16"(%s) cu:% 11"PRIu32"(%s)\n",au,bin(au,16),bu,bin(bu,16),cu,bin(cu,32));
	}
	{
		int32_t as = strtolformat(argv[1]);
		int32_t bs = strtolformat(argv[2]);
		int64_t cs = (int64_t)as - bs;
		printf("as:% 11"PRIi32"(%s) bs:% 11"PRIi32"(%s) cs:% 20"PRIi64"(%s)\n",as,bin(as,32),bs,bin(bs,32),cs,bin(cs,64));
		uint32_t au = strtoulformat(argv[1]);
		uint32_t bu = strtoulformat(argv[2]);
		uint64_t cu = (uint64_t)au - bu;
		printf("au:% 11"PRIu32"(%s) bu:% 11"PRIu32"(%s) cu:% 20"PRIu64"(%s)\n",au,bin(au,32),bu,bin(bu,32),cu,bin(cu,64));
	}

	return 0;
}

int shift(int argc, char** argv) {
	if (argc < 3) {
		fprintf(stderr, "shift NUMBER1 NUMBER2\nPrint NUMBER1<<NUMBER2 where NUMBER1 is in 8, 16, and 32 bits, and NUMBER2 is a signed byte.\n"NUMBERHELP);
		return 1;
	}
	printf("shift\n");
	{
		int8_t as = strtolformat(argv[1]);
		int8_t bs = strtolformat(argv[2]);
		int8_t cs = (bs >= 0)? as << bs : as >> -bs;
		printf("as:% 4"PRIi8"(%s) bs:% 4"PRIi8"(%s) cs:% 4"PRIi8"(%s)\n",as,bin(as,8),bs,bin(bs,8),cs,bin(cs,8));
		uint8_t au = strtoulformat(argv[1]);
		int8_t bu = strtolformat(argv[2]);
		uint8_t cu = (bu >= 0)? au << bu : au >> -bu;
		printf("au:% 4"PRIu8"(%s) bu:% 4"PRIi8"(%s) cu:% 4"PRIu8"(%s)\n",au,bin(au,8),bu,bin(bu,8),cu,bin(cu,8));
	}
	{ 
		int16_t as = strtolformat(argv[1]);
		int16_t bs = strtolformat(argv[2]);
		int16_t cs = (bs >= 0)? as << bs : as >> -bs;
		printf("as:% 6"PRIi16"(%s) bs:% 6"PRIi16"(%s) cs:% 6"PRIi16"(%s)\n",as,bin(as,16),bs,bin(bs,16),cs,bin(cs,16));
		uint16_t au = strtoulformat(argv[1]);
		int16_t bu = strtolformat(argv[2]);
		uint16_t cu = (bu >= 0)? au << bu : au >> -bu;
		printf("au:% 6"PRIu16"(%s) bu:% 6"PRIi16"(%s) cu:% 6"PRIu16"(%s)\n",au,bin(au,16),bu,bin(bu,16),cu,bin(cu,16));
	}
	{
		int32_t as = strtolformat(argv[1]);
		int32_t bs = strtolformat(argv[2]);
		int32_t cs = (bs >= 0)? as << bs : as >> -bs;
		printf("as:% 11"PRIi32"(%s) bs:% 11"PRIi32"(%s) cs:% 11"PRIi32"(%s)\n",as,bin(as,32),bs,bin(bs,32),cs,bin(cs,32));
		uint32_t au = strtoulformat(argv[1]);
		int32_t bu = strtolformat(argv[2]);
		uint32_t cu = (bu >= 0)? au << bu : au >> -bu;
		printf("au:% 11"PRIu32"(%s) bu:% 11"PRIi32"(%s) cu:% 11"PRIu32"(%s)\n",au,bin(au,32),bu,bin(bu,32),cu,bin(cu,32));
	}

	return 0;
}

int and(int argc, char** argv) {
	if (argc < 3) {
		fprintf(stderr, "and NUMBER1 NUMBER2\nPrint the logical and NUMBER1&NUMBER2 where NUMBER1 and NUMBER2 are in 8, 16, and 32 bits.\n"NUMBERHELP);
		return 1;
	}
	printf("and\n");
	{
		int8_t as = strtolformat(argv[1]);
		int8_t bs = strtolformat(argv[2]);
		int8_t cs = as&bs;
		printf("as:% 4"PRIi8"(%s) bs:% 4"PRIi8"(%s) cs:% 4"PRIi8"(%s)\n",as,bin(as,8),bs,bin(bs,8),cs,bin(cs,8));
		uint8_t au = strtoulformat(argv[1]);
		uint8_t bu = strtoulformat(argv[2]);
		uint8_t cu = au&bu;
		printf("au:% 4"PRIu8"(%s) bu:% 4"PRIu8"(%s) cu:% 4"PRIu8"(%s)\n",au,bin(au,8),bu,bin(bu,8),cu,bin(cu,8));
	}
	{ 
		int16_t as = strtolformat(argv[1]);
		int16_t bs = strtolformat(argv[2]);
		int16_t cs = as&bs;
		printf("as:% 6"PRIi16"(%s) bs:% 6"PRIi16"(%s) cs:% 6"PRIi16"(%s)\n",as,bin(as,16),bs,bin(bs,16),cs,bin(cs,16));
		uint16_t au = strtoulformat(argv[1]);
		uint16_t bu = strtoulformat(argv[2]);
		uint16_t cu = as&bs;
		printf("au:% 6"PRIu16"(%s) bu:% 6"PRIu16"(%s) cu:% 6"PRIu16"(%s)\n",au,bin(au,16),bu,bin(bu,16),cu,bin(cu,16));
	}
	{
		int32_t as = strtolformat(argv[1]);
		int32_t bs = strtolformat(argv[2]);
		int32_t cs = as&bs;
		printf("as:% 11"PRIi32"(%s) bs:% 11"PRIi32"(%s) cs:% 11"PRIi32"(%s)\n",as,bin(as,32),bs,bin(bs,32),cs,bin(cs,32));
		uint32_t au = strtoulformat(argv[1]);
		uint32_t bu = strtoulformat(argv[2]);
		uint32_t cu = as&bs;
		printf("au:% 11"PRIu32"(%s) bu:% 11"PRIu32"(%s) cu:% 11"PRIu32"(%s)\n",au,bin(au,32),bu,bin(bu,32),cu,bin(cu,32));
	}

	return 0;
}

int or(int argc, char** argv) {
	if (argc < 3) {
		fprintf(stderr, "or NUMBER1 NUMBER2\nPrint the logical or NUMBER1&NUMBER2 where NUMBER1 and NUMBER2 are in 8, 16, and 32 bits.\n"NUMBERHELP);
		return 1;
	}
	printf("or\n");
	{
		int8_t as = strtolformat(argv[1]);
		int8_t bs = strtolformat(argv[2]);
		int8_t cs = as|bs;
		printf("as:% 4"PRIi8"(%s) bs:% 4"PRIi8"(%s) cs:% 4"PRIi8"(%s)\n",as,bin(as,8),bs,bin(bs,8),cs,bin(cs,8));
		uint8_t au = strtoulformat(argv[1]);
		uint8_t bu = strtoulformat(argv[2]);
		uint8_t cu = au|bu;
		printf("au:% 4"PRIu8"(%s) bu:% 4"PRIu8"(%s) cu:% 4"PRIu8"(%s)\n",au,bin(au,8),bu,bin(bu,8),cu,bin(cu,8));
	}
	{ 
		int16_t as = strtolformat(argv[1]);
		int16_t bs = strtolformat(argv[2]);
		int16_t cs = as|bs;
		printf("as:% 6"PRIi16"(%s) bs:% 6"PRIi16"(%s) cs:% 6"PRIi16"(%s)\n",as,bin(as,16),bs,bin(bs,16),cs,bin(cs,16));
		uint16_t au = strtoulformat(argv[1]);
		uint16_t bu = strtoulformat(argv[2]);
		uint16_t cu = as|bs;
		printf("au:% 6"PRIu16"(%s) bu:% 6"PRIu16"(%s) cu:% 6"PRIu16"(%s)\n",au,bin(au,16),bu,bin(bu,16),cu,bin(cu,16));
	}
	{
		int32_t as = strtolformat(argv[1]);
		int32_t bs = strtolformat(argv[2]);
		int32_t cs = as|bs;
		printf("as:% 11"PRIi32"(%s) bs:% 11"PRIi32"(%s) cs:% 11"PRIi32"(%s)\n",as,bin(as,32),bs,bin(bs,32),cs,bin(cs,32));
		uint32_t au = strtoulformat(argv[1]);
		uint32_t bu = strtoulformat(argv[2]);
		uint32_t cu = as|bs;
		printf("au:% 11"PRIu32"(%s) bu:% 11"PRIu32"(%s) cu:% 11"PRIu32"(%s)\n",au,bin(au,32),bu,bin(bu,32),cu,bin(cu,32));
	}

	return 0;
}

int floating(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "float NUMBER\nPrint the floating-point NUMBER in 32 and 64 bits.\n");
		return 1;
	}
	printf("float\n");
	{
		float af;
		sscanf(argv[1],"%f",&af);
		uint32_t au = *((uint32_t*)&af);
		printf("af:% 14.8e(%s)\n",af,bin(au,32));
	}
	{
		double ad;
		sscanf(argv[1],"%lf",&ad);
		uint64_t au = *((uint64_t*)&ad);
		printf("ad:% 14.8e(%s)\n",ad,bin(au,64));
	}

	return 0;
}

int match(char* argv0, char* name) {
	char* l = strrchr(argv0,'/')+1;
	if (l == NULL)
		l = argv0;
	return strcmp(l, name) == 0;
}

int main(int argc, char** argv) {
	if (match(argv[0], "mult")) {
		return mult(argc,argv);
	} else if (match(argv[0], "sub")) {
		return sub(argc,argv);
	} else if (match(argv[0], "shift")) {
		return shift(argc,argv);
	} else if (match(argv[0], "and")) {
		return and(argc,argv);
	} else if (match(argv[0], "or")) {
		return or(argc,argv);
	} else if (match(argv[0], "float")) {
		return floating(argc,argv);
	} else {
		fprintf(stderr, "unknown command %s.\n", argv[0]);
		return 127;
	}
}
