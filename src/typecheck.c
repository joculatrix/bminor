#include "semantics.h"
#include <stdio.h>
#include <stdlib.h>

struct type* curr_return = 0;

bool type_equals(struct type* a, struct type* b) {
    if (a->kind == b->kind) {
        switch (a->kind) {
            case TYPE_ARRAY:
                return type_equals(a->subtype, b->subtype);
            case TYPE_FUNCTION:
                /* return type: */
                if (!type_equals(a->subtype, b->subtype)) return false;

                /* params: */
                struct param_list* p_a = a->params;
                struct param_list* p_b = b->params;

                while (p_a != 0 && p_b != 0) {
                    if (!type_equals(p_a->type, p_b->type)) return false;
                    p_a = p_a->next;
                    p_b = p_b->next;
                }
                /* return false if unequal number of params: */
                return p_a == 0 && p_b == 0;
            default:
                return true;
        }
    } else {
        return false;
    }
}

struct type* type_copy(struct type* t) {
    if (!t) return NULL;

    struct type* copy = malloc(sizeof(*copy));
    copy->kind = t->kind;
    if (t->subtype) {
        copy->subtype = type_copy(t->subtype);
    }
    if (t->params) {
        copy->params = param_list_copy(t->params);
    }
    return copy;
}

struct param_list* param_list_copy(struct param_list* p) {
    if (!p) return NULL;

    struct param_list* copy = malloc(sizeof(*p));
    copy->name = p->name;
    copy->type = type_copy(p->type);
    copy->next = param_list_copy(p->next);

    return copy;
}

void type_delete(struct type* t) {
    if (!t) return;

    if (t->params) {
        param_list_delete(t->params);
    }
    if (t->subtype) {
        type_delete(t->subtype);
    }
    free(t);
}

void param_list_delete(struct param_list* p) {
    if (p->next) {
        param_list_delete(p->next);
    }
    type_delete(p->type);
    free(p->name);
    free(p);
}

void decl_typecheck(struct decl* d) {
    if (!d) return;

    if (d->value) {
        struct type* t = expr_typecheck(d->value);
        if (!type_equals(t, d->symbol->type)) {
            fprintf(
                stderr,
                "error: cannot initialize `%s` (`%s`) to value of type `%s`\n",
                type_t_str[d->symbol->type->kind],
                d->symbol->name,
                type_t_str[t->kind]
            );
        }
        type_delete(t);
    }
    if (d->code) {
        /* make return type of function available for checking */
        struct type* t = curr_return == 0 ? 0 : type_copy(curr_return);
        curr_return = type_copy(d->type->subtype);

        stmt_typecheck(d->code);

        /* revert return state to previous */
        type_delete(curr_return);
        curr_return = t == 0 ? 0 : type_copy(t);
        type_delete(t);
    }

    decl_typecheck(d->next);
}

void stmt_typecheck(struct stmt* s) {
    if (!s) return;

    struct type* t;
    switch (s->kind) {
        case STMT_EXPR:
            t = expr_typecheck(s->expr);
            type_delete(t);
            break;
        case STMT_IF_ELSE:
            t = expr_typecheck(s->expr);
            if (t->kind != TYPE_BOOLEAN) {
                fprintf(
                    stderr,
                    "error: `if` control expression is type `%s`, must be `boolean`\n",
                    type_t_str[t->kind]
                );
            }
            type_delete(t);
            stmt_typecheck(s->body);
            stmt_typecheck(s->else_body);
            break;
        case STMT_DECL:
            decl_typecheck(s->decl);
            break;
        case STMT_BLOCK:
            stmt_typecheck(s->body);
            break;
        case STMT_RETURN:
            t = expr_typecheck(s->expr);
            if (!type_equals(t, curr_return)) {
                fprintf(
                    stderr,
                    "error: cannot return type `%s`, expected `%s`\n",
                    type_t_str[t->kind],
                    type_t_str[curr_return->kind]
                );
            }
            type_delete(t);
            break;
        case STMT_FOR:
            t = expr_typecheck(s->init_expr);
            type_delete(t);
            t = expr_typecheck(s->expr);
            type_delete(t);
            t = expr_typecheck(s->next_expr);
            type_delete(t);
            stmt_typecheck(s->body);
            break;
        case STMT_PRINT:
            t = expr_typecheck(s->expr);
            type_delete(t);
            break;
    }

    stmt_typecheck(s->next);
}

