#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#include "col_result.h"

struct col_allocator;
struct col_elem_metadata;
struct col_rb_tree_node;

struct col_rb_tree
{
    struct col_allocator *allocator;
    struct col_elem_metadata *elem_metadata;
    struct col_rb_tree_node *root;
};

enum col_result
col_rb_tree_init(
    struct col_rb_tree *to_init,
    struct col_allocator *allocator,
    struct col_elem_metadata *elem_metadata
);

enum col_result
col_rb_tree_copy(
    struct col_rb_tree *dest,
    struct col_rb_tree *src
);

void
col_rb_tree_clear(
    struct col_rb_tree *to_clear
);

enum col_result
col_rb_tree_insert(
    struct col_rb_tree *self,
    void *to_insert
);

enum col_result
col_rb_tree_search(
    struct col_rb_tree *self,
    void *elem_to_search,
    void **found_elem
);

enum col_result
col_rb_tree_remove(
    struct col_rb_tree *self,
    void *elem_to_remove,
    void *removed_elem
);

enum col_result
col_rb_tree_for_each(
    struct col_rb_tree *self,
    void *closure_context,
    bool(*closure)(void *context, void *elem)
);

// first will contain all elements
// second will be cleared
enum col_result
col_rb_tree_merge(
    struct col_rb_tree *first,
    struct col_rb_tree *second
);
