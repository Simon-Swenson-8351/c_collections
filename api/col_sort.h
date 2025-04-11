#pragma once

#include <stddef.h>

#include "col_result.h"

struct col_allocator;
struct col_elem_metadata;

typedef enum col_result (*col_sort_fn)(struct col_allocator *allocator, struct col_elem_metadata *md, void *array_base, size_t count);

enum col_result col_sort_quick(struct col_allocator *allocator, struct col_elem_metadata *md, void *array_base, size_t count);
