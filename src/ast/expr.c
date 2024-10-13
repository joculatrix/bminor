#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

struct expr* expr_create(
    expr_t kind,
    struct expr* left,
    struct expr* right,
    char* name,
    int value,
    const char* str_value
) {
    struct expr* e = malloc(sizeof(*e));
    e->kind = kind;
    e->left = left;
    e->right = right;
    e->name = name;
    e->value = value;
    e->str_value = str_value;
    return e;
}

struct expr* expr_ident(char* name) {
    return expr_create(EXPR_IDENT, 0, 0, name, 0, 0);
}

struct expr* expr_binary(expr_t kind, struct expr* left, struct expr* right) {
    return expr_create(kind, left, right, 0, 0, 0);
}

struct expr* expr_unary(expr_t kind, struct expr* left) {
    return expr_create(kind, left, 0, 0, 0, 0);
}

struct expr* expr_int_lit(int value) {
    return expr_create(EXPR_INT_LIT, 0, 0, 0, value, 0);
}

struct expr* expr_bool_lit(bool value) {
    return expr_create(EXPR_BOOL_LIT, 0, 0, 0, value, 0);
}

struct expr* expr_char_lit(char value) {
    return expr_create(EXPR_CHAR_LIT, 0, 0, 0, atoi(&value), 0);
}

struct expr* expr_str_lit(const char* value) {
    return expr_create(EXPR_STR_LIT, 0, 0, 0, 0, value);
}

void print_expr(struct expr* expr, int tab_level) {
    char tabs[MAX_INDENT] = { '\0' };
    char* tabs_ptr = tabs;

    for (int i = 0; i < tab_level; i++) {
        if (tabs_ptr - tabs > MAX_INDENT) {
            break;
        }
        *tabs_ptr++ = '\t';
    }

    printf("%sexpr {\n", tabs);
    
    printf("%s\t%s\n", tabs, expr_t_str[expr->kind]);

    if (expr->name != 0) {
        printf("%s\tname: %s\n", tabs, expr->name);
    }

    if (
        expr->kind == EXPR_BOOL_LIT ||
        expr->kind == EXPR_CHAR_LIT ||
        expr->kind == EXPR_INT_LIT
    ) {
        printf("%s\tvalue: %d\n", tabs, expr->value);
    }

    if (expr->kind == EXPR_STR_LIT) {
        printf("%s\tvalue: %s\n", tabs, expr->str_value);
    }

    if (expr->left != 0) {
        printf("%s\tleft:\n", tabs);
        print_expr(expr->left, tab_level + 2);
    }

    if (expr->right != 0) {
        printf("%s\tright:\n", tabs);
        print_expr(expr->right, tab_level + 2);
    }

    printf("%s}\n", tabs);
}
