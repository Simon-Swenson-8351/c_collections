#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "col_allocator.h"
#include "col_elem.h"
#include "col_result.h"
#include "col_sort.h"

/**
 * A dynamic array structure
 */
struct col_dyn_ary
{
    struct col_allocator *allocator; ///< optional
    struct col_elem_metadata *elem_metadata; ///< required
    uint8_t *data; ///< data buffer
    size_t len; ///< number of elements in the array
    size_t cap; ///< capacity of elements of the buffer
    float growth_factor; ///< how large to grow the array when it is full
};

// init, copy, clear
/**
 * Initializes a dynamic array
 * 
 * @param to_init required, must be uninitialized or cleared
 * @param allocator optional
 * @param elem_metadata required
 * @param initial_cap
 * @param growth_factor must be > 1.0, a reasonable value is 2.0
 * @return any error encountered
 */
enum col_result
col_dyn_ary_init(
    struct col_dyn_ary *to_init,
    struct col_allocator *allocator,
    struct col_elem_metadata *elem_metadata,
    size_t initial_cap,
    float growth_factor
);

/**
 * Copies a dynamic array from src to dest
 * 
 * @param dest required, must be uninitialized or cleared
 * @param src required
 * @return any error encountered
 */
enum col_result
col_dyn_ary_copy(
    struct col_dyn_ary *dest,
    struct col_dyn_ary *src
);

/**
 * Clears a dynamic array, clearing each element and freeing the dynamic array's buffer
 * 
 * @param to_clear required, must be initialized
 */
void
col_dyn_ary_clear(
    struct col_dyn_ary *to_clear
);

// metadata
size_t
col_dyn_ary_len(
    struct col_dyn_ary *self
);

// element-level operations
// insertion via move
enum col_result
col_dyn_ary_insert_at(
    struct col_dyn_ary *dyn_ary,
    void *to_insert,
    size_t index
);
enum col_result
col_dyn_ary_push_back(
    struct col_dyn_ary *dyn_ary,
    void *to_insert
);

// retrieval via borrowing
void *
col_dyn_ary_get(
    struct col_dyn_ary const *dyn_ary,
    size_t idx
);
// removal via move
enum col_result
col_dyn_ary_rm(
    struct col_dyn_ary *dyn_ary,
    size_t idx,
    void *removed_elem // must be uninitialized or cleared
);
enum col_result
col_dyn_ary_pop_back(
    struct col_dyn_ary *dyn_ary,
    void *removed_elem // must be uninitialized or cleared
);
// searching
enum col_result
col_dyn_ary_lin_search(
    struct col_dyn_ary *dyn_ary,
    void *elem,
    size_t *idx_if_found
);
enum col_result
col_dyn_ary_bin_search(
    struct col_dyn_ary *dyn_ary,
    void *elem,
    size_t *idx_if_found
);


// list-level operations
// removes unused data in the dynamic array such that capacity == length
enum col_result
col_dyn_ary_trim(
    struct col_dyn_ary *dyn_ary
);
// second will be cleared
enum col_result
col_dyn_ary_cat(
    struct col_dyn_ary *first,
    struct col_dyn_ary *second
);

enum col_result
col_dyn_ary_sort_cmp(
    struct col_dyn_ary *to_sort,
    col_sort_cmp_fn sort_fn
);
