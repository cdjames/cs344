/*********************************************************************
** Author: Collin James
** Date: 11/2/16
** Description: 
*********************************************************************/
#include "builtins.h"

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

int checkAndChangeDir(char * fullarg, struct stat * checkfor) {
	char cwd[512];
	if (stat(fullarg, checkfor) != -1) {
	    chdir(fullarg);
		getcwd(cwd, 512);
		printOut(cwd, 1);
		return 0;
	}
	else {
		printOut("no such directory", 1);
		return 1;
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
	char* restofpath;
	char* fullarg;
	char cwd[512];
	struct stat checkfor;
	home = getenv ("HOME");
	fullarg = CK->args[0].arg;
 
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
			// printOut(home, 1);
			chdir(home);
			getcwd(cwd, 512);
			printOut(cwd, 1);
			// change to home directory
		}
		else{
			// check for existence of directory
			printOut(fullarg, 1);
			/* if ~, change to home directory plus remainder of string (check first) */
			if(fullarg[0] == '~'){
				restofpath = strtok(fullarg, "~");
				if(restofpath != NULL) {
					fullarg = strcat(home, restofpath);
					printOut(fullarg, 1);
					int c = checkAndChangeDir(fullarg, &checkfor);
					return c;
				}
				else{
					chdir(home);
					getcwd(cwd, 512);
					printOut(cwd, 1);
				}
			}
			else
			{			
			/* if /, ., or .., just check that dir and change if possible */
				int c = checkAndChangeDir(fullarg, &checkfor);
				return c;
			}
			
			// 
			// path = getenv ("PATH");
			// printOut(path, 1);
				// change to directory
			// otherwise print an error and exit
		}
	}
	return 0; // successful ending
}