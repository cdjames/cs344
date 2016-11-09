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
	int type,
		sk_sig;
};

struct Statuskeeper * new_sk(int type, int sk_sig);

enum Status { CONTINUE, EXIT };

#endif