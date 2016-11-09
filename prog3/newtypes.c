/*********************************************************************
** Author: Collin James
** Date: 11/9/16
** Description: Custom types used in smallsh; implementation
*********************************************************************/

#include "newtypes.h"

#ifndef __ST
#define __ST
# define ST      struct Statuskeeper
# define ST_SIZE sizeof(struct Statuskeeper)
# endif

struct Statuskeeper * new_sk(int type, int sk_sig){
	struct Statuskeeper * st = (struct Statuskeeper *) malloc(sizeof(struct Statuskeeper));
	st->type = type;
	st->sk_sig = sk_sig;

	return st;
}