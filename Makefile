all: mult

mult: mult.c
	gcc --std=c99 -O0 -g -o mult mult.c
	gcc --std=c99 -O0 -g -S mult.c
	ln -fs mult shift
	ln -fs mult and

clean:
	rm -f mult shift
