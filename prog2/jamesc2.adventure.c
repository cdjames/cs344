/*********************************************************************
** Author: Collin James, CS344
** Date: 10/27/16
** Description: An adventure-like game in C
** compile with gcc jamesc2.adventure.c –o jamesc2.adventure -lpthread
*********************************************************************/

/* includes */
#include <fcntl.h>		// for file manipulation
#include <stdio.h>		// printing, etc
#include <stdlib.h>		// mkdir, etc.
#include <unistd.h> 	// for process id
#include <sys/types.h> 	// for struct stat
#include <sys/stat.h> 	// for stat()
#include <string.h>  	// for memset
#include <time.h>

/* constants */
const int NUM_ROOMS = 7;	
const int RM_NM_RD = 11;	// point in string to start reading (ROOM TYPE: & ROOM NAME: )
const int CXN_NM_RD = 14;	// point in string to start reading (CONNECTION #: )
const int MAXIDX = 9;		// max index in size 10 array
const char * DIR = "jamesc2.adventure"; // beginning of directory name
const char * DELIM = ";";	// delimiter used in various string operations

/* function declarations; functions and descriptions follow main() */
int getRandom(int min, int max);
void clearString(char * theString, int size);
int adjustIdx(int idx, int maxIdx);
int createFiles(char * dirName, char ** rooms, int len, int startPt, int endPt);
int validateInput(char * input, char * rooms);
void getInput(char **retString, char * roomNmOnly, char * allRooms, char * possRooms);
void printAndGetInput(FILE * file, char **nextRoom);
char figureRoomType(FILE * file, signed int seekFrom, unsigned int size);
int doRoomActions(char * dir, char * room, char **nextRoom, char **path);
void doEnding(char * path, int roomCount);


/*********************************************************************
** Description: 
** The main process for the game. Create variables, directory, files,
** then read start room, present, and get user input. Loop until end room
** has been found. Print a message, free memory, and exit 
*********************************************************************/
int main()
{
	/* seed the randomizer */
	srand(time(NULL));
	
	/** variables **/
	int pid = getpid(), 			// process id
		result = -1,				// if 1, game over
		roomCount = 0,				// # rooms visited
		startPt = getRandom(0,9), 	// start is a random room
		file_status;				// hold status of opened file (1 is error)

	char dirName[30];				// hold a directory name
	clearString(dirName, 30);
	sprintf(dirName, "%s.%d", DIR, pid); 	// fill dirName: jamesc.adventure.####
	char * input;	// hold user input; init with malloc, needs to be freed
	char * path;	// hold string of visited rooms; init with malloc, needs to be freed
	path = malloc( sizeof(char) * 200 ); // allocate 200 bytes for path
	
	/* create room/file name array */
	char * rooms[10];
	rooms[0] = "Foyer";
	rooms[1] = "Drawing";
	rooms[2] = "Office";
	rooms[3] = "Living";
	rooms[4] = "Attic";
	rooms[5] = "Pantry";
	rooms[6] = "Closet";
	rooms[7] = "Basement";
	rooms[8] = "Dining";
	rooms[9] = "Bedroom";

	/* create directory, checking for existence first */
	struct stat checkfor;
	if (stat(dirName, &checkfor) == -1) {
		/* create directory with rwx ___ ___ (don't let others cheat!) */
	    mkdir(dirName, 0700);
	}

	/* open/create files */
	file_status = createFiles(dirName, rooms, 50, startPt, startPt+NUM_ROOMS);
	/* exit if file(s) couldn't be created */
	if(file_status == 1)
		exit(1);

	/* read start file and present to user */
	result = doRoomActions(dirName, rooms[startPt], &input, &path);

	/* continue through the remaining rooms */
	while(result != 1){
		/* read file selected by user and present */
		result = doRoomActions(dirName, input, &input, &path);
		/* increment turn counter */
		roomCount++;
	}

	/* if you're out of the loop, that means the last room has been found */
	doEnding(path, roomCount);
	
	/* free variables initialized with malloc */
	free(input);
	free(path);

	/* end program with no errors */
	return 0;
}

