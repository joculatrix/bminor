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

#ifndef YY_YY_INCLUDE_YY_H_INCLUDED
# define YY_YY_INCLUDE_YY_H_INCLUDED
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
    TOKEN_COMMA = 258,             /* TOKEN_COMMA  */
    TOKEN_COLON = 259,             /* TOKEN_COLON  */
    TOKEN_CURLY_LEFT = 260,        /* TOKEN_CURLY_LEFT  */
    TOKEN_CURLY_RIGHT = 261,       /* TOKEN_CURLY_RIGHT  */
    TOKEN_ERROR = 262,             /* TOKEN_ERROR  */
    TOKEN_IDENT = 263,             /* TOKEN_IDENT  */
    TOKEN_KW_ARRAY = 264,          /* TOKEN_KW_ARRAY  */
    TOKEN_KW_BOOLEAN = 265,        /* TOKEN_KW_BOOLEAN  */
    TOKEN_KW_CHAR = 266,           /* TOKEN_KW_CHAR  */
    TOKEN_KW_ELSE = 267,           /* TOKEN_KW_ELSE  */
    TOKEN_KW_FALSE = 268,          /* TOKEN_KW_FALSE  */
    TOKEN_KW_FOR = 269,            /* TOKEN_KW_FOR  */
    TOKEN_KW_FUNCTION = 270,       /* TOKEN_KW_FUNCTION  */
    TOKEN_KW_IF = 271,             /* TOKEN_KW_IF  */
    TOKEN_KW_INTEGER = 272,        /* TOKEN_KW_INTEGER  */
    TOKEN_KW_PRINT = 273,          /* TOKEN_KW_PRINT  */
    TOKEN_KW_RETURN = 274,         /* TOKEN_KW_RETURN  */
    TOKEN_KW_STRING = 275,         /* TOKEN_KW_STRING  */
    TOKEN_KW_TRUE = 276,           /* TOKEN_KW_TRUE  */
    TOKEN_KW_VOID = 277,           /* TOKEN_KW_VOID  */
    TOKEN_KW_WHILE = 278,          /* TOKEN_KW_WHILE  */
    TOKEN_LIT_CHAR = 279,          /* TOKEN_LIT_CHAR  */
    TOKEN_LIT_INTEGER = 280,       /* TOKEN_LIT_INTEGER  */
    TOKEN_LIT_STRING = 281,        /* TOKEN_LIT_STRING  */
    TOKEN_OP_ADD = 282,            /* TOKEN_OP_ADD  */
    TOKEN_OP_AND = 283,            /* TOKEN_OP_AND  */
    TOKEN_OP_ASSIGN = 284,         /* TOKEN_OP_ASSIGN  */
    TOKEN_OP_DEC = 285,            /* TOKEN_OP_DEC  */
    TOKEN_OP_DIV = 286,            /* TOKEN_OP_DIV  */
    TOKEN_OP_EQ = 287,             /* TOKEN_OP_EQ  */
    TOKEN_OP_EXP = 288,            /* TOKEN_OP_EXP  */
    TOKEN_OP_GE = 289,             /* TOKEN_OP_GE  */
    TOKEN_OP_GT = 290,             /* TOKEN_OP_GT  */
    TOKEN_OP_INC = 291,            /* TOKEN_OP_INC  */
    TOKEN_OP_LE = 292,             /* TOKEN_OP_LE  */
    TOKEN_OP_LT = 293,             /* TOKEN_OP_LT  */
    TOKEN_OP_MOD = 294,            /* TOKEN_OP_MOD  */
    TOKEN_OP_MULT = 295,           /* TOKEN_OP_MULT  */
    TOKEN_OP_NE = 296,             /* TOKEN_OP_NE  */
    TOKEN_OP_NOT = 297,            /* TOKEN_OP_NOT  */
    TOKEN_OP_OR = 298,             /* TOKEN_OP_OR  */
    TOKEN_OP_SUB = 299,            /* TOKEN_OP_SUB  */
    TOKEN_PAREN_LEFT = 300,        /* TOKEN_PAREN_LEFT  */
    TOKEN_PAREN_RIGHT = 301,       /* TOKEN_PAREN_RIGHT  */
    TOKEN_SEMICOLON = 302,         /* TOKEN_SEMICOLON  */
    TOKEN_SQ_LEFT = 303,           /* TOKEN_SQ_LEFT  */
    TOKEN_SQ_RIGHT = 304           /* TOKEN_SQ_RIGHT  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 8 "src/parser.y"

    char char_val;
    int int_val;
    char* str_val;
    char* text;
    struct decl* decl;
    struct stmt* stmt;
    struct expr* expr;
    struct type* type;
    struct param_list* param_list;

#line 125 "include/yy.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_INCLUDE_YY_H_INCLUDED  */
