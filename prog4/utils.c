#include "utils.h"

int sendAll(int socketFD, char * msg, int * amountToSend) {
	// figure out how much needs to be sent

	int total = 0; // amount sent
	int amt;
	int bytesToSend = *amountToSend;
	
	while(total < *amountToSend){
		// send
		amt = send(socketFD, msg+total, bytesToSend, 0);
		/* get out of loop on send error */
		if(amt == -1)
			break;

		total += amt;
		bytesToSend -= amt;
	}

	// figure out how much was sent and return
	*amountToSend = total;
	
	/* return an error or success depending on result */
	if(amt == -1)
		return -1;
	else
		return 0;
}

int recvAll(int socketFD, char * buf, int * amountToRecv) {
	// figure out how much needs to be sent

	int total = 0; // amount received
	int amt;
	int bytesToRecv = *amountToRecv;
	
	while(total < *amountToRecv){
		// send
		amt = recv(socketFD, buf+total, bytesToRecv, 0);
		/* get out of loop on send error */
		if(amt == -1)
			break;

		total += amt;
		bytesToRecv -= amt;
	}

	// figure out how much was sent and return
	*amountToRecv = total;
	
	/* return an error or success depending on result */
	if(amt == -1)
		return -1;
	else
		return 0;
}

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues
