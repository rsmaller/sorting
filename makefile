CC = gcc
OFLAGS = -O3 -march=native -funroll-loops -fomit-frame-pointer -flto -o optsort
UFLAGS = -o unoptsort

default: all

all: optsort unoptsort

optsort: sort.c
	$(CC) $(OFLAGS) ./sort.c

unoptsort: sort.c
	$(CC) $(UFLAGS) ./sort.c

clean:
	rm optsort unoptsort