#pragma once

#include <stddef.h>

struct col_bin_tree_node
{
    struct col_bin_tree_node *left;
    struct col_bin_tree_node *right;
    void *data;
};

struct col_bin_tree
{
    struct col_allocator *allocator;
    struct col_elem_metadata *elem_metadata;
    struct col_bin_tree_node *root;
};

struct col_bin_tree_loc
{
    size_t path;
    unsigned int path_size;
};
