/* File: ep3compile.c
 * 
 * Compiles ep3 code
 */

#include "ep3.h"
#include <string.h>

int ifdepth=0, ifcount=0;
int dodepth=0, docount=0;

int ifstack[100];
int dostack[100];

/* compilePush
 * 
 * Compiles a pop to an identifier
 */
void
compilePop(char* symbol)
{
   if ((getsym(symbol)==0)) {
      fprintf(stderr,"Error: Symbol %s not defined",symbol);
      exit(1);
   }
   fprintf(fileOutput,"\tpop _%s\n",symbol);
}

/* startdo
 * 
 * Compiles the start of a DO 
 */
startdo()
{
   docount++;
   dodepth++;
   dostack[dodepth]=docount;
   fprintf(fileOutput,"%s_do_%d_%d_start:\n",funcname,docount,dodepth);
}

/* enddo
 * 
 * Compiles the end of a DO
 */
enddo()
{
   int i;
   
   i=dostack[dodepth];
   
   fprintf(fileOutput,"\tjmp %s_do_%d_%d_start\n",funcname,i,dodepth);
   fprintf(fileOutput,"%s_do_%d_%d_exit:\n",funcname,i,dodepth);
   dodepth--;
}

/* dobreak
 * 
 * Compiles a break in a do statement
 */
dobreak()
{
   int i;
   
   i=dostack[dodepth];
   
   fprintf(fileOutput,"\tjmp %s_do_%d_%d_exit\n",funcname,docount,dodepth);
}

/* doif
 * 
 * Compiles the start of an if block
 */
doif()
{       
   ifdepth++;
   ifcount++;
   ifstack[ifdepth]=ifcount;
   
   fprintf(fileOutput,"\tpop ax\n");
   fprintf(fileOutput,"\tcmp ax,0\n");
   fprintf(fileOutput,"\tje %s_if_%d_%d_else\n",funcname,ifcount,ifdepth);
}

/* doelse
 * 
 * Compiles the else in an if block
 */
doelse()
{
   int i;
   i=ifstack[ifdepth];
   
   fprintf(fileOutput,"\tjmp %s_if_%d_%d_exit\n",funcname,i,ifdepth);
   fprintf(fileOutput,"%s_if_%d_%d_else:\n",funcname,i,ifdepth);
}

/* doendif
 * 
 * Compiles the last part of an if block
 */
doendif()
{
   int i;
   i=ifstack[ifdepth];
   
   fprintf(fileOutput,"%s_if_%d_%d_exit:\n",funcname,i,ifdepth);
   ifdepth--;
}
   
/* startfunc
 *
 * function start code
 */
startfunc(char* name)
{
   ifcount=0;
   docount=0;
   
   fprintf(fileOutput,"%s\tproc near\n",funcname);
   fprintf(fileOutput,"\tpop ax\n");
   fprintf(fileOutput,"\tmov es:[si],ax\n");
   fprintf(fileOutput,"\tadd si,2\n\n");
}

/* endfunc
 * 
 * writes function termination code
 */
endfunc(char* name)
{
   if (name==NULL) {
      return;
   }
   fprintf(fileOutput,"%s_e:\t\n",name);
   fprintf(fileOutput,"\tsub si,2\n");
   fprintf(fileOutput,"\tmov ax,es:[si]\n");
   fprintf(fileOutput,"\tpush ax\n");
   fprintf(fileOutput,"\tret\n");
   fprintf(fileOutput,"%s\tendp\n\n",name);
   name=NULL;
}

/* doident
 * 
 * Deals with an identifier, either pushing the value of a variable 
 * or calling a function
 */
doident(char* symbol)
{   
   symrec* gotsym;
   gotsym=getsym(symbol);
   
   if (gotsym==0) {
      fprintf(stderr,"Symbol %s not found",symbol);
      exit(1);
   }
   
   switch(gotsym->type) {
    case TYPE_BYTE:
      fprintf(fileOutput,"\tpush _%s\n",symbol);
      break;
    case TYPE_WORD:
      fprintf(fileOutput,"\tpush _%s\n",symbol);
      break;
    case TYPE_DWORD:
      fprintf(fileOutput,"\tpush _%s\n",symbol);
      break;
    case TYPE_FUNC:
      fprintf(fileOutput,"\tcall _%s\n",symbol);
      break;
    default:
      fprintf(stderr,"\n!! %s Unidentified symbol type %d !!\n",symbol,gotsym->type);
      showsyms();
      exit(1);
   }
}

/* doreference
 * 
 * Compiles a reference to something
 */
doreference(char* symbol)
{
   if (symbol==0)
     return;
   fprintf(fileOutput,"\tlea di,_%s\n",symbol);
   fprintf(fileOutput,"\tpush di\n");
}

/* dodereference
 * 
 * Compiles a dereference to something
 */
dodereference(char* symbol)
{
   if (symbol==0)
     return;
   fprintf(fileOutput,"\tmov di,_%s\n",symbol);
   fprintf(fileOutput,"\tpush WORD PTR [di]\n");
}
