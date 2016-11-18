/*********************************************************************
** Author: Collin James
** Date: 11/2/16
** Description: Function header files for mycd, myexit, and mystatus functions
*********************************************************************/

#ifndef BI_H
#define BI_H

#include "newTypes.h"
#include <stdio.h>		// printing, etc
#include <unistd.h>		// chdir
#include <sys/stat.h> 	// for stat()

/*********************************************************************
** Description: 
** Print using fputs and flush each time; newln - 1 = print newline, 0 = don't
*********************************************************************/
void printOut(char * outString, int newln);

/*********************************************************************
** Description: 
** Check that a directory exists and change to that directory
*********************************************************************/
int checkAndChangeDir(char * fullarg, struct stat * checkfor);

/*********************************************************************
** Description: 
** mycd changes directories; it supports relative and absolute paths

mycd receives two parameters: one which is a string that should represent
a path and one which is an int that contains the num of params passed on 
the command line. If the path begins with '/', it is an absolute path. 
Otherwise it is relative.
*********************************************************************/
int mycd(struct Commandkeeper * CK);

/*********************************************************************
** Description: 
** Automates printing a message like "exit value 0"
*********************************************************************/
void printStatusMsg(int sk_sig, char * msg);

/*********************************************************************
** Description: 
** Prints "exit value #" or "terminated by signal #"
*********************************************************************/
int mystatus(struct Commandkeeper * CK, struct Statuskeeper * SK);

/*********************************************************************
** Description: 
** Checks for extra args. If command is "exit", returns 0
*********************************************************************/
int myexit(struct Commandkeeper * CK);

#endif

