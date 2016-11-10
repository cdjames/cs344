/*********************************************************************
** Author: Collin James
** Date: 11/9/16
** Description: Custom types used in smallsh; implementation
*********************************************************************/

#include "newtypes.h"

#ifndef __ST
#define __ST
#define ST struct Statuskeeper
#define ST_SIZE sizeof(struct Statuskeeper)
#endif

struct Statuskeeper * new_sk(unsigned int type, int sk_sig){
	struct Statuskeeper * st = (ST *) malloc(ST_SIZE);
	st->type = type;
	st->sk_sig = sk_sig;

	return st;
}

void free_sk(struct Statuskeeper * sk){
	free(sk);
}