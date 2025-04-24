#include "col_avl_tree_priv.h"

#include <string.h>

#include "col_allocator_priv.h"
#include "col_elem_priv.h"
#include "col_result_priv.h"

static void clear_node(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_avl_tree_node *to_clear);
static struct col_avl_tree_node *copy_node(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_avl_tree_node *to_copy);
static size_t node_count(struct col_avl_tree_node *node);
static size_t node_depth(struct col_avl_tree_node *node);

enum leaning
{
    BALANCED,
    LEFT_LEANING,
    RIGHT_LEANING
};
struct insert_node_result
{
    enum col_result result;
    int depth;
    enum leaning leaning;
    struct col_avl_tree_node *subtree_root;
};
static struct insert_node_result insert_node(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_avl_tree_node *insertion_point, void *to_insert);

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

enum col_result col_avl_tree_insert(struct col_avl_tree *self, void *to_insert)
{

}

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

static struct insert_node_result insert_node(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_avl_tree_node *insertion_point, void *to_insert)
{
    struct insert_node_result result;
    if(!insertion_point)
    {
        struct col_avl_tree_node *new_node = col_allocator_malloc(allocator, sizeof(struct col_avl_tree_node) + md->elem_size);
        if(!new_node)
        {
            result.result = COL_RESULT_ALLOC_FAILED;
            return result;
        }

        new_node->data = new_node + 1;
        new_node->left = NULL;
        new_node->right = NULL;
        memcpy(new_node + 1, to_insert, md->elem_size);

        result.result = COL_RESULT_SUCCESS;
        result.depth = 1;
        result.leaning = BALANCED;
        result.subtree_root = new_node;
        return result;
    }
    //  1) insert left
    //      1.1) balanced
    //      1.2) unbalanced (depth(left) > depth(right) + 1)
    //          note precondition: depth(left) >= 2
    //          1.2.1) left->left is too big: rotate_right(self)
    //          1.2.2) left->right is too big: rotate_left(left), rotate_right(self)
    //  2) insert right
    //      2.1) balanced
    //      2.2) unbalanced (depth(right) > depth(left) + 1)
    //          note precondition: depth(right) >= 2

    struct insert_node_result subresult;
    int cmp_res = col_elem_cmp(md, insertion_point->data, to_insert);
    if(cmp_res > 0)
    {
        subresult = insert_node(allocator, md, insertion_point->left, to_insert);
        if(subresult.result)
        {
            result.result = subresult.result;
            return result;
        }
        insertion_point->left = subresult.subtree_root;
        size_t right_depth = node_depth(insertion_point->right);
        if(subresult.depth <= right_depth + 1)
        {
            // no rotation needed
            result.result = COL_RESULT_SUCCESS;
            result.depth = subresult.depth + 1;
            if(subresult.depth > right_depth)
            {
                result.leaning = LEFT_LEANING;
            }
            else if(subresult.depth == right_depth)
            {
                result.leaning = BALANCED;
            }
            else
            {
                result.leaning = RIGHT_LEANING;
            }
            result.subtree_root = insertion_point; 
        }
        else
        {
            // rotation needed, left-leaning tree
            // is it left-left leaning or left-right leaning?
        }
    }
    else
    {

    }
}
