/**********************************************************************
 *                             CODEGEN.H                              *
 **********************************************************************
 * This header defines types, global values, and functions for generating
 * the program executable. Currently this process works off the control-
 * flow graph and generates x86_64 Assembly.
 * 
 * Implementation of this header is separated into `codegen/codegen.c`,
 * `codegen/print.c`, and `codegen/utility.c`
 */
#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "cfg.h"
#include "semantics.h"
#include <string.h>

/**********************************************************************
 *                           TYPES & GLOBALS                          *
 **********************************************************************/
typedef struct {
    const char* name;
    bool used;
} reg;

typedef struct data_entry data_entry;

struct data_entry {
    const char* entry;
    data_entry* next;
};

/**********************************************************************
 *                              FUNCTIONS                             *
 **********************************************************************/

const char* symbol_address(symbol* s);

/* for strings: */

int add_str(const char* s, bool newline);
const char* str_label(int label);

/* for register allocation: */

int scratch_alloc();
void scratch_free(int r);
const char* scratch_name(int r);

/* for jump labels: */

int create_label();
const char* label_name(int label);

/* codegen: */

void codegen(cfg* cfg);

void cfg_codegen(cfg* cfg);

void expr_bool_codegen(expr* e, const char* instruction, int true_label);
void bool_val_codegen(expr* e, const char* instruction);

void decl_codegen(decl* d);

void func_codegen(cfg* func_decl);
void func_body_codegen(const char* func_name, cfg_node* node);

void stmt_codegen(stmt* s, const char* func_name);
void expr_codegen(expr* e);

/* print: */

void print_bool(int reg);
void print_char(int reg);
void print_str_codegen(int reg);
void print_str_lit_codegen(const char* s);
void print_i_to_a(int reg);

#endif