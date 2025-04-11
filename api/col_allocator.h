#pragma once

#include <stddef.h>

struct col_allocator;

typedef void *(*col_malloc_fn)(struct col_allocator *self, size_t size);
typedef void *(*col_free_fn)(struct col_allocator *self, void *to_free);

typedef void col_allocator_priv_td;
struct col_allocator
{
    col_malloc_fn          malloc; // required
    col_free_fn            free;   // optional
    col_allocator_priv_td *priv; // If your functions above need additional state, it can be stored here.
};
