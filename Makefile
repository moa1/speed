ifndef CC
	CC=gcc
endif
ifndef CFLAGS
	CFLAGS=-march=native
endif

clean:
	rm -f *.o vec mmx-asm inner-loop

branch-prediction:
	$(CC) --std=c99 -o branch-prediction branch-prediction.c

vec: vec.c
	$(CC) -g --std=gnu99 ${CFLAGS} -o vec ./vec.c

mmx-asm: mmx-asm.c
	$(CC) -g -O3 --std=gnu99 ${CFLAGS} -o mmx-asm ./mmx-asm.c

inner-loop: inner-loop.c
	$(CC) -g -O3 --std=c99 ${CFLAGS} -lm -o inner-loop ./inner-loop.c
	#$(CC) -g -O0 --std=c99 ${CFLAGS} -lm -o inner-loop ./inner-loop.c

