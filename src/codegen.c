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

/**********************************************************************
 *                              CODEGEN                               *
 **********************************************************************/

void expr_bool_codegen(
    expr* e,
    const char* instruction,
    int true_label
) {
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf(
        "CMPQ %s, %s\n",
        scratch_name(e->left->reg),
        scratch_name(e->right->reg)
    );
    scratch_free(e->left);
    scratch_free(e->right);
    printf(
        "%s %s\n",
        instruction,
        label_name(true_label)
    );
}

void bool_val_codegen(expr* e, const char* instruction) {
    int true_label = create_label();
    int done_label = create_label();
    expr_bool_codegen(e, "instruction", true_label);
    e->reg = scratch_alloc();
    /* false branch: */
    printf(
        "MOVQ $0, %s\n",
        scratch_name(e->reg)
    );
    printf(
        "JMP %s\n",
        label_name(done_label)
    );
    /* true branch: */
    printf(
        "%s:\n",
        label_name(true_label)
    );
    printf(
        "MOVQ $1, %s\n",
        scratch_name(e->reg)
    );
    printf(
        "%s:\n",
        label_name(done_label)
    );
}

void decl_codegen(decl* d) {
    if (!d) return;

    if (d->symbol->kind == SYMBOL_LOCAL) {
        if (d->value) {
            expr_codegen(d->value);
            printf(
                "MOVQ %s, %s\n",
                scratch_name(d->value->reg),
                symbol_address(d->symbol)
            );
            scratch_free(d->value->reg);
        }
    } else if (d->symbol->kind == SYMBOL_GLOBAL) {
        if (d->value) {
            data_entry* entry = malloc(sizeof(*entry));
            switch (d->symbol->type->kind) {
                case TYPE_BOOLEAN:   __attribute__((fallthrough));
                case TYPE_CHARACTER: __attribute__((fallthrough));
                case TYPE_INTEGER:
                    if (0 > asprintf(
                        &entry->entry,
                        "%s: %d\n",
                        d->symbol->name,
                        d->value->value
                    )) {
                        fprintf(
                            stderr,
                            "error: failed to declare global variable\n"
                        );
                    }
                    break;
                case TYPE_STRING:
                    if (0 > asprintf(
                        &entry->entry,
                        "%s: db %s\n",
                        d->symbol->name,
                        d->value->str_value
                    )) {
                        fprintf(
                            stderr,
                            "error: failed to declare global variable\n"
                        );
                    }
                    break;
            }
            entry->next = data;
            data = entry;
        } else if (d->code) {
            func_codegen(d->symbol, d->code);
        }
    }
}

void func_codegen(symbol* f, stmt* code) {
    printf(".global %s\n", f->name);
    printf("%s:\n", f->name);
    printf("MOVQ %%rsp, %%rbp\n");
    int i = 0;
    param_list* p = f->type->params;
    while (p != NULL) {
        if (i < 6) {
            printf(
                "PUSHQ %s\n",
                ARG_REGS[i]
            );
        }
        i++;
        p = p->next;
    }

    int locals = f->stack_size - i;

    printf("SUBQ $%d, %%rsp\n", locals * 8);

    printf("PUSHQ %%rbx\n");
    printf("PUSHQ %%r12\n");
    printf("PUSHQ %%r13\n");
    printf("PUSHQ %%r14\n");
    printf("PUSHQ %%r15\n");

    stmt_codegen(code, f->name);

    printf(".%s_epilogue:\n");

    printf("POPQ %%r15\n");
    printf("POPQ %%r14\n");
    printf("POPQ %%r13\n");
    printf("POPQ %%r12\n");
    printf("POPQ %%rbx\n");

    printf("MOVQ %%rbp, %%rsp\n");
    printf("POPQ %%rbp\n");
    printf("RET\n");
}

