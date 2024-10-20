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

#include "cfg.h"
#include "semantics.h"
#include <string.h>

/**********************************************************************
 *                           TYPES & GLOBALS                          *
 **********************************************************************/

int label_count = 0;
int str_count = 0;

typedef struct {
    const char* name;
    bool used;
} reg;

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

typedef struct data_entry data_entry;

struct data_entry {
    const char* entry;
    data_entry* next;
};

data_entry* data;

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

void expr_bool_codegen(expr* e, const char* instruction, int true_label);
void bool_val_codegen(expr* e, const char* instruction);

void decl_codegen(decl* d);

void func_codegen(symbol* f, stmt* code);

void stmt_codegen(stmt* s, const char* func_name);
void expr_codegen(expr* e);

/* print: */

void print_bool(int reg);
void print_char(int reg);
void print_str_codegen(int reg);
void print_str_lit_codegen(const char* s);
void print_i_to_a(int reg);

#endif