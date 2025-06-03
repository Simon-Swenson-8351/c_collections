#pragma once

#include "col_elem.h"
#include "col_rb_tree.h"
#include "col_result.h"

typedef void col_map_entry;

struct col_allocator;
struct col_elem_metadata;

struct col_map
{
    struct col_rb_tree rbt;
    struct col_elem_metadata entry_md;
    struct col_elem_metadata *key_md;
    struct col_elem_metadata *value_md;
};

enum col_result
col_map_init(
    struct col_map *to_init,
    struct col_allocator *allocator,
    struct col_elem_metadata *key_md,
    struct col_elem_metadata *value_md
);

void col_map_clear(struct col_map *to_clear);

void *map_entry_key(col_map_entry *e);
void *map_entry_value(col_map_entry *e);