void stmt_codegen(stmt* s, const char* func_name) {
    if (!s) return;

    switch (s->kind) {
        case STMT_BLOCK:
            stmt_codegen(s->body, func_name);
            break;
        case STMT_DECL:
            decl_codegen(s->decl);
            break;
        case STMT_EXPR:
            decl_codegen(s->expr);
            scratch_free(s->expr->reg);
            break;
        case STMT_PRINT:
            switch (expr_typecheck(s->expr)->kind) {
                case TYPE_STRING:
                    if (s->expr->kind == EXPR_STR_LIT) {
                        print_str_lit_codegen(s->expr->str_value);
                    } else {
                        expr_codegen(s->expr);
                        print_str_codegen(s->expr->reg);
                        scratch_free(s->expr->reg);
                    }
                    break;
                case TYPE_INTEGER:
                    expr_codegen(s->expr);
                    print_i_to_a(s->expr->reg);
                    scratch_free(s->expr->reg);
                    break;
                case TYPE_CHARACTER:
                    expr_codegen(s->expr);
                    print_char(s->expr->reg);
                    scratch_free(s->expr->reg);
                    break;
                case TYPE_BOOLEAN:
                    expr_codegen(s->expr);
                    print_bool(s->expr->reg);
                    scratch_free(s->expr->reg);
                    break;
            }
            break;
        case STMT_RETURN:
            expr_codegen(s->expr);
            printf(
                "MOVQ %s, %%rax\n",
                scratch_name(s->expr->reg)
            );
            scratch_free(s->expr->reg);
            printf(
                "JMP .%s_epilogue\n",
                func_name
            );
            break;
        default:
            fprintf(
                stderr,
                "error: invalid `stmt` type in CFG\n"
            );
            break;
    }

    stmt_codegen(s->next, func_name);
}

