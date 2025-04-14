#pragma once

#include <stddef.h>

#include "col_result.h"

struct col_allocator;

typedef enum col_result (*col_sort_cmp_fn)(struct col_allocator *allocator, struct col_elem_metadata *md, void *array_base, size_t count);

enum col_result col_sort_cmp_quick(struct col_allocator *allocator, struct col_elem_metadata *md, void *array_base, size_t count);
enum col_result col_sort_cmp_std_qsort(struct col_allocator *allocator, struct col_elem_metadata *md, void *array_base, size_t count);
