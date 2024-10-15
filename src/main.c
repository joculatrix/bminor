#include "ast.h"
#include <stdio.h>

extern FILE *yyin;
extern int yyparse();
extern struct decl* parser_result;

extern void scope_enter();
extern void scope_exit();
extern void decl_resolve(struct decl* d);

extern void decl_typecheck(struct decl* d);

enum {ARG_NAME,ARG_FILE,ARG_NARGS};

int main(int argc, char** argv) {
    /* verify number of arguments is correct */
    if (argc != ARG_NARGS) {
        fprintf(stderr, "Usage: scanner filename\n");
        return 1;
    }
    /* open file to parse */
    yyin = fopen(argv[ARG_FILE], "r");
    if(!yyin) {
        fprintf(stderr, "could not open file: %s\n", argv[ARG_FILE]);
        return 1;
    }
    /* parse */
    if (yyparse()==0) {
        printf("Parsed successfully.\n");
        
        /* resolve names */
        scope_enter();
        decl_resolve(parser_result);
        scope_exit();

        /* type check */
        decl_typecheck(parser_result);
    } else {
        printf("Parse failed.\n");
    }
    /* close file */
    if (fclose(yyin) != 0) {
        fprintf(stderr, "could not close file\n");
        return 1;
    }

    return 0;
}
