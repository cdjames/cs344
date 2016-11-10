all: smallsh

smallsh: newtypes.o builtins.o dynArray.o smallsh.o
	gcc -g -Wall -std=c99 -o smallsh newtypes.o builtins.o dynArray.o smallsh.o -lm

smallsh.o: smallsh.c smallsh.h
	gcc -g -Wall -std=c99 -c smallsh.c

dynArray.o: dynArray.c dynArray.h
	gcc -g -Wall -std=c99 -c dynArray.c
	
builtins.o: builtins.c builtins.h 
	gcc -g -Wall -std=c99 -c builtins.c 

newtypes.o: newtypes.c newtypes.h 
	gcc -g -Wall -std=c99 -c newtypes.c 

clean:
	rm builtins.o
	rm newtypes.o
	rm dynArray.o
	rm smallsh.o