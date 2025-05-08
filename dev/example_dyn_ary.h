#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "col_allocator.h"
#include "col_elem.h"
#include "col_result.h"
#include "col_sort.h"
#include "example_external_defns.h"

////////////////////////////////////////////////////////////////////////////////
/// BEGIN required external defns
////////////////////////////////////////////////////////////////////////////////
/**
 * A dynamic array structure
 */
typedef struct
{
    struct col_allocator *allocator; ///< optional
    MACRO_ARG_datatype *data; ///< data buffer
    size_t len; ///< number of elements in the array
    size_t cap; ///< capacity of elements of the buffer
    float growth_factor; ///< how large to grow the array when it is full
} MACRO_ARG_dyn_ary_typename;

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
MACRO_ARG_dyn_ary_typename_init(
    MACRO_ARG_dyn_ary_typename *to_init,
    struct col_allocator *allocator,
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
MACRO_ARG_dyn_ary_typename_copy(
    MACRO_ARG_dyn_ary_typename *dest,
    MACRO_ARG_dyn_ary_typename *src
);

/**
 * Clears a dynamic array, clearing each element and freeing the dynamic array's buffer
 * 
 * @param to_clear required, must be initialized
 */
void
MACRO_ARG_dyn_ary_typename_clear(
    MACRO_ARG_dyn_ary_typename *to_clear
);

// metadata
size_t
MACRO_ARG_dyn_ary_typename_len(
    MACRO_ARG_dyn_ary_typename *self
);

// element-level operations
// insertion via move
enum col_result
MACRO_ARG_dyn_ary_typename_insert_at(
    MACRO_ARG_dyn_ary_typename *dyn_ary,
    MACRO_ARG_datatype *to_insert,
    size_t index
);
enum col_result
MACRO_ARG_dyn_ary_typename_push_back(
    MACRO_ARG_dyn_ary_typename *dyn_ary,
    MACRO_ARG_datatype *to_insert
);

// retrieval via borrowing
MACRO_ARG_datatype *
MACRO_ARG_dyn_ary_typename_get(
    MACRO_ARG_dyn_ary_typename *dyn_ary,
    size_t idx
);
// removal via move
enum col_result
MACRO_ARG_dyn_ary_typename_rm(
    MACRO_ARG_dyn_ary_typename *dyn_ary,
    size_t idx,
    MACRO_ARG_datatype *removed_elem // must be uninitialized or cleared
);
enum col_result
MACRO_ARG_dyn_ary_typename_pop_back(
    MACRO_ARG_dyn_ary_typename *dyn_ary,
    MACRO_ARG_datatype *removed_elem // must be uninitialized or cleared
);
// searching
enum col_result
MACRO_ARG_dyn_ary_typename_lin_search(
    MACRO_ARG_dyn_ary_typename *dyn_ary,
    MACRO_ARG_datatype *elem,
    size_t *idx_if_found
);
enum col_result
MACRO_ARG_dyn_ary_typename_bin_search(
    MACRO_ARG_dyn_ary_typename *dyn_ary,
    MACRO_ARG_datatype *elem,
    size_t *idx_if_found
);


// list-level operations
// removes unused data in the dynamic array such that capacity == length
enum col_result
MACRO_ARG_dyn_ary_typename_trim(
    MACRO_ARG_dyn_ary_typename *dyn_ary
);
// second will be cleared
enum col_result
MACRO_ARG_dyn_ary_typename_cat(
    MACRO_ARG_dyn_ary_typename *first,
    MACRO_ARG_dyn_ary_typename *second
);

enum col_result
MACRO_ARG_dyn_ary_typename_sort_cmp(
    MACRO_ARG_dyn_ary_typename *to_sort,
    col_sort_cmp_fn sort_fn
);
