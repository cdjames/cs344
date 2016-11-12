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
void clearString(char * theString, int size);
void removeLineEnding(char ** input);
void printOut(char * outString, int newln);

/* Global constants */
const int MAX_CMD_SIZE = 2048;

int main(int argc, char const *argv[])
{
	/* variables */
	struct Statuskeeper * theSK = new_sk(0, -1);
	enum Status theStatus = CONTINUE;
	char * input;
	input = NULL; // create null pointer
	char exitSt[] = "exit";
	// *input = malloc( sizeof(char) * (2048) );

	/* main logic */
	while(theStatus != EXIT){
		getInput(&input); // validates input and puts in input variable
		printOut(input, 1); // print with a line ending
		/* look for exit command */
		if(strcmp(input, exitSt) == 0)
			theStatus = EXIT;
	}
	free_sk(theSK);
	free(input);
	return 0;
}

void removeLineEnding(char ** input){
	char tmp[MAX_CMD_SIZE+1];
	strcpy(tmp, *input);
	int le = strlen(tmp)-1;
	if(tmp[le] == '\n'){
		tmp[le] = '\0';
		strcpy(*input, tmp);
	}
}

/*********************************************************************
** Description: 
** Validate user's room entries and return 0 if a possible command, 
** 1 if a comment or blank line
*********************************************************************/
int validateInput(char **input) {
	// int i = 0;
	/* if time, implement input validation*/
	// while(**input != '\0'){
	// }

	/* if string contains no info, return 1 */
	if(strlen(*input) == 1){ // i.e. just \0
		return 1;
	}
	/* if string begins with #, print string and return 1 */
	if(**input == '#'){
		printOut(*input, 0);
		return 1;
	}

	return 0;
}

/********************************************************************
** Description: 
** Receive a command string, max size 2048 (Basement)
** retString = pointer to char * (this is the "return" value)
** roomNmOnly = name of a room ("Attic")
** allRooms = string of all connected rooms ("Foyer, Dining, Bedroom.")
********************************************************************/
void getInput(char **retString){
	/* allocate your string w/ room for \0 */
	// if(retString == NULL)
	*retString = malloc( sizeof(char) * (MAX_CMD_SIZE + 1) );
	// clearString(*retString, MAX_CMD_SIZE+1);
	/* print and accept input while the string is not a possible connection */
	do {
		printOut(": ", 0);
		fgets(*retString, MAX_CMD_SIZE + 1, stdin);
	} while (validateInput(retString) != 0);
	removeLineEnding(retString);
}

/*********************************************************************
** Description: 
** Print using fputs and flush each time
*********************************************************************/
void printOut(char * outString, int newln){
	fputs(outString, stdout);
	fflush(stdout); // flush the print buffer
	if(newln){	
		fputs("\n", stdout);
		fflush(stdout);
	}
}

/*********************************************************************
** Description: 
** Automates memset() because 'memset' isn't very descriptive
*********************************************************************/
void clearString(char * theString, int size) {
	memset(theString, '\0', size);
}