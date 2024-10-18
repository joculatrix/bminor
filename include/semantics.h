#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "ast.h"

/* name resolution -- in `symbol.c` */

void decl_resolve(decl* d);
void expr_resolve(expr* e);
void stmt_resolve(stmt* s);
void param_list_resolve(param_list* p);

/* type checking -- in `typecheck.c` */

void decl_typecheck(decl* d);
void stmt_typecheck(stmt* s);
type* expr_typecheck(expr* e);

bool type_equals(type* a, type* b);
type* type_copy(type* t);
void type_delete(type* t);
param_list* param_list_copy(param_list* p);
void param_list_delete(param_list* p);

#endif