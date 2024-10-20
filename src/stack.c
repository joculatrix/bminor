#include "stack.h"

stack* stack_new() {
    stack* s = malloc(sizeof(stack));
    if (s == NULL) {
        return NULL;
    }
    s->head = NULL;
    s->length = 0;
    return s;
}

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

list_node* peek(stack* s) {
    if (s->length == 0) {
        return NULL;
    }
    return s->head;
}

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