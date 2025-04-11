#pragma once

#include <stdbool.h>
#include <stddef.h>

struct col_elem_metadata;

typedef bool (*col_elem_cp_fn)  (struct col_elem_metadata *md, void *dest, void *src);
typedef void (*col_elem_clr_fn) (struct col_elem_metadata *md, void *to_clear);
typedef bool (*col_elem_eq_fn)  (struct col_elem_metadata *md, void *a, void *b);
typedef int  (*col_elem_cmp_fn) (struct col_elem_metadata *md, void *a, void *b);

typedef void col_elem_metadata_priv_td;
struct col_elem_metadata
{
    // If cp_fn is defined, clr_fn should probably also be defined.
    // (Refer to C++'s "Rule of Three" for the rationale.)
    col_elem_cp_fn             cp_fn;     // optional, if not present, will use memcpy on copy
    col_elem_clr_fn            clr_fn;    // optional, if not present, will do nothing on clear
    col_elem_eq_fn             eq_fn;     // optional, if not present, will compare memory contents for equality
    col_elem_cmp_fn            cmp_fn;    // optional, if not present, will compare memory contents by byte in little endian order
    size_t                     elem_size; // required, must be > 0
    col_elem_metadata_priv_td *priv;      // If your functions above need additional state, it can be stored here.
};
