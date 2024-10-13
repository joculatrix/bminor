/*
Based on an article by Ben Hoyt:
https://benhoyt.com/writings/hash-table-in-c/ 

The code was released with the following license:

MIT License

Copyright (c) 2021 Ben Hoyt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef HASH_H
#define HASH_H

#include <stdbool.h>
#include <stddef.h>

typedef struct ht ht;

ht* ht_create(void);

void ht_destroy(ht* table);

/* Get item with given key from table, and return the value, or null. */
void* ht_get(ht* table, const char* key);

/* Set the item with key `key` to value `value` (can't be null).
 * The key is copied to new memory (if not present), and the address
 * is returned. */
const char* ht_set(ht* table, const char* key, void* value);

size_t ht_length(ht* table);

/* Hash table iterator */
typedef struct {
    const char* key;    /* current key */
    void* value;        /* current value */
    /* don't use: */
    ht* _table;       /* reference to table being iterated over */
    size_t _index;      /* current index */
} ht_iter;

ht_iter ht_iterate(ht* table);

/* Move to next item, update key and value of hash_iter. Return
 * true if there's a new item, false if there are no more items. */
bool ht_next(ht_iter* it);

#endif