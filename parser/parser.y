/* * ARQUIVO: parser/parser.y
 * Conteúdo completo e modificado.
 */
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Incluir a AST e declarar a raiz global */
#include "../ast/ast.h"
NoAST *ast_root = NULL;

void yyerror(const char* s);
int yylex(void);
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
    NoAST *no_ast; /* Ponteiro para a AST */
}

/* Modificado: Quase tudo agora retorna um <no_ast> */
%type <no_ast> program statements statement
%type <no_ast> var_decl print_stmt return_stmt if_stmt while_stmt for_stmt
%type <no_ast> for_init for_condition for_increment
%type <no_ast> var_decl_no_semicolon expr_stmt_no_semicolon
%type <no_ast> expr_stmt block fun_decl params class_decl
%type <no_ast> method_declarations method_decl
%type <no_ast> expression assignment logic_or logic_and comparison addition
%type <no_ast> multiplication unary call primary arguments

%start program

%%

program:
    statements { ast_root = $1; } /* Salva a árvore final */
;

statements:
      /* vazio */ { $$ = NULL; }
    | statements statement { $$ = anexarNoLista($1, $2); }
;

statement:
      var_decl    { $$ = $1; }
    | print_stmt  { $$ = $1; }
    | return_stmt { $$ = $1; }
    | if_stmt     { $$ = $1; }
    | while_stmt  { $$ = $1; }
    | for_stmt    { $$ = $1; }
    | expr_stmt   { $$ = $1; }
    | block       { $$ = $1; }
    | fun_decl    { $$ = $1; }
    | class_decl  { $$ = $1; }
;

var_decl:
      VAR IDENTIFIER EQUAL expression SEMICOLON { $$ = criarNoVarDecl($2, $4); }
    | VAR IDENTIFIER SEMICOLON { $$ = criarNoVarDecl($2, NULL); }
;

print_stmt:
    PRINT expression SEMICOLON { $$ = criarNoPrintStmt($2); }
;

return_stmt:
      RETURN SEMICOLON { $$ = criarNoReturn(NULL); }
    | RETURN expression SEMICOLON { $$ = criarNoReturn($2); }
;

if_stmt:
      IF LPAREN expression RPAREN statement 
          { $$ = criarNoIf($3, $5, NULL); }
    | IF LPAREN expression RPAREN statement ELSE statement
          { $$ = criarNoIf($3, $5, $7); }
;

while_stmt:
    WHILE LPAREN expression RPAREN block 
          { $$ = criarNoWhile($3, $5); }
;

for_stmt:
    FOR LPAREN for_init SEMICOLON for_condition SEMICOLON for_increment RPAREN block
          { $$ = criarNoFor($3, $5, $7, $9); }
;

for_init:
      /* vazio */ { $$ = NULL; }
    | var_decl_no_semicolon { $$ = $1; }
    | expr_stmt_no_semicolon { $$ = $1; }
;

for_condition:
      /* vazio */ { $$ = NULL; } /* Será tratado como 'true' na semântica */
    | expression { $$ = $1; }
;

for_increment:
      /* vazio */ { $$ = NULL; }
    | expression { $$ = $1; }
;

var_decl_no_semicolon:
      VAR IDENTIFIER EQUAL expression { $$ = criarNoVarDecl($2, $4); }
    | VAR IDENTIFIER { $$ = criarNoVarDecl($2, NULL); }
;

expr_stmt_no_semicolon:
    expression { $$ = criarNoExprStmt($1); }
;

expr_stmt:
    expression SEMICOLON { $$ = criarNoExprStmt($1); }
;

fun_decl:
    FUN IDENTIFIER LPAREN params RPAREN block
          { $$ = criarNoFunDecl($2, $4, $6); }
;

params:
      /* vazio */ { $$ = NULL; }
    | IDENTIFIER { $$ = criarNoParam($1); }
    | params COMMA IDENTIFIER { $$ = anexarNoLista($1, criarNoParam($3)); }
;

class_decl:
    CLASS IDENTIFIER LBRACE method_declarations RBRACE
          { $$ = criarNoClassDecl($2, $4); }
;

method_declarations:
      /* vazio */ { $$ = NULL; }
    | method_declarations method_decl { $$ = anexarNoLista($1, $2); }
;

method_decl:
    /* Um método é apenas uma declaração de função */
    IDENTIFIER LPAREN params RPAREN block
          { $$ = criarNoFunDecl($1, $3, $5); }
;

expression:
    assignment { $$ = $1; }
;

assignment:
      call DOT IDENTIFIER EQUAL assignment 
          { $$ = criarNoSetAttr($1, $3, $5); }
    | IDENTIFIER EQUAL assignment 
          { $$ = criarNoAssign($1, $3); }
    | logic_or { $$ = $1; }
;

logic_or:
      logic_and { $$ = $1; }
    | logic_or OR logic_and { $$ = criarNoOpLogico(OR, $1, $3); }
;

logic_and:
      comparison { $$ = $1; }
    | logic_and AND comparison { $$ = criarNoOpLogico(AND, $1, $3); }
;

comparison:
      addition { $$ = $1; }
    | comparison GREATER addition { $$ = criarNoOpBinario(GREATER, $1, $3); }
    | comparison GREATER_EQUAL addition { $$ = criarNoOpBinario(GREATER_EQUAL, $1, $3); }
    | comparison LESS addition { $$ = criarNoOpBinario(LESS, $1, $3); }
    | comparison LESS_EQUAL addition { $$ = criarNoOpBinario(LESS_EQUAL, $1, $3); }
    | comparison EQUAL_EQUAL addition { $$ = criarNoOpBinario(EQUAL_EQUAL, $1, $3); }
    | comparison BANG_EQUAL addition { $$ = criarNoOpBinario(BANG_EQUAL, $1, $3); }
;

addition:
      multiplication { $$ = $1; }
    | addition PLUS multiplication { $$ = criarNoOpBinario(PLUS, $1, $3); }
    | addition MINUS multiplication { $$ = criarNoOpBinario(MINUS, $1, $3); }
;

multiplication:
      unary { $$ = $1; }
    | multiplication STAR unary { $$ = criarNoOpBinario(STAR, $1, $3); }
    | multiplication SLASH unary { $$ = criarNoOpBinario(SLASH, $1, $3); }
;

unary:
      call { $$ = $1; }
    | BANG unary { $$ = criarNoOpUnario(BANG, $2); }
    | MINUS unary { $$ = criarNoOpUnario(MINUS, $2); }
;

call:
    primary { $$ = $1; }
    | call LPAREN arguments RPAREN { $$ = criarNoCall($1, $3); }
    | call DOT IDENTIFIER { $$ = criarNoGetAttr($1, $3); }
;

primary:
    NUM { $$ = criarNoNum($1); }
    | STRING { $$ = criarNoString($1); }
    | IDENTIFIER { $$ = criarNoIdentifier($1); }
    | TRUE { $$ = criarNoBool(1); }
    | FALSE { $$ = criarNoBool(0); }
    | NIL { $$ = criarNoNil(); }
    | THIS { $$ = criarNoThis(); }
    | LPAREN expression RPAREN { $$ = $2; }
;

arguments:
      /* vazio */ { $$ = NULL; }
    | expression { $$ = $1; }
    | arguments COMMA expression { $$ = anexarNoLista($1, $3); }
;

block:
    LBRACE statements RBRACE { $$ = criarNoBlock($2); }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintatico na linha %d: %s\n", yylineno, s);
}