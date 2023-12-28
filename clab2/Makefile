all: 	cstring.c
	gcc -g -W -Wall cstring.c -o cstring
	
memtest:	all
	valgrind --leak-check=yes ./cstring
		
clean:
	rm -rf *.o
