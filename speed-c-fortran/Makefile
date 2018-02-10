all: l2norm-fortran l2norm-c

l2norm-fortran: l2norm.f95
	gfortran -g -O2 -o l2norm-fortran l2norm.f95

l2norm-c: l2norm.c
	gcc -g --std=c99 -O2 -o l2norm-c l2norm.c

run: l2norm-fortran l2norm-c
	./l2norm-fortran 1; ./l2norm-c

clean:
	rm -f l2norm-fortran l2norm-c
