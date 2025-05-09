#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "col_result.h"

struct col_avl_tree_node
{
    struct col_avl_tree_node *left;
    struct col_avl_tree_node *right;
    uint8_t *data;
};

struct col_avl_tree
{
    struct col_allocator *allocator;
    struct col_elem_metadata *elem_metadata;
    struct col_avl_tree_node *root;
};

enum col_result
col_avl_tree_init(
    struct col_avl_tree *to_init,
    struct col_allocator *allocator,
    struct col_elem_metadata *elem_metadata
);

enum col_result
col_avl_tree_copy(
    struct col_avl_tree *dest,
    struct col_avl_tree *src
);

void
col_avl_tree_clear(
    struct col_avl_tree *to_clear
);

size_t
col_avl_tree_count(
    struct col_avl_tree *self
);

size_t
col_avl_tree_depth(
    struct col_avl_tree *self
);

enum col_result
col_avl_tree_insert(
    struct col_avl_tree *self,
    void *to_insert
);

enum col_result
col_avl_tree_search(
    struct col_avl_tree *self,
    void *elem_to_search,
    void *found_elem
);

enum col_result
col_avl_tree_rm(
    struct col_avl_tree *self,
    void *removed_elem
);

// first will contain all elements
// second will be cleared
enum col_result
col_avl_tree_merge(
    struct col_avl_tree *first,
    struct col_avl_tree *second
);
