/* ********************************************************************
 *                                CFG.H                               *
 * ******************************************************************** 
 * This header describes functions and types for restructuring the AST
 * as a CFG -- Control Flow Graph (not to be confused with context-free
 * grammar). In summary, passing the root `decl` of the AST into
 * `cfg_construct()` will return the CFG of the program. This, of course,
 * mostly affects function bodies, and not so much top-level declarations.
 */
#ifndef CFG_H
#define CFG_H

#include "ast.h"

/**********************************************************************
 *                                TYPES                               *
 **********************************************************************/

/* group of linear statements */
typedef struct {
    stmt* stmt; /* retains its own `next` values until a conditional */
} cfg_block;

/* branching statement */
typedef struct {
    expr* condition;
    cfg_node* true_branch;
    cfg_node* false_branch;
} cfg_branch;

/* differentiates between linear and branching nodes */
union cfg_node_u {
    cfg_block* block;
    cfg_branch* branch;
};

/* for safety */
typedef enum {
    CFG_BLOCK,
    CFG_BRANCH
} cfg_node_t;

/* the actual nodes in the graph structure */
typedef struct {
    cfg_node_t kind;
    union cfg_node_u value;
    cfg_node* next;
} cfg_node;

union cfg_u {
    expr* exp; /* for global variables */
    cfg_node* cfg_node; /* for functions */
};

typedef enum {
    VAR,
    FUNC,
} cfg_t;

/* Top-level structure to encompass both global variable declarations
 * and the CFGs for functions in one IR */
typedef struct {
    cfg_t kind;
    symbol* symbol;
    union cfg_u value;
    cfg* next;
} cfg;

/**********************************************************************
 *                              FUNCTIONS                             *
 **********************************************************************/

/* cfg structure/utility */

cfg_node* cfg_block_node(stmt* stmt);
cfg_node* cfg_branch_node(expr* exp);
int cfg_set_next(cfg_node* node, cfg_node* next);
int cfg_set_true(cfg_node* node, cfg_node* true_branch);
int cfg_set_false(cfg_node* node, cfg_node* false_branch);
void cfg_push_back(cfg_node* branch, cfg_node* back);

/* construction */

/* This is the primary function that should be called externally
 * to construct the graph. The others are helper functions. */
cfg* cfg_construct(decl* d);

cfg_node* cfg_construct_block(stmt* s);
cfg_node* cfg_for_loop(stmt* s);
cfg_node* cfg_if_else(stmt* s);

#endif