/* functions */
/*********************************************************************
** Description: 
** Return a random number
**
** Ex: min = 3, max = 9, rand % (7) = 0...6 + 3 = 3...9
*********************************************************************/
int getRandom(int min, int max) {
	return (rand() % (max+1-min) + min);
}

/*********************************************************************
** Description: 
** Automates memset() because 'memset' isn't very descriptive
*********************************************************************/
void clearString(char * theString, int size) {
	memset(theString, '\0', size);
}

/*********************************************************************
** Description: 
** Given a number, convert to a logical index
** Ex: receives idx 10, maxIdx 9, returns 0 (10-(9+1))
*********************************************************************/
int adjustIdx(int idx, int maxIdx){
	if(idx > maxIdx)
		idx -= (maxIdx+1);
	return idx;
}

/*********************************************************************
** Description: 
** Create a directory, create and write to 7 files representing rooms.
** dirName = directory name
** rooms = array of char * (strings)
** len = length of the filepath string
** startPt = starting index (0-9)
** endPt = ending index (7-16)
*********************************************************************/
int createFiles(char * dirName, char ** rooms, int len, int startPt, int endPt){
	/* variables */
	int file_descriptor,	// hold file status
		maxIdx = MAXIDX,	
		i,
		curIdx;				// hold logical index for rooms array
	/* strings */
	char rmName[] = "ROOM NAME: ",
	 	cnxn[] = "CONNECTION ",
	 	rmType[] = "ROOM TYPE: ",
	 	filepath[50], 		// hold a file path
	 	typeTxt[11],		// hold type of room (MID_ROOM, etc.)
	 	prntName[30],		// hold rmName[] and name of room
	 	prntCnxn[40],		// hold cnxn[] and name of connected room
	 	prntType[30];		// hold rmType[] and typeTxt[]

	/* create a file for 7 rooms from startPt to endPt */
	for (i = startPt; i < endPt; i++)
	{
		/* get logical index (0-9) */
		curIdx = adjustIdx(i, maxIdx);

		/* make a filepath */
		clearString(filepath, len);
		sprintf(filepath, "%s/%s.room", dirName, rooms[curIdx]);
		
		/* open/create file (rw_r__r__) */
		file_descriptor = open(filepath, O_RDWR | O_CREAT | O_APPEND, 0644);
		if (file_descriptor == -1) // get out if can't create file
			return 1;

		/* prepare room name string */
		clearString(prntName, 30);
		sprintf(prntName, "%s%s\n", rmName, rooms[curIdx]);
		
		/* write name line to file */
		write(file_descriptor, prntName, strlen(prntName) * sizeof(char));

		/* figure connections */
		int x; 	// for your loop
		signed int magicIdx = -1, 	// hold index of last room
				   getIdx = getRandom(startPt,endPt), // start at a random room for connections
				   prevGetIdx;		// save var for getIdx
		
		/* after start, must have a connection to previous room */
		if(i != startPt)
			magicIdx = (i-1);

		/* make sure you don't link a room to itself*/
		while(getIdx == i || getIdx == magicIdx)
			getIdx = getRandom(startPt,endPt);

		/* for each room, make a random # of connections */
		int incDec = getRandom(1,2),   // for randomizing selection order of connections
			numLoops = getRandom(3,6), // create a random number of connections
			magicX = getRandom(0,(numLoops-1)); // insert your magicIdx at this point in the file
		for (x = 0; x < numLoops; x++)
		{
			/* check for magic index after start room */
			if(x == magicX && i != startPt){
				prevGetIdx = getIdx; // save your getIdx to resume later
				getIdx = magicIdx;
			}
			/* if not magic index */
			else 
			{
				if(getIdx == i || getIdx == magicIdx)		// don't want a room linked to itself or magic index twice!
					getIdx++;
				/* check for going over endpoint */
				if(getIdx >= endPt)		// make sure we don't include rooms that don't matter! (endPt-1 adjusted is last index in group of rooms)
				{
					getIdx = startPt;
					if(getIdx == i || getIdx == magicIdx)
						getIdx++;
				}

				prevGetIdx = getIdx;
			}
			/* clear the string out, make a line, and write out */
			clearString(prntCnxn, 40);
			sprintf(prntCnxn, "%s%d: %s\n", cnxn, x+1, rooms[adjustIdx(getIdx,maxIdx)]);
			write(file_descriptor, prntCnxn, strlen(prntCnxn) * sizeof(char));
			
			/* start next iteration at next getIdx */
			getIdx = prevGetIdx;
			getIdx++;
		}

		/* prepare room type string */
		clearString(typeTxt, 11);
		if(i == startPt)
			sprintf(typeTxt,"START_ROOM");
		else if(i < endPt-1)
			sprintf(typeTxt,"MID_ROOM");
		else
			sprintf(typeTxt,"END_ROOM");
		clearString(prntType, 30);
		sprintf(prntType, "%s%s\n", rmType, typeTxt);
		/* finish writing type line and close */
		write(file_descriptor, prntType, strlen(prntType) * sizeof(char));
		close(file_descriptor);
	}
	/* return with no errors */
	return 0;
}

