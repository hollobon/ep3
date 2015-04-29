/* File: ep3symtab.c
 * 
 * Symbol table management routines
 *
 * Copyright (c) Peter Hollobon 1997-2015
 */

#include <stdlib.h>
#include <string.h>
#include "ep3.h"

/* The symbol table: a linked list of struct symrec */
symrec *sym_table = (symrec *)0;

/* putsym
 * 
 * Add a symbol to the symbol table
 */
symrec*
putsym(char* sym_name,int sym_type)
{
   symrec *ptr;
/*   printf("adding symbol with type %d\n",sym_type);*/
   /* allocate memory for this new symrec */
   ptr = (symrec *)malloc(sizeof (symrec));
   
   /* allocate memory for the name and copy it in */
   ptr->name = (char *)malloc(strlen (sym_name) + 1);
   strcpy (ptr->name,sym_name);
    
   ptr->type = sym_type;
   /*ptr->value.var = 0; */ /* set value to 0 even if fctn.  */
    
   /* link to the next record */
   ptr->next = (struct symrec *)sym_table;
   
   /* point sym_table at this new record - the top of the list */
   sym_table = ptr;
   return ptr;
}

/* getsym
 * 
 * Looks for a symbol in the table, returns a pointer to it if it exists,
 * or 0 if it doesnt
 */
symrec *
getsym (char* sym_name)
{
   symrec *ptr;
   for (ptr = sym_table; ptr != (symrec*) 0; ptr = (symrec *)ptr->next) {      
      if (strcmp (ptr->name,sym_name) == 0) {	 
	 return ptr;
      }
   }
   
   return 0;
}

/* showsyms
 * 
 * Shows the names with types of all symbols in the table (debugging)
 */
void
showsyms(void)
{
   symrec *ptr;
   for (ptr = sym_table; ptr != (symrec*) 0; ptr = (symrec *)ptr->next) {      
      printf("\nName: %s\n",ptr->name);
      printf("Type: %d\n",ptr->type);
   }
}

/* addnewsym
 * 
 * Takes care of adding a new symbol to the table nicely
 */
int
addnewsym(char* name,int type,int initValue,int arraySize)
{
   symrec* s;
   
   s=(symrec*)getsym(name);
   if (s==0) {
      if (type==1) {
	 name++;
	 name[strlen(name)-1]='\0';
/*	 printf("** Defining function %s **",name);*/
	 s=(symrec*)putsym(name,type);
      } else {
	 if (arraySize!=0) {
	    fprintf(fileOutput,"\t_%s\tdb\t%d dup (%d)\n",name,arraySize,initValue);
	 } else {	    
	    /* variable symbol */
	    switch(type) {
	     case TYPE_BYTE:
	       fprintf(fileOutput,"\t_%s\tdb\t%d\n",name,initValue);
	       break;
	     case TYPE_WORD:
	       fprintf(fileOutput,"\t_%s\tdw\t%d\n",name,initValue);
	       break;
	     case TYPE_DWORD:
	       fprintf(fileOutput,"\t_%s\tdd\t%d\n",name,initValue);
	       break;
	     case TYPE_DF:
	     case TYPE_DQ:
	     case TYPE_DT:
	       fprintf(stderr,"Type not implemented\n");
	       exit(1);
	       break;
	     default:
	       fprintf(stderr,"Internal compile error: Undefined type\n");
	       exit(1);
	    }
	 }
	 s=(symrec*)putsym(name,type);
      }
   }
}