void expr_codegen(expr* e) {
    if (!e) return;

    switch (e->kind) {
        case EXPR_IDENT:
            e->reg = scratch_alloc();
            printf(
                "MOVQ %s, %s\n",
                symbol_address(e->symbol),
                scratch_name(e->reg)
            );
            break;
        case EXPR_BOOL_LIT: __attribute__((fallthrough));
        case EXPR_CHAR_LIT: __attribute__((fallthrough));
        case EXPR_INT_LIT:
            e->reg = scratch_alloc();
            printf(
                "MOVQ $%s, %s\n",
                e->value,
                scratch_name(e->reg)
            );
            break;
        case EXPR_STR_LIT:
            int str = add_str(e->str_value, false);
            e->reg = scratch_alloc();
            printf(
                "MOVQ %s, %s\n",
                str_label(str),
                scratch_name(e->reg)
            );
            break;
        case EXPR_ASSIGN:
            expr_codegen(e->right);
            printf(
                "MOVQ %s, %s\n",
                scratch_name(e->right->reg),
                symbol_address(e->left->symbol)
            );
            e->reg = e->right->reg;
            break;
        case EXPR_ADD:
            expr_codegen(e->left);
            expr_codegen(e->right);
            printf(
                "ADDQ %s, %s\n",
                scratch_name(e->left->reg),
                scratch_name(e->right->reg)
            );
            e->reg = e->right->reg;
            scratch_free(e->left->reg);
            break;
        case EXPR_SUB:
            expr_codegen(e->left);
            expr_codegen(e->right);
            printf(
                "SUBQ %s, %s\n",
                scratch_name(e->left->reg),
                scratch_name(e->right->reg)
            );
            e->reg = e->right->reg;
            scratch_free(e->left->reg);
            break;
        case EXPR_INC:
            e->reg = scratch_alloc();
            printf( /* load variable into register */
                "MOVQ %s, %s\n",
                symbol_address(e->left->symbol),
                scratch_name(e->reg)
            );
            printf( /* increment value */
                "INCQ %s\n",
                scratch_name(e->left->symbol)
            );
            printf( /* copy new value back to variable */
                "MOVQ %s, %s\n",
                scratch_name(e->reg),
                symbol_address(e->left->symbol)
            );
            break;
        case EXPR_DEC:
            e->reg = scratch_alloc();
            printf( /* load variable into register */
                "MOVQ %s, %s\n",
                symbol_address(e->left->symbol),
                scratch_name(e->reg)
            );
            printf( /* decrement value */
                "DECQ %s\n",
                scratch_name(e->left->symbol)
            );
            printf( /* copy new value back to variable */
                "MOVQ %s, %s\n",
                scratch_name(e->reg),
                symbol_address(e->left->symbol)
            );
            break;
        case EXPR_MUL:
            expr_codegen(e->left);
            expr_codegen(e->right);
            printf( /* move `left` into `%rax` */
                "MOVQ %s, %%rax\n",
                scratch_name(e->left->reg)
            );
            scratch_free(e->left->reg);
            printf( /* multiply `%rax` by `right` */
                "IMUL %s\n",
                scratch_name(e->right->reg)
            );
            scratch_free(e->right->reg);
            e->reg = scratch_alloc();
            printf( /* move result into register of `e` */
                "MOVQ %%rax, %s\n",
                scratch_name(e->reg)
            );
            break;
        case EXPR_DIV:
            expr_codegen(e->left);
            expr_codegen(e->right);
            printf( /* move `left` into `%rax` */
                "MOVQ %s, %%rax\n",
                scratch_name(e->left->reg)
            );
            scratch_free(e->left->reg);
            printf( /* divide `%rax` by `right` */
                "IDIV %s\n",
                scratch_name(e->right->reg)
            );
            scratch_free(e->right->reg);
            e->reg = scratch_alloc();
            printf( /* move result into register of `e` */
                "MOVQ %%rax, %s\n",
                scratch_name(e->reg)
            );
            break;
        case EXPR_MOD:
            expr_codegen(e->left);
            expr_codegen(e->right);
            printf( /* move `left` into `%rax` */
                "MOVQ %s, %%rax\n",
                scratch_name(e->left->reg)
            );
            scratch_free(e->left->reg);
            printf( /* divide `%rax` by `right` */
                "IDIV %s\n",
                scratch_name(e->right->reg)
            );
            scratch_free(e->right->reg);
            e->reg = scratch_alloc();
            printf( /* move `%rdx` (remainder) into `e` */
                "MOVQ %%rdx, %s\n",
                scratch_name(e->reg)
            );
            break;
        case EXPR_EQ:
            bool_val_codegen(e, "JE");
            break;
        case EXPR_N_EQ:
            bool_val_codegen(e, "JNE");
            break;
        case EXPR_LESS:
            bool_val_codegen(e, "JL");
            break;
        case EXPR_L_EQ:
            bool_val_codegen(e, "JLE");
            break;
        case EXPR_GREATER:
            bool_val_codegen(e, "JG");
            break;
        case EXPR_G_EQ:
            bool_val_codegen(e, "JGE");
            break;
        case EXPR_FUN_CALL:
            expr* arg = e->right;
            int i = 0;
            while (arg != NULL) {
                expr_codegen(arg);
                if (i < 6) {
                    printf(
                        "MOVQ %s, %s\n",
                        scratch_name(arg->reg),
                        ARG_REGS[i]
                    );
                } else {
                    printf(
                        "PUSHQ %s\n",
                        scratch_name(arg->reg)
                    );
                }
                scratch_free(arg->reg);
                
                i++;
                arg = arg->right;
            }
            printf("PUSHQ %%r10\n");
            printf("PUSHQ %%r11\n");

            printf("CALL .%s\n", e->symbol->name);

            printf("POPQ %%r11\n");
            printf("POPQ %%r10\n");

            e->reg = scratch_alloc();
            printf(
                "MOVQ %%rax, %s\n",
                scratch_name(e->reg)
            );
            break;
    }
}

/**********************************************************************
 *                          PRINT FUNCTIONS                           *
 **********************************************************************/

void print_bool(int reg) {
    printf(
        "CMP %s, $0\n",
        scratch_name(reg)
    );
    int true_label = create_label();
    int done_label = create_label();
    printf(
        "JNE %s\n",
        label_name(true_label)
    );
    printf("MOVQ $0x30, %s\n", scratch_name(reg));
    printf("JMP %s\n", label_name(done_label));
    printf("%s:\n", label_name(true_label));
    printf("MOVQ $0x31, %s\n", scratch_name(reg));
    printf("%s:\n", label_name(done_label));
    print_char(reg);
}

