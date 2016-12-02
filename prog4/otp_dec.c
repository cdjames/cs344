/*********************************************************************
** Author: Collin James
** Date: 12/1/16
** Description: Connect to otp_dec_d; send text and key; receive encryption
*********************************************************************/

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
	int socketFD, portNumber;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[hdShakeLen];
	char plaintext[maxBufferLen];
    
	if (argc < 4) { 
		fprintf(stderr,"USAGE: %s hostname port\n", argv[0]);
		exit(1); 
	} // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "otp_dec: ERROR, no such host\n"); exit(0); }
	memcpy((char*)serverHostInfo->h_addr, (char*)&serverAddress.sin_addr.s_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) 
		error2("otp_dec: ERROR opening socket");
	
	// Connect to server
	int connected = connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	// printf("otp_dec: connected=%d\n", connected);
	if (connected < 0) // Connect socket to address
		error2("otp_dec: ERROR connecting");
	// printf("errno = %s\n", strerror(errno));

	clearString(buffer, hdShakeLen+1); // Clear out the buffer array
	
	/* get command name and put it in buffer */
	int n = snprintf(buffer, hdShakeLen+1, "%s", "otp_dec");

	/* Send command name to server */
	int amountToSend = strlen(buffer);
	int sendFail;
	// printf("otp_dec: sending handshake\n");
	sendFail = sendAll(socketFD, buffer, &amountToSend); // Write to the server
	
	if (sendFail < 0) 
		errorCloseSocket("otp_dec: can only connect to otp_dec_d", socketFD);
	if (amountToSend < strlen(buffer)) 
		printOut("otp_dec: WARNING: Not all data written to socket!", 1);

	/* find out if the connection was accepted by receiving 1 or 0 */
	int accepted = 0;
	int recvFail;
	int amountToRecv = sizeof(accepted);
	recvFail = recvAll(socketFD, &accepted, &amountToRecv); // Read int from the socket
	
	if (recvFail < 0) 
		errorCloseSocket("otp_dec: ERROR reading from socket", socketFD);
	else if(recvFail > 0)
		errorCloseSocket("otp_dec: Socket closed by server", socketFD);
	
	// printf("otp_dec: accepted = %d\n", accepted);
	
	if(accepted) {
		FILE * file;
		FILE * file2;

		/* checking for existence of file first, exit if necessary */
		checkFile(argv[1], socketFD);
		
		/* open the file and check for errors */
		file = fopen(argv[1],"r");

		if (file == NULL)
			errorCloseSocket("otp_dec: No such file", socketFD);

		/* get plaintext */
		clearString(plaintext, maxBufferLen+1);
		if(fgets(plaintext, maxBufferLen, file) == NULL)
			errorCloseSocket("otp_dec: Could not get contents of file", socketFD);

		fclose(file);
		
		/* trim newline from string */
		plaintext[strcspn(plaintext, "\n")] = '\0';
		/* save length of plaintext */
		int ptLength = strlen(plaintext);

		/* check your text and exit if not okay */
		checkText(plaintext, socketFD, argv[1]);

		/* send plaintext */
		int sendFail = sendMsg(plaintext, socketFD);
		if(sendFail < 0){
			errorCloseSocket("otp_dec: ERROR sending plaintext", socketFD);
		}

		/* get key */
		/* check for existence of file first, exit if necessary */
		checkFile(argv[2], socketFD);

		file2 = fopen(argv[2],"r"); // open the key file

		clearString(plaintext, maxBufferLen+1);
		if(fgets(plaintext, maxBufferLen, file2) == NULL)
			errorCloseSocket("otp_dec: Could not get contents of file", socketFD);
		
		fclose(file2);
		
		/* trim newline from string */
		plaintext[strcspn(plaintext, "\n")] = '\0';
		int keyLength = strlen(plaintext);

		/* check your key and exit if not okay */
		checkText(plaintext, socketFD, argv[2]);

		/* exit if key is too short */
		if(keyLength < ptLength)
			errorCloseSocket("otp_dec: key is too short", socketFD);

		/* otherwise send key */
		sendFail = sendMsg(plaintext, socketFD);
		if(sendFail < 0){
			errorCloseSocket("otp_dec: ERROR sending key", socketFD);
		}

		/* receive encrypted text; it will have newline already appended */
		clearString(plaintext, maxBufferLen+1);
		recvFail = recvMsg(plaintext, maxBufferLen+1, socketFD);
		if(recvFail < 0)
			errorCloseSocket("otp_dec: ERROR reading encrypted text", socketFD);
		else if(recvFail > 0)
			errorCloseSocket("otp_dec: Socket closed by server", socketFD);

		/* print our encrypted text to standard out */
		printOut(plaintext, 0);

	}
	else // connection not accepted
	{
		errorCloseSocket("otp_dec: can only connect to otp_dec_d", socketFD);
	}

	close(socketFD); // Close the socket

	return 0;
}


