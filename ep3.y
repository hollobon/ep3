/* ep3.y
 * 
 * Bison parser for EP3 compiler
 * 
 * (C) Peter Hollobon 1997
 */

%{
#include "ep3.h"
%}

%union {
int val;
symrec* tptr;
char* str;
}

%token <val> NUM
%token <str> IDENTIFIER
%token POINTER BYTE WORD DWORD
%token SEMICOLON COMMA
%token ASSIGN
%token NL
%token BLOCKSTART BLOCKEND
%token REFSTART REFEND
%token IF ELSE
%token DO BREAK
%token ASM
%token REFERENCE DEREFERENCE
%token <str> FUNCDEF

/*%type <val> exp*/
/*%type <str> declarator*/
%%

input:
    | input function
;

declaration_list
    : declaration
    | declaration_list declaration
;

declaration
    : type_specifier init_declarator_list
;

type_specifier
    : BYTE              { curtype=TYPE_BYTE; }
    | WORD              { curtype=TYPE_WORD; }
    | DWORD             { curtype=TYPE_DWORD; }
;

init_declarator_list
    : declarator            
    | init_declarator_list COMMA declarator 
;

declarator
    : declarator2
    | pointer declarator2
;

declarator2
    : IDENTIFIER                        { addnewsym(curident,curtype,0,0); puts(curident); }
    | IDENTIFIER ASSIGN NUM             { addnewsym(curident,curtype,$3,0); puts(curident); }
    | IDENTIFIER REFSTART REFEND        { addnewsym(curident,curtype,0,1); puts("array defined"); }
    | IDENTIFIER REFSTART NUM REFEND    { addnewsym(curident,curtype,0,$3); puts("sized array defined"); }
;

pointer
    : DEREFERENCE
    | DEREFERENCE type_specifier
    | DEREFERENCE pointer
    | DEREFERENCE type_specifier pointer
;

statement
    : compound_statement
    | expression_statement
    | selection_statement
    | iteration_statement
    | jump_statement
;

expression_statement
    : exp
    | expression_statement exp
;

jump_statement
    : BREAK             { dobreak() }
;

compound_statement
    : BLOCKSTART BLOCKEND
    | BLOCKSTART statement_list BLOCKEND 
;

statement_list
    : statement
    | statement_list statement
;

if_definition
    : IF                { doif(); }
;

else_definition
    : ELSE              { doelse(); }
;

selection_statement
    : if_definition statement else_definition statement { doendif(); }
    | if_definition statement { doendif(); }
;

iteration_statement
    : DO { startdo(); } statement { enddo(); }
;

function_definition
    : FUNCDEF           { sprintf(funcname,"_%s",$1+1); startfunc(funcname); }
;    

function     
    : function_definition declaration_list compound_statement { endfunc(funcname); }
    | function_definition compound_statement { endfunc(funcname); }
;

exp: 
    NUM                     { fprintf(fileOutput,"\tpush %d\n",$1); }
    | REFERENCE IDENTIFIER  { doreference(curident); }
    | DEREFERENCE IDENTIFIER { dodereference($2); }
    | ASM                   { }
    | IDENTIFIER REFSTART NUM REFEND { printf("\n%s+%d\n",curident,curident); }
    | IDENTIFIER            { printf("Identifier: %s",curident); doident(curident); } 
    | exp ASSIGN IDENTIFIER { compilePop(curident); }
    | exp ASSIGN IDENTIFIER REFSTART NUM REFEND  { printf("\n%s+%d\n",curident,$5); }
;

%%
#include <ctype.h>
#include <stdio.h>
#include "ep3.h"

/* The name of the current function being compiled */
char funcname[100];

/* The name of the current identifier */
char curident[100];

/* The current type of things being defined */
int curtype=100;

yyerror (char* s)  /* Called by yyparse on error */
{
   printf("\nep3 Compile: %s\n", s);
}
