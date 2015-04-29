/* ep3main.c
 * 
 * ep3 - the next generation of EP languages.
 * 
 * Compiled RPN programming language
 * Generates 80x86 Intel assembler (tasm)
 * 
 * Copyright (c) Peter Hollobon 1997-2015
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ep3.h"

char header[]=".386\n\
\n\
assume cs:text_seg\n\
assume ds:data_seg\n\
assume es:s2_seg\n\
assume ss:s1_seg\n\
\n\
data_seg   segment use16\n\
";

char start[]="data_seg   ends\n\
\n\
text_seg   segment use16\n\
\n\
start:\n\
    push seg data_seg\n\
    pop ds\n\
    push seg s2_seg\n\
    pop es\n\
    mov si,0\n\
\n\
    call _main          ; call the main function\n\
exit:\n\
    mov ax,4c00h        ; exit with errorlevel 0\n\
    int 21h             ; call dos\n\
";

char stack[]="text_seg   ends\n\
\n\
s1_seg  segment use16 stack 'Stack'\n\
    dq  512 dup (?)\n\
s1_seg  ends\n\
				 \n\
s2_seg  segment use16 'FuncStack'\n\
dq      4096 dup (?)\n\
s2_seg  ends\n\
\n\
end     start								  \n\
";

FILE* fileInput;    /* the file we're opening */
FILE* fileOutput;   /* the output assembler file */
extern FILE* yyin;  /* the current file being lexically analysed */

int pass;           /* the number of the pass we're on */

int
main(int argc,char** argv)
{
   char inFilename[128];
   char outFilename[128];
   
   if (argc==0) {
      fputs("Error: need filename to compile",stderr);
      exit(1);
   }
   
   strncpy(inFilename, argv[1], 128);
   strncat(inFilename, ".ep3", 128);
   if ((fileInput=fopen(inFilename,"r"))==NULL) {
      fprintf(stderr,"Error: could not open file %s\n",inFilename);
      exit(1);
   }
   
   strncpy(outFilename, argv[1], 128);
   strncat(outFilename, ".asm", 128);
   if ((fileOutput=fopen(outFilename,"w"))==NULL) {
      fprintf(stderr,"Error: could not open output file %s\n",outFilename);
      exit(1);
   }
   
   printf("Compiling file %s\n",inFilename);
   yyin=fileInput;

   puts("Pass 1...");
   fputs(header,fileOutput);
   pass=1;
   while(yylex()!=0);
   
   if (getsym("main")==0) {
      fputs("Error: Expecting 'main' function\n",stderr);
      exit(1);
   }
   
   puts("Pass 2...");
   rewind(yyin);
   fputs(start,fileOutput);
   
   pass=2;
   yyparse();
   
   fputs(stack,fileOutput);
   puts("Finished");
}
