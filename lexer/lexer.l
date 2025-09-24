%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

%}

%%

"//".*                          { /* Comentário de linha - ignorar */ }
[ \t\r]+                        { /* Espaços em branco - ignorar */ }
\n                              { yylineno++; }

"and"                           { printf("AND\n"); }
"class"                         { printf("CLASS\n"); }
"else"                          { printf("ELSE\n"); }
"false"                         { printf("FALSE\n"); }
"for"                           { printf("FOR\n"); }
"fun"                           { printf("FUN\n"); }
"if"                            { printf("IF\n"); }
"nil"                           { printf("NIL\n"); }
"or"                            { printf("OR\n"); }
"print"                         { printf("PRINT\n"); }
"return"                        { printf("RETURN\n"); }
"super"                         { printf("SUPER\n"); }
"this"                          { printf("THIS\n"); }
"true"                          { printf("TRUE\n"); }
"var"                           { printf("VAR\n"); }
"while"                         { printf("WHILE\n"); }

"=="                            { printf("EQUAL_EQUAL\n"); }
"="                             { printf("EQUAL\n"); }
"!="                            { printf("BANG_EQUAL\n"); }
"!"                             { printf("BANG\n"); }
"<="                            { printf("LESS_EQUAL\n"); }
"<"                             { printf("LESS\n"); }
">="                            { printf("GREATER_EQUAL\n"); }
">"                             { printf("GREATER\n"); }
"("                             { printf("LEFT_PAREN\n"); }
")"                             { printf("RIGHT_PAREN\n"); }
"{"                             { printf("LEFT_BRACE\n"); }
"}"                             { printf("RIGHT_BRACE\n"); }
","                             { printf("COMMA\n"); }
"."                             { printf("DOT\n"); }
"-"                             { printf("MINUS\n"); }
"+"                             { printf("PLUS\n"); }
";"                             { printf("SEMICOLON\n"); }
"*"                             { printf("STAR\n"); }
"/"                             { printf("SLASH\n"); }

[0-9]+(\.[0-9]+)?               { 
                                  printf("NUMBER(%s)\n", yytext);
                               }

\"([^\\\"]|\\.)*\"              { 
                                  char* str = strdup(yytext + 1);
                                  str[strlen(str) - 1] = '\0';
                                  printf("STRING(%s)\n", str);
                                  free(str);
                               }

[0-9]+[a-zA-Z_][a-zA-Z0-9_]*          { 
                                  { printf("Identificador invalido: '%s'\n", yytext); }
                               }
                               
[a-zA-Z_][a-zA-Z0-9_]*          { 
                                  printf("IDENTIFIER(%s)\n", yytext);
                               }

.                               { printf("Caractere invalido: '%s'\n", yytext); }

%%