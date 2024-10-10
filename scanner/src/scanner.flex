%{
#include "token.h"
#include <stdio.h>

#define MAX_STR_CONST 256

char str_buf[MAX_STR_CONST];
char* str_buf_ptr;

int curr_line;

void err_print(const char*);

%}

%x COMMENT
%x STR

DIGIT   [0-9]
LETTER  [a-zA-Z]

%%

    /*          WHITESPACE        */

[ \b\t\f]+    /* eat whitespace */

    /*          SEMICOLONS        */

";"           return TOKEN_SEMICOLON;

    /*          NEWLINES          */

\n            ++curr_line;

    /*          LITERALS          */

{DIGIT}+      return TOKEN_LIT_INTEGER;

\'.\'         return TOKEN_LIT_CHAR;

\'\\n\'       return TOKEN_LIT_CHAR;

\'\\0\'       return TOKEN_LIT_CHAR;

    /*          STRINGS           */

\" {
    str_buf_ptr = str_buf;
    BEGIN(STR);
}

<STR>{
    \"  {
        BEGIN(INITIAL);
        *str_buf_ptr = '\0';
        return TOKEN_LIT_STRING;
    }

    \\\n      ++curr_line;

    \\[^n0]   *str_buf_ptr++ = yytext[1];

    \\[n]     *str_buf_ptr++ = '\n';

    \\[0]     *str_buf_ptr++ = '\0';

    <<EOF>> {
        err_print("EOF in string");
        return TOKEN_ERROR;
    }

    \n {
        BEGIN(INITIAL);
        ++curr_line;
        err_print("unescaped newline in string");
        return TOKEN_ERROR;
    }

    \0 {
        err_print("unescaped null in string");
        return TOKEN_ERROR;
    }

    .         *str_buf_ptr++ = *yytext;
}

    /*          BRACKETS          */

"{"           return TOKEN_CURLY_LEFT;

"}"           return TOKEN_CURLY_RIGHT;

"("           return TOKEN_PAREN_LEFT;

")"           return TOKEN_PAREN_RIGHT;

"["           return TOKEN_SQ_LEFT;

"]"           return TOKEN_SQ_RIGHT;

    /*          COMMENTS          */

"//"[^\n]*\n  ++curr_line;

"/*"          BEGIN(COMMENT);

<COMMENT>{
    <<EOF>> {
        err_print("EOF in comment");
        return TOKEN_ERROR;
    }

    [^*\n]*       /* eat comment contents */

    "*"+[^*/\n]*  /* eat asterisks without closing slash in comments */

    \n            ++curr_line;

    "*/"          BEGIN(INITIAL);
}

    /*          KEYWORDS          */

"array"       return TOKEN_KW_ARRAY;

"boolean"     return TOKEN_KW_BOOLEAN;

"char"        return TOKEN_KW_CHAR;

"else"        return TOKEN_KW_ELSE;

"false"       return TOKEN_KW_FALSE;

"for"         return TOKEN_KW_FOR;

"function"    return TOKEN_KW_FUNCTION;

"if"          return TOKEN_KW_IF;

"integer"     return TOKEN_KW_INTEGER;

"print"       return TOKEN_KW_PRINT;

"return"      return TOKEN_KW_RETURN;

"string"      return TOKEN_KW_STRING;

"true"        return TOKEN_KW_TRUE;

"void"        return TOKEN_KW_VOID;

"while"       return TOKEN_KW_WHILE;

    /*          IDENTIFIERS       */

{LETTER}({LETTER}|{DIGIT}|"_")* {
    return TOKEN_IDENT;
}

    /*          OPERATORS         */

"&&"          return TOKEN_OP_AND;

"||"          return TOKEN_OP_OR;

"!="          return TOKEN_OP_NE;

"!"           return TOKEN_OP_NOT;

"=="          return TOKEN_OP_EQ;

">="          return TOKEN_OP_GE;

"<="          return TOKEN_OP_LE;

">"           return TOKEN_OP_GT;

"<"           return TOKEN_OP_LT;

"="           return TOKEN_OP_ASSIGN;

"--"          return TOKEN_OP_DEC;

"++"          return TOKEN_OP_INC;

"-"           return TOKEN_OP_SUB;

"+"           return TOKEN_OP_ADD;

"^"           return TOKEN_OP_EXP;

"*"           return TOKEN_OP_MULT;

"/"           return TOKEN_OP_DIV;

"%"           return TOKEN_OP_MOD;

    /*            ERROR           */

. {
    err_print("unexpected token");
    return TOKEN_ERROR;
}

%%

void err_print(const char* e) {
    fprintf(stderr, "Line %d: %s at `%s`\n", curr_line, e, yytext);
}

int yywrap() { return 1; }
