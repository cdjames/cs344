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

int main(int argc, char const *argv[])
{
	/* variables */
	struct Statuskeeper * theSK = new_sk(0, -1);
	enum Status theStatus = CONTINUE;

	/* main logic */
	
	free_sk(theSK);
	return 0;
}