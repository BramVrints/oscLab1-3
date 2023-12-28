argvunsafe:
	gcc -o prog bufferoverflow1.c -fno-stack-protector

argvsafe: 
	gcc -o prog bufferoverflow1.c 

getsunsafe:
	gcc -w -o prog buffergets.c -fno-stack-protector

getssafe:
	gcc -w -o prog buffergets.c

passunsafe:
	gcc -w -o prog passwordfail.c -fno-stack-protector

passsafe:
	gcc -w -o prog passwordfail.c

snprintfunsafe: 
	gcc -w -o prog buffersnprintf.c -fno-stack-protector
