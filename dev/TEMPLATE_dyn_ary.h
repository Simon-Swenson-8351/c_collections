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
typedef struct
{
    TEMPLATE_allocator *allocator; ///< optional
    TEMPLATE_datatype *data; ///< data buffer
    size_t len; ///< number of elements in the array
    size_t cap; ///< capacity of elements of the buffer
    float growth_factor; ///< how large to grow the array when it is full
} TEMPLATE_dyn_ary_typename;

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
TEMPLATE_dyn_ary_typename ## _init(
    TEMPLATE_dyn_ary_typename *to_init,
    TEMPLATE_allocator *allocator,
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
TEMPLATE_dyn_ary_typename ## _copy(
    TEMPLATE_dyn_ary_typename *dest,
    TEMPLATE_dyn_ary_typename *src
);

/**
 * Clears a dynamic array, clearing each element and freeing the dynamic array's buffer
 * 
 * @param to_clear required, must be initialized
 */
void
TEMPLATE_dyn_ary_typename ## _clear(
    TEMPLATE_dyn_ary_typename *to_clear
);

// metadata
size_t
TEMPLATE_dyn_ary_typename ## _len(
    TEMPLATE_dyn_ary_typename *self
);

// element-level operations
// insertion via move
enum col_result
TEMPLATE_dyn_ary_typename ## _insert_at(
    TEMPLATE_dyn_ary_typename *dyn_ary,
    TEMPLATE_datatype *to_insert,
    size_t index
);
enum col_result
TEMPLATE_dyn_ary_typename ## _push_back(
    TEMPLATE_dyn_ary_typename *dyn_ary,
    TEMPLATE_datatype *to_insert
);

// retrieval via borrowing
TEMPLATE_datatype *
TEMPLATE_dyn_ary_typename ## _get(
    TEMPLATE_dyn_ary_typename *dyn_ary,
    size_t idx
);
// removal via move
enum col_result
TEMPLATE_dyn_ary_typename ## _rm(
    TEMPLATE_dyn_ary_typename *dyn_ary,
    size_t idx,
    TEMPLATE_datatype *removed_elem // must be uninitialized or cleared
);
enum col_result
TEMPLATE_dyn_ary_typename ## _pop_back(
    TEMPLATE_dyn_ary_typename *dyn_ary,
    TEMPLATE_datatype *removed_elem // must be uninitialized or cleared
);
// searching
enum col_result
TEMPLATE_dyn_ary_typename ## _lin_search(
    TEMPLATE_dyn_ary_typename *dyn_ary,
    TEMPLATE_datatype *elem,
    size_t *idx_if_found
);
enum col_result
TEMPLATE_dyn_ary_typename ## _bin_search(
    TEMPLATE_dyn_ary_typename *dyn_ary,
    TEMPLATE_datatype *elem,
    size_t *idx_if_found
);


// list-level operations
// removes unused data in the dynamic array such that capacity == length
enum col_result
TEMPLATE_dyn_ary_typename ## _trim(
    TEMPLATE_dyn_ary_typename *dyn_ary
);
// second will be cleared
enum col_result
TEMPLATE_dyn_ary_typename ## _cat(
    TEMPLATE_dyn_ary_typename *first,
    TEMPLATE_dyn_ary_typename *second
);

enum col_result
TEMPLATE_dyn_ary_typename ## _sort_cmp(
    TEMPLATE_dyn_ary_typename *to_sort,
    int(*sort_fn)(TEMPLATE_datatype *a, TEMPLATE_datatype *b)
);
