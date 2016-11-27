all: server client

server: newtypes.o utils.o server.o
	gcc -g -Wall -o server newtypes.o utils.o server.o -lm

client: client.c utils.o
	gcc -g -Wall -o client client.c utils.o -lm

server.o: server.c
	gcc -g -Wall -c server.c

newtypes.o: newtypes.c newtypes.h 
	gcc -g -Wall -c newtypes.c 

utils.o: utils.c utils.h
	gcc -g -Wall -c utils.c 

clean:
	rm server.o
	rm newtypes.o