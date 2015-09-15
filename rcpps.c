#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>

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
	  If we multiply (1./r) by a large value, we will exaggerate the error. Therefore test with the largest possible value for (target - x), i.e. 1<<31.
	 */
	for (int ri=1;ri<((int64_t)1<<31);ri++) {
		v4si r = (v4si){ri,ri,ri,ri};
		v4sf recip_r = __builtin_ia32_rcpps(__builtin_ia32_cvtdq2ps(r));
		v4si target_x = (v4si){1<<31,1<<31,1<<31,1<<31};
		v4sf dstep = __builtin_ia32_cvtdq2ps(target_x) * recip_r;
		v4si error = __builtin_ia32_cvtps2dq(dstep) * r - target_x;
		printf("r:%i error=%i\n", r[0], error[0]);
	}
}
