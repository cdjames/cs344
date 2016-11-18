all: smallsh

smallsh: newtypes.o builtins.o cirListDeque.o smallsh.o
	gcc -g -Wall -o smallsh newtypes.o builtins.o cirListDeque.o smallsh.o -lm

smallsh.o: smallsh.c smallsh.h
	gcc -g -Wall -c smallsh.c

newtypes.o: newtypes.c newtypes.h 
	gcc -g -Wall -c newtypes.c 

cirListDeque.o: cirListDeque.c cirListDeque.h newtypes.h
	gcc -g -Wall -c cirListDeque.c
	
builtins.o: builtins.c builtins.h 
	gcc -g -Wall -c builtins.c 

clean:
	rm builtins.o
	rm newtypes.o
	rm cirListDeque.o
	rm smallsh.o