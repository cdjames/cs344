// compile with gcc –o jamesc2.adventure jamesc2.adventure.c -lpthread
#include <fcntl.h>		// for file manipulation
#include <stdio.h>		// printing, etc
#include <stdlib.h>		// mkdir, etc.
#include <unistd.h> 	// for process id
#include <sys/types.h> 	// for struct stat
#include <sys/stat.h> 	// for stat()
#include <string.h>  	// for memset
#include <time.h>

const int NUM_ROOMS = 7;
const int RM_NM_RD = 11;
const int CXN_NM_RD = 14;
const int MAXIDX = 9;


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

// void copySubString(char * string, char * retString, int start, char until) {
// 	int i = start;

// 	while(string[i] != until){
// 		retString[i] = string[i];
// 		// printf("%c, ", retString[i]);
// 		i++;
// 	}
// }

int validateInput(char * input, char * rooms) {
	int i;
	char tmp[9];
	strcpy(tmp, input);
	if(strstr(rooms, strcat(tmp, ";")) != NULL)
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
	/* back to beginning and process rest of file */
	// *nextRoom = malloc( sizeof(char) * 9 );
	rewind(file);
	char allRooms[100];
	char buffer[100];
	clearString(allRooms, 100);
	char roomNameOnly[10];
	char roomOnly[11];

	int i=0;
	char possRooms[100];
	clearString(possRooms, 100);
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
			strcat(possRooms, ";");
			strcat(roomOnly, ", ");
			strncat(allRooms, roomOnly, strlen(roomOnly));
		}
		i++;
	}
	allRooms[strlen(allRooms)-4] = '.';
	allRooms[strlen(allRooms)-3] = '\0';

	/* get input from user */
	getInput(nextRoom, roomNameOnly, allRooms, possRooms);

	/* add next room to path list */
	// strcat(path, *nextRoom);
	// strcat(path, ";");
}

int doRoomActions(char * dir, char * room, char **nextRoom, char **path, int visitedStFlag){
	// path = malloc( sizeof(char) * 200 );
	char filepath[50];
	clearString(filepath, 50);
	sprintf(filepath, "%s/%s.room", dir, room);
	FILE * file;
	file = fopen(filepath,"r");
	// puts(filepath);
	if (file != NULL)
	{
		
	// 	// struct stat statbuf;
	// 	// int r;
	// 	// r = stat(filepath, &statbuf);
	// 	// printf("The logical size of %s is %ld bytes\n", filepath, statbuf.st_size);

		char buffer[100];
		char letter;
		/* figure room type */
		letter = figureRoomType(file, -22, 100);

	// 	/* check for S, M, E (start, mid, end)*/
		// if(buffer[RM_NM_RD] == 'E')
			// printf("%c\n", letter);
		if(letter == 'E'){
			// puts("end");
			fclose(file);
			return 1;
			/* do game over stuff */
		}
		// else(letter == 'M') {
		else {
			// puts("start or mid");
			/* increment turn counter */
			printAndGetInput(file, nextRoom);
			/* append the next room to the path */
			// printf("the next room is %s\n", *nextRoom);
			strcat(*path, *nextRoom);
			strcat(*path, ";");
		}
		// else {
		// 	puts("start");
		// 	/* append first room to the path */
		// 	if(visitedStFlag != 1){
		// 		strcat(*path, room);
		// 		strcat(*path, ";");
		// 	}
		// 	printAndGetInput(file, nextRoom);
		// 	strcat(*path, *nextRoom);
		// 	strcat(*path, ";");
		// }
		

	// 	/* back to beginning and process rest of file */

	// 	/* room is validated, repeat cycle */
	}
	// fclose(file);
	printf("\n");
	return 0;
}

int createFiles(char * filepath, char * dirName, char ** rooms, int len, int startPt, int endPt){
	int file_descriptor,
		maxIdx = 9,
		i,
		curIdx;
	char rmName[] = "ROOM NAME: ",
	 	cnxn[] = "CONNECTION ",
	 	rmType[] = "ROOM TYPE: ",
	 	typeTxt[10],
	 	prntName[30],
	 	prntCnxn[40],
	 	prntType[30];

	
	for (i = startPt; i < endPt; i++)
	{
		curIdx = adjustIdx(i, maxIdx);

		/* open/create files */
		clearString(filepath, len);
		sprintf(filepath, "%s/%s.room", dirName, rooms[curIdx]);
		file_descriptor = open(filepath, O_RDWR | O_CREAT | O_APPEND, 0644);
		if (file_descriptor == -1)
		{
			return 1;
		}

		/* prepare strings */
		clearString(typeTxt, 10);
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

		/* write to file */
		write(file_descriptor, prntName, strlen(prntName) * sizeof(char));
		// figure connections
		int x,
			getIdx = getRandom(startPt,endPt);
		while(getIdx == i) {
			getIdx = getRandom(startPt,endPt);
			printf("getIdx(%d) equaled i(%d)\n", i, getIdx);
		}

		int numLoops = getRandom(3,6);
		for (x = 0; x < numLoops; x++)
		{
			if(getIdx == i)			// don't want a room linked to itself!
				getIdx += 1;
			if(getIdx >= endPt)		// make sure we don't include rooms that don't matter! (endPt-1 adjusted is last index in group of rooms)
				getIdx = startPt;
				if(getIdx == i)			// don't want a room linked to itself! (one more check)
					getIdx += 1;
			printf("i=%d, getidx=%d\n", i, getIdx);
			clearString(prntCnxn, 40);
			sprintf(prntCnxn, "%s%d: %s\n", cnxn, 1, rooms[adjustIdx(getIdx,maxIdx)]);
			write(file_descriptor, prntCnxn, strlen(prntCnxn) * sizeof(char));
			getIdx++;
		}
		/* finish writing */
		write(file_descriptor, prntType, strlen(prntType) * sizeof(char));
		close(file_descriptor);

	}
	
	return 0;
}


int main(int argc, char const *argv[])
{
	srand(time(NULL));
	/* create directory name */
	char dirName[30];
	int pid = getpid();
	memset(dirName, '\0', 30);
	sprintf(dirName, "jamesc2.adventure.%d", pid);
	char * input;
	char * path;
	path = malloc( sizeof(char) * 200 );
	FILE * pFile;

	/* create directory, checking for existence first */
	struct stat checkfor = {0};

	// if (stat(dirName, &checkfor) == -1) {
	//     mkdir(dirName, 0755);
	// }

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
	// char possRooms[100];
	// char path[200];
	char filepath[50];
	int file_descriptor,
		startPt = getRandom(0,9),
		file_status;
	// file_status = createFiles(filepath, dirName, rooms, 50, startPt, startPt+NUM_ROOMS);
	// /* exit if file(s) couldn't be created */
	// if(file_status == 1)
	// 	exit(1);

	/* read start file and present to user */
	/* open/create files */

	startPt = 4; // testing only
	int endPt = startPt + NUM_ROOMS-1;
	char testDir[] = "test";
	int result = -1;
	int roomCount = 0;
	result = doRoomActions(testDir, rooms[startPt], &input, &path, 0);
	while(result != 1){
		result = doRoomActions(testDir, input, &input, &path, 1);
		roomCount++;
	}
	
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS.\n", roomCount);
	printf("YOUR PATH TO VICTORY WAS:\n");
	/* print visited rooms */
	printf("%s\n", path);
	
	free(input);

	return 0;
}

