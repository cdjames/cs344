/*********************************************************************
** Author: Collin James
** Date: 11/9/16
** Description: Custom types used in smallsh
*********************************************************************/

#ifndef NEWT_H
#define NEWT_H
#include <stdlib.h>

struct Statuskeeper
{
	unsigned int type;
	int sk_sig;
};

struct Statuskeeper * new_sk(unsigned int type, int sk_sig);

void free_sk(struct Statuskeeper * sk);

enum Status { CONTINUE, EXIT };

#endif