/* ep3.y
 * 
 * Bison parser for EP3 compiler
 * 
 * Copyright (c) Peter Hollobon 1997-2015
 */

%{
#include "ep3.h"
%}

%union {
int val;
symrec* tptr;
char str[100];
}

%token <val> NUM
%token <str> IDENTIFIER
%token <str> STRING
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

/*declarator
    : declarator2
    | pointer declarator2
;*/

declarator
    : IDENTIFIER                        { addnewsym($1,curtype,0,0); printf("identifier %s declared as %d\n", $1, curtype); }
    | IDENTIFIER ASSIGN NUM             { addnewsym($1,curtype,$3,0); puts($1); }
    | IDENTIFIER REFSTART REFEND        { addnewsym($1,curtype,0,1); puts("array defined"); }
    | IDENTIFIER REFSTART NUM REFEND    { addnewsym($1,curtype,0,$3); puts("sized array defined"); }
;

/*pointer
    : DEREFERENCE
    | DEREFERENCE type_specifier
    | DEREFERENCE pointer
    | DEREFERENCE type_specifier pointer
;*/

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
    : BREAK             { dobreak(); }
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
| STRING                { char *start = $1, *c = start; while (*c++); c--; do { fprintf(fileOutput, "\tpush %d\n", *c); } while (--c >= start) }
    | REFERENCE IDENTIFIER  { doreference($2); }
    | DEREFERENCE IDENTIFIER { dodereference($2); }
    | ASM                   { }
    | IDENTIFIER REFSTART NUM REFEND { printf("\nidentifier with index %s+%d\n",$1,$3); }
    | IDENTIFIER            { printf("identifier: %s\n",$1); doident($1); } 
    | exp ASSIGN IDENTIFIER { printf("assignment to identifier %s\n", $3); compilePop($3); }
    | exp ASSIGN IDENTIFIER REFSTART NUM REFEND  { printf("assignment to Identifier with index %s+%d\n",$3,$5); }
;

%%
#include <ctype.h>
#include <stdio.h>
#include "ep3.h"

/* The name of the current function being compiled */
char funcname[100];

/* The current type of things being defined */
int curtype=100;

yyerror (char* s)  /* Called by yyparse on error */
{
   printf("\nep3 Compile: %s\n", s);
}
