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
    node->value.block->next = NULL;
    return node;
}

cfg_node* cfg_branch_node(expr* exp) {
    cfg_node* node = malloc(sizeof(*node));
    node->kind = CFG_BRANCH;
    node->value.branch->condition = exp;
    return node;
}

cfg_node* cfg_return_node() {
    cfg_node* node = malloc(sizeof(*node));
    node->kind = CFG_RETURN;
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
    switch (node->kind) {
        case CFG_BLOCK:
            if (node->value.block->next == NULL) {
                node->value.block->next = back;
            } else {
                cfg_push_back(node->value.block->next, back);
            }
            break;
        case CFG_BRANCH:
            cfg_push_back(node->value.branch->true_branch, back);
            cfg_push_back(node->value.branch->false_branch, back);
            break;
        case CFG_RETURN:
            break;
    }
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
    } else return d->next;

    cfg->next = cfg_construct(d->next);
    return cfg;
}

cfg_node* cfg_construct_block(stmt* s) {
    if (!s) return NULL;
    
    cfg_node* node;
    stmt* p = s;
    stmt* q = s; /* follower */
    while (p != NULL) {
        switch(p->kind) {
            case STMT_FOR:
                if (q != p) q->next = NULL;
                node = cfg_block_node(s);

                cfg_node* loop_node = cfg_for_loop(p);
                cfg_push_back(
                    loop_node->value.branch->false_branch,
                    cfg_construct_block(p->next)
                );

                if (node) {
                    cfg_push_back(node, loop_node);
                    return node;
                } else return loop_node;
            case STMT_IF_ELSE:
                if (q != p) q->next = NULL;
                node = cfg_block_node(s);

                cfg_node* if_node = cfg_if_else(p);
                cfg_push_back(if_node, cfg_construct_block(p->next));
                
                if (node) {
                    cfg_push_back(node, if_node);
                    return node;
                } else return loop_node;
            case STMT_RETURN:
                stmt* dead = p->next;
                p->next = NULL;
                node = cfg_block_node(s);
                
                if (dead) cfg_dead_code(dead);

                if (node) {
                    cfg_push_back(node, cfg_return_node());
                    return node;
                } else return cfg_return_node();
            default:
                q = p;
                p = p->next;
                break;
        }
    }
    return cfg_block_node(s);
}

void cfg_dead_code(stmt* s) {
    stmt* p = s;
    while (p != NULL) {
        fpritnf(
            stderr,
            "warning: unreachable\n"
        );
        p = p->next;
    }
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
    cfg_push_back(comp->value.branch->true_branch, comp);

    cfg_set_false(comp, cfg_block_node(NULL));

    cfg_push_back(init, comp);
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