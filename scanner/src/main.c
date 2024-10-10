#include "token.h"
#include <stdio.h>

extern FILE *yyin;
extern int yylex();
extern char **yytext;

enum {ARG_NAME,ARG_FILE,ARG_NARGS};

int main(int argc, char** argv) {
    /* verify number of arguments is correct */
    if (argc != ARG_NARGS) {
        fprintf(stderr, "Usage: scanner filename\n");
        return 1;
    }
    /* open file to scan */
    yyin = fopen(argv[ARG_FILE], "r");
    if(!yyin) {
        fprintf(stderr, "could not open file: %s\n", argv[ARG_FILE]);
        return 1;
    }
    /* scan */
    while(1) {
        token_t t = yylex();
        if (t == TOKEN_EOF) break;
        else if (t != TOKEN_ERROR) {
            fprintf(stdout, "token: %d\n", t);
        }
    }
    /* close file */
    if (fclose(yyin) != 0) {
        fprintf(stderr, "could not close file\n");
        return 1;
    }

    return 0;
}
