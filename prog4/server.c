#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>		// for file manipulation
#include <sys/wait.h> 	// for wait/waitpid
#include "newtypes.h"

const int maxConnections = 5;
void error(const char *msg) { 
	perror(msg); 
	exit(1); 
} // Error function used for reporting issues

struct Pidkeeper doEncryptInChild(int cnctFD) ;

int setUpSocket(struct sockaddr_in * serverAddress, int maxConn);

int main(int argc, char *argv[])
{
	/* Check usage & args */
	if (argc < 2) { 
		fprintf(stderr,"SERVER: USAGE: %s port\n", argv[0]); 
		exit(1); 
	}

	/* socket variables */
	int listenSocketFD, 
		establishedConnectionFD, 
		portNumber, 
		charsRead;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in serverAddress, clientAddress;
	
	/* keeping track of pids and connections */
	int exitSignal = 0,
		pid = -1, // for storing the child pid
		numConnections = 0, // store number of connections (5 max)
		status,
		wpid;
	struct Pidkeeper thePK = new_PK(pid, exitSignal);

	/* select variables */
	fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax; // for max fd in select
    
    /* setup select sets by clearing */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

	/*Set up the address struct for this process (the server)*/
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket and start listen()
	listenSocketFD = setUpSocket(&serverAddress, maxConnections);

	/* add listenSocketFD to master set (for select); set the max fd to be the listener */
	FD_SET(listenSocketFD, &master);
	fdmax = listenSocketFD;

	/* loop until exitSignal is received (change to endless loop in final code) 
		In each loop, first collect finished processes, then select an active connection, 
		then process connections */
	int printerr = 1;
	while(exitSignal == 0 && pid != 0) { // do until message is "exit"
		/* collect finished processes */
		do {
			wpid = waitpid(-1, &status, WNOHANG);
			// printf("wpid=%d\n", wpid);
		} while (wpid > 0);

		/* copy the master set into a temporary set for this iteration */
		read_fds = master;

		if (printerr)
			printf("number of connections = %d\n", numConnections);

		/* do your select and make sure there are no errors */
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            error("SERVER: select failed");
            // exit(4);
        }
        else { // no errors, look for new connections or data
        	
        	// printf("looking for connections\n");
        	int i;
        	for (i = 0; i <= fdmax; i++)
        	{
        		if(FD_ISSET(i, &read_fds)) { // we have a connection/data to read
        			// printf("new connection\n");
        			/* there is a new listener */
        			if(i == listenSocketFD){
        				sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect

        				/* check that you still have available connections and perform handshake */
						if(numConnections < maxConnections) {
							// Accept a connection, blocking if one is not available until one connects
							establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
							if (establishedConnectionFD < 0) 
								error("SERVER: ERROR on accept");
							/* if no error, add new connection to your set */
							else {
								numConnections += 1;
								// printf("adding new connection to master\n");
								/* add new connection to master set */
								FD_SET(establishedConnectionFD, &master);

								/* make a new max if necessary */
								if(establishedConnectionFD > fdmax)
									fdmax = establishedConnectionFD;

								/* don't want to accept any more connections if reached max */
								if(numConnections >= maxConnections){
									close(listenSocketFD); // close to reject new connections
									FD_CLR(i, &master); // remove the listening socket from the set; re-add after new client data is read
								}
							}
	        			}
        			}
        			/* we have something to read from a client */
        			else {
	        			// printf("getting data\n");
	        			/* fork a process with doEncrypt... encrypt here */
						thePK = doEncryptInChild(i);
						pid = thePK.pid;
						exitSignal = thePK.status;

						/* remove the connection from the master set and decrement connections */
						FD_CLR(i, &master);
						numConnections -= 1;

						/* check for need to restart socket (don't do in child [pid==0]) */
						if(numConnections < maxConnections && pid != 0){
							/* check whether socket needs to be reopened (don't do in child [pid==0]) */
							if(!FD_ISSET(listenSocketFD, &master) && pid != 0) {
								printf("SERVER: resetting the connection\n");
								/* re-open the socket */
								listenSocketFD = setUpSocket(&serverAddress, maxConnections);
		        				/* add it back to the set and make a new max if necessary */
								FD_SET(listenSocketFD, &master);
								if(listenSocketFD > fdmax)
									fdmax = listenSocketFD;
							}
						}
	        		}
        		} // END FD_ISSET if	
			} // END for loop
		} // END select else
	} // END main while loop

/**** this is done in the parent at the end of the program ****/
	if(pid != 0) {
		close(listenSocketFD); // Close the listening socket
		/* collect finished processes */
		do {
			wpid = waitpid(-1, &status, WNOHANG);
			// printf("wpid=%d\n", wpid);
		} while (wpid > 0);
	}

	return 0; 
}

int setUpSocket(struct sockaddr_in * serverAddress, int maxConn){
	// Set up the socket
	int yes = 1;
	int listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) 
		error("SERVER: ERROR opening socket");

	// reuse previously used ports before they are released by OS (from beej.us) -- doesn't work
    setsockopt(listenSocketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)serverAddress, sizeof(*serverAddress)) < 0) // Connect socket to port
		error("SERVER: ERROR on binding");
	
	int listening;
	listening = listen(listenSocketFD, maxConn); // Flip the socket on - it can now receive up to 5 connections
	if(listening < 0)
		error("SERVER: failed to listen");

	return listenSocketFD;
}

struct Pidkeeper doEncryptInChild(int cnctFD) {
	// Get the message from the client and display it
	/* set up pipe for communicating failures with parent */
	int r, 
		pipeFDs[2],
		exitSignal = 0, // send this data to parent
		stat_msg,	// receive data here
		pipe_status; // save status of pipe
	long int msg_size = sizeof(send);

	if( (pipe_status = pipe(pipeFDs)) == -1)
		perror("failed to set up pipe");

	/* fork a process */
	int pid = fork(),
		status;
	printf("connection ID is %d\n", cnctFD);
	/* in child, do the stuff */
	if(pid == 0) {
		if(pipe_status != -1){
			close(pipeFDs[0]); // close input pipe
			fcntl(pipeFDs[1], F_SETFD, FD_CLOEXEC); // close output pipe on exec
		}

		char buffer[256];
		int charsRead;
		memset(buffer, '\0', 256);
		charsRead = recv(cnctFD, buffer, 255, 0); // Read the client's message from the socket
		if (charsRead < 0) 
			error("SERVER: ERROR reading from socket");
		printf("SERVER: I received this from the client: \"%s\"\n", buffer);
		if(strcmp(buffer, "exit") == 0)
			exitSignal = 1;
		// Send a Success message back to the client
		charsRead = send(cnctFD, "I am the server, and I got your message", 39, 0); // Send success back
		if (charsRead < 0) error("SERVER: ERROR writing to socket");
		close(cnctFD); // Close the existing socket which is connected to the client

		/* send error status message to parent, i.e. 1 (sending int disguised as void *) 
			you will never get to this point if exec occurs, and output pipe will be closed
			on exec, causing read to receive 0 */
		if(pipe_status != -1)	
			write(pipeFDs[1], &exitSignal, msg_size);
	} 
	/* let the parent wait to collect, but don't hang */
	else if (pid > 0) {
		if(pipe_status != -1)
			close(pipeFDs[1]); // close output pipe

		pid_t exitpid;
		exitpid = waitpid(pid, &status, WNOHANG);

		if(pipe_status != -1){
			r = read(pipeFDs[0], &stat_msg, msg_size);
			if (r > 0)
				exitSignal = stat_msg;
		}
	}

	return new_PK(pid, exitSignal);
}