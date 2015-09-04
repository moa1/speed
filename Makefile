clean:
	rm -f *.o vec mmx-asm inner-loop

vec: vec.c
	gcc -g --std=gnu99 -march=pentium4 -o vec ./vec.c

mmx-asm: mmx-asm.c
	gcc -g -O3 --std=gnu99 -march=pentium4 -o mmx-asm ./mmx-asm.c

inner-loop: inner-loop.c
	gcc -g -O3 --std=c99 -march=pentium4 -o inner-loop ./inner-loop.c

