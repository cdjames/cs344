#include "utils.h"

int sendAll(int socketFD, void * msg, int * amountToSend) {
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

int sendMsg(char * text, int cnctFD){
	/* get size of message */
	int sizeOfString = strlen(text),
		sendFail,
		amtToSend = sizeof(sizeOfString);

	sendFail = sendAll(cnctFD, &sizeOfString, &amtToSend); // Read int from the socket
	if (sendFail < 0) {
		perror("CLIENT: ERROR reading from socket");
		return -1;
	}
	// printf("CLIENT: sent size\n");
	/* send the plaintext message */
	amtToSend = sizeOfString;
	sendFail = sendAll(cnctFD, text, &amtToSend); // Read the client's message from the socket
	if (sendFail < 0) {
		perror("SERVER: ERROR reading from socket");
		return -1;
	}
	
	// printf("SERVER: I received this from the client: \"%s\"\n", text);

	return 0;
}

int recvAll(int socketFD, void * buf, int * amountToRecv) {
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

int recvMsg(char * buf, int buf_len, int cnctFD){
	/* get size of message */
	int sizeOfString = 0,
		recvFail,
		amtToRecv = sizeof(sizeOfString);

	recvFail = recvAll(cnctFD, &sizeOfString, &amtToRecv); // Read int from the socket
	if (recvFail < 0) {
		perror("CLIENT: ERROR reading from socket");
		return -1;
	}
	// printf("SERVER: received size\n");
	/* read the plaintext message */
	memset(buf, '\0', buf_len);
	amtToRecv = sizeOfString;
	recvFail = recvAll(cnctFD, buf, &amtToRecv); // Read the client's message from the socket
	if (recvFail < 0) {
		perror("SERVER: ERROR reading from socket");
		return -1;
	}
	
	// printf("SERVER: I received this from the client: \"%s\"\n", buf);

	return 0;
}

void error(const char *msg) { 
	perror(msg); 
	exit(1); 
} // Error function used for reporting issues

void errorCloseSocket(const char *msg, int socketFD) { 
	perror(msg); 
	close(socketFD);
	exit(1); 
} // Error function used for reporting issues
