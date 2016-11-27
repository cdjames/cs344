#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef UTILS_H
#define UTILS_H

int sendAll(int socketFD, char * msg, int * amountToSend);

int recvAll(int socketFD, char * buf, int * amountToRcv);

void error(const char *msg);

#endif