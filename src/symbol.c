#include "symbol.h"
#include "semantics.h"

/*
 * SYMBOL TABLE AND SCOPE MANAGEMENT
 */

stack* symbol_stack = NULL;
bool main_exists = false;

symbol* symbol_create(symbol_t kind, type* type, char* name) {
    symbol* s = malloc(sizeof(*s));
    s->kind = kind;
    s->type = type;
    s->name = name;
    return s;
}

void scope_enter() {
    if (symbol_stack == NULL) {
        symbol_stack = stack_new();
        if (symbol_stack == NULL) {
            fprintf(stderr, "error: could not allocate symbol stack\n");
            exit(1);
        }
    }
    ht* table = ht_create();
    push(symbol_stack, table);
}

void scope_exit() {
    if (pop(symbol_stack) == NULL) {
        fprintf(stderr, "error: attempt to pop nonexistent table from stack\n");
        exit(1);
    }
}

int scope_level() {
    return symbol_stack->length;
}

void scope_bind(const char* name, symbol* sym) {
    list_node* scope = peek(symbol_stack);
    if (scope == NULL) {
        fprintf(
            stderr,
            "error: attempt to bind symbol `%s` to nonexistent table\n",
            name
        );
        exit(1);
    }
    sym->name = name;
    if (ht_set(scope->table, name, (void*)sym) == NULL) {
        fprintf(
            stderr,
            "error: couldn't add symbol `%s` to table\n",
            name
        );
        exit(1);
    }
}

symbol* scope_lookup(const char* name) {
    if (symbol_stack == NULL) {
        return NULL;
    }
    list_node* stack_p = peek(symbol_stack);
    while (stack_p != NULL) {
        void* sym = ht_get(stack_p->table, name);
        if (sym != NULL) {
            return (symbol*)sym;
        }
        stack_p = stack_p->next;
    }
    return NULL;
}

symbol* scope_lookup_current(const char* name) {
    if (symbol_stack == NULL) {
        return NULL;
    }
    void* sym = ht_get(symbol_stack->head->table, name);
    if (sym != NULL) {
        return (symbol*)sym;
    } else {
        return NULL;
    }
}

/*
 * NAME RESOLUTION
 */

void decl_resolve(decl* d) {
    if (!d) return;

    if (scope_lookup_current(d->name) != NULL) {
        fprintf(
            stderr,
            "error: attempt to re-declare identifier `%s` in same scope ",
            d->name
        );
        fprintf(
            stderr,
            "(did you mean to assign `=` a new value?)\n"
        );
    } else {
        symbol_t kind = scope_level() > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL;
        d->symbol = symbol_create(kind, d->type, d->name);

        expr_resolve(d->value);

        if (d->type == TYPE_ARRAY) {
            d->value->symbol = d->symbol;
        }
        
        if (d->code) {
            if (d->name == "main") {
                if (d->type->subtype == "integer") {
                    main_exists = true;
                    d->name = "_start";
                } else {
                    fprintf(
                        stderr,
                        "error: expected `main` to return type `integer`\n"
                    );
                }
            }
            scope_bind(d->name, d->symbol);

            scope_enter();
            param_list_resolve(d->type->params);
            stmt_resolve(d->code);
            scope_exit();
        } else {
            scope_bind(d->name, d->symbol);
        }
    }

    decl_resolve(d->next);
}

void expr_resolve(expr* e) {
    if (!e) return;

    if (e->kind == EXPR_IDENT) {
        e->symbol = scope_lookup(e->name);
        if (e->symbol == NULL) {
            fprintf(
                stderr,
                "error: attempt to use undeclared variable `%s`\n",
                e->name
            );
        }
        /* in case the IDENT is a function call argument: */
        expr_resolve(e->right);
    } else if (e->kind == EXPR_FUN_CALL) {
        e->left->symbol = scope_lookup(e->left->name);
        if (e->left->symbol == NULL) {
            fprintf(
                stderr,
                "error: attempt to call undeclared function `%s` ",
                e->name
            );
            fprintf(
                stderr,
                "(functions must be defined or prototyped before call)\n"
            );
        }
        expr_resolve(e->right);
    } else {
        expr_resolve(e->left);
        expr_resolve(e->right);
    }
}

void stmt_resolve(stmt* s) {
    if (!s) return;

    switch (s->kind) {
        case STMT_DECL:
            decl_resolve(s->decl);
            break;
        case STMT_EXPR:
            expr_resolve(s->expr);
            break;
        case STMT_IF_ELSE:
            expr_resolve(s->expr);

            scope_enter();
            stmt_resolve(s->body);
            scope_exit();

            if (s->else_body != NULL) {
                scope_enter();
                stmt_resolve(s->else_body);
                scope_exit();
            }
            break;
        case STMT_FOR:
            scope_enter();

            expr_resolve(s->init_expr);
            expr_resolve(s->expr);
            expr_resolve(s->next_expr);

            stmt_resolve(s->body);

            scope_exit();
            break;
        case STMT_PRINT:
            expr_resolve(s->expr);
            break;
        case STMT_RETURN:
            expr_resolve(s->expr);
            break;
        case STMT_BLOCK:
            scope_enter();
            stmt_resolve(s->body);
            scope_exit();
            break;
    }

    stmt_resolve(s->next);
}

void param_list_resolve(param_list* p) {
    if (!p) return;

    p->symbol = symbol_create(SYMBOL_PARAM, p->type, p->name);
    scope_bind(p->name, p->symbol);

    param_list_resolve(p->next);
}