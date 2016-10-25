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

	/* create files */
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
	char testDir[] = "test";
	clearString(filepath, 50);
	sprintf(filepath, "%s/%s.room", testDir, rooms[startPt]);
	file_descriptor = open(filepath, O_RDONLY);
	if (file_descriptor == -1)
	{
		return 1;
	}

	struct stat statbuf;
	int r;
	r = stat(filepath, &statbuf);
	printf("The logical size of %s is %ld bytes\n", filepath, statbuf.st_size);
	char buffer[200];
	clearString(buffer, 100);
	// lseek(file_descriptor, 0, SEEK_SET); // Reset the file pointer to the beginning of the file
	// int nread = read(file_descriptor, buffer, statbuf.st_size);
	// printf("%d\n",nread);
	// printf("%s\n",buffer);
	// close(file_descriptor);

	FILE * pFile;
	pFile = fopen(filepath,"r");
	if (pFile != NULL)
	{
		fgets (buffer, 100,pFile);
		puts(buffer);
	}
	fclose(pFile);

	// clearString(buffer, 100);
	// lseek(file_descriptor, 1+CXN_NM_RD, SEEK_CUR); // Reset the file pointer to the beginning of the file
	// nread = read(file_descriptor, buffer, sizeof(buffer)-1);
	// printf("%d\n",nread);
	// printf("%s\n",buffer);
	return 0;
}