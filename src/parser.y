%{
#include "ast.h"
#include <stdio.h>
#include <string.h>
decl* parser_result = 0;
%}

%union {
    char char_val;
    int int_val;
    char* str_val;
    char* text;
    decl* decl;
    stmt* stmt;
    expr* expr;
    type* type;
    param_list* param_list;
};

/* Token definitions */
%token TOKEN_COMMA
%token TOKEN_COLON
%token TOKEN_CURLY_LEFT
%token TOKEN_CURLY_RIGHT
%token TOKEN_ERROR
%token <text> TOKEN_IDENT
%token TOKEN_KW_ARRAY
%token TOKEN_KW_BOOLEAN
%token TOKEN_KW_CHAR
%token TOKEN_KW_ELSE
%token TOKEN_KW_FALSE
%token TOKEN_KW_FOR
%token TOKEN_KW_FUNCTION
%token TOKEN_KW_IF
%token TOKEN_KW_INTEGER
%token TOKEN_KW_PRINT
%token TOKEN_KW_RETURN
%token TOKEN_KW_STRING
%token TOKEN_KW_TRUE
%token TOKEN_KW_VOID
%token TOKEN_KW_WHILE
%token <char_val> TOKEN_LIT_CHAR
%token <int_val> TOKEN_LIT_INTEGER
%token <str_val> TOKEN_LIT_STRING
%token TOKEN_OP_ADD
%token TOKEN_OP_AND
%token TOKEN_OP_ASSIGN
%token TOKEN_OP_DEC
%token TOKEN_OP_DIV
%token TOKEN_OP_EQ
%token TOKEN_OP_EXP
%token TOKEN_OP_GE
%token TOKEN_OP_GT
%token TOKEN_OP_INC
%token TOKEN_OP_LE
%token TOKEN_OP_LT
%token TOKEN_OP_MOD
%token TOKEN_OP_MULT
%token TOKEN_OP_NE
%token TOKEN_OP_NOT
%token TOKEN_OP_OR
%token TOKEN_OP_SUB
%token TOKEN_PAREN_LEFT
%token TOKEN_PAREN_RIGHT
%token TOKEN_SEMICOLON
%token TOKEN_SQ_LEFT
%token TOKEN_SQ_RIGHT

/* Precedence */
%right TOKEN_OP_ASSIGN
%left TOKEN_OP_OR
%left TOKEN_OP_AND
%left TOKEN_OP_EQ TOKEN_OP_NE
%left TOKEN_OP_LE TOKEN_OP_LT TOKEN_OP_GE TOKEN_OP_GT
%left TOKEN_OP_ADD TOKEN_OP_SUB
%left TOKEN_OP_MULT TOKEN_OP_DIV TOKEN_OP_MOD TOKEN_OP_EXP
%right TOKEN_OP_NOT
%left TOKEN_OP_DEC TOKEN_OP_INC

/* Return types */
%type <decl> program decl_list decl
%type <stmt> stmt stmt_list block print_list print_item
%type <expr> expr term factor arg args_list literal array item item_list id
%type <type> type data_type func_type
%type <param_list> param_list param

/* expect dangling else: */
%expect 1

%%

program     : decl_list
                { parser_result = $1; }
            ;

decl_list   : decl decl_list
                { $$ = $1; $1->next = $2; }
            | /* epsilon */
                { $$ = 0; }
            ;

decl        : id TOKEN_COLON data_type TOKEN_OP_ASSIGN expr TOKEN_SEMICOLON
                { $$ = decl_variable($1->name, $3, $5, 0); }
            | id TOKEN_COLON func_type TOKEN_OP_ASSIGN stmt
                { $$ = decl_function($1->name, $3, $5, 0); }
            | id TOKEN_COLON type TOKEN_SEMICOLON
                { $$ = decl_prototype($1->name, $3, 0); }
            ;

