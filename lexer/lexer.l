%option noyywrap
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.tab.h"  
%}

%%

"//".*                          { /* comentário de linha */ }
[ \t\r]+                        { /* espaço em branco */ }
\n                              { yylineno++; }

"and"                           { return AND; }
"class"                         { return CLASS; }
"else"                          { return ELSE; }
"false"                         { return FALSE; }
"for"                           { return FOR; }
"fun"                           { return FUN; }
"if"                            { return IF; }
"nil"                           { return NIL; }
"or"                            { return OR; }
"print"                         { return PRINT; }
"return"                        { return RETURN; }
"super"                         { return SUPER; }
"this"                          { return THIS; }
"true"                          { return TRUE; }
"var"                           { return VAR; }
"while"                         { return WHILE; }

"=="                            { return EQUAL_EQUAL; }
"="                             { return EQUAL; }
"!="                            { return BANG_EQUAL; }
"!"                             { return BANG; }
"<="                            { return LESS_EQUAL; }
"<"                             { return LESS; }
">="                            { return GREATER_EQUAL; }
">"                             { return GREATER; }
"("                             { return LPAREN; }
")"                             { return RPAREN; }
"{"                             { return LBRACE; }
"}"                             { return RBRACE; }
","                             { return COMMA; }
"."                             { return DOT; }
"-"                             { return MINUS; }
"+"                             { return PLUS; }
";"                             { return SEMICOLON; }
"*"                             { return STAR; }
"/"                             { return SLASH; }

[0-9]+(\.[0-9]+)?               { 
                                    yylval.number = atof(yytext);
                                    return NUM;
                                }

\"([^\\\"]|\\.)*\"              { 
                                    char* str = strdup(yytext + 1);
                                    str[strlen(str) - 1] = '\0';
                                    yylval.string = str;
                                    return STRING;
                                }

[0-9]+[a-zA-Z_][a-zA-Z0-9_]*    { 
                                    fprintf(stderr, "Identificador invalido na linha %d: '%s'\n", yylineno, yytext);
                                }

[a-zA-Z_][a-zA-Z0-9_]*          { 
                                    yylval.identifier = strdup(yytext);
                                    return IDENTIFIER;
                                }

.                               { 
                                    fprintf(stderr, "Caractere invalido na linha %d: '%s'\n", yylineno, yytext);
                                }

%%