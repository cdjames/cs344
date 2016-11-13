/*********************************************************************
** Author: Collin James
** Date: 11/2/16
** Description: Function header files for mycd, myexit, and mystatus functions
*********************************************************************/

#ifndef BI_H
#define BI_H

/*********************************************************************
** Description: 
** mycd changes directories; it supports relative and absolute paths

mycd receives two parameters: one which is a string that should represent
a path and one which is an int that contains the num of params passed on 
the command line. If the path begins with '/', it is an absolute path. 
Otherwise it is relative.
*********************************************************************/
int mycd(struct Commandkeeper * CK);

#endif

