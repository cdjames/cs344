/*********************************************************************
** Author: Collin James
** Date: 11/2/16
** Description: 
*********************************************************************/

/*********************************************************************
** Description: 
** mycd

mycd changes directories; it supports relative and absolute paths

mycd receives two parameters: one which is a string that should represent
a path and one which is an int that contains the num of params passed on 
the command line. If the path begins with '/', it is an absolute path. 
Otherwise it is relative.
*********************************************************************/

int mycd(char * dir, int numParams){
	/* if numParams is greater than 1*/
		// print usage and return 1
	/* else you've got 0 or 1 params */
		/* if string is empty */
			// get environment variable
			// change to home directory
		/* else */
			// check for existence of directory
				// change to directory
			// otherwise print an error and exit
	return 0; // successful ending
}