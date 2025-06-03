#pragma once

#include "col_result.h"

struct col_allocator;
struct col_elem_metadata;

struct col_pair
{
    struct col_elem_metadata *md1;
    struct col_elem_metadata *md2;
};

size_t
col_pair_size(
    struct col_elem_metadata *first_md,
    struct col_elem_metadata *second_md
);

enum col_result
col_pair_init(
    struct col_pair *to_init,
    struct col_elem_metadata *first_md,
    struct col_elem_metadata *second_md,
    void *first,
    void *second
);
enum col_result col_pair_copy(struct col_pair *dest, struct col_pair *src);
void col_pair_clear(struct col_pair *to_clear);

void *col_pair_first(struct col_pair *pair);
void *col_pair_second(struct col_pair *pair);
