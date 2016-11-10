/*********************************************************************
** Author: Collin James
** Date: 11/2/16
** Description: 
*********************************************************************/

/*********************************************************************
** Description: 
** mycd changes directories; it supports relative and absolute paths

** more info in header file
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