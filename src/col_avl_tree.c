#include "col_avl_tree_priv.h"

#include "col_allocator_priv.h"
#include "col_elem_priv.h"
#include "col_result_priv.h"

static void clear_node(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_avl_tree_node *to_clear);
static struct col_avl_tree_node *copy_node(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_avl_tree_node *to_copy);
static size_t node_count(struct col_avl_tree_node *node);
static size_t node_depth(struct col_avl_tree_node *node);

enum col_result col_avl_tree_init(struct col_avl_tree *to_init, struct col_allocator *allocator, struct col_elem_metadata *elem_metadata)
{
    if(!to_init) return COL_RESULT_BAD_ARG;
    if(!elem_metadata) return COL_RESULT_BAD_ARG;
    to_init->allocator = allocator;
    to_init->elem_metadata = elem_metadata;
    to_init->root = NULL;
}

enum col_result col_avl_tree_copy(struct col_avl_tree *dest, struct col_avl_tree *src)
{
    *dest = *src;
    dest->root = copy_node(src->allocator, src->elem_metadata, src->root);
}

void col_avl_tree_clear(struct col_avl_tree *to_clear)
{
    clear_node(to_clear->allocator, to_clear->elem_metadata, to_clear->root);
}

size_t col_avl_tree_count(struct col_avl_tree *self)
{
    return node_count(self->root);
}

size_t col_avl_tree_depth(struct col_avl_tree *self)
{
    return node_depth(self->root);
}

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

static void clear_node(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_avl_tree_node *to_clear)
{
    if(!to_clear) return;
    clear_node(allocator, md, to_clear->left);
    clear_node(allocator, md, to_clear->right);
    col_elem_clr(allocator, md, to_clear->data);
    col_allocator_free(allocator, to_clear);
}

static struct col_avl_tree_node *copy_node(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_avl_tree_node *to_copy)
{
    if(!to_copy) return NULL;
    struct col_avl_tree_node *result = col_allocator_malloc(allocator, sizeof(struct col_avl_tree_node) + md->elem_size);
    if(!result) return NULL;
    if(!col_elem_cp(allocator, md, result + 1, to_copy + 1))
    {
        col_allocator_free(allocator, result);
        return NULL;
    }
    result->left = copy_node(allocator, md, to_copy->left);
    result->right = copy_node(allocator, md, to_copy->right);
    return result;
}

static size_t node_count(struct col_avl_tree_node *node)
{
    if(!node) return 0;
    return node_count(node->left) + node_count(node->right) + 1;
}

static size_t node_depth(struct col_avl_tree_node *node)
{
    if(!node) return 0;
    size_t dl = node_depth(node->left);
    size_t dr = node_depth(node->right);
    if(dl > dr) return dl + 1;
    return dr + 1;
}
