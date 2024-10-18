#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

stmt* stmt_create(
    stmt_t kind,
    decl* decl,
    expr* init_expr,
    expr* exp,
    expr* next_expr,
    stmt* body,
    stmt* else_body,
    stmt* next
) {
    stmt* s = malloc(sizeof(*s));
    s->kind = kind;
    s->decl = decl;
    s->init_expr = init_expr;
    s->expr = exp;
    s->next_expr = next_expr;
    s->body = body;
    s->else_body = else_body;
    s->next = next;
    return s;
}

stmt* stmt_decl(decl* decl, stmt* next) {
    return stmt_create(STMT_DECL, decl, 0, 0, 0, 0, 0, next);
}

stmt* stmt_expr(expr* exp, stmt* next) {
    return stmt_create(STMT_EXPR, 0, 0, exp, 0, 0, 0, next);
}

stmt* stmt_if(expr* control, stmt* body, stmt* next) {
    return stmt_create(STMT_IF_ELSE, 0, 0, control, 0, body, 0, next);
}

stmt* stmt_if_else(
    expr* control,
    stmt* body,
    stmt* else_body,
    stmt* next
) {
    return stmt_create(STMT_IF_ELSE, 0, 0, control, 0, body, else_body, next);
}

stmt* stmt_for(
    expr* init_expr,
    expr* exp,
    expr* next_expr,
    stmt* body,
    stmt* next
) {
    return stmt_create(STMT_FOR, 0, init_expr, exp, next_expr, body, 0, next);
}

stmt* stmt_print(expr* exp, stmt* next) {
    return stmt_create(STMT_PRINT, 0, 0, exp, 0, 0, 0, next);
}

stmt* stmt_return(expr* exp, stmt* next) {
    return stmt_create(STMT_RETURN, 0, 0, exp, 0, 0, 0, next);
}

stmt* stmt_block(stmt* body, stmt* next) {
    return stmt_create(STMT_BLOCK, 0, 0, 0, 0, body, 0, next);
}

void print_stmt(stmt* stmt, int tab_level) {
    char tabs[MAX_INDENT] = { '\0' };
    char* tabs_ptr = tabs;

    for (int i = 0; i < tab_level; i++) {
        if (tabs_ptr - tabs > MAX_INDENT) {
            break;
        }
        *tabs_ptr++ = '\t';
    }

    printf("%sstmt {\n", tabs);

    printf("%s\t%s\n", tabs, stmt_t_str[stmt->kind]);
    
    switch (stmt->kind) {
        case STMT_DECL:
            print_decl(stmt->decl, tab_level + 1);
            break;
        case STMT_EXPR:
            print_expr(stmt->expr, tab_level + 1);
            break;
        case STMT_IF_ELSE:
            printf("%s\tcondition:\n", tabs);
            print_expr(stmt->expr, tab_level + 1);

            printf("%s\tbody:\n", tabs);
            print_stmt(stmt->body, tab_level + 1);

            if (stmt->else_body != 0) {
                printf("%s\telse body:\n", tabs);
                print_stmt(stmt->else_body, tab_level + 1);
            }
            break;
        case STMT_FOR:
            printf("%s\tinit expression:\n", tabs);
            print_expr(stmt->init_expr, tab_level + 2);
            
            printf("%s\tcondition:\n", tabs);
            print_expr(stmt->expr, tab_level + 2);

            printf("%s\tincrement:\n", tabs);
            print_expr(stmt->next_expr, tab_level + 2);

            printf("%s\tbody:\n", tabs);
            print_stmt(stmt->body, tab_level + 2);
            
            break;
        case STMT_PRINT:
        case STMT_RETURN:
            print_expr(stmt->expr, tab_level + 1);
            break;
        case STMT_BLOCK:
            print_stmt(stmt->body, tab_level + 1);
            break;
    }

    if (stmt->next != 0) {
        printf("%s},\n", tabs);
        print_stmt(stmt->next, tab_level);
    } else {
        printf("%s}\n", tabs);
    }
}