struct type* expr_typecheck(struct expr* e) {
    if (!e) return 0;

    struct type* left = expr_typecheck(e->left);
    struct type* right = expr_typecheck(e->right);

    struct type* result;

    switch (e->kind) {
        case EXPR_AND:      __attribute__((fallthrough));
        case EXPR_OR:       __attribute__((fallthrough));
        case EXPR_EQ:       __attribute__((fallthrough));
        case EXPR_N_EQ:     __attribute__((fallthrough));
        case EXPR_LESS:     __attribute__((fallthrough));
        case EXPR_L_EQ:     __attribute__((fallthrough));
        case EXPR_GREATER:  __attribute__((fallthrough));
        case EXPR_G_EQ:
            if (left->kind == TYPE_VOID) {
                fprintf(stderr, "error: cannot compare type `void`\n");
            } else if (left->kind == TYPE_ARRAY) {
                fprintf(stderr, "error: cannot compare type `array []`\n");
            } else if (left->kind == TYPE_FUNCTION) {
                fprintf(stderr, "error: cannot compare type `function ()`\n");
            } else if (!type_equals(left, right)) {
                fprintf(
                    stderr,
                    "error: cannot compare type `%s` with type `%s`\n",
                    type_t_str[left->kind],
                    type_t_str[right->kind]
                );
            }
            __attribute__((fallthrough));
        case EXPR_BOOL_LIT:
            result = type_create(TYPE_BOOLEAN, 0, 0);
            break;
        case EXPR_NOT:
            if (left->kind != TYPE_BOOLEAN) {
                fprintf(
                    stderr,
                    "error: cannot negate non-boolean expression\n"
                );
            }
            result = type_create(TYPE_BOOLEAN, 0, 0);
            break;
        case EXPR_ARRAY:
            struct expr* item_p = e->right;
            while (item_p) {
                if (expr_typecheck(item_p) != left) {
                    fprintf(
                        stderr,
                        "error: item of type `%s` in array, expected `%s`\n",
                        type_t_str[item_p->symbol->type->kind],
                        type_t_str[left->kind]
                    );
                }
                item_p = item_p->right;
            }
            result = type_create(TYPE_ARRAY, left, 0);
            break;
        case EXPR_CHAR_LIT:
            result = type_create(TYPE_CHARACTER, 0, 0);
            break;
        case EXPR_INT_LIT:
            result = type_create(TYPE_INTEGER, 0, 0);
            break;
        case EXPR_STR_LIT:
            result = type_create(TYPE_STRING, 0, 0);
            break;
        case EXPR_IDENT:
            result = type_copy(e->symbol->type);
            break;
        case EXPR_INDEX:
            if (left->kind == TYPE_ARRAY) {
                if (right->kind != TYPE_INTEGER) {
                    fprintf(
                        stderr,
                        "error: cannot index array with type `%s`\n",
                        type_t_str[right->kind]
                    );
                }
                result = type_copy(left->subtype);
            } else {
                fprintf(
                    stderr,
                    "error: cannot index type `%s`\n",
                    type_t_str[left->kind]
                );
                result = type_copy(left);
            }
            break;
        case EXPR_ASSIGN:
            if (!type_equals(left, right)) {
                fprintf(
                    stderr,
                    "error: cannot assign `%s` (`%s`) to value of type `%s`\n",
                    e->left->name,
                    type_t_str[left->kind],
                    type_t_str[right->kind]
                );
            }
            result = type_copy(left);
            break;
        case EXPR_ADD:
            if (!type_equals(left, right)) {
                fprintf(
                    stderr,
                    "error: cannot add `%s` with `%s`\n",
                    type_t_str[left->kind],
                    type_t_str[right->kind]
                );
            }
            result = type_copy(left);
            if (result->kind != TYPE_INTEGER) {
                fprintf(
                    stderr,
                    "error: cannot perform addition on type `%s`\n",
                    type_t_str[result->kind]
                );
            }
            break;
        case EXPR_SUB:
            if (!type_equals(left, right)) {
                fprintf(
                    stderr,
                    "error: cannot subtract `%s` by `%s`\n",
                    type_t_str[left->kind],
                    type_t_str[right->kind]
                );
            }
            result = type_copy(left);
            if (result->kind != TYPE_INTEGER) {
                fprintf(
                    stderr,
                    "error: cannot perform subtraction on type `%s`\n",
                    type_t_str[result->kind]
                );
            }
            break;
        case EXPR_MUL:
            if (!type_equals(left, right)) {
                fprintf(
                    stderr,
                    "error: cannot multiply `%s` by `%s`\n",
                    type_t_str[left->kind],
                    type_t_str[right->kind]
                );
            }
            result = type_copy(left);
            if (result->kind != TYPE_INTEGER) {
                fprintf(
                    stderr,
                    "error: cannot perform multiplication on type `%s`\n",
                    type_t_str[result->kind]
                );
            }
            break;
        case EXPR_DIV:
            if (!type_equals(left, right)) {
                fprintf(
                    stderr,
                    "error: cannot divide `%s` by `%s`\n",
                    type_t_str[left->kind],
                    type_t_str[right->kind]
                );
            }
            result = type_copy(left);
            if (result->kind != TYPE_INTEGER) {
                fprintf(
                    stderr,
                    "error: cannot perform division on type `%s`\n",
                    type_t_str[result->kind]
                );
            }
            break;
        case EXPR_MOD:
            if (!type_equals(left, right)) {
                fprintf(
                    stderr,
                    "error: cannot mod `%s` by `%s`\n",
                    type_t_str[left->kind],
                    type_t_str[right->kind]
                );
            }
            result = type_copy(left);
            if (result->kind != TYPE_INTEGER) {
                fprintf(
                    stderr,
                    "error: cannot perform modulo on type `%s`\n",
                    type_t_str[result->kind]
                );
            }
            break;
        case EXPR_EXP:
            if (left->kind != TYPE_INTEGER || right->kind != TYPE_INTEGER) {
                fprintf(
                    stderr,
                    "error: cannot exponentiate type `%s` by type `%s`\n",
                    type_t_str[left->kind],
                    type_t_str[right->kind]
                );
            }
            result = type_copy(left);
            break;
        case EXPR_INC:
            if (left->kind != TYPE_INTEGER) {
                fprintf(
                    stderr,
                    "error: cannot increment type `%s`\n",
                    type_t_str[left->kind]
                );
            }
            result = type_copy(left);
            break;
        case EXPR_DEC:
            if (left->kind != TYPE_INTEGER) {
                fprintf(
                    stderr,
                    "error: cannot decrement type `%s`\n",
                    type_t_str[left->kind]
                );
            }
            result = type_copy(left);
            break;
        case EXPR_FUN_CALL:
            struct expr* arg_p = e->right;
            struct param_list* param_p = left->params;

            while (arg_p && param_p) {
                struct type* arg_type = expr_typecheck(arg_p);
                if (!type_equals(arg_type, param_p->type)) {
                    fprintf(
                        stderr,
                        "error: argument of type `%s`, expected `%s`\n",
                        type_t_str[arg_type->kind],
                        type_t_str[param_p->type->kind]
                    );
                }
                arg_p = arg_p->right;
                param_p = param_p->next;
                type_delete(arg_type);
            }
            if (arg_p) {
                fprintf(
                    stderr,
                    "error: too many arguments\n"
                );
            } else if (param_p) {
                fprintf(
                    stderr,
                    "error: not enough arguments\n"
                );
            }

            result = type_copy(left->subtype);
            break;
    }

    type_delete(left);
    type_delete(right);

    return result;
}