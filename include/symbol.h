#ifndef SYMBOL_H
#define SYMBOL_H

#include "ast.h"

typedef enum {
    SYMBOL_LOCAL,
    SYMBOL_PARAM,
    SYMBOL_GLOBAL
} symbol_t;

struct symbol {
    symbol_t kind;
    struct type* type;
    char* name;
    int which;
};

struct symbol* symbol_create(symbol_t kind, struct type* type, char* name);

/* pushes a new symbol table on to the stack */
void scope_enter();
/* pops the topmost symbol table off the stack */
void scope_exit();
/* returns the current number of symbol tables on the stack
 * (identify global scope) */
int scope_level();
/* adds a symbol to the topmost symbol table, binding an identifier */
void scope_bind(const char* name, struct symbol* sym);
/* searches the stack for the most recent match (or null) */
struct symbol* scope_lookup(const char* name);
/* searches only the topmost table */
struct symbol* scope_lookup_current(const char* name);

#endif