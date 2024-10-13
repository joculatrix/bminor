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
#include "hash.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define INITIAL_CAPACITY 16

#define FNV_OFFSET 14695981038346656037UL
#define FNV_PRIME 1099511628211UL

typedef struct {
    const char* key; /* NULL if empty */
    void* value;
} ht_entry;

struct ht {
    ht_entry* entries;
    size_t capacity;    /* size of `entries` */
    size_t length;      /* number of actual items */
};

ht* ht_create() {
    /* allocate space for table struct: */
    ht* table = malloc(sizeof(ht));
    if (table == NULL) {
        return NULL;
    }
    table->length = 0;
    table->capacity = INITIAL_CAPACITY;

    /* allocate space for entries */
    table->entries = calloc(table->capacity, sizeof(ht_entry));
    if (table->entries == NULL) {
        free(table);
        return NULL;
    }
    return table;
}

void ht_destroy(ht* table) {
    /* free allocated keys */
    for (size_t i = 0; i < table->capacity; i++) {
        free((void*)table->entries[i].key);
    }

    /* free entries and table */
    free(table->entries);
    free(table);
}

/* return 64-bit FNV-1a hash for key */
static uint64_t hash_key(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void* ht_get(ht* table, const char* key) {
    uint64_t hash = hash_key(key);
    size_t i = (size_t)(hash & (uint64_t)(table->capacity - 1));

    while (table->entries[i].key != NULL) {
        if (strcmp(key, table->entries[i].key) == 0) {
            return table->entries[i].value;
        }
        i++;
        if (i >= table->capacity) {
            i = 0;
        }
    }
    return NULL;
}

static const char* ht_set_entry(
    ht_entry* entries,
    size_t capacity,
    const char* key,
    void* value,
    size_t* plength
) {
    uint64_t hash = hash_key(key);
    size_t i = (size_t)(hash & (uint64_t)(capacity - 1));

    while (entries[i].key != NULL) {
        if (strcmp(key, entries[i].key) == 0) {
            entries[i].value = value;
            return entries[i].key;
        }
        i++;
        if (i >= capacity) {
            i = 0;
        }
    }

    if (plength != NULL) {
        key = strdup(key);
        if (key == NULL) {
            return NULL;
        }
        (*plength)++;
    }
    entries[i].key = (char*)key;
    entries[i].value = value;
    return key;
}

/* true = success, false = out of memory */
static bool ht_expand(ht* table) {
    size_t new_capacity = table->capacity * 2;
    if (new_capacity < table->capacity) {
        return false;
    }

    ht_entry* new_entries = calloc(new_capacity, sizeof(ht_entry));
    if (new_entries == NULL) {
        return false;
    }

    /* move all non-empty entries to new table */
    for (size_t i = 0; i < table->capacity; i++) {
        ht_entry entry = table->entries[i];
        if (entry.key != NULL) {
            ht_set_entry(
                new_entries,
                new_capacity,
                entry.key,
                entry.value,
                NULL
            );
        }
    }

    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
    return true;
}

const char* ht_set(ht* table, const char* key, void* value) {
    if (value == NULL) {
        return NULL;
    }

    /* expand if length exceeds capacity / 2 */
    if (table->length >= table->capacity / 2) {
        if (!ht_expand(table)) {
            return NULL;
        }
    }

    /* set entry, update length */
    return ht_set_entry(
        table->entries,
        table->capacity,
        key,
        value,
        &table->length
    );
}

size_t ht_length(ht* table) {
    return table->length;
}

ht_iter ht_iterate(ht* table) {
    ht_iter it;
    it._table = table;
    it._index = 0;
    return it;
}

bool ht_next(ht_iter* it) {
    ht* table = it->_table;
    while (it->_index < table->capacity) {
        size_t i = it->_index;
        it->_index++;
        if (table->entries[i].key != NULL) {
            ht_entry entry = table->entries[i];
            it->key = entry.key;
            it->value = entry.value;
            return true;
        }
    }
    return false;
}