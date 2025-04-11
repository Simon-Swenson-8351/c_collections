#pragma once

#include "col_elem.h"

bool
col_elem_cp(
    struct col_elem_metadata *md,
    void *dest,
    void *src
);
bool
col_elem_cp_many(
    struct col_elem_metadata *md,
    void *dest,
    void *src,
    size_t count
);
void
col_elem_clr(
    struct col_elem_metadata *md,
    void *to_clear
);
void
col_elem_clr_many(
    struct col_elem_metadata *md,
    void *to_clear,
    size_t count
);
bool
col_elem_eq(
    struct col_elem_metadata *md,
    void *a,
    void *b
);
int
col_elem_cmp(
    struct col_elem_metadata *md,
    void *a,
    void *b
);
