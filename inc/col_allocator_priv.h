#pragma once

#include "col_allocator.h"

#include <stddef.h>

void *col_allocator_malloc(struct col_allocator *self, size_t size);
void col_allocator_free(struct col_allocator *self, void *to_free);