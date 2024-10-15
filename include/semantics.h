#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "ast.h"

/* name resolution -- in `symbol.c` */

void decl_resolve(struct decl* d);
void expr_resolve(struct expr* e);
void stmt_resolve(struct stmt* s);
void param_list_resolve(struct param_list* p);

/* type checking -- in `typecheck.c` */

void decl_typecheck(struct decl* d);
void stmt_typecheck(struct stmt* s);
struct type* expr_typecheck(struct expr* e);

bool type_equals(struct type* a, struct type* b);
struct type* type_copy(struct type* t);
void type_delete(struct type* t);
void param_list_delete(struct param_list* p);

#endif