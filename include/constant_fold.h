/**********************************************************************
 *                           CONSTANT_FOLD.H                          *
 **********************************************************************
 * This header holds functions for performing a recursive constant-
 * folding optimization on the AST. The program MUST have already been
 * through typechecking without any errors, or certain assumptions made
 * in the implementation will fail.
 */
#ifndef CONSTANT_FOLD_H
#define CONSTANT_FOLD_H

#include "ast.h"
#include <stdlib.h>

bool is_constant(expr* e);
int pow_int(int a, int b);

decl* constant_fold_decl(decl* d);
stmt* constant_fold_stmt(stmt* s);
expr* constant_fold_expr(expr* e);

#endif