/*********************************************************************
** Description: 
** Validate user's room entries and return 0 if good, print error and 1 if bad
** input = a room ("Dining")
** rooms = string of room names delimited by ; ("Dining;Foyer;Attic;")
*********************************************************************/
int validateInput(char * input, char * rooms) {
	int i;
	char tmp[9];
	/* we want to use 'input' elsewhere, so preserve it */
	strcpy(tmp, input);
	/* search rooms for 'roomname;' and exit successfully if found */
	if(strstr(rooms, strcat(tmp, DELIM)) != NULL)
		return 0;
	/* string not found */
	printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
	return 1;
}

/*********************************************************************
** Description: 
** Print location, etc., and receive a string, max size 8 (Basement)
** retString = pointer to char * (this is the "return" value)
** roomNmOnly = name of a room ("Attic")
** allRooms = string of all connected rooms ("Foyer, Dining, Bedroom.")
*********************************************************************/
void getInput(char **retString, char * roomNmOnly, char * allRooms, char * possRooms){
	/* allocate your string w/ room for \0 */
	*retString = malloc( sizeof(char) * 9 );
	/* print and accept input while the string is not a possible connection */
	do {
		printf("CURRENT LOCATION: %s\n", roomNmOnly);
		printf("POSSIBLE CONNECTIONS: %s\n", allRooms);
		printf("WHERE TO? >");
		scanf("%8s", *retString);
	} while (validateInput(*retString, possRooms) != 0);
}

/*********************************************************************
** Description: Read a file from beginning to last connection, store values
** in variables, and print via getInput()
** file = FILE pointer to hold file status
** nextRoom = pointer to a char *, will be sent to getInput() and "returned" with name of next room
*********************************************************************/
void printAndGetInput(FILE * file, char **nextRoom){
	char allRooms[100],			// 
	 	buffer[100],
	 	roomNameOnly[10],
	 	roomOnly[11],
	 	possRooms[100];
	clearString(allRooms, 100);
	clearString(possRooms, 100);

	// int i=0;
	/* back to beginning and process rest of file */
	rewind(file);
	while( !feof(file) ){
		/* get a line from the file */
		clearString(buffer, 100);
		fgets(buffer, 100, file);

		/* it's the first room (R)OOM NAME/(R)OOM TYPE*/
		if(buffer[0] == 'R'){
			/* process room name - ROOM (N)AME */
			if(buffer[5] == 'N'){
				/* process name by copying info in buffer AFTER location RM_NM_RD */
				/* only copy max 8 characters */
				clearString(roomNameOnly, 10);
				strncpy(roomNameOnly, buffer+RM_NM_RD, 8);
				/* manually get rid of \n at end of strings */
				if(roomNameOnly[strlen(roomNameOnly)-1] == '\n'){
					roomNameOnly[strlen(roomNameOnly)-1] = '\0';
				}
			} // note that ROOM TYPE lines are not processed!
		} 
		else {
			/* process connection room name, similar to above */
			clearString(roomOnly, 11);
			strncpy(roomOnly, buffer+CXN_NM_RD, 9);
			roomOnly[strlen(roomOnly)-1] = '\0'; // lose \n
			/* append connection room name to possRooms string */
			strcat(possRooms, roomOnly);
			/* append a delimiter */ 
			strcat(possRooms, DELIM);
			/* append a comma and space after room name*/
			strcat(roomOnly, ", ");
			/* append room name to string of all rooms (Foyer, Attic, )*/
			strncat(allRooms, roomOnly, strlen(roomOnly));
		}
		// i++;
	}
	/* change ',' into '.' */
	allRooms[strlen(allRooms)-4] = '.';
	/* get rid of \n at end of string */
	allRooms[strlen(allRooms)-3] = '\0';

	/* get input from user */
	getInput(nextRoom, roomNameOnly, allRooms, possRooms);
}

