all: server

server: newtypes.o server.o
	gcc -g -Wall -o server newtypes.o server.o -lm

server.o: server.c
	gcc -g -Wall -c server.c

newtypes.o: newtypes.c newtypes.h 
	gcc -g -Wall -c newtypes.c 

clean:
	rm server.o
	rm newtypes.o