all: test_server sensor_node

run: runserver

clean:
	rm test_server
	rm sensor_node
	rm *.o
	rm *.a
	rm *.so

test_server: test_server.c lib/tcpsock.c
	gcc -g test_server.c lib/tcpsock.c -o test_server

sensor_node: sensor_node.c lib/tcpsock.c
	gcc -g sensor_node.c lib/tcpsock.c -o sensor_node
	
statictcplib: lib/tcpsock.c
	gcc -c lib/tcpsock.c -o obj/tcpsock.o
	ar -crs slib/libtcpsock.a obj/tcpsock.o
	
server_withstaticlib: statictcplib test_server.c
	gcc test_server.c slib/libtcpsock.a -o serverstaticlib
	
dyntcplib: lib/tcpsock.c
	gcc -c -fPIC lib/tcpsock.c -o obj/tcpsock.o
	gcc -shared -o dlib/libtcpsock.so obj/tcpsock.o
	
server_withdynlib: dyntcplib test_server.c
	gcc test_server.c -o serverdynlib -Ldlib -ltcpsock
	
runserver: test_server
	./test_server 5678 4

runclient1: sensor_node
	./sensor_node 1 2 127.0.0.1 5678

runclient2: sensor_node
	./sensor_node 2 5 127.0.0.1 5678
