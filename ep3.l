/*
 * Copyright (c) Peter Hollobon 1997-2015
 */

%{
#include "ep3.h"
#include "ep3.tab.h"
%}

DIGIT   [0-9]
ID      [a-z]*    
FUNC    "["{ID}"]"
%x      asm

%%
"byte"          return BYTE;
"word"          return WORD;
"dword"         return DWORD;
"pointer"       return POINTER;
";"             return SEMICOLON;
"->"            return ASSIGN;
"{"             return BLOCKSTART;
"}"             return BLOCKEND;
"["             return REFSTART;
"]"             return REFEND;
"if"            return IF;
"else"          return ELSE;
"do"            return DO;
"break"         return BREAK;
"&"             return REFERENCE;
"*"             return DEREFERENCE;
","             return COMMA;
"'"."'"         yylval.val = yytext[1]; return NUM;
"\"".+"\""      { size_t i; for (i = 0; i < sizeof(yylval.str) && yytext[i + 1] != '"'; i++) { yylval.str[i] = yytext[i + 1]; } yylval.str[i] = '\0'; return STRING; }
"0"{DIGIT}+     sscanf(yytext,"%o",&yylval.val); return NUM; /* octal */
"0x"{DIGIT}+    sscanf(yytext,"%x",&yylval.val); return NUM; /* hexadecimal */
{DIGIT}+        yylval.val = atoi(yytext); return NUM;       /* decimal */
{ID}            { strncpy(yylval.str, yytext, 100); return IDENTIFIER; }
{FUNC}          { addnewsym(yytext,TYPE_FUNC,0,0); strncpy(yylval.str, yytext, 100); return FUNCDEF; }
[ \t\n]+        /* Eat up whitespace */

"asm {"         BEGIN(asm); if(pass==2) fputs("; inline code starts\n",fileOutput); return ASM;
"asm{"          BEGIN(asm); if(pass==2) fputs("; inline code starts\n",fileOutput); return ASM;
<asm>[ \t\n]+   /* Eat up extraneous whitespace */
<asm>[^}\n]*    { if(pass==2) fprintf(fileOutput,"\t%s\n",yytext); }
<asm>"}"        if(pass==2) fputs("; inline code ends\n",fileOutput); BEGIN(INITIAL); 

%%

yywrap() {
   return 1;
}
