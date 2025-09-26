%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(void);
void yyerror(const char *s);
%}

%define parse.error verbose

%token AND CLASS ELSE FALSE FOR FUN IF NIL OR PRINT RETURN SUPER THIS TRUE VAR WHILE
%token EQUAL_EQUAL EQUAL BANG_EQUAL BANG LESS_EQUAL LESS GREATER_EQUAL GREATER
%token LPAREN RPAREN LBRACE RBRACE COMMA DOT MINUS PLUS SEMICOLON STAR SLASH
%token NUM STRING IDENTIFIER

%union {
    double number;
    char* string;
    char* identifier;
}

%type <number> expression term factor
%type <identifier> IDENTIFIER
%type <string> STRING

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
      expression PLUS term
    | expression MINUS term
    | expression STAR term
    | expression SLASH term
    | term
;

term:
      NUM
    | IDENTIFIER
    | STRING
    | LPAREN expression RPAREN
    | comparison
;

comparison:
      expression GREATER expression
    | expression GREATER_EQUAL expression
    | expression LESS expression
    | expression LESS_EQUAL expression
    | expression EQUAL_EQUAL expression
    | expression BANG_EQUAL expression
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintatico: %s\n", s);
}
