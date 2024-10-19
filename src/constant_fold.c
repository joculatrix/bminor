#include "constant_fold.h"

bool is_constant(expr* e) {
    switch (e->kind) {
        case EXPR_ARRAY:    __attribute__((fallthrough));
        case EXPR_BOOL_LIT: __attribute__((fallthrough));
        case EXPR_CHAR_LIT: __attribute__((fallthrough));
        case EXPR_INT_LIT:  __attribute__((fallthrough));
        case EXPR_STR_LIT:
            return true;
        default:
            return false;
    }
}

int pow_int(int a, int b) {
    if (b == 0) return 1;
    if (b == 1) return a;
    
    return a * pow_int(a, b - 1);
}

decl* constant_fold_decl(decl* d) {
    if (!d) return NULL;

    if (d->value) {
        d->value = constant_fold_expr(d->value);
    }
    if (d->code) {
        d->code = constant_fold_stmt(d->code);
    }

    d->next = constant_fold_decl(d->next);

    return d;
}

stmt* constant_fold_stmt(stmt* s) {
    if (!s) return NULL;

    switch (s->kind) {
        case STMT_BLOCK:
            s->body = constant_fold_stmt(s->body);
            break;
        case STMT_DECL:
            s->decl = constant_fold_decl(s->decl);
            break;
        case STMT_EXPR:
            s->expr = constant_fold_expr(s->expr);
            break;
        case STMT_FOR:
            s->init_expr = constant_fold_expr(s->init_expr);
            s->expr = constant_fold_expr(s->expr);
            s->next_expr = constant_fold_expr(s->next_expr);
            s->body = constant_fold_stmt(s->body);
            break;
        case STMT_IF_ELSE:
            s->expr = constant_fold_expr(s->expr);
            s->body = constant_fold_stmt(s->body);
            s->else_body = constant_fold_stmt(s->else_body);
            break;
        case STMT_PRINT:
            s->expr = constant_fold_expr(s->expr);
            break;
        case STMT_RETURN:
            s->expr = constant_fold_expr(s->expr);
            break;
    }

    s->next = constant_fold_stmt(s->next);
    
    return s;
}

expr* constant_fold_expr(expr* e) {
    if (!e) return NULL;

    if ((!e->left && !e->right) || is_constant(e)) {
        return e;
    }

    switch (e->kind) {
        case EXPR_ADD:
            e->left = constant_fold_expr(e->left);
            e->right = constant_fold_expr(e->right);

            if (is_constant(e->left) && is_constant(e->right)) {
                /* typechecking should have caught non-integer arithmetic */
                e->kind = EXPR_INT_LIT;
                e->value = e->left->value + e->right->value;
                free(e->left);
                free(e->right);
            }
            return e;
        case EXPR_SUB:
            e->left = constant_fold_expr(e->left);
            e->right = constant_fold_expr(e->right);

            if (is_constant(e->left) && is_constant(e->right)) {
                e->kind = EXPR_INT_LIT;
                e->value = e->left->value - e->right->value;
                free(e->left);
                free(e->right);
            }
            return e;
        case EXPR_MUL:
            e->left = constant_fold_expr(e->left);
            e->right = constant_fold_expr(e->right);

            if (is_constant(e->left) && is_constant(e->right)) {
                e->kind = EXPR_INT_LIT;
                e->value = e->left->value * e->right->value;
                free(e->left);
                free(e->right);
            }
            return e;
        case EXPR_DIV:
            e->left = constant_fold_expr(e->left);
            e->right = constant_fold_expr(e->right);

            if (is_constant(e->left) && is_constant(e->right)) {
                e->kind = EXPR_INT_LIT;
                e->value = e->left->value / e->right->value;
                free(e->left);
                free(e->right);
            }
            return e;
        case EXPR_EXP:
            e->left = constant_fold_expr(e->left);
            e->right = constant_fold_expr(e->right);

            if (is_constant(e->left) && is_constant(e->right)) {
                e->kind = EXPR_INT_LIT;
                e->value = pow_int(e->left->value, e->right->value);
                free(e->left);
                free(e->right);
            }
            return e;
        case EXPR_MOD:
            e->left = constant_fold_expr(e->left);
            e->right = constant_fold_expr(e->right);

            if (is_constant(e->left) && is_constant(e->right)) {
                e->kind = EXPR_INT_LIT;
                e->value = e->left->value % e->right->value;
                free(e->left);
                free(e->right);
            }
            return e;
        case EXPR_AND:
            e->left = constant_fold_expr(e->left);
            e->right = constant_fold_expr(e->right);

            if (is_constant(e->left) && is_constant(e->right)) {
                e->kind = EXPR_BOOL_LIT;
                e->value = e->left->value && e->right->value;
                free(e->left);
                free(e->right);
            }
            return e;
        case EXPR_OR:
            e->left = constant_fold_expr(e->left);
            e->right = constant_fold_expr(e->right);

            if (is_constant(e->left) && is_constant(e->right)) {
                e->kind = EXPR_BOOL_LIT;
                e->value = e->left->value || e->right->value;
                free(e->left);
                free(e->right);
            }
            return e;
        case EXPR_EQ:
            e->left = constant_fold_expr(e->left);
            e->right = constant_fold_expr(e->right);

            if (is_constant(e->left) && is_constant(e->right)) {
                e->kind = EXPR_BOOL_LIT;
                e->value = e->left->value == e->right->value;
                free(e->left);
                free(e->right);
            }
            return e;
        case EXPR_N_EQ:
            e->left = constant_fold_expr(e->left);
            e->right = constant_fold_expr(e->right);

            if (is_constant(e->left) && is_constant(e->right)) {
                e->kind = EXPR_BOOL_LIT;
                e->value = e->left->value != e->right->value;
                free(e->left);
                free(e->right);
            }
            return e;
        case EXPR_LESS:
            e->left = constant_fold_expr(e->left);
            e->right = constant_fold_expr(e->right);

            if (is_constant(e->left) && is_constant(e->right)) {
                e->kind = EXPR_BOOL_LIT;
                e->value = e->left->value < e->right->value;
                free(e->left);
                free(e->right);
            }
            return e;
        case EXPR_L_EQ:
            e->left = constant_fold_expr(e->left);
            e->right = constant_fold_expr(e->right);

            if (is_constant(e->left) && is_constant(e->right)) {
                e->kind = EXPR_BOOL_LIT;
                e->value = e->left->value <= e->right->value;
                free(e->left);
                free(e->right);
            }
            return e;
        case EXPR_GREATER:
            e->left = constant_fold_expr(e->left);
            e->right = constant_fold_expr(e->right);

            if (is_constant(e->left) && is_constant(e->right)) {
                e->kind = EXPR_BOOL_LIT;
                e->value = e->left->value > e->right->value;
                free(e->left);
                free(e->right);
            }
            return e;
        case EXPR_G_EQ:
            e->left = constant_fold_expr(e->left);
            e->right = constant_fold_expr(e->right);

            if (is_constant(e->left) && is_constant(e->right)) {
                e->kind = EXPR_BOOL_LIT;
                e->value = e->left->value >= e->right->value;
                free(e->left);
                free(e->right);
            }
            return e;
        case EXPR_NOT:
            e->left = constant_fold_expr(e->left);

            if (is_constant(e->left)) {
                e->kind = EXPR_BOOL_LIT;
                e->value = !(e->left->value);
            }
            return e;
        default:
            return e;
    }
}