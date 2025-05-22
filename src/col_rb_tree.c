#include "col_rb_tree_priv.h"

#include <string.h>

#include "col_allocator_priv.h"
#include "col_elem.h"

enum node_color
{
    NODE_COLOR_RED,
    NODE_COLOR_BLACK,

    NODE_COLOR__COUNT
};

struct col_rb_tree_node
{
    struct col_rb_tree_node *parent;
    struct col_rb_tree_node *left;
    struct col_rb_tree_node *right;
    enum node_color color;
};

static void node_clear(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_rb_tree_node *to_clear);
static struct col_rb_tree_node *node_copy(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_rb_tree_node *to_copy);
static enum node_color node_color(struct col_rb_tree_node *node);
static void *node_data(struct col_rb_tree_node *node);
static void fixup_after_insert(struct col_rb_tree_node *node);
static struct col_rb_tree_node *uncle(struct col_rb_tree_node *node);
static struct col_rb_tree_node *search_internal(struct col_rb_tree *self, void *elem_to_search);
static struct col_rb_tree_node *node_succ(struct col_rb_tree_node *node);

static void fixup_black_leaf_removal(struct col_rb_tree_node *node);
// Preconditions: node is not NULL, node->right is not NULL
static struct col_rb_tree_node *rotate_from_r(struct col_rb_tree *tree, struct col_rb_tree_node *node);
// Preconditions: node is not NULL, node->left is not NULL
static struct col_rb_tree_node *rotate_from_l(struct col_rb_tree *tree, struct col_rb_tree_node *node);
/*
 *  Preconditions: node, node->left, and node->left->right are not NULL
 *  Pattern 2:
 *    C <- node
 *   /
 *  A
 *   \
 *    B
 *  To:
 *    B <- return value
 *   / \
 *  A   C
 */
static struct col_rb_tree_node *rotate_from_lr(struct col_rb_tree *tree, struct col_rb_tree_node *node);
/*
 *  Preconditions: node, node->right, and node->right->left are not NULL
 *  From:
 *  A <- node
 *   \
 *    C
 *   /
 *  B
 *  To:
 *    B <- return value
 *   / \
 *  A   C
 */
static struct col_rb_tree_node *rotate_from_rl(struct col_rb_tree *tree, struct col_rb_tree_node *node);

enum col_result
col_rb_tree_init(
    struct col_rb_tree *to_init,
    struct col_allocator *allocator,
    struct col_elem_metadata *elem_metadata
)
{
    assert(to_init);
    assert(elem_metadata);
    to_init->allocator = allocator;
    to_init->elem_metadata = elem_metadata;
    to_init->root = NULL;
}

enum col_result
col_rb_tree_copy(
    struct col_rb_tree *dest,
    struct col_rb_tree *src
)
{
    assert(dest);
    assert(src);
    dest->allocator = src->allocator;
    dest->elem_metadata = src->elem_metadata;
    dest->root = node_copy(src->allocator, src->elem_metadata, src->root);
}

void
col_rb_tree_clear(
    struct col_rb_tree *to_clear
)
{
    node_clear(to_clear->allocator, to_clear->elem_metadata, to_clear->root);
}

enum col_result
col_rb_tree_insert(
    struct col_rb_tree *self,
    void *to_insert
)
{
    assert(self);
    assert(to_insert);
    struct col_rb_tree_node *node_to_insert = col_allocator_malloc(self->allocator, sizeof(struct col_rb_tree_node) + self->elem_metadata->elem_size);
    assert(node_to_insert);
    node_to_insert->left = NULL;
    node_to_insert->right = NULL;
    node_to_insert->color = NODE_COLOR_RED;
    memcpy(node_data(node_to_insert), to_insert, self->elem_metadata->elem_size);
    struct col_rb_tree_node *parent = NULL;
    struct col_rb_tree_node **insertion_point = &(self->root);
    while(*insertion_point)
    {
        // a node already occupies the insertion point, keep descending.
        if(col_elem_cmp(self->elem_metadata, node_data(*insertion_point), node_data(node_to_insert)) > 0)
        {
            // current > to_insert
            parent = *insertion_point;
            insertion_point = &((*insertion_point)->left);
        }
        else
        {
            // current <= to_insert
            parent = *insertion_point;
            insertion_point = &((*insertion_point)->right);
        }
    }
    node_to_insert->parent = parent;
    *insertion_point = node_to_insert;
    fixup_after_insert(node_to_insert);
    return COL_RESULT_SUCCESS;
}

