/**********************************************************************
 *                               STACK.H                              *
 **********************************************************************
 * This header defines a stack of hash tables (symbol tables) for name-
 * resolution, with `push()`, `pop()`, and `peek()` functions.
 */
#ifndef STACK_H
#define STACK_H

#include "hash.h"
#include <stdlib.h>

/**********************************************************************
 *                                TYPES                               *
 **********************************************************************/

/* typedef goes up here so a `list_node` can have another `list_node`: */
typedef struct list_node list_node;

/* The list node defined here specifically holds symbol tables.
 * Obviously this type should change if reused for another application. */
struct list_node {
    ht* table;
    list_node* next;
};

typedef struct {
    int length;
    list_node* head;
} stack;

/**********************************************************************
 *                              FUNCTIONS                             *
 **********************************************************************/

/* Initializes a pointer to a new stack. The stack is initialized 
 * with length 0 -- it does not come with an initial node assigned
 * to head. */
stack* stack_new();

/* Inserts a new node holding the given table at the stacks' head.
 * Returns `true` if successful, or `false` if memory allocation fails. */
bool push(stack* s, ht* table);

/* Return a pointer to the stack's head, without removing it. */
list_node* peek(stack* s);

/* Remove the stack's head, and return the hash table. */
ht* pop(stack* s);

#endif