# To run, enter
# make all

all: main.o barrier.o
	gcc -o testbarrier main.o barrier.o  -lpthread

main.o: main.c barrier.h
	gcc -c main.c -lpthread

barrier.o: barrier.c barrier.h
	gcc -c barrier.c -lpthread

run: testbarrier
	./testbarrier 10

