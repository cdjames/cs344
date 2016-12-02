/*********************************************************************
** Author: Collin James
** Date: 12/1/16
** Description: Generates a key of a given length, passed in when run
*********************************************************************/

#include <stdio.h>		// printing, etc
#include <stdlib.h>		// mkdir, etc.
#include <time.h>
#include "utils.h"

const int MIN = 0;
const int MAX = 26;

int main(int argc, char const *argv[])
{
	if(argc < 2){
		error("Usage: keygen keylength");
	}

	/* seed the randomizer */
	srand(time(NULL));

	/* get the length of the file to create */
	int length = atoi(argv[1]);

	if(length < 1){
		error("keygen: invalid length");
	}

	/* make your string with room for \n and \0 */
	char key[length+2]; 
	memset(key, '\0', sizeof(key));

	int randomN;
	int i;
	/* loop the length passed in, appending a random character within
		the defined range */
	for (i = 0; i < length; i++)
	{
		randomN = getRandom(MIN, MAX);

		if(randomN == MAX)
			randomN = 32; // ASCII for space
		else
			randomN += 65; // 65 = A, and so on until Z

		/* assign the character to the string */
		key[i] = randomN;
	}

	key[strlen(key)] = '\n'; // append newline

	printOut(key, 0); // print with no extra newline

	return 0;
}