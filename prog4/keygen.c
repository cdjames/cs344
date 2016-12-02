#include <stdio.h>		// printing, etc
#include <stdlib.h>		// mkdir, etc.
#include <time.h>
#include "utils.h"

int main(int argc, char const *argv[])
{
	if(argc < 2){
		error("Usage: keygen keylength");
		return 1;
	}

	/* seed the randomizer */
	srand(time(NULL));

	/* get the length of the file to create */
	int length = atoi(argv[1]);

	if(length < 1){
		error("keygen: invalid length");
	}


	/* make your character with room for \n and \0 */
	char key[length+2]; 
	memset(key, '\0', sizeof(key));

	int randomN;
	int i;
	for (i = 0; i < length; i++)
	{
		randomN = getRandom(0, 26);

		if(randomN == 26)
			randomN = 32; // ASCII for space
		else
			randomN += 65; // 65 = A, and so on until Z

		key[i] = randomN;
	}

	key[strlen(key)] = '\n'; // append newline

	printOut(key, 0); // print with no extra newline

	return 0;
}