void print_char(int reg) {
    printf( /* set length to 1 */
        "MOVQ $1, %%rdx\n"
    );
    printf( /* move char to input buffer */
        "MOVQ %s, %%rsi\n",
        scratch_name(reg)
    );
    printf( /* set fd to stdout */
        "MOVQ $1, %%rdi\n"
    );
    printf( /* set syscall to write */
        "MOVQ $4, %%rax\n"
    );
    printf("SYSCALL\n");
}

void print_str_codegen(int reg) {
    int count = scratch_alloc();
    int pointer = scratch_alloc();
    printf(
        "MOVQ %s, %s\n",
        scratch_name(reg),
        scratch_name(pointer)
    );
    int loop = create_label();
    int done = create_label();
    printf("%s:\n", label_name(loop));
    printf("CMP %s, $0\n", scratch_name(pointer));
    printf("JE %s\n", label_name(done));
    printf("INCQ %s\n", scratch_name(count));
    printf("INCQ %s\n", scratch_name(pointer));
    printf("JMP %s\n", label_name(loop));
    printf("%s:\n", label_name(done));
    scratch_free(pointer);
    printf("MOVQ %s, %%rdx\n", scratch_name(count));
    scratch_free(count);
    printf("MOVQ %s, %%rsi\n", scratch_name(reg));
    printf("MOVQ $1, %%rdi\n");
    printf("MOVQ $4, %%rax\n");
    printf("SYSCALL\n");
}

void print_str_lit_codegen(const char* s) {
    int orig_size = strlen(s);
    int prev_size = 0;
    char* str = strtok(s, '\n');
    bool newline = false;

    while (str != NULL) {
        newline = strlen(str) < (orig_size - prev_size);

        int str = add_str(str, newline);
        printf( /* move string length to third arg */
            "MOVQ %s_len, %%rdx\n",
            str_label(str)
        );
        printf( /* move string to second arg */
            "MOVQ %s ,%%rsi\n",
            str_label(str)
        );
        printf( /* move "1" (stdout) to first arg */
            "MOVQ $1, %%rdi\n"
        );
        printf( /* move "4" (write) to %rax */
            "MOVQ $4, %%rax\n"
        );
        printf( /* invoke the system call*/
            "SYSCALL\n"
        );

        prev_size += strlen(str);
        str = strtok(NULL, '\n');
    }
}

void print_i_to_a(int reg) {
    /* store number in %rax */
    printf(
        "MOVQ %s, %%rax\n",
        scratch_name(reg)
    );
    /* count # of converted digits */
    int num_digits = scratch_alloc();
    printf("MOVQ $0, %s\n", scratch_name(num_digits));
    /* create loop label */
    int convert_loop = create_label();
    printf("%s:\n", label_name(convert_loop));
    printf( /* divide %rax by 10 */
        "IDIV $10\n"
    );
    printf( /* convert remainder to ASCII */
        "ADD $0x30, %%rdx\n"
    );
    printf( /* push character to stack: */
        "PUSH %%rdx\n"
    );
    printf("INC %s\n", scratch_name(num_digits));
    printf("CMP %%rax, $0\n"); 
    printf("JE %s\n", label_name(convert_loop));

    /* check negative */
    printf("CMP %s, $0\n", scratch_name(reg));
    int print_loop = create_label();
    printf("JGE %s\n", label_name(print_loop));
    printf("PUSH $0x2D\n");
    printf("INC %s\n", scratch_name(num_digits));

    /* create print loop label */
    printf("%s:\n", label_name(print_loop));
    printf( /* prepare string length arg */
        "MOVQ $1, %%rdx\n"
    );
    printf( /* prepare stdout arg */
        "MOVQ $1, %%rdi\n"
    );
    printf(/* pop character */
        "POP %%rsi\n"
    );
    printf( /* prepare syscall arg */
        "MOVQ $4, %%rax\n"
    );
    printf("SYSCALL\n");
    printf("DEC %s\n", scratch_name(num_digits));
    printf("CMP %s, $0\n", scratch_name(num_digits));
    printf("JNE %s\n", label_name(print_loop));
}