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
		perror("ERROR reading from socket");
		return -1;
	}
	if (amtToSend < sizeof(sizeOfString)) 
		printOut("WARNING: Not all data written to socket!", 1);
	// printf("CLIENT: sent size\n");
	/* send the plaintext message */
	amtToSend = sizeOfString;
	sendFail = sendAll(cnctFD, text, &amtToSend); // Read the client's message from the socket
	if (sendFail < 0) {
		perror("ERROR reading from socket");
		return -1;
	}
	if (amtToSend < sizeOfString) 
		printOut("WARNING: Not all data written to socket!", 1);
	
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
		perror("ERROR reading from socket");
		return -1;
	}
	if (amtToRecv < sizeof(sizeOfString)) 
		printOut("WARNING: Not all data read from socket!", 1);
	// printf("SERVER: received size\n");
	/* read the plaintext message */
	memset(buf, '\0', buf_len);
	amtToRecv = sizeOfString;
	recvFail = recvAll(cnctFD, buf, &amtToRecv); // Read the client's message from the socket
	if (recvFail < 0) {
		perror("ERROR reading from socket");
		return -1;
	}
	if (amtToRecv < sizeOfString) 
		printOut("WARNING: Not all data read from socket!", 1);
	
	// printf("SERVER: I received this from the client: \"%s\"\n", buf);

	return 0;
}

/*********************************************************************
** Description: 
** Automates memset() because 'memset' isn't very descriptive
*********************************************************************/
void clearString(char * theString, int size) {
	memset(theString, '\0', size);
}

void printOutError(const char * outString, int newln){
	fputs(outString, stderr);
	fflush(stderr); // flush the print buffer
	if(newln){	
		fputs("\n", stderr);
		fflush(stderr);
	}
}

void printOut(char * outString, int newln){
	fputs(outString, stdout);
	fflush(stdout); // flush the print buffer
	if(newln){	
		fputs("\n", stdout);
		fflush(stdout);
	}
}

void error(const char *msg) { 
	// fprintf(stderr, "%s\n", msg);
	printOutError(msg, 1);
	// perror(msg); 
	exit(1); 
} // Error function used for reporting issues

void errorCloseSocket(const char *msg, int socketFD) { 
	// fprintf(stderr, "%s\n", msg);
	printOutError(msg, 1);
	// perror(msg); 
	close(socketFD);
	exit(1); 
} // Error function used for reporting issues

void errorCloseSocketNoExit(const char *msg, int socketFD) { 
	// perror(msg); 
	printOutError(msg, 1);
	close(socketFD);
} // Error function used for reporting issues

/*********************************************************************
** Description: 
** Return a random number
**
** Ex: min = 3, max = 9, rand % (7) = 0...6 + 3 = 3...9
*********************************************************************/
int getRandom(int min, int max) {
	return (rand() % (max+1-min) + min);
}