#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>

typedef int32_t v4si __attribute__ ((vector_size (16)));
typedef int16_t v4hi __attribute__ ((vector_size (8)));

typedef struct {
	int32_t x;
	int32_t y;
	int32_t z;
} v3;

#define ISTEPS 1 //7 seems to be the maximal number which is still unrolled with -funroll-loops using gcc.
#define DEBUG 0
#define NEXT_TODO_LOCAL 1 //for some reason gcc is faster with this option; else it doesn't generate sse instructions for the next_todo().
#define NEXT_TODO_MACRO 2 //clang doesn't support inline functions.
#define NEXT_TODO NEXT_TODO_LOCAL
//#define NEXT_TODO NEXT_TODO_MACRO
#define HM_FUNCTION 1 //HM_FUNCTION means call function "hm_function"
#define HM_ARRAY 2 //HM_ARRAY means look up in "hm_function_arg"
#define HM_CONSTANT 3 //HM_CONSTANT uses 0 as the height at all positions.
#define HM_METHOD HM_FUNCTION
//#define HM_METHOD HM_ARRAY
#define GETNEXT_IF 1
#define GETNEXT_PMAX 2
#define GETNEXT GETNEXT_IF
#define SCAN_LINE_LOOP 200 //number of times scan_line is called to test speed.

v4hi horizontal_max(v4hi h0) {
	// horizontal maximum of index
	v4hi h1 = __builtin_shuffle(h0, (v4hi){1,0,3,2});
	v4hi h2 = __builtin_ia32_pmaxsw(h1, h0);
	v4hi h3 = __builtin_shuffle(h2, (v4hi){2,2,0,0});
	v4hi h4 = __builtin_ia32_pmaxsw(h2, h3);
	return h4;
}

