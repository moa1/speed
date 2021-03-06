# the shitty make doesn't allow me to specify a default CC: CC ?= gcc and running make executes "cc" instead of "gcc".
CFLAGS ?= -O3 -march=native

all: branch-prediction vec mmx-asm inner-loop rcpps sigmoid-speed princess inline cache mult

clean:
	rm -f *.o branch-prediction vec mmx-asm inner-loop rcpps sigmoid-speed princess inline cache mult mult.s sub shift and or float


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

princess: princess.c
	$(CC) -g --std=c99 -O3 -o princess princess.c

inline: inline.c
	$(CC) -g --std=c99 -lm -O3 -o inline inline.c

cache: cache.c
	$(CC) -g --std=c99 -O3 -o cache cache.c

mult: mult.c
	gcc --std=c99 -O0 -g -o mult mult.c
	gcc --std=c99 -O0 -g -S mult.c
	ln -fs mult sub
	ln -fs mult shift
	ln -fs mult and
	ln -fs mult or
	ln -fs mult float
