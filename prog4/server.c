#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

const int maxConnections = 5;
void error(const char *msg) { 
	perror(msg); 
	exit(1); 
} // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	int exitSignal = 0,
		pid = -1, // for storing the child pid
		numConnections = 0; // store number of connections (5 max)
	int status,
		wpid;
		
	socklen_t sizeOfClientInfo;
	// char buffer[256];
	struct sockaddr_in serverAddress, clientAddress;

	/* select stuff */
	fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax,
    	newfdm,
    	yes = 1; // for setsockopt;

    /* setup select sets by clearing */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

	if (argc < 2) { fprintf(stderr,"SERVER: USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("SERVER: ERROR opening socket");

	// lose the pesky "address already in use" error message (from beej.us)
    // setsockopt(listenSocketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("SERVER: ERROR on binding");

	listen(listenSocketFD, maxConnections); // Flip the socket on - it can now receive up to 5 connections
	// if(listen(listenSocketFD, 5) < 0); // Flip the socket on - it can now receive up to 5 connections
	// 	error("ERROR on listening");

	/* add listenSocketFD to master set (for select); set the max fd to be the listener */
	FD_SET(listenSocketFD, &master);
	fdmax = listenSocketFD;

	while(exitSignal == 0 && pid != 0) { // do until message is "exit"
		/* collect finished processes */
		do {
			wpid = waitpid(-1, &status, WNOHANG);
		} while (wpid > 0);

		/* copy the master set into a temporary set for this iteration */
		read_fds = master;

		/* do your select and make sure there are no errors */
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            error("SERVER: select failed");
            // exit(4);
        }
        else { // no errors, look for new connections or data
        	printf("looking for connections\n");
        	int i;
        	for (i = 0; i <= fdmax; i++)
        	{
        		if(FD_ISSET(i, &read_fds)) { // we have a connection/data to read
        			printf("new connection\n");
        			if(i == listenSocketFD){ // there is a new listener
        				/* check that you still have available connections and perform handshake */
						// if(numConnections < maxConnections) {
						// Accept a connection, blocking if one is not available until one connects
						sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
						establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
						if (establishedConnectionFD < 0) 
							error("SERVER: ERROR on accept");
						else { // add the new connection to the set of connections
							// numConnections += 1;
							printf("adding new connection to master\n");
							FD_SET(establishedConnectionFD, &master);
							if(establishedConnectionFD > fdmax)
								fdmax = establishedConnectionFD;
						}
        			}
        			else { // we have something to read
	        			printf("getting data\n");
	        			/**** do fork here; the following should be in the child ****/
						
						exitSignal = doEncryptInChild(i);
						FD_CLR(i, &master); // remove the connection from the master set
						// numConnections -= 1; // make another connection available
	        		}
        		}	
			}
		}
	}

/**** this is done in the parent at the end of the program ****/
	close(listenSocketFD); // Close the listening socket
	return 0; 
}

int doEncryptInChild(int cnctFD) {
	// Get the message from the client and display it
	char buffer[256];
	int charsRead, exitSignal = 0;
	memset(buffer, '\0', 256);
	charsRead = recv(cnctFD, buffer, 255, 0); // Read the client's message from the socket
	if (charsRead < 0) error("SERVER: ERROR reading from socket");
	printf("SERVER: I received this from the client: \"%s\"\n", buffer);
	if(strcmp(buffer, "exit") == 0)
		exitSignal = 1;
	// Send a Success message back to the client
	charsRead = send(cnctFD, "I am the server, and I got your message", 39, 0); // Send success back
	if (charsRead < 0) error("SERVER: ERROR writing to socket");
	close(cnctFD); // Close the existing socket which is connected to the client
	return exitSignal;
}