// NOTE: hpstart, hrayn, hp0, hp1 must have space for at least (n+4) items!
// NOTE: the 4 elements at the end of the array in hpstart and hrayn must be initialized like this to prevent scan_line from looping infinitely: hpstart[i] = (v3){0,0,0};hrayn[i] = (v3){0,0,0}; TODO: somehow incorporate this in scan_line. (But without removing the const from hpstart and hrayn.)
// NOTE: hw and hh must be powers of 2 (so that "& offset_mask" works)
void scan_line(const int n, const v3* hpstart, const v3* hrayn, const int hw_log2, const int hh_log2, const int hd, const v4si hm_function(const v4si x, const v4si y, void* arg), void* hm_function_arg, v3* hp0, v3* hp1, const int debug_min, const int debug_max) {
	/*
	  1. Set ign := 0
	  2. Repeat:
	  if ign == 0: hp1 = hp + hrayn
	  check bounds (hw,hh,hd and whether hpz<h) and set ign := 1 if outside
	  if ign == 0: hp0 = hp1
	  3. Return when all pixels have ign==1
	 */

	const int32_t hw = 1 << hw_log2;
	const int32_t hh = 1 << hh_log2;

	// TODO: assert the conditions for scan_line noted above.
	
	v4si hp1x, hp1y, hp1z;
	//v4si hp1x, hp1y, hp1z; rather subtract hrayn from hp0 when it's needed
	v4si hraynx, hrayny, hraynz;
	v4hi index={-1,-1,-1,-1};
	int16_t todo = 0;
	v4si ign;

#if NEXT_TODO==NEXT_TODO_LOCAL
	void inline next_todo(int i) {
		index[i] = todo;
		ign[i] = 0;
		hraynx[i] = hrayn[todo].x;
		hrayny[i] = hrayn[todo].y;
		hraynz[i] = hrayn[todo].z;
		hp1x[i] = hpstart[todo].x;
		hp1y[i] = hpstart[todo].y;
		hp1z[i] = hpstart[todo].z;
		todo++;
	}
#elif NEXT_TODO==NEXT_TODO_MACRO
#define next_todo(i)							\
	index[i] = todo;							\
	ign[i] = 0;									\
	hraynx[i] = hrayn[todo].x;					\
	hrayny[i] = hrayn[todo].y;					\
	hraynz[i] = hrayn[todo].z;					\
	hp1x[i] = hpstart[todo].x;					\
	hp1y[i] = hpstart[todo].y;					\
	hp1z[i] = hpstart[todo].z;					\
	todo++;
#endif

	for (int i=0;i<4;i++) {
		next_todo(i); // no bounds check necessary because hpstart,hrayn have space for n+4 items.
	}
	const int32_t hwmax_i=(hw<<16)-1;
	v4si hwmax = {hwmax_i,hwmax_i,hwmax_i,hwmax_i};
	const int32_t hhmax_i=(hh<<16)-1;
	v4si hhmax = {hhmax_i,hhmax_i,hhmax_i,hhmax_i};
	const int32_t hdmax_i=hd-1;
	v4si hdmax = {hdmax_i,hdmax_i,hdmax_i,hdmax_i};
#if HM_METHOD==HM_ARRAY
	const int32_t stride_shift_i=hw_log2;
	const v4si stride_shift={hw_log2,hw_log2,hw_log2,hw_log2};
	const int32_t offset_mask_i=(hh<<stride_shift_i)-1;
	const v4si offset_mask={offset_mask_i,offset_mask_i,offset_mask_i,offset_mask_i};
	const int32_t* hm = hm_function_arg;
#endif
	do {
		for (int isteps=0;isteps<ISTEPS;isteps++) {
			if (DEBUG) {
				for (int i=0;i<4;i++) {
					if (index[i] >= debug_min && index[i] <= debug_max) {
						printf("isteps:%i\n",isteps);
						printf("hp1[%i]:(%x,%x,%x) hrayn[%i]:(%x,%x,%x)\n",i,hp1x[i],hp1y[i],hp1z[i],i,hraynx[i],hrayny[i],hraynz[i]);
					}
				}
			}
			// move position
			v4si hp2x = hp1x + hraynx;
			v4si hp2y = hp1y + hrayny;
			v4si hp2z = hp1z + hraynz;
			hp1x = (hp2x & ~ign) | (hp1x & ign);
			hp1y = (hp2y & ~ign) | (hp1y & ign);
			hp1z = (hp2z & ~ign) | (hp1z & ign);
			if (DEBUG) {
				for (int i=0;i<4;i++) {
					if (index[i] >= debug_min && index[i] <= debug_max) {
						printf("hp2[%i]:(%x,%x,%x) ign[%i]:%i\n",i,hp2x[i],hp2y[i],hp2z[i],i,ign[i]);
						printf("hwmax:%x hhmax:%x hdmax:%x\n",hwmax_i,hhmax_i,hdmax_i);
					}
				}
			}
			v4si outside = {0,0,0,0};
			outside |= (hp1x < (v4si){0,0,0,0}) | (hp1x > hwmax);
			outside |= (hp1y < (v4si){0,0,0,0}) | (hp1y > hhmax);
			outside |= (hp1z > hdmax);
#if HM_METHOD==HM_FUNCTION
			v4si h = (*hm_function)(hp1x, hp1y, hm_function_arg);
#elif HM_METHOD==HM_ARRAY
			v4si offset = (((hp1y >> (v4si){16,16,16,16}) << stride_shift) + (hp1x >> (v4si){16,16,16,16})) & offset_mask;
			v4si h = {hm[offset[0]],hm[offset[1]],hm[offset[2]],hm[offset[3]]};
#elif HM_METHOD==HM_CONSTANT
			v4si h = {0,0,0,0};
#endif
			if (DEBUG) {
				for (int i=0;i<4;i++) {
					if (index[i] >= debug_min && index[i] <= debug_max) {
						printf("hp1x[%i]>>16:%i hp1y[%i]>>16:%i h[%i]:%x outside_borders[%i]:%i\n",i,hp1x[i]>>16,i,hp1y[i]>>16,i,h[i],i,outside[i]);
					}
				}
			}
			outside |= (hp1z < h);
			ign = ign | outside;
			if (DEBUG) {
				for (int i=0;i<4;i++) {
					if (index[i] >= debug_min && index[i] <= debug_max) {
						printf("outside[%i]:%i ign[%i]:%i\n",i,outside[i],i,ign[i]);
					}
				}
			}
		}
#if GETNEXT==GETNEXT_IF
		for (int i=0;i<4;i++) {
			if (DEBUG) {
				if (index[i] >= debug_min && index[i] <= debug_max) {
					printf("index[%i]:%i ign[%i]:%i\n",i,index[i],i,ign[i]);
				}
			}
			if (ign[i]) {
				int done=index[i];
				// store result
				hp0[done].x = hp1x[i] - hraynx[i];
				hp0[done].y = hp1y[i] - hrayny[i];
				hp0[done].z = hp1z[i] - hraynz[i];
				hp1[done].x = hp1x[i];
				hp1[done].y = hp1y[i];
				hp1[done].z = hp1z[i];
				// get new
				next_todo(i);
			}
		}
		for (int i=0;i<4;i++) {
			if (DEBUG) {
				if (index[i] >= debug_min && index[i] <= debug_max) {
					printf("new index[%i]:%i ign[%i]:%i\n",i,index[i],i,ign[i]);
					//printf("new hp1[%i]:(%i,%i,%i)\n",i,hp1x[i],hp1y[i],hp1z[i]);
				}
			}
		}
#elif GETNEXT==GETNEXT_PMAX
		v4si hraynx1;
		v4si hrayny1;
		v4si hraynz1;
		v4si hp1x1;
		v4si hp1y1;
		v4si hp1z1;
		for (int i=0;i<4;i++) {
			if (DEBUG) {
				if (index[i] >= debug_min && index[i] <= debug_max) {
					printf("index[%i]:%i ign[%i]:%i\n",i,index[i],i,ign[i]);
				}
			}
			// store result
			int done=index[i];
			hp0[done].x = hp1x[i] - hraynx[i];
			hp0[done].y = hp1y[i] - hrayny[i];
			hp0[done].z = hp1z[i] - hraynz[i];
			hp1[done].x = hp1x[i];
			hp1[done].y = hp1y[i];
			hp1[done].z = hp1z[i];
			
			// get new task
			v4hi h4 = horizontal_max(index);
			index[i] = ((h4[i]+1) & ign[i]) | (index[i] & ~ign[i]);
			
			int16_t ni = index[i];
			hraynx1[i] = hrayn[ni].x;
			hrayny1[i] = hrayn[ni].y;
			hraynz1[i] = hrayn[ni].z;
			hp1x1[i] = hpstart[ni].x;
			hp1y1[i] = hpstart[ni].y;
			hp1z1[i] = hpstart[ni].z;
		}

		hraynx = (hraynx1 & ign) | (hraynx & ~ign);
		hrayny = (hrayny1 & ign) | (hrayny & ~ign);
		hraynz = (hraynz1 & ign) | (hraynz & ~ign);
		hp1x = (hp1x1 & ign) | (hp1x & ~ign);
		hp1y = (hp1y1 & ign) | (hp1y & ~ign);
		hp1z = (hp1z1 & ign) | (hp1z & ~ign);
		ign = (v4si){0,0,0,0};

		for (int i=0;i<4;i++) {
			if (DEBUG) {
				if (index[i] >= debug_min && index[i] <= debug_max) {
					printf("new index[%i]:%i ign[%i]:%i\n",i,index[i],i,ign);
				}
			}
		}
		
		// compute todo
		//printf("index:(%i,%i,%i,%i)\n",index[0],index[1],index[2],index[3]);
		v4hi h4 = horizontal_max(index);
		todo = h4[0] + 1;
#endif
		if (DEBUG) {
			int print=0;
			for (int i=0;i<4;i++) {
				if (index[i] >= debug_min && index[i] <= debug_max)
					print = 1;
			}
			if (print)
				printf("todo:%i\n",todo);
		}
	} while (todo < n+4);
	
}

