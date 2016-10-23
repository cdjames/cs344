// compile with gcc –o jamesc2.adventure jamesc2.adventure.c -lpthread
#include <fcntl.h>
#include <stdio.h>	// printing, etc
#include <stdlib.h>	// mkdir, etc.
#include <unistd.h> // for process id
#include <sys/types.h> // for struct stat
#include <sys/stat.h> // for stat()
#include <string.h>  // for memset

int createFiles(char * filepath, char * dirName, char ** rooms, int len){
	int i;
	for (i = 0; i < 7; i++)
	{
		memset(filepath, '\0', len);
		sprintf(filepath, "%s/%s.room", dirName, rooms[i]);
		int file_descriptor = open(filepath, O_RDWR | O_CREAT | O_APPEND, 0644);
		write(file_descriptor, rooms[i], strlen(rooms[i]) * sizeof(char));
		close(file_descriptor);
	}
}

int main(int argc, char const *argv[])
{
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
	createFiles(filepath, dirName, rooms, 50);

	

	return 0;
}