stmt_list   : stmt stmt_list
                {
                    $$ = $1;
                    if ($1->kind == STMT_PRINT) {
                        stmt_print_pushback($$, $2);
                    } else {
                        $1->next = $2;
                    }
                }
            | /* epsilon */
                { $$ = 0; }
            ;

stmt        : decl
                { $$ = stmt_decl($1, 0); }
            | expr TOKEN_SEMICOLON
                { $$ = stmt_expr($1, 0); }
            | TOKEN_KW_IF TOKEN_PAREN_LEFT expr TOKEN_PAREN_RIGHT stmt TOKEN_KW_ELSE stmt
                { $$ = stmt_if_else($3, $5, $7, 0); }
            | TOKEN_KW_IF TOKEN_PAREN_LEFT expr TOKEN_PAREN_RIGHT stmt
                { $$ = stmt_if($3, $5, 0); }
            | TOKEN_KW_FOR TOKEN_PAREN_LEFT expr TOKEN_SEMICOLON expr TOKEN_SEMICOLON expr
              TOKEN_PAREN_RIGHT stmt
                { $$ = stmt_for($3, $5, $7, $9, 0); }
            | TOKEN_KW_PRINT print_item print_list TOKEN_SEMICOLON
                { $$ = $2; $$->next = $3; }
            | TOKEN_KW_RETURN expr TOKEN_SEMICOLON
                { $$ = stmt_return($2, 0); }
            | block
                { $$ = $1; }
            ;

block       : TOKEN_CURLY_LEFT stmt_list TOKEN_CURLY_RIGHT
                { $$ = stmt_block($2, 0); }
            ;

expr        : id TOKEN_OP_ASSIGN expr
                { $$ = expr_binary(EXPR_ASSIGN, $1, $3); }
            | expr TOKEN_OP_AND expr
                { $$ = expr_binary(EXPR_AND, $1, $3); }
            | expr TOKEN_OP_OR expr
                { $$ = expr_binary(EXPR_OR, $1, $3); }
            | expr TOKEN_OP_EQ expr
                { $$ = expr_binary(EXPR_EQ, $1, $3); }
            | expr TOKEN_OP_NE expr
                { $$ = expr_binary(EXPR_N_EQ, $1, $3); }
            | expr TOKEN_OP_LE expr
                { $$ = expr_binary(EXPR_L_EQ, $1, $3); }
            | expr TOKEN_OP_GE expr
                { $$ = expr_binary(EXPR_G_EQ, $1, $3); }
            | expr TOKEN_OP_LT expr
                { $$ = expr_binary(EXPR_LESS, $1, $3); }
            | expr TOKEN_OP_GT expr
                { $$ = expr_binary(EXPR_GREATER, $1, $3); }
            | expr TOKEN_OP_ADD term
                { $$ = expr_binary(EXPR_ADD, $1, $3); }
            | TOKEN_OP_NOT expr
                { $$ = expr_unary(EXPR_NOT, $2); }
            | expr TOKEN_OP_SUB term
                { $$ = expr_binary(EXPR_SUB, $1, $3); }
            | term
                { $$ = $1; }
            ;

term        : term TOKEN_OP_MULT factor
                { $$ = expr_binary(EXPR_MUL, $1, $3); }
            | term TOKEN_OP_DIV factor
                { $$ = expr_binary(EXPR_DIV, $1, $3); }
            | term TOKEN_OP_EXP factor
                { $$ = expr_binary(EXPR_EXP, $1, $3); }
            | term TOKEN_OP_MOD factor
                { $$ = expr_binary(EXPR_MOD, $1, $3); }
            | factor
                { $$ = $1; }
            ;