typedef struct {
	int32_t* hm;
	v4si stride_shift;
	v4si offset_mask;
} hm_info;

const v4si __attribute__((noinline)) get_hm_height(const v4si x, const v4si y, void* arg) {
	hm_info* hm_info = arg;
	int32_t* hm = hm_info->hm;
	const v4si offset_mask = hm_info->offset_mask;
	const v4si stride_shift = hm_info->stride_shift;
	v4si offset = (((y >> (v4si){16,16,16,16}) << stride_shift) + (x >> (v4si){16,16,16,16})) & offset_mask;
	v4si h = {hm[offset[0]],hm[offset[1]],hm[offset[2]],hm[offset[3]]};
	return h;

//	return (v4si){0,0,0,0};
}

int main(void) {
	int32_t hw=512<<16;
	int32_t hh=512<<16;
	int32_t hd=48<<16;
	int32_t* hm = malloc(sizeof(int32_t)*(hw>>16)*(hh>>16));
	if (hm==NULL) {
		perror("Not enough memory");
		exit(1);
	}

	srandom(1);
	for (int i=0;i<(hw>>16)*(hh>>16);i++) {
		hm[i] = random() % hd;
		//hm[i] = 0;
	}
	const int32_t stride_shift_i=log2(hw>>16);
	const v4si stride_shift={stride_shift_i,stride_shift_i,stride_shift_i,stride_shift_i};
	const int32_t offset_mask_i=((hh>>16)<<stride_shift_i)-1;
	const v4si offset_mask={offset_mask_i,offset_mask_i,offset_mask_i,offset_mask_i};
	hm_info hm_info = {hm, stride_shift, offset_mask};

	int n=20000;
	v3 *hpstart = malloc(sizeof(v3) * (n+4));
	v3 *hrayn = malloc(sizeof(v3) * (n+4));
	if (hpstart==NULL || hrayn==NULL) {
		perror("Not enough memory");
		exit(1);
	}
	for (int i=0;i<n;i++) {
		hpstart[i].x = random() % hw;
		hpstart[i].y = random() % hh;
		hpstart[i].z = random() % (hd-1); // so that hpstart[i] (where i >= n) is above the heightmap. TODO: when I implement two scan_line functions, one for starting position below and one for starting position above the heightmap, and only a intersection of the current position with the heightmap surface causes scan_line to quit, remove this.
		hrayn[i].x = (random() % (1<<16)) * ((random()%2)==0?1:-1);
		hrayn[i].y = (random() % (1<<16)) * ((random()%2)==0?1:-1);
		hrayn[i].z = (random() % (1<<16)) * ((random()%2)==0?1:-1);
		switch(random()%4) {
			case 0: hrayn[i].x = 1<<16; break;
			case 1: hrayn[i].y = 1<<16; break;
			case 2: hrayn[i].x = -(1<<16); break;
			case 3: hrayn[i].y = -(1<<16); break;
		}
	}
	// TODO: somehow incorporate this in scan_line: it prevents scan_line from looping infinitely.
	for (int i=n;i<n+4;i++) {
		hpstart[i] = (v3){0,0,hd-1};
		hrayn[i] = (v3){0,0,0};
	}

	if (DEBUG) {
		printf("hw:%i hw>>16:%i hh:%i hh>>16:%i hd:%i n:%i\n",hw,hw>>16,hh,hh>>16,hd,n);
	}
	
	v3 *hp0 = malloc(sizeof(v3) * (n+4));
	v3 *hp1 = malloc(sizeof(v3) * (n+4));
	if (hp0==NULL || hp1==NULL) {
		perror("Not enough memory");
		exit(1);
	}
#if HM_METHOD==HM_FUNCTION
	void* hm_function_arg = &hm_info;
#elif HM_METHOD==HM_ARRAY
	void* hm_function_arg = hm;
#else
	void* hm_function_arg = NULL;
#endif
	clock_t scan_line_diff = -1;
	for (int i=0;i<SCAN_LINE_LOOP;i++) {
		clock_t scan_line_start = clock();
		scan_line(n, hpstart, hrayn, (int)log2(hw>>16), (int)log2(hh>>16), hd, get_hm_height, hm_function_arg, hp0, hp1, -1,-1);
		__asm__("emms\n");
		clock_t scan_line_stop = clock();
		clock_t diff = scan_line_stop - scan_line_start;
		if (scan_line_diff<0 || diff < scan_line_diff)
			scan_line_diff = diff;
	}
	
	if (DEBUG) {
		for (int i=0;i<n;i++) {
			int x = (hp1[i].x >> 16);
			int y = (hp1[i].y >> 16);
			int z = hp1[i].z;
			int offset = ((y * (hw>>16)) + x) & ((hw>>16)*(hh>>16)-1);
			int is_below = z < hm[offset];
			int is_outside = x < 0 || x >= (hw >> 16) || y < 0 || y >= (hh >> 16) || z >= hd;
			printf("hpstart[%i]:(%x,%x,%x) hrayn[%i]:(%x,%x,%x) hp0[%i]:(%x,%x,%x) hp1[%i]:(%x,%x,%x) x:%i y:%i offset:%i hm[offset]:%i is_below:%i is_outside:%i\n",i,hpstart[i].x,hpstart[i].y,hpstart[i].z,i,hrayn[i].x,hrayn[i].y,hrayn[i].z,i,hp0[i].x,hp0[i].y,hp0[i].z,i,hp1[i].x,hp1[i].y,hp1[i].z,x,y,offset,hm[offset],is_below,is_outside);
			assert(is_below || is_outside);
		}
	}

	printf("a:");
	int a=0;
	for (int i=0;i<n;i++) {
		a += hp0[i].x + hp0[i].y + hp0[i].z;
		a += hp1[i].x + hp1[i].y + hp1[i].z;
	}
	printf("%i\n",a);

	float scan_line_time = ((float)scan_line_diff)/CLOCKS_PER_SEC;
	float pixels_per_second = (float)n/scan_line_time;
	printf("scan_line time: %f size of square rendered at 25fps: %i pixels per second: %i\n", scan_line_time, (int)sqrt(pixels_per_second/25), (int)pixels_per_second);

	return 0;
}
