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

int hasValidChars(char * text){
	int textlen = strlen(text),
		i;

	for (i = 0; text[i] != '\0' || i < textlen; i++)
	{
		// printf("char is %d\n", text[i]);
		if( text[i] != ' ' && (text[i] < 'A' || text[i] > 'Z') )
			return 0;
	}

	/* all characters checked out! */
	return 1;
}

void checkText(char * text) {
	int okay = hasValidChars(text);
	if(!okay){
		error("CLIENT: plaintext contains bad characters");
	}
}

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[hdShakeLen];
	char plaintext[maxBufferLen];
    
	if (argc < 4) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)serverHostInfo->h_addr, (char*)&serverAddress.sin_addr.s_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) 
		error2("CLIENT: ERROR opening socket");
	
	// Connect to server
	int connected = connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	printf("CLIENT: connected=%d\n", connected);
	if (connected < 0) // Connect socket to address
		error2("CLIENT: ERROR connecting");
	// printf("errno = %s\n", strerror(errno));

	memset(buffer, '\0', hdShakeLen+1); // Clear out the buffer array
	
	/* get command name and put it in buffer */
	int n = snprintf(buffer, hdShakeLen+1, "%s", "opt_enc");

	/* Send command name to server */
	int amountToSend = strlen(buffer);
	int sendFail;
	printf("CLIENT: sending handshake\n");
	sendFail = sendAll(socketFD, buffer, &amountToSend); // Write to the server
	// printf("amount sent = %d\n", amountToSend);
	if (sendFail < 0) 
		errorCloseSocket("CLIENT: ERROR writing to socket", socketFD);
	if (amountToSend < strlen(buffer)) 
		printOut("CLIENT: WARNING: Not all data written to socket!", 1);

	/* find out if the connection was accepted by receiving 1 or 0 */
	int accepted = 0;
	int recvFail;
	int amountToRecv = sizeof(accepted);
	recvFail = recvAll(socketFD, &accepted, &amountToRecv); // Read int from the socket
	
	if (recvFail < 0) 
		errorCloseSocket("CLIENT: ERROR reading from socket", socketFD);
	
	printf("CLIENT: accepted = %d\n", accepted);
	
	if(accepted) {
		FILE * file;
		file = fopen(argv[1],"r");

		if (file == NULL)
			errorCloseSocket("No such file", socketFD);

		/* get plaintext */
		memset(plaintext, '\0', maxBufferLen+1);
		if(fgets(plaintext, maxBufferLen, file) == NULL)
			errorCloseSocket("Could not open file", socketFD);

		// n = snprintf(plaintext, maxBufferLen+1, "%s", "DUMMY TEXT HERE");

		
		/* trim newline from string */
		plaintext[strcspn(plaintext, "\n")] = '\0';
		int ptLength = strlen(plaintext);

		/* check your text and exit if not okay */
		checkText(plaintext);

		/* send plaintext */
		int sendFail = sendMsg(plaintext, socketFD);
		if(sendFail < 0){
			errorCloseSocket("CLIENT: ERROR sending plaintext", socketFD);
		}

		/* get key */
		memset(plaintext, '\0', maxBufferLen+1);
		n = snprintf(plaintext, maxBufferLen+1, "%s", "SIZLESCRAT MEEK");

		/* check your text and exit if not okay */
		checkText(plaintext);

		int keyLength = strlen(plaintext);
		/* trim newline from string */
		if(plaintext[keyLength] == '\n'){
			plaintext[keyLength] = '\0';
			keyLength -= 1;
		}

		/* exit if key is too short */
		if(keyLength < ptLength)
			errorCloseSocket("CLIENT: key is too short", socketFD);

		/* otherwise send key */
		sendFail = sendMsg(plaintext, socketFD);
		if(sendFail < 0){
			errorCloseSocket("CLIENT: ERROR sending key", socketFD);
		}

		/* receive encrypted text */
		memset(plaintext, '\0', maxBufferLen+1);
		recvFail = recvMsg(plaintext, maxBufferLen+1, socketFD);
		if(recvFail < 0)
			errorCloseSocket("CLIENT: ERROR reading encrypted text", socketFD);

		/* print our encrypted text to standard out */
		printOut(plaintext, 0);

	}

	close(socketFD); // Close the socket

	return 0;
}
