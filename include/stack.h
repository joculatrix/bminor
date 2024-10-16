#ifndef STACK_H
#define STACK_H

#include "hash.h"
#include <stdlib.h>

typedef struct list_node list_node;

/* The list node defined here specifically holds symbol tables.
 * Obviously this type should change if reused for another application. */
struct list_node {
    ht* table;
    /* compiler gets unhappy when this doesn't say struct: */
    list_node* next;
};

typedef struct {
    int length;
    list_node* head;
} stack;

/* Because stacks are pretty simple, it's defined in-header. */

/* Initializes a pointer to a new stack. The stack is initialized 
 * with length 0 -- it does not come with an initial node assigned
 * to head. */
stack* stack_new() {
    stack* s = malloc(sizeof(stack));
    if (s == NULL) {
        return NULL;
    }
    s->head = NULL;
    s->length = 0;
    return s;
}

/* Inserts a new node holding the given table at the stacks' head.
 * Returns `true` if successful, or `false` if memory allocation fails. */
bool push(stack* s, ht* table) {
    list_node* node = malloc(sizeof(list_node));
    if (node == NULL) {
        return false;
    }
    node->table = table;
    node->next = s->head;
    s->head = node;
    ++(s->length);
    return true;
}

/* Return a pointer to the stack's head, without removing it. */
list_node* peek(stack* s) {
    if (s->length == 0) {
        return NULL;
    }
    return s->head;
}

/* Remove the stack's head, and return the hash table. */
ht* pop(stack* s) {
    if (s->length == 0) {
        return NULL;
    }
    
    list_node* new_head = NULL;
    if (s->head->next != NULL) new_head = s->head->next;

    ht* table = s->head->table;
    free(s->head);
    s->head = new_head;
    --(s->length);

    return table;
}

#endif