#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "col_allocator.h"

typedef bool (*col_elem_cp_fn)  (struct col_allocator *allocator, void *dest, void *src);
typedef void (*col_elem_clr_fn) (struct col_allocator *allocator, void *to_clear);
typedef bool (*col_elem_eq_fn)  (void *a, void *b);
typedef int  (*col_elem_cmp_fn) (void *a, void *b);

struct col_elem_metadata
{
    // If cp_fn is defined, clr_fn should probably also be defined.
    // (Refer to C++'s "Rule of Three" for the rationale.)
    col_elem_cp_fn  cp_fn;     // optional, if not present, will use memcpy on copy
    col_elem_clr_fn clr_fn;    // optional, if not present, will do nothing on clear
    col_elem_eq_fn  eq_fn;     // optional, if not present, will use memcmp
    col_elem_cmp_fn cmp_fn;    // optional, if not present, will use memcmp
    size_t          elem_size; // required, must be > 0
};
