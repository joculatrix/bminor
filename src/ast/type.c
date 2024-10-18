#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

type* type_create(
    type_t kind,
    type* subtype,
    param_list* params
) {
    type* t = malloc(sizeof(*t));
    t->kind = kind;
    t->subtype = subtype;
    t->params = params;
    return t;
}

type* type_data(type_t kind) {
    return type_create(kind, 0, 0);
}

type* type_array(type* subtype) {
    return type_create(TYPE_ARRAY, subtype, 0);
}

type* type_function(type* subtype, param_list* params) {
    return type_create(TYPE_FUNCTION, subtype, params);
}

void print_type(type* type, int tab_level) {
    char tabs[MAX_INDENT] = { '\0' };
    char* tabs_ptr = tabs;

    for (int i = 0; i < tab_level; i++) {
        if (tabs_ptr - tabs > MAX_INDENT) {
            break;
        }
        *tabs_ptr++ = '\t';
    }

    printf("%stype {\n", tabs);

    switch (type->kind) {
        case TYPE_VOID:
            printf("%s\tvoid\n", tabs);
            break;
        case TYPE_BOOLEAN:
            printf("%s\tboolean\n", tabs);
            break;
        case TYPE_CHARACTER:
            printf("%s\tchar\n", tabs);
            break;
        case TYPE_INTEGER:
            printf("%s\tinteger\n", tabs);
            break;
        case TYPE_STRING:
            printf("%s\tstring\n", tabs);
            break;
        case TYPE_ARRAY:
            printf("%s\tarray []\n", tabs);
            print_type(type->subtype, tab_level + 1);
            break;
        case TYPE_FUNCTION:
            printf("%s\tfunction ()\n", tabs);
            print_type(type->subtype, tab_level + 1);
            
            if (type->params != 0) {
                printf("%s\tparams: [\n", tabs);
                print_param_list(type->params, tab_level + 2);
                printf("%s\t]\n", tabs);
            }
            break;
    }

    printf("%s}\n", tabs);
}
