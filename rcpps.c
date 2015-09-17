#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef int32_t v4si __attribute__((vector_size(16)));
typedef float v4sf __attribute__ ((vector_size(16)));

int main() {
	/*
	  We compute:
	  round(((target - x) * (1./r)) * r) + x = computed_target
	  And we know that the relative error of (1./r) is equal or less than 1.5*2**-12. (according to INTEL)
	  Relative error means abs((1./r)-recip_r)/(1.r) <= 1.5*2**-12, where recip_r is the value computed by RCPPS for (1./r).
	  Can we correct (1./r) by a factor?/summand? so that 0 <= (computed_target - target) < 65536?
	  
	  round((target - x) * (1./r)) * r = (computed_target - x)
	  If we multiply (1./r) with increasingly larger values, we will get an increasingly larger error. Therefore test with the largest possible value for (target - x), i.e. 1<<31.
	 */
	int min_error = 0x7fffffff;
	int min_error_r = 0;
	int max_error = -0x80000000;
	int max_error_r = 0;
	for (int r=1;r<((int64_t)1<<31);r++) {
		v4si ri = (v4si){r,r,r,r};
		v4sf rf = __builtin_ia32_cvtdq2ps(ri);
		//const int tx = 1<<15;
		const int tx = 1<<11<<16;
		v4si target_x = (v4si){tx,tx,tx,tx};
		//v4sf recip_r = __builtin_ia32_rcpps(rf);
		//v4sf recip_r = (v4sf){1,1,1,1} / rf;
		//v4sf dstep = __builtin_ia32_cvtdq2ps(target_x) * recip_r;
		v4sf dstep = __builtin_ia32_cvtdq2ps(target_x) / rf;
		assert(dstep[0] >= 0);
		v4si error = __builtin_ia32_cvtps2dq(dstep * rf) - target_x;
		int e = error[0];
		//printf("r:%i e:%i\n", r, e);
		if (min_error > e) {
			min_error = e;
			min_error_r = r;
		}
		if (max_error < e) {
			max_error = e;
			max_error_r = r;
		}
		if ((r % 100000)==0) {
			printf("min_error:%i max_error:%i\n",min_error,max_error);
			printf("min_error_r:%i max_error_r:%i\n",min_error_r,max_error_r);
		}
	}
	printf("min_error:%i max_error:%i\n",min_error,max_error);
	printf("min_error_r:%i max_error_r:%i\n",min_error_r,max_error_r);
}
