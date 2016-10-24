// compile with gcc –o jamesc2.adventure jamesc2.adventure.c -lpthread
#include <fcntl.h>
#include <stdio.h>	// printing, etc
#include <stdlib.h>	// mkdir, etc.
#include <unistd.h> // for process id
#include <sys/types.h> // for struct stat
#include <sys/stat.h> // for stat()
#include <string.h>  // for memset
#include <time.h>

int getRandom(int min, int max) {
	return (rand() % (max+1-min) + min);
}

int createFiles(char * filepath, char * dirName, char ** rooms, int len){
	int i;
	int file_descriptor;
	int loops = 7;
	char rmName[] = "ROOM NAME: ";
	char cnxn[] = "CONNECTION ";
	char rmType[] = "ROOM TYPE: ";
	char typeTxt[10];
	char prntName[30];
	char prntCnxn[40];
	char prntType[30];

	for (i = 0; i < loops; i++)
	{
		/* open/create files */
		memset(filepath, '\0', len);
		sprintf(filepath, "%s/%s.room", dirName, rooms[i]);
		file_descriptor = open(filepath, O_RDWR | O_CREAT | O_APPEND, 0644);
		if (file_descriptor == -1)
		{
			return 1;
		}

		/* prepare strings */
		memset(typeTxt, '\0', 10);
		if(i == 0)
			sprintf(typeTxt,"START_ROOM");
		else if(i < loops-1)
			sprintf(typeTxt,"MID_ROOM");
		else
			sprintf(typeTxt,"END_ROOM");
		memset(prntName, '\0', 30);
		memset(prntCnxn, '\0', 40);
		memset(prntType, '\0', 30);
		sprintf(prntName, "%s%s\n", rmName, rooms[i]);
		sprintf(prntType, "%s%s\n", rmType, typeTxt);
		// int rmNum = i+1;
		// if(rmNum >= loops)
		// 	rmNum = 0;
		// sprintf(prntCnxn, "%s%d: %s\n", cnxn, 1, rooms[rmNum]);

		/* write to file */
		write(file_descriptor, prntName, strlen(prntName) * sizeof(char));
		// figure connections
		int x;
		int start = getRandom(0,6);
		while(start == i)
			start = getRandom(0,6);
		int numLoops = getRandom(3,6);
		for (x = 0; x < numLoops; x++)
		{
			if(start == i)
				start++;
				if(start >= loops)
					start = 0;
			sprintf(prntCnxn, "%s%d: %s\n", cnxn, 1, rooms[start++]);
			write(file_descriptor, prntCnxn, strlen(prntCnxn) * sizeof(char));
		}
		
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

	if (stat(dirName, &checkfor) == -1) {
	    mkdir(dirName, 0755);
	}

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
	int file_descriptor;
	char filepath[50];
	int file_status = createFiles(filepath, dirName, rooms, 50);
	/* exit if file(s) couldn't be created */
	if(file_status == 1)
		exit(1);

	return 0;
}