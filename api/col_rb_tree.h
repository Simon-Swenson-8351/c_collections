#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#include "col_dyn_ary.h"
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

enum col_rb_tree_iter_type
{
    CRTIT_IN_ORDER,
    CRTIT_PRE_ORDER,
    CRTIT_POST_ORDER,

    CRTIT__COUNT
};

struct col_rb_tree_iter
{
    struct col_rb_tree *tree;
    enum col_rb_tree_iter_type type;
    struct col_dyn_ary node_stack;
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
col_rb_tree_for_each_in_order(
    struct col_rb_tree *self,
    void *context,
    bool (*operator)(void *context, void *element)
);

enum col_result
col_rb_tree_for_each_pre_order(
    struct col_rb_tree *self,
    void *context,
    bool (*operator)(void *context, void *element)
);

enum col_result
col_rb_tree_for_each_post_order(
    struct col_rb_tree *self,
    void *context,
    bool (*operator)(void *context, void *element)
);
