CC = g++
OFLAGS = -O3 -march=native -funroll-loops -fomit-frame-pointer -flto -o optsort
UFLAGS = -o unoptsort

default: all

all: optsort unoptsort

optsort: sort.cpp
	$(CC) $(OFLAGS) ./sort.cpp

unoptsort: sort.cpp
	$(CC) $(UFLAGS) ./sort.cpp

clean:
	rm optsort unoptsort