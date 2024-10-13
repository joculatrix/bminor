#ifndef AST_H
#define AST_H

#include <stdbool.h>

#define MAX_INDENT 10

struct decl;
struct stmt;
struct expr;
struct type;
struct params_list;

/* A declaration of some variable or function and its type, optionally
 * initializing its value. A B-Minor program, at its top level, is a list of
 * declarations. */
struct decl {
    /* Identifier of the variable or function. */
    char* name;
    /* The data type of the variable; or function if a function. */
    struct type* type;
    /* The value a variable is initialized to. If none is provided, it is set
     * to 0. This is unused for functions. */
    struct expr* value;
    /* If the decl declares a function, and the declaration is not a prototype,
     * this points to the body of the function. */
    struct stmt* code;
    /* In the top-level list of declarations that makes up the program, this
     * points to the next declaration in the list, if this isn't the last one. */
    struct decl* next;
};

struct decl* decl_create(
    char* name,
    struct type* type,
    struct expr* value,
    struct stmt* code,
    struct decl* next
);
struct decl* decl_variable(
    char* name,
    struct type* type,
    struct expr* value,
    struct decl* next
);
struct decl* decl_prototype(char* name, struct type* type, struct decl* next);
struct decl* decl_function(
    char* name,
    struct type* type,
    struct stmt* code,
    struct decl* next
);
/* for displaying the AST: */
void print_decl(struct decl* decl, int tab_level);

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
 #define X(a, b)    a,
    X_STMT_T
 #undef X
} stmt_t;

static char* stmt_t_str[] = {
 #define X(a, b)    b,
    X_STMT_T
 #undef X
};

struct stmt {
    stmt_t kind;
    struct decl* decl;
    struct expr* init_expr;
    struct expr* expr;
    struct expr* next_expr;
    struct stmt* body;
    struct stmt* else_body;
    struct stmt* next;
};

/* Function to initialize a stmt. Not recommended to call this function directly.
 * Instead, call one of the other stmt_ functions for specific use-cases to
 * avoid entering redundant unneeded values. */
struct stmt* stmt_create(
    stmt_t kind,
    struct decl* decl,
    struct expr* init_expr,
    struct expr* expr,
    struct expr* next_expr,
    struct stmt* body,
    struct stmt* else_body,
    struct stmt* next
);
struct stmt* stmt_decl(struct decl* decl, struct stmt* next);
struct stmt* stmt_expr(struct expr* expr, struct stmt* next);
struct stmt* stmt_if(struct expr* control, struct stmt* body, struct stmt* next);
struct stmt* stmt_if_else(
    struct expr* control,
    struct stmt* body,
    struct stmt* else_body,
    struct stmt* next
);
struct stmt* stmt_for(
    struct expr* init_expr,
    struct expr* expr,
    struct expr* next_expr,
    struct stmt* body,
    struct stmt* next
);
struct stmt* stmt_print(struct expr* expr, struct stmt* next);
struct stmt* stmt_return(struct expr* expr, struct stmt* next);
struct stmt* stmt_block(struct stmt* body, struct stmt* next);
/* for displaying the AST: */
void print_stmt(struct stmt* stmt, int tab_level);

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
    X(EXPR_FUN_ARG, "ARG") \
    X(EXPR_BOOL_LIT, "BOOL_LIT") \
    X(EXPR_CHAR_LIT, "CHAR_LIT") \
    X(EXPR_INT_LIT, "INT_LIT") \
    X(EXPR_STR_LIT, "STR_LIT")

typedef enum {
    #define X(a, b)   a,
        X_EXPR_T
    #undef X
} expr_t;

static char *expr_t_str[] = {
 #define X(a, b)      b,
    X_EXPR_T
 #undef X
};

struct expr {
    expr_t kind;
    struct expr* left;
    struct expr* right;
    char* name;
    int value;
    const char* str_value;
};

/* Function to create an expr. Not recommended to call this function directly.
 * Use one of the other functions instead. */
struct expr* expr_create(
    expr_t kind,
    struct expr* left,
    struct expr* right,
    char* name,
    int value,
    const char* str_value
);
struct expr* expr_ident(char* name);
struct expr* expr_binary(expr_t kind, struct expr* left, struct expr* right);
struct expr* expr_unary(expr_t kind, struct expr* left);
struct expr* expr_int_lit(int value);
struct expr* expr_bool_lit(bool value);
struct expr* expr_char_lit(char value);
struct expr* expr_str_lit(const char* value);
/* for displaying the AST: */
void print_expr(struct expr* expr, int tab_level);

typedef enum {
    TYPE_VOID,
    TYPE_BOOLEAN,
    TYPE_CHARACTER,
    TYPE_INTEGER,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_FUNCTION
} type_t;

struct type {
    type_t kind;
    /* For functions and arrays. */
    struct type* subtype;
    /* For functions. */
    struct param_list *params;
};

struct type* type_create(type_t kind, struct type* subtype, struct param_list* params);
struct type* type_data(type_t kind);
struct type* type_array(struct type* subtype);
struct type* type_function(struct type* subtype, struct param_list* params);
/* for displaying the AST: */
void print_type(struct type* type, int tab_level);

struct param_list {
    char* name;
    struct type* type;
    struct param_list* next;
};

struct param_list* param_list(char* name, struct type* type, struct param_list* params);
/* for displaying the AST: */
void print_param_list(struct param_list*, int tab_level);

#endif
