/**********************************************************************
 *                                  AST.H                             *
 **********************************************************************
 * This header defines types and functions for constructing the Abstract
 * Syntax Tree, which is done in the parser. At its top level, the AST
 * is a list of `decl`s (declarations) of variables, which can be
 * initialized to `expr`s (expressions), or functions, which can be
 * initialized to `stmt`s (statements) which, in turn, can contain `decl`s
 * or `expr`s. Much of the structure of the AST for this compiler was
 * described in Douglas Thain's 'Introduction to Compilers and Language
 * Design', but implementation and specifics (e.g. the various `expr`
 * types) were left open-ended.
 * 
 * The AST also contains `type`s and `param_list`s (part of the type of
 * a function).
 * 
 * Most functions are convenient constructors for variants of the AST
 * node types, but there are also print functions for displaying the
 * AST.
 * 
 * Also of note are the X macros in use. The `kind` enums for `stmt`,
 * `expr`, and `type` are populated by the macros `X_STMT_T`, `X_EXPR_T`,
 * and `X_TYPE_T`, which also hold associated strings which populate the
 * arrays `stmt_t_str[]`, `expr_t_str[]`, and `type_t_str[]`. This allows,
 * for example, for the type of an `expr` node `e` to be printed by
 * referencing `expr_t_str[e->kind]`.
 */
#ifndef AST_H
#define AST_H

#include "symbol.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_INDENT 10

typedef struct decl decl;
typedef struct stmt stmt;
typedef struct expr expr;
typedef struct type type;
typedef struct param_list param_list;

/**********************************************************************
 *                             DECLARATIONS                           *
 **********************************************************************/

/* A declaration of some variable or function and its type, optionally
 * initializing its value. */
struct decl {
    /* Identifier of the variable or function. */
    char* name;
    /* The data type of the variable; or function if a function. */
    type* type;
    /* The value a variable is initialized to. If none is provided, it is set
     * to 0. This is unused for functions. */
    expr* value;
    /* If the decl declares a function, and the declaration is not a prototype,
     * this points to the body of the function. */
    stmt* code;
    /* In the top-level list of declarations that makes up the program, this
     * points to the next declaration in the list, if this isn't the last one. */
    decl* next;
    /* Points to the symbol initialized by this declaration. */
    symbol* symbol;
};

decl* decl_create(
    char* name,
    type* type,
    expr* value,
    stmt* code,
    decl* next
);
decl* decl_variable(
    char* name,
    type* type,
    expr* value,
    decl* next
);
decl* decl_prototype(char* name, type* type, decl* next);
decl* decl_function(
    char* name,
    type* type,
    stmt* code,
    decl* next
);
/* for displaying the AST: */
void print_decl(decl* decl, int tab_level);

/**********************************************************************
 *                               STATEMENTS                           *
 **********************************************************************/

#define X_STMT_T \
    /* A local declaration such that: \
     *  kind = STMT_DECL \
     *  decl = declaration \
     * */ \
    X(STMT_DECL, "DECL") \
    /* An expression statement such that: \
     *  kind = STMT_EXPR \
     *  expr = expression \
     * */ \
    X(STMT_EXPR, "EXPR") \
    /* An if-else statement such that: \
     *  kind = STMT_IF_ELSE \
     *  expr = control expression \
     *  body = if body \
     *  else_body = else body \
     * */ \
    X(STMT_IF_ELSE, "IF") \
    /* A for-loop such that: \
     *  kind = STMT_FOR \
     *  init_expr = initialize counter \
     *  expr = condition \
     *  next_expr = post-loop counter operation \
     *  body = loop body \
     * */ \
    X(STMT_FOR, "FOR") \
    /* A print statement such that: \
     *  kind = STMT_PRINT \
     *  expr = expression to print \
     * */ \
    X(STMT_PRINT, "PRINT") \
    /* A return statement such that: \
     *  kind: STMT_RETURN \
     *  expr = expression to return \
     * */ \
    X(STMT_RETURN, "RETURN") \
    /* A block of statements, enclosed in curly braces, such that: \
     *  kind = STMT_BLOCK \
     *  body = enclosed statements \
     * */ \
    X(STMT_BLOCK, "BLOCK")

typedef enum {
    #define X(a, b) a,
        X_STMT_T
    #undef X
} stmt_t;

/* not actually unused, but marking as such to make gcc quieter: */
__attribute__((unused)) static char* stmt_t_str[] = {
    #define X(a, b) b,
        X_STMT_T
    #undef X
};

struct stmt {
    stmt_t kind;
    decl* decl;
    expr* init_expr;
    expr* expr;
    expr* next_expr;
    stmt* body;
    stmt* else_body;
    stmt* next;
};

/* Function to initialize a stmt. Not recommended to call this function
 * directly. Instead, call one of the other stmt_ functions for specific
 * use-cases to avoid entering redundant unneeded values. */
