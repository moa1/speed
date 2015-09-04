#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

typedef int32_t v4si32 __attribute__ ((vector_size (16)));

typedef struct {
	int32_t x;
	int32_t y;
	int32_t z;
} v3;

#define ISTEPS 10
#define DEBUG 1

// NOTE: hpstart, hrayn, hp0, hp1 must have space for at least (n+4) items!
// NOTE: hw and hh must be powers of 2 (so that "& offset_mask" works)
int scan_line(const int n, const v3* hpstart, const v3* hrayn, const int hw_log2, const int hh_log2, const int hd, const int32_t* hm, v3* hp0, v3* hp1, const int debug) {
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
	
	v4si32 hp1x, hp1y, hp1z;
	//v4si32 hp1x, hp1y, hp1z; rather subtract hrayn from hp0 when it's needed
	v4si32 hraynx, hrayny, hraynz;
	v4si32 index={-1,-1,-1,-1};
	int todo = 0;
	v4si32 ign;
	void next_todo(int i) {
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
	
	for (int i=0;i<4;i++) {
		next_todo(i); // no bounds check necessary because hpstart,hrayn have space for n+4 items.
	}
	const int32_t hwmax=(hw<<16)-1;
	const int32_t hhmax=(hh<<16)-1;
	const int32_t hdmax=hd-1;
	const int32_t stride_shift=hw_log2;
	const int32_t offset_mask=(hh<<stride_shift)-1;
	do {
		for (int isteps=0;isteps<ISTEPS;isteps++) {
			if (DEBUG) {
				for (int i=0;i<4;i++) {
					if (index[i] == debug) {
						printf("isteps:%i\n",isteps);
						printf("hp1[%i]:(%x,%x,%x) hrayn[%i]:(%x,%x,%x)\n",i,hp1x[i],hp1y[i],hp1z[i],i,hraynx[i],hrayny[i],hraynz[i]);
					}
				}
			}
			// move position
			v4si32 hp2x = hp1x + hraynx;
			v4si32 hp2y = hp1y + hrayny;
			v4si32 hp2z = hp1z + hraynz;
			hp1x = (hp2x & ~ign) | (hp1x & ign);
			hp1y = (hp2y & ~ign) | (hp1y & ign);
			hp1z = (hp2z & ~ign) | (hp1z & ign);
			if (DEBUG) {
				for (int i=0;i<4;i++) {
					if (index[i] == debug) {
						printf("hp2[%i]:(%x,%x,%x) ign[%i]:%i\n",i,hp2x[i],hp2y[i],hp2z[i],i,ign[i]);
						printf("hwmax:%x hhmax:%x hdmax:%x\n",hwmax,hhmax,hdmax);
					}
				}
			}
			v4si32 outside = {0,0,0,0};
			outside |= (hp1x < 0) | (hp1x > hwmax);
			outside |= (hp1y < 0) | (hp1y > hhmax);
			outside |= (hp1z > hdmax);
			v4si32 offset = (((hp1y >> 16) << stride_shift) + (hp1x >> 16)) & offset_mask;
			v4si32 h = {hm[offset[0]],hm[offset[1]],hm[offset[2]],hm[offset[3]]};
			if (DEBUG) {
				for (int i=0;i<4;i++) {
					if (index[i] == debug) {
						printf("offset[%i]:%i hp1x[%i]>>16:%i hp1y[%i]>>16:%i h[%i]:%x\n",i,offset[i],i,hp1x[i]>>16,i,hp1y[i]>>16,i,h[i]);
					}
				}
			}
			outside |= (hp1z < h);
			ign = ign | outside;
			if (DEBUG) {
				for (int i=0;i<4;i++) {
					if (index[i] == debug) {
						printf("outside[%i]:%i ign[%i]:%i\n",i,outside[i],i,ign[i]);
					}
				}
			}
		}
		for (int i=0;i<4;i++) {
			if (DEBUG) {
				if (index[i] == debug) {
					printf("next ISTEPS\n");
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
	} while (todo <= n+4);
	
}

int main(void) {
	int32_t hw=32<<16;
	int32_t hh=16<<16;
	int32_t hd=48<<16;
	int32_t* hm = malloc(sizeof(int32_t)*(hw>>16)*(hh>>16));

	srandom(1);
	for (int i=0;i<(hw>>16)*(hh>>16);i++) {
		hm[i] = random() % hd;
//		hm[i] = 0;
	}

	int n=5000;
	v3 hpstart[n+4];
	v3 hrayn[n+4];
	for (int i=0;i<n;i++) {
		hpstart[i].x = random() % hw;
		hpstart[i].y = random() % hh;
		hpstart[i].z = random() % hd;
		hrayn[i].x = (random() % (1<<16)) * ((random()%2)==0?1:-1);
		hrayn[i].y = (random() % (1<<16)) * ((random()%2)==0?1:-1);
		hrayn[i].z = (random() % (1<<16)) * ((random()%2)==0?1:-1);
		switch(random()%6) {
			case 0: hrayn[i].x = 1<<16; break;
			case 1: hrayn[i].y = 1<<16; break;
			case 2: hrayn[i].z = 1<<16; break;
			case 3: hrayn[i].x = -(1<<16); break;
			case 4: hrayn[i].y = -(1<<16); break;
			case 5: hrayn[i].z = -(1<<16); break;
		}
	}

	if (DEBUG) {
		printf("hw:%i hw>>16:%i hh:%i hh>>16:%i hd:%i n:%i\n",hw,hw>>16,hh,hh>>16,hd,n);
	}
	
	v3 hp0[n+4];
	v3 hp1[n+4];
	int steps[n+4];
	scan_line(n, hpstart, hrayn, (int)log2(hw>>16), (int)log2(hh>>16), hd, hm, hp0, hp1, DEBUG?-1:-1);

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

	int a=0;
	for (int i=0;i<n;i++) {
		a += hp0[i].x + hp0[i].y + hp0[i].z;
		a += hp1[i].x + hp1[i].y + hp1[i].z;
	}
	
	printf("a:%i\n",a);

	return 0;
}
