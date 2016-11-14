/*********************************************************************
** Author: Collin James
** Date: 11/2/16
** Description: 
*********************************************************************/

#include "newTypes.h"
#include <stdio.h>		// printing, etc
#include <unistd.h>	// chdir
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
** mycd changes directories; it supports relative and absolute paths

** more info in header file
*********************************************************************/
int mycd(struct Commandkeeper * CK){
	char* path;
	char* home;
	char cwd[512];
 
	if(CK->num_args > 1){
		// print usage and return 1
		printOut("too many arguments", 1);
		return 1;
	}
	else {
		if(CK->num_args == 0){
			// get environment variable
			// getcwd(cwd, 512);
			// printOut(cwd, 1);
			home = getenv ("HOME");
			// printOut(home, 1);
			chdir(home);
			// getcwd(cwd, 512);
			// printOut(cwd, 1);
			// change to home directory
		}
		else{
			// check for existence of directory
			home = getenv ("PATH");
			printOut(path, 1);
				// change to directory
			// otherwise print an error and exit
		}
	}
	return 0; // successful ending
}