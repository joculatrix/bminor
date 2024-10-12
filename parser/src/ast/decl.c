#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

struct decl* decl_create(
    char* name,
    struct type* type,
    struct expr* value,
    struct stmt* code,
    struct decl* next
) {
    struct decl* d = malloc(sizeof(*d));
    d->name = name;
    d->type = type;
    d->value = value;
    d->code = code;
    d->next = next;
    return d;
}

struct decl* decl_variable(
    char* name,
    struct type* type,
    struct expr* value,
    struct decl* next
) {
    return decl_create(name, type, value, 0, next);
}

struct decl* decl_prototype(char* name, struct type* type, struct decl* next) {
return decl_create(name, type, 0, 0, next);
}

struct decl* decl_function(
    char* name,
    struct type* type,
    struct stmt* code,
    struct decl* next
) {
    return decl_create(name, type, 0, code, next);
}

void print_decl(struct decl* decl, int tab_level) {
    char tabs[MAX_INDENT] = { '\0' };
    char* tabs_ptr = tabs;

    for (int i = 0; i < tab_level; i++) {
        if (tabs_ptr - tabs > MAX_INDENT) {
            break;
        }
        *tabs_ptr++ = '\t';
    }

    printf("%sdecl {\n", tabs);

    printf("%s\tname: %s\n", tabs, decl->name);

    print_type(decl->type, tab_level + 1);
    
    if (decl->value != 0) {
        printf("%s\tvalue:\n", tabs);
        print_expr(decl->value, tab_level + 2);
    }

    if (decl->code != 0) {
        printf("%s\tcode:\n", tabs);
        print_stmt(decl->code, tab_level + 2);
    }
    
    if (decl->next != 0) {
        printf("%s},\n", tabs);
        print_decl(decl->next, tab_level);
    } else {
        printf("%s}\n", tabs);
    }
}
