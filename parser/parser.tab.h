/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    AND = 258,                     /* AND  */
    CLASS = 259,                   /* CLASS  */
    ELSE = 260,                    /* ELSE  */
    FALSE = 261,                   /* FALSE  */
    FOR = 262,                     /* FOR  */
    FUN = 263,                     /* FUN  */
    IF = 264,                      /* IF  */
    NIL = 265,                     /* NIL  */
    OR = 266,                      /* OR  */
    PRINT = 267,                   /* PRINT  */
    RETURN = 268,                  /* RETURN  */
    SUPER = 269,                   /* SUPER  */
    THIS = 270,                    /* THIS  */
    TRUE = 271,                    /* TRUE  */
    VAR = 272,                     /* VAR  */
    WHILE = 273,                   /* WHILE  */
    EQUAL_EQUAL = 274,             /* EQUAL_EQUAL  */
    EQUAL = 275,                   /* EQUAL  */
    BANG_EQUAL = 276,              /* BANG_EQUAL  */
    BANG = 277,                    /* BANG  */
    LESS_EQUAL = 278,              /* LESS_EQUAL  */
    LESS = 279,                    /* LESS  */
    GREATER_EQUAL = 280,           /* GREATER_EQUAL  */
    GREATER = 281,                 /* GREATER  */
    LPAREN = 282,                  /* LPAREN  */
    RPAREN = 283,                  /* RPAREN  */
    LBRACE = 284,                  /* LBRACE  */
    RBRACE = 285,                  /* RBRACE  */
    COMMA = 286,                   /* COMMA  */
    DOT = 287,                     /* DOT  */
    MINUS = 288,                   /* MINUS  */
    PLUS = 289,                    /* PLUS  */
    SEMICOLON = 290,               /* SEMICOLON  */
    STAR = 291,                    /* STAR  */
    SLASH = 292,                   /* SLASH  */
    NUM = 293,                     /* NUM  */
    STRING = 294,                  /* STRING  */
    IDENTIFIER = 295,              /* IDENTIFIER  */
    TIMES = 296,                   /* TIMES  */
    DIVIDE = 297                   /* DIVIDE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
