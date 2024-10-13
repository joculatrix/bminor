#include "ast.h"
#include "symbol.h"
#include "hash.h"
#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * SYMBOL TABLE AND SCOPE MANAGEMENT
 */

stack* symbol_stack = NULL;

struct symbol* symbol_create(symbol_t kind, struct type* type, char* name) {
    struct symbol* s = malloc(sizeof(*s));
    s->kind = kind;
    s->type = type;
    s->name = name;
    return s;
}

void scope_enter() {
    if (symbol_stack == NULL) {
        symbol_stack = stack_new();
        if (symbol_stack == NULL) {
            fprintf(stderr, "Error: could not allocate symbol stack\n");
            exit(1);
        }
    }
    ht* table = ht_create();
    push(symbol_stack, table);
}

void scope_exit() {
    if (pop(symbol_stack) == NULL) {
        fprintf(stderr, "Error: attempt to pop nonexistent table from stack\n");
        exit(1);
    }
}

int scope_level() {
    return symbol_stack->length;
}

void scope_bind(const char* name, struct symbol* sym) {
    list_node* scope = peek(symbol_stack);
    if (scope == NULL) {
        fprintf(stderr, "Error: attempt to bind symbol to nonexistent table\n");
        exit(1);
    }
    sym->name = name;
    if (ht_set(scope->table, name, (void*)sym) == NULL) {
        fprintf(stderr, "Error: couldn't add symbol to table\n");
        exit(1);
    }
}

struct symbol* scope_lookup(const char* name) {
    if (symbol_stack == NULL) {
        return NULL;
    }
    list_node* stack_p = symbol_stack->head;
    while (stack_p != NULL) {
        void* sym = ht_get(stack_p->table, name);
        if (sym != NULL) {
            return (struct symbol*)sym;
        }
    }
    return NULL;
}

struct symbol* scope_lookup_current(const char* name) {
    if (symbol_stack == NULL) {
        return NULL;
    }
    void* sym = ht_get(symbol_stack->head->table, name);
    if (sym != NULL) {
        return (struct symbol*)sym;
    } else {
        return NULL;
    }
}

/*
 * NAME RESOLUTION
 */

void decl_resolve(struct decl* d) {
    if (!d) return;

    symbol_t kind = scope_level() > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL;

    d->symbol = symbol_create(kind, d->type, d->name);

    expr_resolve(d->value);
    scope_bind(d->name, d->symbol);

    if (d->code) {
        scope_enter();
        param_list_resolve(d->type->params);
        stmt_resolve(d->code);
        scope_exit();
    }

    decl_resolve(d->next);
}

void expr_resolve(struct expr* e) {
    if (!e) return;

    if (e->kind == EXPR_IDENT) {
        e->symbol = scope_lookup(e->name);
    } else {
        expr_resolve(e->left);
        expr_resolve(e->right);
    }
}