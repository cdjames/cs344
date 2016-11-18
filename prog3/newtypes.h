/*********************************************************************
** Author: Collin James
** Date: 11/9/16
** Description: Custom types used in smallsh
*********************************************************************/

#ifndef NEWT_H
#define NEWT_H
#include <stdlib.h> // for malloc
#include <string.h>  	// for memset
#include <unistd.h>		// for pid_t
#include <sys/types.h> 	// for struct stat

# ifndef TYPE
# define TYPE      struct Pidkeeper
# define TYPE_SIZE sizeof(struct Pidkeeper)
# endif

# ifndef LT
# define LT(A, B) ((A) < (B))
# endif

# ifndef EQ
# define EQ(A, B) ((A) == (B))
# endif

// int MAX_NUM_ARGS = 512;

typedef unsigned int us_int;

struct Statuskeeper
{
	us_int type;
	int sk_sig;
};

struct argArray {
	char * arg;
};

struct Commandkeeper 
{	
	us_int bg,		// 0/1 -- background?
		bltin,		// 0/1 -- built in function?
		red_in,		// 0/1 -- redirect in?
		red_out,		// 0/1 -- redirect out?
		red_error,
		no_cmd,		
		// io_error,
		num_args;	// >= 0 -- # of arguments
	
	struct argArray * args; // any arguments
	char * cmd;		// the command
	char * infile;		// input file
	char * outfile;	// output file
};

struct Pidkeeper
{
	pid_t pid;
	int status;
};

struct Pidkeeper new_PK(pid_t pid, int status);

struct Statuskeeper * new_SK(us_int type, int sk_sig);

struct Commandkeeper new_CK(char * cmd, struct argArray * args, int num_args);

void free_sk(struct Statuskeeper * sk);

enum Status { CONTINUE, EXIT };

/********************************************************************
** Description: 
** Automates memset() because 'memset' isn't very descriptive
********************************************************************/
// void clearString(char * theString, int size) {
// 	memset(theString, '\0', size);
// }

#endif