%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);

typedef struct {
    double number;
    char *string;
    char *identifier;
    int boolean;
} yystype;

#define YYSTYPE yystype
%}

%token AND CLASS ELSE FALSE FOR FUN IF NIL OR PRINT RETURN SUPER THIS TRUE VAR WHILE
%token EQUAL_EQUAL EQUAL BANG_EQUAL BANG LESS_EQUAL LESS GREATER_EQUAL GREATER
%token LPAREN RPAREN LBRACE RBRACE COMMA DOT MINUS PLUS SEMICOLON STAR SLASH
%token NUM STRING IDENTIFIER


%left PLUS MINUS
%left TIMES DIVIDE

%%

expressao:
    expressao PLUS expressao
  | expressao MINUS expressao
  | expressao TIMES expressao
  | expressao DIVIDE expressao
  | LPAREN expressao RPAREN
  | NUM
  ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático: %s\n", s);
}

int main(void) {
    yyparse();
    return 0;
}
