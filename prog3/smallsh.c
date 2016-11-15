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
#include <signal.h> 	//for sigset_t
#include "newtypes.h"
#include "builtins.h"
#include "dynArray.h"

void getInput(char **retString);
void clearString(char * theString, int size);
void removeLineEnding(char ** input);
void printOut(char * outString, int newln);
struct Commandkeeper parseInString(char ** inputStr);
void caughtSig();
void setUpSignals();

/* Global constants */
const int MAX_CMD_SIZE = 2048;
const char * DELIM = " ";

/* Global vars */
char * USAGE = "Usage: command [arg1 arg2 ...] [< input_file] [> output_file] [&]";
// const int MAX_NUM_ARGS = 512;

void structTest(){
	char * str = "hello";
	int num_args = 3;
	struct argArray args[num_args];
	// args = malloc( sizeof(struct argArray) * 3 );
	int i;
	for (i = 0; i < 3; i++)
	{
		args[i].arg = str;
		printf("%s\n", args[i].arg);
	}
	// args -= (sizeof(struct argArray) * 3);
	// printf("%s\n", args[1].arg);
	struct Commandkeeper theCK = new_CK(str, args, num_args);

	for (i = 0; i < 3; i++)
	{
		// strcpy(args[i].arg, str);
		printf("%s\n", theCK.args[i].arg);
		// args += (sizeof(struct argArray));
	}
}

void printAllCK(struct Commandkeeper ck) {
	printf("bg=%d, bltin=%d, red_in=%d, red_out=%d, red_error=%d, num_args=%d\n", ck.bg, ck.bltin, ck.red_in, ck.red_out, ck.red_error, ck.num_args);
}

void usage(){
	printOut(USAGE, 1);
}

int main(int argc, char const *argv[])
{
	setUpSignals();

	/* variables */
	struct Statuskeeper * theSK = new_SK(0, -1);
	enum Status theStatus = CONTINUE;
	struct Commandkeeper theCK;	
	char * input; // allocated in getInput
	char exitSt[] = "exit";
	// int r; 

	/* main logic */
	while(theStatus != EXIT){
		getInput(&input); // validates input and puts in input variable
		// printOut(input, 1); // print with a line ending; TESTING

		/* look for exit command*/
		if(strcmp(input, exitSt) != 0){
		// 	/* not an exit command; process string */
			theCK = parseInString(&input);

			/* check for redirect errors and print usage */
			if(theCK.red_error){
				/* print usage and go to next loop iteration */
				usage();
				continue;
			}
			// if(theCK.io_error) // check for interrupted commands
			// 	continue;
			/* otherwise check for built in commands and run */
			if(theCK.bltin){
				theSK->type = 1;
				if(strcmp(theCK.cmd, "cd") == 0){
					/* run cd */
					theSK->sk_sig = mycd(&theCK);
					continue;
				}
				else if(strcmp(theCK.cmd, "status") == 0){
					/* run status*/
					theSK->sk_sig = mystatus(&theCK, theSK);
					continue;
				} 
				else if(strcmp(theCK.cmd, exitSt) == 0){
					/* run exit */
					theStatus = EXIT;
					continue;
				}
			}
			/* otherwise hand external commands */
			else{
				/* if no bg symbol, run in foreground */
				// runInFore(&theCK)
				/* otherwise, run in background */
			}
		} else {
			theStatus = EXIT;
			/* run any other exit routines here */
		}

	}
	free_sk(theSK);
	free(input);

	// structTest();
	return 0;
}

struct Commandkeeper parseInString(char ** inputStr){
	// char tmp[MAX_CMD_SIZE+1];
	// strcpy(tmp, *inputStr);
	char * tmp = *inputStr;
	us_int bg = 0,
		bltin = 0;
	char * cmd;
	char * infile = NULL;
	char * outfile = NULL;
	struct argArray args[512];
	struct Commandkeeper CK;
	int str_len = strlen(tmp);
	int red_in_count = 0;
	int red_out_count = 0;
	int red_in_sat = 0;
	int red_out_sat = 0;
	int red_error = 0;
	/* check last character for & and set bg */
	if(tmp[str_len-1] == '&'){
		bg = 1;
		tmp[str_len-1] = '\0';
		str_len = strlen(tmp);
		/* remove space at end of string if necessary */
		if(tmp[str_len] == ' '){
			tmp[str_len] = '\0';
			str_len = strlen(tmp);
		}
	}

	/* extract your command from the front of the string */
	char * subString;
	subString = strtok(tmp,DELIM);
	cmd = subString;
	
	// CK = new_CK(NULL, NULL, 0);
	// if(cmd == NULL){
	// 	CK.io_error = 1;
	// 	return CK; // something went wrong
	// }

	if(strcmp(cmd, "cd") == 0 || strcmp(cmd, "status") == 0 || strcmp(cmd, "exit") == 0){
		bltin = 1;
	}
	
	// printf("%s\n", cmd);

