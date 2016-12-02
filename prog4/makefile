all: opt_enc_d opt_enc keygen

opt_enc_d: utils.o opt_enc_d.o 
	gcc -g -Wall -o opt_enc_d  utils.o opt_enc_d.o -lm

opt_enc: opt_enc.c utils.o
	gcc -g -Wall -o opt_enc opt_enc.c utils.o -lm

opt_enc_d.o: opt_enc_d.c
	gcc -g -Wall -c opt_enc_d.c

keygen: keygen.c utils.o
	gcc -g -Wall -o keygen keygen.c utils.o -lm

utils.o: utils.c utils.h 
	gcc -g -Wall -c utils.c 

# encrypt.o: encrypt.c encrypt.h
# 	gcc -g -Wall -c encrypt.c

# newtypes.o: newtypes.c newtypes.h 
# 	gcc -g -Wall -c newtypes.c 

clean:
	rm *.o

cleanall: clean
	rm keygen opt_enc opt_enc_d 