#include "cfg.h"
#include <stdio.h>
#include <stdlib.h>

/**********************************************************************
 *                          CFG UTILITY FUNCTIONS                     *
 **********************************************************************/

cfg_node* cfg_block_node(stmt* stmt) {
    cfg_node* node = malloc(sizeof(*node));
    node->kind = CFG_BLOCK;
    node->value.block->stmt = stmt;
    node->next = NULL;
    return node;
}

cfg_node* cfg_branch_node(expr* exp) {
    cfg_node* node = malloc(sizeof(*node));
    node->kind = CFG_BRANCH;
    node->value.branch->condition = exp;
    node->next = NULL;
    return node;
}

int cfg_set_true(cfg_node* node, cfg_node* true_branch) {
    if (node->kind == CFG_BRANCH) {
        node->value.branch->true_branch = true_branch;
        return 0;
    } else {
        fprintf(
            stderr,
            "error: cannot set `true branch` of `CFG_BLOCK`.\n"
        );
        return 1;
    }
}

int cfg_set_false(cfg_node* node, cfg_node* false_branch) {
    if (node->kind == CFG_BRANCH) {
        node->value.branch->false_branch = false_branch;
        return 0;
    } else {
        fprintf(
            stderr,
            "error: cannot set `false branch` of `CFG_BLOCK`.\n"
        );
        return 1;
    }
}

void cfg_push_back(cfg_node* node, cfg_node* back) {
    cfg_node* p = node;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = back;
}

/**********************************************************************
 *                              CONSTRUCTION                          *
 **********************************************************************/

cfg* cfg_construct(decl* d) {
    if (!d) return NULL;

    cfg* cfg = malloc(sizeof(*cfg));

    if (d->value) {
        cfg->kind = VAR;
        cfg->symbol = d->symbol;
        cfg->value.exp = d->value;
    } else if (d->code) {
        cfg->kind = FUNC;
        cfg->symbol = d->symbol;
        cfg->value.cfg_node = cfg_construct_block(d->code);
    } else {
        return d->next;
    }

    cfg->next = cfg_construct(d->next);
    return cfg;
}

cfg_node* cfg_construct_block(stmt* s) {
    if (!s) return NULL;
    
    cfg_node* node;
    stmt* p = s;
    while (p->next != NULL) {
        switch(p->next->kind) {
            case STMT_FOR:
                stmt* for_loop = p->next;
                p->next = NULL;
                node = cfg_block_node(s);

                cfg_node* loop_node = cfg_for_loop(for_loop);
                cfg_push_back(loop_node, cfg_construct_block(for_loop->next));

                if (node) {
                    node->next = loop_node;
                    return node;
                } else {
                    return loop_node;
                }
            case STMT_IF_ELSE:
                stmt* if_else = p->next;
                p->next = NULL;
                node = cfg_block_node(s);

                cfg_node* if_node = cfg_if_else(if_else);
                cfg_push_back(if_node, cfg_construct_block(if_else->next));
                
                if (node) {
                    node->next = if_node;
                    return node;
                } else {
                    return loop_node;
                }
            default:
                p = p->next;
                break;
        }
    }
    return cfg_block_node(s);
}

cfg_node* cfg_for_loop(stmt* s) {
    cfg_node* init = cfg_block_node(stmt_expr(s->init_expr, NULL));

    cfg_node* comp = cfg_branch_node(s->expr);
    /* append `next_expr` to end of loop body: */
    stmt* p_body = s->body;
    while (p_body->next != NULL) {
        p_body = p_body->next;
    }
    p_body->next = stmt_expr(s->next_expr, NULL);

    /* set branches (false branch just exits loop): */
    cfg_set_true(comp, cfg_construct_block(s->body));
    cfg_set_false(comp, cfg_block_node(NULL));

    init->next = comp;
    return init;
}

cfg_node* cfg_if_else(stmt* s) {
    cfg_node* node = cfg_branch_node(s->expr);

    cfg_node* true_branch = s->body ?
        cfg_construct_block(s->body) : cfg_block_node(NULL);

    cfg_node* false_branch = s->else_body ?
        cfg_construct_block(s->else_body) : cfg_block_node(NULL);

    cfg_set_true(node, true_branch);
    cfg_set_false(node, false_branch);

    return node;
}