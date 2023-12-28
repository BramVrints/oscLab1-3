all: ex1main.o ex1support.o
	gcc -g -o myprogram ex1main.o ex1support.o
	
ex1main.o: ex1main.c
	gcc -g -c ex1main.c -o ex1main.o

ex1support.o: ex1support.c
	gcc -g -c ex1support.c -o ex1support.o
	
clean:
	rm *.o
	rm myprogram
	
	
