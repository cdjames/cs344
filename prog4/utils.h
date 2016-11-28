#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef UTILS_H
#define UTILS_H

/*********************************************************************
** Description: 
** Send all bytes or return error
w/ help from : https://beej.us/guide/bgnet/output/html/multipage/advanced.html
*********************************************************************/
int sendAll(int socketFD, void * msg, int * amountToSend);

/*********************************************************************
** Description: 
** Receive all bytes or return error
w/ help from : https://beej.us/guide/bgnet/output/html/multipage/advanced.html
*********************************************************************/
int recvAll(int socketFD, void * buf, int * amountToRcv);

/*********************************************************************
** Description: 
** run perror(msg) and exit(1)
*********************************************************************/
void error(const char *msg);

#endif