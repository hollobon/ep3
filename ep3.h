/* ep3.h
 * 
 * Header file for ep3
 */

#ifndef EP3H
#define EP3H

#include <stdio.h>

/* Struct for symbol records */
struct symrec {
   char *name;              /* name of symbol */
   int type;                /* type of symbol */
   struct symrec *next;     /* link field  */
};

#define TYPE_FUNC   1
#define TYPE_BYTE   2
#define TYPE_WORD   3
#define TYPE_DWORD  4
#define TYPE_DF     5
#define TYPE_DQ     6
#define TYPE_DT     7

int addnewsym(char*,int,int,int);

typedef struct symrec symrec;

extern char funcname[100];
extern char curident[100];
extern int curtype;

/* The symbol table */
extern symrec *sym_table;

/* Symbol table functions */
symrec *putsym();
symrec *getsym();
void showsyms();

/* Current pass number */
extern int pass;

/* Files */
extern FILE* fileOutput;
extern FILE* fileInput;
#endif
