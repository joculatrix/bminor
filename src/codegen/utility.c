#include "codegen.h"

/**********************************************************************
 *                         UTILITY FUNCTIONS                          *
 **********************************************************************/

const char* symbol_address(symbol* s) {
    switch (s->kind) {
        case SYMBOL_GLOBAL:
            return s->name;
        case SYMBOL_LOCAL: __attribute__((fallthrough));
        case SYMBOL_PARAM:
            int bytes = 8 * s->which;
            
            const char* res;
            if (0 > asprintf(&res, "-%d(%%rbp)", bytes)) {
                fprintf(
                    stderr,
                    "error: failed to construct symbol address\n"
                );
                return NULL;
            }
            return res;
    }
}

int add_str(const char* s, bool newline) {
    int label = str_count++;
    data_entry* entry = malloc(sizeof(*entry));
    
    if (0 > asprintf(
        &entry->entry,
        "%s: db %s%s\n",
        str_label(label),
        s,
        newline ? ", $0xA" : ""
    )) {
        fprintf(
            stderr,
            "error: failed to add string to data section\n"
        );
        return NULL;
    }

    data_entry* length = malloc(sizeof(*entry));

    if (0 > asprintf(
        &length->entry,
        "%s_len: equ $-%s\n",
        str_label(label),
        str_label(label)
    )) {
        fprintf(
            stderr,
            "error: failed to add string length to data section\n"
        );
        return NULL;
    }

    length->next = data;
    entry->next = length;
    data = entry;

    return label;
}

const char* str_label(int label) {
    const char* res;
    if (0 > asprintf(&res, "str%d", label)) {
        fprintf(
            stderr,
            "error: failed to construct string label\n"
        );
        return NULL;
    }
    return res;
}

int scratch_alloc() {
    for (int i = 0; i < NUM_SCRATCH; i++) {
        if (!scratch[i].used) {
            scratch[i].used = true;
            return i;
        }
    }
    fprintf(stderr, "error: could not allocate scratch register\n");
    exit(1);
}

void scratch_free(int r) {
    scratch[r].used = false;
}

const char* scratch_name(int r) {
    return scratch[r].name;
}

int create_label() {
    return label_count++;
}

const char* label_name(int label) {
    const char* res;
    if (0 > asprintf(&res, ".L%d", label)) {
        fprintf(
            stderr,
            "error: failed to construct jump label\n"
        );
        return NULL;
    }
    return res;
}