enum col_result
col_rb_tree_search(
    struct col_rb_tree *self,
    void *elem_to_search,
    void **found_elem
)
{
    *found_elem = search_internal(self, elem_to_search);
    if(*found_elem) return COL_RESULT_SUCCESS;
    else return COL_RESULT_ELEM_NOT_FOUND;
}

enum col_result
col_rb_tree_remove(
    struct col_rb_tree *self,
    void *elem_to_remove,
    void *removed_elem
)
{
    struct col_rb_tree_node *to_remove = search_internal(self, elem_to_remove);
    if(!to_remove) return COL_RESULT_ELEM_NOT_FOUND;
    memcpy(removed_elem, node_data(to_remove), self->elem_metadata->elem_size);
    if(to_remove->left && to_remove->right)
    {
        struct col_rb_tree_node *succ = node_succ(to_remove);
        memcpy(node_data(to_remove), node_data(succ), self->elem_metadata->elem_size);
        to_remove = succ;
    }
    // We want the above case where we replace with successor to fall-through so a node is eventually removed.
    if(to_remove->left)
    {
        to_remove->left->parent = to_remove->parent;
        if(to_remove->parent)
        {
            if(to_remove->parent->left == to_remove) to_remove->parent->left = to_remove->left;
            else to_remove->parent->right = to_remove->left;
        }
        to_remove->left->color = NODE_COLOR_BLACK;
    }
    else if(to_remove->right)
    {
        to_remove->right->parent = to_remove->parent;
        if(to_remove->parent)
        {
            if(to_remove->parent->left == to_remove) to_remove->parent->left = to_remove->right;
            else to_remove->parent->right = to_remove->right;
        }
        to_remove->right->color = NODE_COLOR_BLACK;
    }
    else
    {
        if(to_remove->parent)
        {
            if(to_remove->parent->left == to_remove) to_remove->parent->left = NULL;
            else to_remove->parent->right = NULL;
        }
        if(node_color(to_remove) == NODE_COLOR_BLACK) fixup_black_leaf_removal(to_remove);
    }

    col_allocator_free(self->allocator, to_remove);
    return COL_RESULT_SUCCESS;
}

// first will contain all elements
// second will be cleared
enum col_result
col_rb_tree_merge(
    struct col_rb_tree *first,
    struct col_rb_tree *second
);

enum col_result
col_rb_tree_for_each(
    struct col_rb_tree *self,
    void *closure_context,
    bool(*closure)(void *context, void *elem)
);

static void node_clear(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_rb_tree_node *to_clear)
{
    if(!to_clear) return;
    node_clear(allocator, md, to_clear->left);
    node_clear(allocator, md, to_clear->right);
    col_elem_clr(allocator, md, node_data(to_clear));
    col_allocator_free(allocator, to_clear);
}

static struct col_rb_tree_node *node_copy(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_rb_tree_node *to_copy)
{
    if(!to_copy) return NULL;
    struct col_rb_tree_node *result = col_allocator_malloc(allocator, sizeof(struct col_rb_tree_node) + md->elem_size);
    if(!result) return NULL;
    if(!col_elem_cp(allocator, md, result + 1, to_copy + 1))
    {
        col_allocator_free(allocator, result);
        return NULL;
    }
    result->left = node_copy(allocator, md, to_copy->left);
    result->right = node_copy(allocator, md, to_copy->right);
    result->color = to_copy->color;
    return result;
}

static enum node_color node_color(struct col_rb_tree_node *node)
{
    if(!node) return NODE_COLOR_BLACK;
    return node->color;
}

