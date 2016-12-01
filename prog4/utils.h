#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifndef UTILS_H
#define UTILS_H

/*********************************************************************
** Description: 
** Send all bytes or return error
w/ help from : https://beej.us/guide/bgnet/output/html/multipage/advanced.html
*********************************************************************/
int sendAll(int socketFD, void * msg, int * amountToSend);

int sendMsg(char * text, int cnctFD);

/*********************************************************************
** Description: 
** Receive all bytes or return error
w/ help from : https://beej.us/guide/bgnet/output/html/multipage/advanced.html
*********************************************************************/
int recvAll(int socketFD, void * buf, int * amountToRcv);

int recvMsg(char * buf, int buf_len, int cnctFD);
/*********************************************************************
** Description: 
** run perror(msg) and exit(1)
*********************************************************************/
void clearString(char * theString, int size);

void printOut(char * outString, int newln);

void printOutError(const char * outString, int newln);

void error(const char *msg);

void errorCloseSocket(const char *msg, int socketFD);

int getRandom(int min, int max);

#endif