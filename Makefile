# the shitty make doesn't allow me to specify a default CC: CC ?= gcc and running make executes "cc" instead of "gcc".
CFLAGS ?= -O3 -march=native

clean:
	rm -f *.o vec mmx-asm inner-loop rcpps sigmoid-speed

branch-prediction:
	$(CC) --std=c99 -o branch-prediction branch-prediction.c

vec: vec.c
	$(CC) -g --std=gnu99 ${CFLAGS} -o vec vec.c

mmx-asm: mmx-asm.c
	$(CC) -g -O3 --std=gnu99 ${CFLAGS} -o mmx-asm mmx-asm.c

inner-loop: inner-loop.c
	$(CC) -g --std=c99 ${CFLAGS} -lm -o inner-loop inner-loop.c

rcpps: rcpps.c
	$(CC) -g --std=c99 ${CFLAGS} -lm -o rcpps rcpps.c

sigmoid-speed: sigmoid-speed.c
	$(CC) -g --std=c99 -O2 -lm -o sigmoid-speed sigmoid-speed.c

