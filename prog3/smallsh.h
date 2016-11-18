/*********************************************************************
** Author: Collin James
** Date: 11/2/16
** Description: Function header files for smallsh program
*********************************************************************/

#ifndef SMSH_H
#define SMSH_H

/* includes */
#include <fcntl.h>		// for file manipulation
#include <stdio.h>		// printing, etc
#include <stdlib.h>		// mkdir, etc.
// #include <unistd.h> 	// in builtins.h
// #include <sys/types.h> 	// in builtins.h
// #include <sys/stat.h> 	// in builtins.h
#include <string.h>  	// for memset
#include <time.h>
#include <signal.h> 	//for sigset_t
#include <sys/wait.h> 	// for wait/waitpid
#include "newtypes.h"
#include "builtins.h"
#include "cirListDeque.h"

/*********************************************************************
** Description: 
** prints out ": " and recieves string
*********************************************************************/
void getInput(char **retString);

/*********************************************************************
** Description: 
** Validate user's room entries and return 0 if a possible command, 
** 1 if a comment or blank line
*********************************************************************/
int validateInput(char **input);

/*********************************************************************
** Description: 
** Remove the last character of a string if it is \n
*********************************************************************/
void removeLineEnding(char ** input);

/*********************************************************************
** Description: 
** Does the heavy lifting of checking a queue for finished jobs.
** by passing 1 in killp, kills jobs and waits instead of just waiting
*********************************************************************/
void queueChecker(struct cirListDeque * Pid_queue, struct Pidkeeper PK, int killp);

/*********************************************************************
** Description: 
** Called from queueChecker to print a status message when a job has
** been collected
*********************************************************************/
void printWaitPidStatus(pid_t bgpid, int bgstatus, struct Pidkeeper PK);

/*********************************************************************
** Description: 
** Takes a pointer to a string, extracts &, command, and arguments
** Creates a Commandkeeper struct based on the results and returns
*********************************************************************/
struct Commandkeeper parseInString(char ** inputStr);

/*********************************************************************
** Description: 
** Forking function for foreground processes. Sets up child to catch
** interrupts, sets up a pipe for communicating failures to parent,
** forks process, sets up child signal catching, executes command, and
** returns a status message in theSK; returns a pid to caller
*********************************************************************/
int runInFore(struct Commandkeeper * theCK, struct Statuskeeper * theSK);

/*********************************************************************
** Description: 
** Forking function for background processes. Sets up child to catch
** interrupts, sets up a pipe for communicating failures to parent,
** forks process, sets up child signal catching, executes command, and
** returns a status message in theSK; returns a PK struct to caller with
** pid and status
**
** (Would love to combine with above function but ran out of time!)
*********************************************************************/
struct Pidkeeper runInBack(struct Commandkeeper * theCK, struct Statuskeeper * theSK);

/*********************************************************************
** Description: 
** Automation of redirecting input to a file; pass 1 if it is a background
** process
*********************************************************************/
int redirectIn(struct Commandkeeper * theCK, int bg);

/*********************************************************************
** Description: 
** Automation of redirecting output to a file; pass 1 if it is a background
** process
*********************************************************************/
int redirectOut(struct Commandkeeper * theCK, int bg);

/*********************************************************************
** Description: 
** Automated status printing - "echo value 1", with or without newline
** (1 or 0)
*********************************************************************/
void printStatusMsgNL(int sk_sig, char * msg, int nl);

/*********************************************************************
** Description: 
** Prints a usage statement
*********************************************************************/
void usage();

/* test functions; not used */
void caughtSig();
void childSig();
void setUpSignals();
void printAllCK(struct Commandkeeper ck);

#endif