#include "codegen.h"
#include "symbol.h"

int label_count = 0;
int str_count = 0;

const int NUM_SCRATCH = 7;

reg scratch[] = {
    { .name = "%rbx", .used = false },
    { .name = "%r10", .used = false },
    { .name = "%r11", .used = false },
    { .name = "%r12", .used = false },
    { .name = "%r13", .used = false },
    { .name = "%r14", .used = false },
    { .name = "%r15", .used = false },
};

const char* ARG_REGS[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

data_entry* data;

/**********************************************************************
 *                              CODEGEN                               *
 **********************************************************************/

void codegen(cfg* cfg) {
    printf("section .text\n");
    cfg_codegen(cfg);
    printf("section .data\n");
    while (data != NULL) {
        printf("%s", data->entry);
        data = data->next;
    }
}

void cfg_codegen(cfg* cfg) {
    if (!cfg) return;

    if (cfg->kind == VAR) {
        data_entry* entry = malloc(sizeof(*entry));
        switch (cfg->symbol->type->kind) {
            case TYPE_BOOLEAN:   __attribute__((fallthrough));
            case TYPE_CHARACTER: __attribute__((fallthrough));
            case TYPE_INTEGER:
                if (0 > asprintf(
                    &entry->entry,
                    "%s: $%d\n",
                    cfg->symbol->name,
                    cfg->value.exp->value
                )) {
                    fprintf(
                        stderr,
                        "error: failed to allocate global variable\n"
                    );
                }
                entry->next = data;
                data = entry;
                break;
            case TYPE_STRING:
                if (0 > asprintf(
                    &entry->entry,
                    "%s: \"%s\"\n",
                    cfg->symbol->name,
                    cfg->value.exp->str_value
                )) {
                    fprintf(
                        stderr,
                        "error: failed to allocate global variable\n"
                    );
                }
                entry->next = data;
                data = entry;
                break;
            case TYPE_ARRAY:
                if (0 > asprintf(&entry->entry, "%s: ", cfg->symbol->name)) {
                    fprintf(
                        stderr,
                        "error: failed to allocate global variable\n"
                    );
                }
                expr* item = cfg->value.exp;
                while (item != NULL) {
                    const char* item_txt;
                    if (0 > asprintf(&item_txt, "%d, ", item->value)) {
                        fprintf(
                            stderr,
                            "error: failed to allocate global variable\n"
                        );
                    }
                    strcat(entry->entry, item_txt);
                    item = item->right;
                }
                strcat(entry->entry, "\n");
                entry->next = data;
                data = entry;
                break;
        }
    } else {
        func_codegen(cfg);
    }

    cfg_codegen(cfg->next);
}

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
        }
    }
}

void func_codegen(cfg* func_decl) {
    printf(".global %s\n", func_decl->symbol->name);
    printf("%s:\n", func_decl->symbol->name);
    printf("MOVQ %%rsp, %%rbp\n");
    int i = 0;
    param_list* p = func_decl->symbol->type->params;
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

    int locals = func_decl->symbol->stack_size - i;

    printf("SUBQ $%d, %%rsp\n", locals * 8);

    printf("PUSHQ %%rbx\n");
    printf("PUSHQ %%r12\n");
    printf("PUSHQ %%r13\n");
    printf("PUSHQ %%r14\n");
    printf("PUSHQ %%r15\n");

    func_body_codegen(func_decl->symbol->name, func_decl->value.cfg_node);

    printf("%s_epilogue:\n");

    printf("POPQ %%r15\n");
    printf("POPQ %%r14\n");
    printf("POPQ %%r13\n");
    printf("POPQ %%r12\n");
    printf("POPQ %%rbx\n");

    printf("MOVQ %%rbp, %%rsp\n");
    printf("POPQ %%rbp\n");
    printf("RET\n");
}

void func_body_codegen(const char* func_name, cfg_node* node) {
    if (!node) return;
    switch (node->kind) {
        case CFG_BLOCK:
            stmt_codegen(
                node->value.block->stmt,
                func_name
            );
            func_body_codegen(func_name, node->value.block->next);
            break;
        case CFG_BRANCH:
            expr_codegen(node->value.branch->condition);
            printf(
                "CMP %s, $0\n",
                scratch_name(node->value.branch->condition->reg)
            );
            scratch_free(node->value.branch->condition->reg);
            int true_label = create_label();
            printf(
                "JNE %s\n",
                label_name(true_label)
            );
            func_body_codegen(func_name, node->value.branch->false_branch);
            printf("%s:\n", label_name(true_label));
            func_body_codegen(func_name, node->value.branch->true_branch);
            break;
        case CFG_RETURN:
            printf(
                "JMP %s_epilogue\n",
                func_name
            );
            break;
    }
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
                scratch_name(e->reg)
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
                scratch_name(e->reg)
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
        case EXPR_ARRAY:
            printf(
                "MOVQ $%s, %%rax\n",
                e->symbol->type->size
            );
            printf(
                "IMULQ $8\n"
            );
            e->reg = scratch_alloc();
            printf( /* save soon-to-be array address to register */
                "MOVQ %%rsp, %s\n",
                scratch_name(e->reg)
            );
            printf(
                "SUBQ %%rax, %%rsp\n"
            );
            int pointer = scratch_alloc();
            printf(
                "MOVQ %s, %s\n",
                scratch_name(e->reg),
                scratch_name(pointer)
            );
            expr* item = e;
            while (item != NULL) {
                printf(
                    "MOVQ $%d, (%s)\n",
                    e->value,
                    scratch_name(pointer)
                );
                printf(
                    "SUBQ $4, %s\n",
                    scratch_name(pointer)
                );
                item = item->right;
            }
            scratch_free(pointer);
            break;
        case EXPR_INDEX:
            expr_codegen(e->left);
            expr_codegen(e->right);
            e->reg = scratch_alloc();
            printf(
                "MOVQ (%s, %s, $8), %s\n",
                scratch_name(e->left->reg),
                scratch_name(e->right->reg),
                scratch_name(e->reg)
            );
            scratch_free(e->left->reg);
            scratch_free(e->right->reg);
            break;
    }
}