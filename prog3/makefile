all: smallsh

smallsh: newtypes.o builtins.o dynArray.o smallsh.o
	gcc -g -Wall -o smallsh newtypes.o builtins.o dynArray.o smallsh.o -lm

smallsh.o: smallsh.c smallsh.h
	gcc -g -Wall -c smallsh.c

dynArray.o: dynArray.c dynArray.h
	gcc -g -Wall -c dynArray.c
	
builtins.o: builtins.c builtins.h 
	gcc -g -Wall -c builtins.c 

newtypes.o: newtypes.c newtypes.h 
	gcc -g -Wall -c newtypes.c 

clean:
	rm builtins.o
	rm newtypes.o
	rm dynArray.o
	rm smallsh.o