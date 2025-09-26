%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(void);
void yyerror(const char *s);
extern int yylineno;
%}

%define parse.error verbose

%token AND OR CLASS ELSE FALSE FOR FUN IF NIL OR PRINT RETURN SUPER THIS TRUE VAR WHILE
%token EQUAL_EQUAL EQUAL BANG_EQUAL BANG LESS_EQUAL LESS GREATER_EQUAL GREATER
%token LPAREN RPAREN LBRACE RBRACE COMMA DOT MINUS PLUS SEMICOLON STAR SLASH
%token NUM STRING IDENTIFIER

%union {
    double number;
    char* string;
    char* identifier;
    int boolean;
}

%type <number> expression term factor
%type <identifier> IDENTIFIER
%type <string> STRING
%type <boolean> BOOL_EXPR

%start program

%%

program:
    statements
;

statements:
      /* vazio */
    | statements statement
;

statement:
      var_decl
    | print_stmt
    | if_stmt
    | expr_stmt
    | block
;

var_decl:
    VAR IDENTIFIER EQUAL expression SEMICOLON
;

print_stmt:
    PRINT expression SEMICOLON
;

if_stmt:
    IF LPAREN expression RPAREN block
;

block:
    LBRACE statements RBRACE
;

expr_stmt:
    expression SEMICOLON
;

expression:
      logic_or
;

logic_or:
      logic_and
    | logic_or OR logic_and
;

logic_and:
      comparison
    | logic_and AND comparison
;

comparison:
      addition
    | comparison GREATER addition
    | comparison GREATER_EQUAL addition
    | comparison LESS addition
    | comparison LESS_EQUAL addition
    | comparison EQUAL_EQUAL addition
    | comparison BANG_EQUAL addition
;

addition:
      multiplication
    | addition PLUS multiplication
    | addition MINUS multiplication
;

multiplication:
      unary
    | multiplication STAR unary
    | multiplication SLASH unary
;

unary:
      factor
    | BANG unary
    | MINUS unary
;

factor:
      NUM
    | IDENTIFIER
    | STRING
    | TRUE                         { $$ = 1; }   /* valor booleano verdadeiro */
    | FALSE                        { $$ = 0; }   /* valor booleano falso */
    | LPAREN expression RPAREN
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintatico na linha %d: %s\n", yylineno, s);
}
