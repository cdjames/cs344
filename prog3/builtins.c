/*********************************************************************
** Author: Collin James
** Date: 11/2/16
** Description: 
*********************************************************************/
#include "builtins.h"

char nsd[] = ": No such file or directory"; // for cd error message

int mystatus(struct Commandkeeper * CK, struct Statuskeeper * SK){
	/* check arguments */
	if(CK->num_args > 0){
		// print usage and return 1
		printOut("too many arguments", 1);
		return 1;
	}
	/* check statuskeeper and perform necessary functions */
	else {
		// printf("signal was %d\n", SK->sk_sig);
		// printf("type was %d\n", SK->type);
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

/*********************************************************************
** Description: 
** mycd changes directories; it supports relative and absolute paths

** more info in header file
*********************************************************************/
int mycd(struct Commandkeeper * CK){
	// char* path;
	char* home;
	char* restofpath;
	char* fullarg;
	char cwd[512];
	int r = 0;
	struct stat checkfor;
	home = getenv ("HOME");
	fullarg = CK->args[0].arg;
 
 	/* user entered >1 argument */
	if(CK->num_args > 1){
		// print usage and return 1
		printOut("too many arguments", 1);
		return 1;
	}
	else {
		/* user entered 'cd' -- go home */
		if(CK->num_args == 0){
			chdir(home);
			getcwd(cwd, 512);
			printOut(cwd, 1);
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
					getcwd(cwd, 512);
					printOut(cwd, 1);
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
** Check that a directory exists and change to that directory
*********************************************************************/
int checkAndChangeDir(char * fullarg, struct stat * checkfor) {
	char cwd[512];
	char * fullerror;
	if (stat(fullarg, checkfor) != -1) {
	    chdir(fullarg);
		getcwd(cwd, 512);
		printOut(cwd, 1);
		return 0;
	}
	else {
		fullerror = strcat(fullarg, nsd);
		printOut(fullerror, 1);
		return 1;
	}
} 

void printStatusMsg(int sk_sig, char * msg){
	int buf_s = 30;
	// char * fullmessage;
	char sig[buf_s];
	int n;

	n = snprintf(sig, buf_s, "%d", sk_sig);
	printOut(msg, 0);
	printOut(sig, 1);
}