static struct col_rb_tree_node *rotate_from_r(struct col_rb_tree *tree, struct col_rb_tree_node *node)
{
    assert(tree);
    assert(node);
    assert(node->right);
    if(node->parent)
    {
        if(node->parent->left == node) node->parent->left = node->right;
        else node->parent->right = node->right;
    }
    else
    {
        tree->root = node->right;
    }
    node->right->parent = node->parent;
    struct col_rb_tree_node *new_top = node->right;
    node->right = new_top->left;
    if(node->right) node->right->parent = node;
    new_top->left = node;
    node->parent = new_top;
    return new_top;
}

static struct col_rb_tree_node *rotate_from_l(struct col_rb_tree *tree, struct col_rb_tree_node *node)
{
    assert(tree);
    assert(node);
    assert(node->left);
    if(node->parent)
    {
        if(node->parent->left == node) node->parent->left = node->right;
        else node->parent->right = node->right;
    }
    else
    {
        tree->root = node->right;
    }
    node->left->parent = node->parent;
    struct col_rb_tree_node *new_top = node->left;
    node->left = new_top->right;
    if(node->left) node->left->parent = node;
    new_top->right = node;
    node->parent = new_top;
    return new_top;
}

static struct col_rb_tree_node *rotate_from_lr(struct col_rb_tree *tree, struct col_rb_tree_node *node)
{
    assert(node);
    rotate_from_r(tree, node->left);
    return rotate_from_l(tree, node);
}

static struct col_rb_tree_node *rotate_from_rl(struct col_rb_tree *tree, struct col_rb_tree_node *node)
{
    assert(node);
    rotate_from_l(tree, node->right);
    return rotate_from_r(tree, node);
}

static void *node_data(struct col_rb_tree_node *node)
{
    assert(node);
    return (void *)(node + 1);
}

static void fixup_after_insert(struct col_rb_tree_node *node)
{
    while(true)
    {
        if(!node->parent)
        {
            node->color = NODE_COLOR_BLACK;
            return;
        }
        if(node_color(node) == NODE_COLOR_BLACK || node_color(node->parent) == NODE_COLOR_BLACK) return;
        if(node_color(uncle(node)) == NODE_COLOR_RED)
        {
            node->parent->parent->color = NODE_COLOR_RED;
            node->parent->parent->left->color = NODE_COLOR_BLACK;
            node->parent->parent->right->color = NODE_COLOR_BLACK;
            node = node->parent->parent;
        }
        else
        {
            // gp is black
            // parent is red
            // cur is red
            if(node == node->parent->left)
            {
                if(node->parent == node->parent->parent->left)
                {
                    node = node_rot_ll(node->parent->parent);
                }
                else
                {
                    node = node_rot_rl(node->parent->parent);
                }
            }
            else // node == node->parent->right
            {
                if(node->parent == node->parent->parent->left)
                {
                    node = node_rot_lr(node->parent->parent);
                }
                else
                {
                    node = node_rot_rr(node->parent->parent);
                }
            }
            node->color = NODE_COLOR_BLACK;
            node->left->color = NODE_COLOR_RED;
            node->right->color = NODE_COLOR_RED;
        }
    }
}

static struct col_rb_tree_node *uncle(struct col_rb_tree_node *node)
{
    return (node->parent == node->parent->parent->left) ? node->parent->parent->right : node->parent->parent->left;
}

static struct col_rb_tree_node *search_internal(struct col_rb_tree *self, void *elem_to_search)
{
    struct col_rb_tree_node *cur = self->root;
    while(cur)
    {
        int cmp_res = col_elem_cmp(self->elem_metadata, node_data(cur), elem_to_search);
        if(cmp_res < 0)
        {
            cur = cur->right;
        }
        else if(cmp_res == 0)
        {
            return cur;
        }
        else
        {
            cur = cur->left;
        }
    }
    return NULL;
}

static struct col_rb_tree_node *node_succ(struct col_rb_tree_node *node)
{
    node = node->right;
    while(node->left) node = node->left;
    return node;
}

static void fixup_black_leaf_removal(struct col_rb_tree_node *node)
{

}