/*********************************************************************
** Description: 
** Read the last line of a file and return the first letter of 
** (S)TART_ROOM, (M)ID_ROOM, (E)ND_ROOM, thus determmining room type
** file = open FILE pointer
** seekFrom = negative value greater (abs value) than length of last line
** size = length of string to hold line
*********************************************************************/
char figureRoomType(FILE * file, signed int seekFrom, unsigned int size){
	/* prepare your buffer*/
	char buffer[size];
	clearString(buffer, size);
	/* go back seekFrom places from end of file */
	fseek(file, seekFrom, SEEK_END);
	/* get a line */
	fgets(buffer, size, file);
	/* if the line doesn't begin with 'R', get next line which should begin w/ 'R' */
	while(buffer[0] != 'R'){
		clearString(buffer, size);
		fgets(buffer, size, file);
	}
	/* return S, M, or E */
	return buffer[RM_NM_RD];
}

/*********************************************************************
** Description: 
** Open a file, get room type w/ figureRoomType(), and proceed accordingly,
** either going to ending actions or getting inputs
** dir = directory name
** room = name of room file to open
** nextRoom = pointer to char *, passed along to printAndGetInput()
** path = pointer to char *, append visited rooms to this
*********************************************************************/
int doRoomActions(char * dir, char * room, char **nextRoom, char **path){
	char filepath[50];
	clearString(filepath, 50);
	sprintf(filepath, "%s/%s.room", dir, room);

	FILE * file;
	file = fopen(filepath,"r");
	
	if (file != NULL){

		char buffer[100];
		char letter;
		/* figure room type */
		letter = figureRoomType(file, -22, 100);

		/* check for S, M, E (start, mid, end)*/
		if(letter == 'E'){
			/* close and return 1 to do game over stuff */
			fclose(file);
			return 1;
		}
		else {
			/* print rooms and get input */
			printAndGetInput(file, nextRoom);
			/* append the next room plus delimiter to the path for printing at the end of game */
			/* i.e. "Foyer;Attic;Basement;" */
			strcat(*path, *nextRoom);
			strcat(*path, DELIM);
		}
	}
	/* close the file and return 0 to show game should continue */
	fclose(file);
	printf("\n");
	return 0;
}

/*********************************************************************
** Description: 
** Print out a congratulatory message and visited rooms
** path = list of visited rooms delimited by ";"
** roomCount = number of rooms visited
*********************************************************************/
void doEnding(char * path, int roomCount){
	/* print message for user */
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", roomCount);
	/* print visited rooms */
	/* tokenize path string on ';' and print out each token (room) in turn */
	char * subString;
	subString = strtok(path,DELIM);
	/* print out substring and get next one, if none strtok returns NULL */
	while (subString != NULL)
	{
		printf("%s\n", subString);
		subString = strtok(NULL, DELIM);
	}
}	