all: mult

mult: mult.c
	gcc --std=c99 -O0 -g -o mult mult.c
	gcc --std=c99 -O0 -g -S mult.c
	ln -fs mult shift
	ln -fs mult and
	ln -fs mult or
	ln -fs mult float

clean:
	rm -f mult mult.s shift and or float