factor      : TOKEN_OP_SUB factor
                { $$ = expr_binary(EXPR_SUB, 0, $2); }
            | TOKEN_PAREN_LEFT expr TOKEN_PAREN_RIGHT
                { $$ = $2; }
            | id TOKEN_PAREN_LEFT args_list TOKEN_PAREN_RIGHT
                { $$ = expr_binary(EXPR_FUN_CALL, $1, $3); }
            | id TOKEN_SQ_LEFT expr TOKEN_SQ_RIGHT
                { $$ = expr_binary(EXPR_INDEX, $1, $3); }
            | id TOKEN_OP_INC
                { $$ = expr_unary(EXPR_INC, $1); }
            | id TOKEN_OP_DEC
                { $$ = expr_unary(EXPR_DEC, $1); }
            | id
                { $$ = $1; }
            | array
                { $$ = $1; }
            | literal
                { $$ = $1; }
            ;

id          : TOKEN_IDENT
                {
                    char* ident = malloc(sizeof(char) * (strlen($1) + 1));
                    strcpy(ident, $1);
                    $$ = expr_ident(ident);
                }
            ;

args_list   : arg TOKEN_COMMA args_list
                { $$ = $1; $1->right = $3; }
            | arg
                { $$ = $1; }
            | /* epsilon */
                { $$ = 0; }
            ;

arg         : id
                { $$ = $1; }
            | literal
                { $$ = $1; }
            ;

literal     : TOKEN_LIT_CHAR
                { $$ = expr_char_lit($1); }
            | TOKEN_LIT_INTEGER
                { $$ = expr_int_lit($1); }
            | TOKEN_LIT_STRING
                { $$ = expr_str_lit($1); }
            | TOKEN_KW_FALSE
                { $$ = expr_bool_lit(false); }
            | TOKEN_KW_TRUE
                { $$ = expr_bool_lit(true); }
            ;

array       : TOKEN_CURLY_LEFT item item_list TOKEN_CURLY_RIGHT
                { $$ = $2; $2->kind = EXPR_ARRAY; $2->right = $3; }
            ;

item_list   : item TOKEN_COMMA item_list
                { $$ = $1; $1->right = $3; }
            | item
                { $$ = $1; }
            | /* epsilon */
                { $$ = 0; }
            ;

item        : id
                { $$ = $1; }
            | literal
                { $$ = $1; }
            ;

type        : data_type
                { $$ = $1; }
            | func_type
                { $$ = $1; }
            ;

data_type   : TOKEN_KW_VOID
                { $$ = type_data(TYPE_VOID); }
            | TOKEN_KW_BOOLEAN
                { $$ = type_data(TYPE_BOOLEAN); }
            | TOKEN_KW_CHAR
                { $$ = type_data(TYPE_CHARACTER); }
            | TOKEN_KW_INTEGER
                { $$ = type_data(TYPE_INTEGER); }
            | TOKEN_KW_STRING
                { $$ = type_data(TYPE_STRING); }
            | TOKEN_KW_ARRAY TOKEN_SQ_LEFT TOKEN_SQ_RIGHT type
                { $$ = type_array($4); }
            | TOKEN_KW_ARRAY TOKEN_SQ_LEFT TOKEN_LIT_INTEGER TOKEN_SQ_RIGHT type
                { $$ = type_array($5); }
            ;

func_type   : TOKEN_KW_FUNCTION type TOKEN_PAREN_LEFT param_list TOKEN_PAREN_RIGHT
                { $$ = type_function($2, $4); }

param_list  : param TOKEN_COMMA param_list
                { $$ = $1; $1->next = $3; }
            | param
                { $$ = $1; }
            | /* epsilon */
                { $$ = 0; }
            ;

param       : id TOKEN_COLON type
                { $$ = create_param_list($1->name, $3, 0); }
            ;

print_list  : TOKEN_COMMA print_item print_list
                { $$ = $2; $$->next = $3; }
            | /* epsilon */
                { $$ = 0; }
            ;

print_item  : expr
                { $$ = stmt_print($1, 0); }
%%

int yyerror(char* s) {
    printf("parse error: %s\n", s);
    return 1;
}
