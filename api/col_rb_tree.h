#pragma once

#include <inttypes.h>
#include <stddef.h>

#include "col_result.h"

enum col_rb_tree_node_color
{
    COL_RB_TREE_NODE_COLOR_RED,
    COL_RB_TREE_NODE_COLOR_BLACK,

    COL_RB_TREE_NODE_COLOR__COUNT
};

struct col_rb_tree_node
{
    struct col_rb_tree_node *parent;
    struct col_rb_tree_node *left;
    struct col_rb_tree_node *right;
    enum col_rb_tree_node_color color;
    void *data;
};

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

size_t
col_rb_tree_count(
    struct col_rb_tree *self
);

size_t
col_rb_tree_depth(
    struct col_rb_tree *self
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
    void *found_elem
);

enum col_result
col_rb_tree_rm(
    struct col_rb_tree *self,
    void *removed_elem
);

// first will contain all elements
// second will be cleared
enum col_result
col_rb_tree_merge(
    struct col_rb_tree *first,
    struct col_rb_tree *second
);