	/* parse remaining tokens and determine if you have a > or <, argument, or filename */
	int i = 0;
	subString = strtok(NULL, DELIM);
	while (subString != NULL)
	{
		// printf("%s\n", subString);
		/* get next token */
		
		// printf("word=%s\n", subString);
		// fflush(stdout);
		/* is it < or >? If so, start your counter, which should always be 1 */
		if(strcmp(subString, "<") == 0){
			// printOut("input redirect", 1);
			red_in_count += 1; // set to 
		} 
		else if(strcmp(subString, ">") == 0){
			// printf("output redirect\n");
			red_out_count += 1;
		} 
		// /* not a redirection character */
		else {
			if(red_in_count == 1 || red_out_count == 1){ // you've had a redirection operator, so no more args
				if(red_in_count){ 
					if(red_in_sat == 0){ // you've got a filename
						red_in_sat += 1;
						infile = subString;
					}
				}
				if(red_out_count){ // you've had a redirection operator, so no more args
					if(red_out_sat == 0){ // you've got a filename
						red_out_sat += 1;
						outfile = subString;
					}
				}
			}
			else {	// you've got an argument
				args[i].arg = subString;
				i++;
			}
		}
		/* get next token */
		subString = strtok(NULL, DELIM);
	}
	/* now all args, redirects figured; create initial Commandkeeper object */
	CK = new_CK(cmd, args, i);

	/* check for redirect errors */
	// printf("red_in_count=%d, red_in_sat=%d, red_out_count=%d, red_out_sat=%d\n", red_in_count, red_in_sat, red_out_count, red_out_sat);
	/* no filenames */
	if( (red_in_count > 0 && red_in_sat == 0) || (red_out_count > 0 && red_out_sat == 0))
		red_error = 1;
	/* too many operators */
	else if (red_in_count > 1 || red_out_count > 1)
		red_error = 1;
	
	/* check whether to mark redirects as true */
	if(red_in_count == 1 && red_error == 0) {		
		CK.red_in = 1;
		CK.infile = infile;
	}
	if(red_out_count == 1 && red_error == 0){
		CK.red_out = 1;
		CK.outfile = outfile;
	}
	/* set the rest of the variables in the CK object */
	CK.red_error = red_error;
	CK.bg = bg;
	CK.bltin = bltin;

	return CK;
}

/*********************************************************************
** Description: 
** Remove the last character of a string if it is \n
*********************************************************************/
void removeLineEnding(char ** input){
	/* create a string of 2049 and copy into tmp string */
	char tmp[MAX_CMD_SIZE+1];
	strcpy(tmp, *input);
	/* get size of string, find last character, and delete if \n */
	int le = strlen(tmp)-1;
	if(tmp[le] == '\n'){
		tmp[le] = '\0';
		/* copy the string back to input (returning it) */
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
	if(strlen(*input) <= 1){ // i.e. null or just \0
		/* check for 0 length string or ^C so no newline */
		if(strlen(*input) < 1 || (strlen(*input) == 1 && *input[0] != '\n'))
			printOut("", 1); // print a new line
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
** Receive a command string, max size 2048
** retString = pointer to char * (this is the "return" value)
** roomNmOnly = name of a room ("Attic")
** allRooms = string of all connected rooms ("Foyer, Dining, Bedroom.")
********************************************************************/
void getInput(char **retString){
	/* allocate your string w/ room for \0 */
	// if(retString == NULL)
	*retString = malloc( sizeof(char) * (MAX_CMD_SIZE + 1) );
	
	/* print and accept input while the string is not a possible command */
	do {
		printOut(": ", 0);
		/* get a string of max size 2048 to leave room for \0 */
		fgets(*retString, MAX_CMD_SIZE, stdin);
		// printf("%li\n", strlen(*retString) );
	} while (validateInput(retString) != 0);
	/* get rid of a \n if it exists at end of string */
	removeLineEnding(retString);
}

void caughtSig(){
	// printOut("caught signal ", 1);
	printf("%s\n", "caught signal");
	// int i;
	// i = 0;
}

void setUpSignals(){
	struct sigaction myact;
	// void (*fp) (void) = caughtSig; // function pointer
	// myact.sa_handler = SIG_IGN; // ignore a signal
	myact.sa_handler = caughtSig; // ignore a signal
	// sigset_t my_sig_set;
	// sigfillset(&my_sig_set); // fill w/ all signals
	sigfillset(&(myact.sa_mask)); // fill w/ all signals
	myact.sa_flags = 0;

	sigaction(SIGINT, &myact, NULL);
}
/*********************************************************************
** Description: 
** Print using fputs and flush each time
*********************************************************************/
// void printOut(char * outString, int newln){
// 	fputs(outString, stdout);
// 	fflush(stdout); // flush the print buffer
// 	if(newln){	
// 		fputs("\n", stdout);
// 		fflush(stdout);
// 	}
// }

// ********************************************************************
// ** Description: 
// ** Automates memset() because 'memset' isn't very descriptive
// ********************************************************************
// void clearString(char * theString, int size) {
// 	memset(theString, '\0', size);
// }