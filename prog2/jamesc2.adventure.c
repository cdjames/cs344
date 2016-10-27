
// compile with gcc –o jamesc2.adventure jamesc2.adventure.c -lpthread
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
const int RM_NM_RD = 11;
const int CXN_NM_RD = 14;
const int MAXIDX = 9;
const char * DIR = "jamesc2.adventure";
const char * DELIM = ";";


int getRandom(int min, int max) {
	return (rand() % (max+1-min) + min);
}

void clearString(char * theString, int size) {
	memset(theString, '\0', size);
}

int adjustIdx(int start, int maxIdx){
	if(start > maxIdx)
		start = start - (maxIdx+1);
	return start;
}

int createFiles(char * dirName, char ** rooms, int len, int startPt, int endPt){
	int file_descriptor,
		maxIdx = 9,
		i,
		curIdx;
	char rmName[] = "ROOM NAME: ",
	 	cnxn[] = "CONNECTION ",
	 	rmType[] = "ROOM TYPE: ",
	 	filepath[50],
	 	typeTxt[11],
	 	prntName[30],
	 	prntCnxn[40],
	 	prntType[30];

	
	for (i = startPt; i < endPt; i++)
	{
		curIdx = adjustIdx(i, maxIdx);

		/* open/create files */
		clearString(filepath, len);
		sprintf(filepath, "%s/%s.room", dirName, rooms[curIdx]);
		// puts(filepath);
		file_descriptor = open(filepath, O_RDWR | O_CREAT | O_APPEND, 0644);
		if (file_descriptor == -1)
		{
			return 1;
		}

		/* prepare strings */
		clearString(typeTxt, 11);
		if(i == startPt)
			sprintf(typeTxt,"START_ROOM");
		else if(i < endPt-1)
			sprintf(typeTxt,"MID_ROOM");
		else
			sprintf(typeTxt,"END_ROOM");

		clearString(prntName, 30);
		clearString(prntCnxn, 40);
		clearString(prntType, 30);
		sprintf(prntName, "%s%s\n", rmName, rooms[curIdx]);
		sprintf(prntType, "%s%s\n", rmType, typeTxt);

		/* write name line to file */
		write(file_descriptor, prntName, strlen(prntName) * sizeof(char));

		/* figure connections */
		int x, 	// for your loop
			getIdx = getRandom(startPt,endPt); // get a random number
		/* make sure you don't link a room to itself*/
		while(getIdx == i) {
			getIdx = getRandom(startPt,endPt);
		}

		int numLoops = getRandom(3,6); // create a random number of connections
		for (x = 0; x < numLoops; x++)
		{
			if(getIdx == i)			// don't want a room linked to itself!
				getIdx += 1;
			if(getIdx >= endPt)		// make sure we don't include rooms that don't matter! (endPt-1 adjusted is last index in group of rooms)
				getIdx = startPt;
				if(getIdx == i)			// don't want a room linked to itself! (one more check)
					getIdx += 1;
			/* clear the string out, make a line, and write out */
			clearString(prntCnxn, 40);
			sprintf(prntCnxn, "%s%d: %s\n", cnxn, 1, rooms[adjustIdx(getIdx,maxIdx)]);
			write(file_descriptor, prntCnxn, strlen(prntCnxn) * sizeof(char));
			getIdx++;
		}
		/* finish writing type line and close */
		write(file_descriptor, prntType, strlen(prntType) * sizeof(char));
		close(file_descriptor);
	}
	
	return 0;
}

int validateInput(char * input, char * rooms) {
	int i;
	char tmp[9];
	/* we want input elsewhere, so preserve it */
	strcpy(tmp, input);
	if(strstr(rooms, strcat(tmp, DELIM)) != NULL)
		return 0;
	
	printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
	return 1;
}

char figureRoomType(FILE * file, signed int seekFrom, unsigned int size){
	/* figure process type */
	char buffer[size];
	clearString(buffer, size);

	fseek(file, seekFrom, SEEK_END);
	fgets (buffer, size, file);
	while(buffer[0] != 'R'){
		clearString(buffer, size);
		fgets (buffer, size, file);
	}

	return buffer[RM_NM_RD];
}

void getInput(char **retString, char * roomNmOnly, char * allRooms, char * possRooms){
	// printf("retString=%p\n", retString);
	*retString = malloc( sizeof(char) * 9 );
	int done = 0;
	do {
		printf("CURRENT LOCATION: %s\n", roomNmOnly);
		printf("POSSIBLE CONNECTIONS: %s\n", allRooms);
		printf("WHERE TO? >");
		scanf("%8s", *retString);
		// printf("%s\n", *retString);
	} while (validateInput(*retString, possRooms) != 0);
}

void printAndGetInput(FILE * file, char **nextRoom){
	char allRooms[100],
	 	buffer[100],
	 	roomNameOnly[10],
	 	roomOnly[11],
	 	possRooms[100];
	clearString(allRooms, 100);
	clearString(possRooms, 100);

	// int i=0;
	/* back to beginning and process rest of file */
	rewind(file);
	while(!feof(file)){
		clearString(buffer, 100);
		fgets (buffer, 100 ,file);
			/* it's the first room */
		if(buffer[0] == 'R'){
			/* process rooms*/
			if(buffer[5] == 'N'){
				/* process name */
				clearString(roomNameOnly, 10);
				strncpy(roomNameOnly, buffer+RM_NM_RD, 8);
				if(roomNameOnly[strlen(roomNameOnly)-1] == '\n'){
					/* get rid of \n at end of string */
					roomNameOnly[strlen(roomNameOnly)-1] = '\0';
				}
			}
		} 
		else {
			/* process connection*/
			clearString(roomOnly, 11);
			strncpy(roomOnly, buffer+CXN_NM_RD, 9);
			roomOnly[strlen(roomOnly)-1] = '\0';
			strcat(possRooms, roomOnly);
			strcat(possRooms, DELIM);
			strcat(roomOnly, ", ");
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
			fclose(file);
			return 1;
			/* do game over stuff */
		}
		else {

			printAndGetInput(file, nextRoom);
			/* append the next room to the path */
			strcat(*path, *nextRoom);
			strcat(*path, DELIM);
		}
	}
	fclose(file);
	printf("\n");
	return 0;
}

void doEnding(char * path, int roomCount){
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", roomCount);
	/* print visited rooms */
	/* tokenize path string and print out each token in turn */
	char * subString;
	subString = strtok(path,DELIM);
	while (subString != NULL)
	{
		printf("%s\n", subString);
		subString = strtok(NULL, DELIM);
	}
}	

int main(int argc, char const *argv[])
{
	/* seed the randomizer */
	srand(time(NULL));
	
	/** variables **/
	int pid = getpid(),
		result = -1,
		roomCount = 0,
		file_descriptor,
		startPt = getRandom(0,9),
		file_status;

	char dirName[30];
	clearString(dirName, 30);
	sprintf(dirName, "%s.%d", DIR, pid); 	// fill dirName: jamesc.adventure.####
	char * input;	// init with malloc, needs to be freed
	char * path;	// init with malloc, needs to be freed
	path = malloc( sizeof(char) * 200 );
	
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