stmt* stmt_create(
    stmt_t kind,
    decl* decl,
    expr* init_expr,
    expr* exp,
    expr* next_expr,
    stmt* body,
    stmt* else_body,
    stmt* next
);
stmt* stmt_decl(decl* decl, stmt* next);
stmt* stmt_expr(expr* exp, stmt* next);
stmt* stmt_if(expr* control, stmt* body, stmt* next);
stmt* stmt_if_else(
    expr* control,
    stmt* body,
    stmt* else_body,
    stmt* next
);
stmt* stmt_for(
    expr* init_expr,
    expr* exp,
    expr* next_expr,
    stmt* body,
    stmt* next
);
stmt* stmt_print(expr* exp, stmt* next);
stmt* stmt_return(expr* exp, stmt* next);
stmt* stmt_block(stmt* body, stmt* next);
/* for displaying the AST: */
void print_stmt(stmt* stmt, int tab_level);

/**********************************************************************
 *                            EXPRESSIONS                             *
 **********************************************************************/

#define X_EXPR_T \
    X(EXPR_ADD, "ADD") \
    X(EXPR_SUB, "SUB") \
    X(EXPR_MUL, "MUL") \
    X(EXPR_EXP, "EXP") \
    X(EXPR_DIV, "DIV") \
    X(EXPR_MOD, "MOD") \
    X(EXPR_INC, "INC") \
    X(EXPR_DEC, "DEC") \
    X(EXPR_AND, "AND") \
    X(EXPR_OR, "OR") \
    X(EXPR_EQ, "EQ") \
    X(EXPR_N_EQ, "N_EQ") \
    X(EXPR_LESS, "LESS") \
    X(EXPR_L_EQ, "L_EQ") \
    X(EXPR_GREATER, "GREATER") \
    X(EXPR_G_EQ, "G_EQ") \
    X(EXPR_NOT, "NOT") \
    X(EXPR_ASSIGN, "ASSIGN") \
    X(EXPR_ARRAY, "ARRAY") \
    X(EXPR_IDENT, "ID") \
    X(EXPR_INDEX, "INDEX") \
    X(EXPR_FUN_CALL, "CALL") \
    X(EXPR_BOOL_LIT, "BOOL_LIT") \
    X(EXPR_CHAR_LIT, "CHAR_LIT") \
    X(EXPR_INT_LIT, "INT_LIT") \
    X(EXPR_STR_LIT, "STR_LIT")

typedef enum {
    #define X(a, b) a,
        X_EXPR_T
    #undef X
} expr_t;

/* not actually unused, but marking as such to make gcc quieter: */
__attribute__((unused)) static char* expr_t_str[] = {
    #define X(a, b) b,
        X_EXPR_T
    #undef X
};

struct expr {
    expr_t kind;
    expr* left;
    expr* right;
    char* name;
    int value;
    const char* str_value;
    /* Points to the symbol represented by this expression, if EXPR_IDENT. */
    symbol* symbol;
};

/* Function to create an expr. Not recommended to call this function directly.
 * Use one of the other functions instead. */
expr* expr_create(
    expr_t kind,
    expr* left,
    expr* right,
    char* name,
    int value,
    const char* str_value
);
expr* expr_ident(char* name);
expr* expr_binary(expr_t kind, expr* left, expr* right);
expr* expr_unary(expr_t kind, expr* left);
expr* expr_int_lit(int value);
expr* expr_bool_lit(bool value);
expr* expr_char_lit(char value);
expr* expr_str_lit(const char* value);
/* for displaying the AST: */
void print_expr(expr* expr, int tab_level);

/**********************************************************************
 *                                TYPES                               *
 **********************************************************************/

#define X_TYPE_T \
    X(TYPE_VOID, "void") \
    X(TYPE_BOOLEAN, "boolean") \
    X(TYPE_CHARACTER, "char") \
    X(TYPE_INTEGER, "integer") \
    X(TYPE_STRING, "string") \
    X(TYPE_ARRAY, "array []") \
    X(TYPE_FUNCTION, "function ()")

typedef enum {
    #define X(a, b) a,
        X_TYPE_T
    #undef X
} type_t;

/* not actually unused, but marking as such to make gcc quieter: */
__attribute__((unused)) static char* type_t_str[] = {
    #define X(a, b) b,
        X_TYPE_T
    #undef X
};

struct type {
    type_t kind;
    /* For functions and arrays. */
    type* subtype;
    /* For functions. */
    param_list* params;
};

type* type_create(type_t kind, type* subtype, param_list* params);
type* type_data(type_t kind);
type* type_array(type* subtype);
type* type_function(type* subtype, param_list* params);
/* for displaying the AST: */
void print_type(type* type, int tab_level);

/**********************************************************************
 *                           PARAMETER LISTS                          *
 **********************************************************************/

struct param_list {
    char* name;
    type* type;
    param_list* next;
    symbol* symbol;
};

param_list* create_param_list(char* name, type* type, param_list* params);
/* for displaying the AST: */
void print_param_list(param_list*, int tab_level);

#endif