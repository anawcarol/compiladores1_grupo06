%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.tab.h"



%}

%%

"//".*                          { /* Comentário de linha - ignorar */ }
[ \t\r]+                        { /* Espaços em branco - ignorar */ }
\n                              { yylineno++; }

"and"                           { printf("AND\n"); return AND; }
"class"                         { printf("CLASS\n"); return CLASS; }
"else"                          { printf("ELSE\n"); return ELSE; }
"false"                         { printf("FALSE\n"); yylval.boolean = 0; return FALSE; }
"for"                           { printf("FOR\n"); return FOR; }
"fun"                           { printf("FUN\n"); return FUN; }
"if"                            { printf("IF\n"); return IF; }
"nil"                           { printf("NIL\n"); return NIL; }
"or"                            { printf("OR\n"); return OR; }
"print"                         { printf("PRINT\n"); return PRINT; }
"return"                        { printf("RETURN\n"); return RETURN; }
"super"                         { printf("SUPER\n"); return SUPER; }
"this"                          { printf("THIS\n"); return THIS; }
"true"                          { printf("TRUE\n"); yylval.boolean = 1; return TRUE; }
"var"                           { printf("VAR\n"); return VAR; }
"while"                         { printf("WHILE\n"); return WHILE; }

"=="                            { printf("EQUAL_EQUAL\n"); return EQUAL_EQUAL; }
"="                             { printf("EQUAL\n"); return EQUAL; }
"!="                            { printf("BANG_EQUAL\n"); return BANG_EQUAL; }
"!"                             { printf("BANG\n"); return BANG; }
"<="                            { printf("LESS_EQUAL\n"); return LESS_EQUAL; }
"<"                             { printf("LESS\n"); return LESS; }
">="                            { printf("GREATER_EQUAL\n"); return GREATER_EQUAL; }
">"                             { printf("GREATER\n"); return GREATER; }
"("                             { printf("LEFT_PAREN\n"); return LEFT_PAREN; }
")"                             { printf("RIGHT_PAREN\n"); return RIGHT_PAREN; }
"{"                             { printf("LEFT_BRACE\n"); return LEFT_BRACE; }
"}"                             { printf("RIGHT_BRACE\n"); return RIGHT_BRACE; }
","                             { printf("COMMA\n"); return COMMA; }
"."                             { printf("DOT\n"); return DOT; }
"-"                             { printf("MINUS\n"); return MINUS; }
"+"                             { printf("PLUS\n"); return PLUS; }
";"                             { printf("SEMICOLON\n"); return SEMICOLON; }
"*"                             { printf("STAR\n"); return STAR; }
"/"                             { printf("SLASH\n"); return SLASH; }

[0-9]+(\.[0-9]+)?               { 
                                  printf("NUMBER(%s)\n", yytext);
                                  yylval.number = atof(yytext); 
                                  return NUMBER; 
                               }

\"([^\\\"]|\\.)*\"              { 
                                  printf("STRING(%s)\n", yytext);
                                  yylval.string = strdup(yytext + 1); 
                                  yylval.string[strlen(yylval.string) - 1] = '\0'; 
                                  return STRING; 
                               }

[a-zA-Z_][a-zA-Z0-9_]*          { 
                                  printf("IDENTIFIER(%s)\n", yytext); 
                                  yylval.identifier = strdup(yytext);
                                  return IDENTIFIER; 
                               }

.                               { printf("Caractere inválido: '%s'\n", yytext); }

%%

