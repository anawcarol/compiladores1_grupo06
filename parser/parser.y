%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(void);
void yyerror(const char *s);
extern int yylineno;
%}

%define parse.error verbose

%token AND OR CLASS ELSE FALSE FOR FUN IF NIL PRINT RETURN SUPER THIS TRUE VAR WHILE
%token EQUAL_EQUAL EQUAL BANG_EQUAL BANG LESS_EQUAL LESS GREATER_EQUAL GREATER
%token LPAREN RPAREN LBRACE RBRACE COMMA DOT MINUS PLUS SEMICOLON STAR SLASH

/* Tokens que carregam valores */
%token <number> NUM
%token <string> STRING
%token <identifier> IDENTIFIER

%union {
    double number;
    char* string;
    char* identifier;
    int boolean;
}

/* * CORREÇÃO 1: Unificando os tipos de todas as regras de expressão
 * para evitar erros de "type clash". Isso é fundamental.
 */
%type <number> expression assignment logic_or logic_and comparison addition
%type <number> multiplication unary call primary arguments

/* Removido, pois não é usado */
/* %type <boolean> BOOL_EXPR */

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
    | return_stmt
    | if_stmt
    | while_stmt
    | for_stmt
    | expr_stmt
    | block
    | fun_decl
    | class_decl
;

var_decl:
      VAR IDENTIFIER EQUAL expression SEMICOLON
    | VAR IDENTIFIER SEMICOLON
;

print_stmt:
    PRINT expression SEMICOLON
;

return_stmt:
      RETURN SEMICOLON
    | RETURN expression SEMICOLON
;

if_stmt:
      IF LPAREN expression RPAREN statement
    | IF LPAREN expression RPAREN statement ELSE statement
;

while_stmt:
    WHILE LPAREN expression RPAREN block
;

for_stmt:
    FOR LPAREN for_init SEMICOLON for_condition SEMICOLON for_increment RPAREN block
;

for_init:
      /* vazio */
    | var_decl_no_semicolon
    | expr_stmt_no_semicolon
;

for_condition:
      /* vazio */
    | expression
;

for_increment:
      /* vazio */
    | expression
;

var_decl_no_semicolon:
      VAR IDENTIFIER EQUAL expression
    | VAR IDENTIFIER
;

expr_stmt_no_semicolon:
    expression
;

expr_stmt:
    expression SEMICOLON
;

fun_decl:
    FUN IDENTIFIER LPAREN params RPAREN block
;

params:
      /* vazio */
    | IDENTIFIER
    | params COMMA IDENTIFIER
;

class_decl:
    CLASS IDENTIFIER LBRACE method_declarations RBRACE
;

method_declarations:
      /* vazio */
    | method_declarations method_decl
;

method_decl:
    /* Simplificado. Opcionalmente, pode ser apenas "fun_decl" */
    IDENTIFIER LPAREN params RPAREN block
;

expression:
    assignment
;

/* * CORREÇÃO 2: A regra de atribuição agora permite atribuir a membros de objetos.
 * A expressão à esquerda do '=' (l-value) pode ser um 'call.IDENTIFIER'.
 */
assignment:
    call DOT IDENTIFIER EQUAL assignment
  | IDENTIFIER EQUAL assignment
  | logic_or
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

/* A regra unary agora se baseia na nova regra 'call' */
unary:
      call
    | BANG unary
    | MINUS unary
;

/* * CORREÇÃO 3: Nova regra 'call' para lidar com acesso a membros e chamadas de função.
 * Isso torna a gramática muito mais poderosa e flexível.
 */
call:
    primary
  | call LPAREN arguments RPAREN   /* Chamada de função: foo() */
  | call DOT IDENTIFIER            /* Acesso a membro: foo.bar */
;

/* A antiga regra 'factor' foi renomeada para 'primary' e simplificada. */
primary:
    NUM
  | STRING
  | IDENTIFIER
  | TRUE
  | FALSE
  | NIL
  | THIS
  | LPAREN expression RPAREN
;

arguments:
      /* vazio */
    | expression
    | arguments COMMA expression
;

block:
    LBRACE statements RBRACE
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintatico na linha %d: %s\n", yylineno, s);
}