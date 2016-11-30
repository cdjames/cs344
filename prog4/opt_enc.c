#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <errno.h>
#include "utils.h"

const int maxBufferLen = 70000;
const int hdShakeLen = 7;

// int sendMsg(char * text, int cnctFD){
// 	/* get size of message */
// 	int sizeOfString = strlen(text),
// 		sendFail,
// 		amtToSend = sizeof(sizeOfString);

// 	sendFail = sendAll(cnctFD, &sizeOfString, &amtToSend); // Read int from the socket
// 	if (sendFail < 0) {
// 		perror("CLIENT: ERROR reading from socket");
// 		return -1;
// 	}
// 	// printf("CLIENT: sent size\n");
// 	/* send the plaintext message */
// 	amtToSend = sizeOfString;
// 	sendFail = sendAll(cnctFD, text, &amtToSend); // Read the client's message from the socket
// 	if (sendFail < 0) {
// 		perror("SERVER: ERROR reading from socket");
// 		return -1;
// 	}
	
// 	// printf("SERVER: I received this from the client: \"%s\"\n", text);

// 	return 0;
// }

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[hdShakeLen];
	char plaintext[maxBufferLen];
    
	if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(argv[1]); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)serverHostInfo->h_addr, (char*)&serverAddress.sin_addr.s_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	int connected = connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	printf("CLIENT: connected=%d\n", connected);
	if (connected < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");
	// printf("errno = %s\n", strerror(errno));

	memset(buffer, '\0', hdShakeLen+1); // Clear out the buffer array
	
	/* get command name and put it in buffer */
	int n = snprintf(buffer, hdShakeLen+1, "%s", "opt_enc");

	// Send command name to server
	int amountToSend = strlen(buffer);
	int sendFail;
	printf("CLIENT: sending handshake\n");
	sendFail = sendAll(socketFD, buffer, &amountToSend); // Write to the server
	// printf("amount sent = %d\n", amountToSend);
	if (sendFail < 0) error("CLIENT: ERROR writing to socket");
	if (amountToSend < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	/* find out if the connection was accepted by receiving 1 or 0 */
	int accepted = 0;
	int recvFail;
	int amountToRecv = sizeof(accepted);
	recvFail = recvAll(socketFD, &accepted, &amountToRecv); // Read int from the socket
	if (recvFail < 0) error("CLIENT: ERROR reading from socket");
	printf("CLIENT: accepted = %d\n", accepted);
	if(accepted) {
		memset(plaintext, '\0', maxBufferLen+1);
		n = snprintf(plaintext, maxBufferLen+1, "%s", "DUMMY TEXT HERE");
		/* send plaintext */
		int sendFail = sendMsg(plaintext, socketFD);
		if(sendFail < 0){
			error("CLIENT: ERROR sending plaintext");
		}

		/* send key */
		memset(plaintext, '\0', maxBufferLen+1);
		n = snprintf(plaintext, maxBufferLen+1, "%s", "SIZLESCRAT MEEK");
		/* send plaintext */
		sendFail = sendMsg(plaintext, socketFD);
		if(sendFail < 0){
			error("CLIENT: ERROR sending key");
		}

		/* receive encrypted text */
		memset(plaintext, '\0', maxBufferLen+1);
		recvFail = recvMsg(plaintext, maxBufferLen+1, socketFD);
		if(recvFail < 0)
			error("CLIENT: ERROR reading encrypted text");

		printf("encrypted text = %s\n", plaintext);

	}
	close(socketFD); // Close the socket
	return 0;
}
