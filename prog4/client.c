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

	// Get return message from server
	// memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	// int aliveCheck;
	// aliveCheck = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0
	// printf("aliveCheck = %d\n", aliveCheck);

	// Get input message from user
	// printf("CLIENT: Enter text to send to the server, and then hit enter: ");
	// memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	// fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	// buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds

	memset(buffer, '\0', hdShakeLen+1); // Clear out the buffer array
	/* get command name and put it in buffer */

	int n = snprintf(buffer, hdShakeLen+1, "%s", argv[0]);

	// Send message to server
	// charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	int amountToSend = strlen(buffer);
	int sendFail;
	sendFail = sendAll(socketFD, buffer, &amountToSend); // Write to the server
	printf("amount sent = %d\n", amountToSend);
	// if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (sendFail < 0) error("CLIENT: ERROR writing to socket");
	if (amountToSend < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server
	int amountToRecv = sizeof(buffer);
	int recvFail;
	memset(buffer, '\0', amountToRecv); // Clear out the buffer again for reuse
	// charsRead = recv(socketFD, buffer, sizeof(buffer), 0); // Read data from the socket, leaving \0 at end
	recvFail = recvAll(socketFD, buffer, &amountToRecv); // Read data from the socket, leaving \0 at end
	// printf("charsread = %d\n", charsRead);
	if (recvFail < 0) 
		error("CLIENT: ERROR reading from socket");
	else if (amountToRecv == 0)
		error("CLIENT: no data from server; connection closed");
	else
		printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	close(socketFD); // Close the socket
	return 0;
}
