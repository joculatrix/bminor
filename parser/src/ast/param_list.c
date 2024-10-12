#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

struct param_list* param_list(
    char* name,
    struct type* type,
    struct param_list* params
) {
    struct param_list* p = malloc(sizeof(*p));
    p->name = name;
    p->type = type;
    p->next = params;
    return p;
}

void print_param_list(struct param_list* param, int tab_level) {
    char tabs[MAX_INDENT] = { '\0' };
    char* tabs_ptr = tabs;

    for (int i = 0; i < tab_level; i++) {
        if (tabs_ptr - tabs > MAX_INDENT) {
            break;
        }
        *tabs_ptr++ = '\t';
    }

    printf("%sparam {\n", tabs);

    printf("%s\tname: %s\n", tabs, param->name);

    print_type(param->type, tab_level + 1);

    if (param->next != 0) {
        printf("%s},\n", tabs);
        print_param_list(param->next, tab_level);
    } else {
        printf("%s}\n", tabs);
    }
}
