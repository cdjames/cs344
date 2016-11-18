/*********************************************************************
** Author: Collin James
** Date: 11/2/16
** Description: Logic for mycd, myexit, and mystatus functions. See
** header for more information
*********************************************************************/
#include "builtins.h"

const int MAX_PATH = FILENAME_MAX; // maximum file length

char nsd[] = ": No such file or directory"; // for cd error message

int mystatus(struct Commandkeeper * CK, struct Statuskeeper * SK){
	/* check arguments */
	if(CK->num_args > 0){
		// print usage and return 1
		printOut("status: too many arguments", 1);
		return 1;
	}
	/* check statuskeeper and perform necessary functions */
	else {
		/* if empty SK.type (0), no completed jobs */
		if(SK->type == 0)
		{
			printOut("no completed jobs", 1);
		}
		/* if 1, "exit value _" */
		else if(SK->type == 1)
		{
			printStatusMsg(SK->sk_sig, "exit value ");
		}
		/* if 2, "terminated by signal __" */		
		else
		{
			printStatusMsg(SK->sk_sig, "terminated by signal ");
		}
	}
	return 0;
}

int mycd(struct Commandkeeper * CK){
	// char* path;
	char* home;
	char* restofpath;
	char* fullarg;
	char cwd[MAX_PATH];
	int r = 0;
	struct stat checkfor;
	home = getenv ("HOME");
	fullarg = CK->args[0].arg;
 	// printOut("got here", 1);
 	/* user entered >1 argument */
	if(CK->num_args > 1){
		// print usage and return 1
		printOut("cd: too many arguments", 1);
		return 1;
	}
	else {
		/* user entered 'cd' -- go home */
		if(CK->num_args == 0){
			chdir(home);
			getcwd(cwd, MAX_PATH);
			// printOut(cwd, 1);
			// change to home directory
		}
		/* user entered 'cd arg' */
		else{
			// printOut(fullarg, 1);
			/* if ~ plus more info, change to home directory plus remainder of string  */
			if(fullarg[0] == '~'){
				restofpath = strtok(fullarg, "~");
				/* there is something after ~ */
				if(restofpath != NULL) {
					fullarg = strcat(home, restofpath);
					// printOut(fullarg, 1);
					r = checkAndChangeDir(fullarg, &checkfor);
					return r;
				}
				/* just plain ~ -- go home */
				else
				{
					chdir(home);
					getcwd(cwd, MAX_PATH);
					// printOut(cwd, 1);
				}
			}
			/* if /, ., .., or plain arg, just check that dir and change if possible */
			else
			{			
				r = checkAndChangeDir(fullarg, &checkfor);
				return r;
			}
		}
	}
	return 0; // successful ending
}

int myexit(struct Commandkeeper * CK){
	/* check arguments */
	if(CK->num_args > 0){
		// print usage and return 1
		printOut("exit: too many arguments", 1);
		return 1;
	}
	return 0;
}

void printOut(char * outString, int newln){
	fputs(outString, stdout);
	fflush(stdout); // flush the print buffer
	if(newln){	
		fputs("\n", stdout);
		fflush(stdout);
	}
}


int checkAndChangeDir(char * fullarg, struct stat * checkfor) {
	char cwd[MAX_PATH];
	char * fullerror;
	/* check for your file */
	if (stat(fullarg, checkfor) != -1) {
	    chdir(fullarg);
		getcwd(cwd, MAX_PATH);
		// printOut(cwd, 1);
		return 0;
	}
	/* no file; print error */
	else {
		fullerror = strcat(fullarg, nsd);
		printOut("cd: ", 0);
		printOut(fullerror, 1);
		return 1;
	}
} 

void printStatusMsg(int sk_sig, char * msg){
	int buf_s = 30;
	char sig[buf_s];
	int n;

	/* make a string from the int */
	n = snprintf(sig, buf_s, "%d", sk_sig);
	printOut(msg, 0);
	printOut(sig, 1);
}