/*
 * Defines B-minor's token types for use by the scanner.
 *
 * Keywords and operators are differentiated explicitly as separate token types
 * to avoid having to access data about specific types during parsing.
 *
 * (Some cases will require differentiation at parsing, however, such as separating
 * subtraction from the unary negation operator.)
*/

#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    TOKEN_EOF=0,
    TOKEN_CURLY_LEFT,         /* { */
    TOKEN_CURLY_RIGHT,        /* } */
    TOKEN_ERROR,              /* for invalid tokens */
    TOKEN_IDENT,              /* non-keyword identifiers */
    /*          KEYWORDS          */
    TOKEN_KW_ARRAY,           /* "array" */
    TOKEN_KW_BOOLEAN,         /* "boolean" */
    TOKEN_KW_CHAR,            /* "char" */
    TOKEN_KW_ELSE,            /* "else" */
    TOKEN_KW_FALSE,           /* "false" */
    TOKEN_KW_FOR,             /* "for" */
    TOKEN_KW_FUNCTION,        /* "function" */
    TOKEN_KW_IF,              /* "if" */
    TOKEN_KW_INTEGER,         /* "integer" */
    TOKEN_KW_PRINT,           /* "print" */
    TOKEN_KW_RETURN,          /* "return" */
    TOKEN_KW_STRING,          /* "string" */
    TOKEN_KW_TRUE,            /* "true" */
    TOKEN_KW_VOID,            /* "void" */
    TOKEN_KW_WHILE,           /* "while" */
    /*          LITERALS          */
    TOKEN_LIT_CHAR,           /* char value */
    TOKEN_LIT_INTEGER,        /* int value */
    TOKEN_LIT_STRING,         /* string value */
    /*          OPERATORS         */
    TOKEN_OP_ADD,             /* + */
    TOKEN_OP_AND,             /* && */
    TOKEN_OP_ASSIGN,          /* = */
    TOKEN_OP_DEC,             /* -- */
    TOKEN_OP_DIV,             /* / */
    TOKEN_OP_EQ,              /* == */
    TOKEN_OP_EXP,             /* ^ */
    TOKEN_OP_GE,              /* >= */
    TOKEN_OP_GT,              /* > */
    TOKEN_OP_INC,             /* ++ */
    TOKEN_OP_LE,              /* <= */
    TOKEN_OP_LT,              /* < */
    TOKEN_OP_MOD,             /* % */
    TOKEN_OP_MULT,            /* * */
    TOKEN_OP_NE,              /* != */
    TOKEN_OP_NOT,             /* ! */
    TOKEN_OP_OR,              /* || */
    TOKEN_OP_SUB,             /* - (parser should check unary case) */
    /*                            */
    TOKEN_PAREN_LEFT,         /* ( */
    TOKEN_PAREN_RIGHT,        /* ) */
    TOKEN_SEMICOLON,          /* ; */
    TOKEN_SQ_LEFT,            /* [ */
    TOKEN_SQ_RIGHT,           /* ] */
} token_t;

#endif
