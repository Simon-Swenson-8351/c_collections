#pragma once

#include <stddef.h>

struct col_allocator;

/**
 * A function that allocates memory
 * 
 * Functions using this typedef should assume the following expectations.
 * @pre self != NULL
 * @pre self is properly initialized
 * @pre size > 0
 * @return the newly-allocated memory or NULL if the allocation failed
 */
typedef void *(*col_malloc_fn)(struct col_allocator *self, size_t size);

/**
 * A function that frees memory
 * 
 * Functions using this typedef should assume the following expectations.
 * @pre self != NULL and is properly initialized
 * @pre to_free was previously allocated using the allocator's corresponding malloc function
 */
typedef void (*col_free_fn)(struct col_allocator *self, void *to_free);

/// The private data of the allocator that can be accessed by its corresponding malloc and free functions
typedef void col_allocator_priv_td;

/**
 * represents a memory allocator in a generic way
 */
struct col_allocator
{
    col_malloc_fn          malloc; ///< required
    col_free_fn            free;   ///< optional
    col_allocator_priv_td *priv;   ///< If your functions above need additional state, it can be stored here.
};

/**
 * Allocates memory according to the allocator given
 * 
 * If no allocator is given, stdlib malloc will be used.
 * @pre self may be NULL
 * @pre if self is not null, it is properly initialized
 * @pre size > 0
 */
void *col_allocator_malloc(struct col_allocator *self, size_t size);

/**
 * Frees memory according to the allocator given
 * 
 * If the free function of the given allocator is NULL, no operation will be performed.
 * If no allocator is given, stdlib free will be used.
 * @pre self may be NULL
 * @pre if self is not null, it is properly initialized
 * @pre to_free was previously allocated using the allocator's corresponding malloc function
 */
void col_allocator_free(struct col_allocator *self, void *to_free);
