/*********************************************************************
** Author: Collin James
** Date: 11/2/16
** Description: Logic for smallsh program
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
#include "newtypes.h"
#include "dynArray.h"

void getInput(char **retString);

/* Global constants */
const int MAX_CMD_SIZE = 2048;

int main(int argc, char const *argv[])
{
	/* variables */
	struct Statuskeeper * theSK = new_sk(0, -1);
	enum Status theStatus = CONTINUE;
	char * input;
	// *input = malloc( sizeof(char) * (2048) );

	/* main logic */
	getInput(&input);
	fputs(input, stdout);
	free_sk(theSK);
	return 0;
}

// /*********************************************************************
// ** Description: 
// ** Validate user's room entries and return 0 if good, print error and 1 if bad
// ** input = a room ("Dining")
// ** rooms = string of room names delimited by ; ("Dining;Foyer;Attic;")
// *********************************************************************/
// int validateInput(char * input, char * rooms) {
// 	int i;
// 	char tmp[9];
// 	/* we want to use 'input' elsewhere, so preserve it */
// 	strcpy(tmp, input);
// 	/* search rooms for 'roomname;' and exit successfully if found */
// 	if(strstr(rooms, strcat(tmp, DELIM)) != NULL)
// 		return 0;
// 	/* string not found */
// 	printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
// 	return 1;
// }

/********************************************************************
** Description: 
** Receive a command string, max size 2048 (Basement)
** retString = pointer to char * (this is the "return" value)
** roomNmOnly = name of a room ("Attic")
** allRooms = string of all connected rooms ("Foyer, Dining, Bedroom.")
********************************************************************/
void getInput(char **retString){
	/* allocate your string w/ room for \0 */
	*retString = malloc( sizeof(char) * (MAX_CMD_SIZE + 1) );
	/* print and accept input while the string is not a possible connection */
	do {
		fputs(": ", stdout);
		fflush(stdout); // flush the print buffer
		fgets(*retString, MAX_CMD_SIZE + 1, stdin); 
	// } while (validateInput(*retString) != 0);
	} while (0 != 0);
}