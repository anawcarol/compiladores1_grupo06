%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char* s);
int yylex(void);
extern int yylineno;
%}

%union {
    int boolean;
    double number;
    char* string;
    char* identifier;
}

%token AND CLASS ELSE FALSE FOR FUN IF NIL OR PRINT RETURN SUPER THIS TRUE VAR WHILE
%token EQUAL_EQUAL EQUAL BANG_EQUAL BANG LESS_EQUAL LESS GREATER_EQUAL GREATER
%token LEFT_PAREN RIGHT_PAREN LEFT_BRACE RIGHT_BRACE COMMA DOT MINUS PLUS SEMICOLON STAR SLASH

%token <number> NUMBER
%token <string> STRING
%token <identifier> IDENTIFIER
%token <boolean> TRUE FALSE

%left OR
%left AND
%left EQUAL_EQUAL BANG_EQUAL
%left LESS LESS_EQUAL GREATER GREATER_EQUAL
%left PLUS MINUS
%left STAR SLASH
%right BANG

%start program

%%

program:
    statements
    ;

statements:
    /* empty */
    | statements statement
    ;

statement:
    expression SEMICOLON
    | PRINT expression SEMICOLON
    | VAR IDENTIFIER EQUAL expression SEMICOLON
    ;

expression:
    literal
    | IDENTIFIER
    | expression PLUS expression
    | expression MINUS expression
    | expression STAR expression
    | expression SLASH expression
    | expression EQUAL_EQUAL expression
    | expression BANG_EQUAL expression
    | expression LESS expression
    | expression LESS_EQUAL expression
    | expression GREATER expression
    | expression GREATER_EQUAL expression
    | PLUS expression %prec BANG
    | MINUS expression %prec BANG
    | BANG expression
    | LEFT_PAREN expression RIGHT_PAREN
    ;

literal:
    NUMBER
    | STRING
    | TRUE
    | FALSE
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Erro na linha %d: %s\n", yylineno, s);
}

int main() {
    printf("Interpretador Lox - Digite expressões (Ctrl+D para sair):\n");
    return yyparse();
}