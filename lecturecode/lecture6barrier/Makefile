
all: barrier.c  main.c
	gcc -o testbarrier barrier.c main.c -lpthread

run: all
	./testbarrier 6
