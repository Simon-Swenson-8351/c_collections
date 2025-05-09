#include "col_rb_tree_priv.h"

#include <string.h>

#include "col_allocator_priv.h"
#include "col_elem.h"

static void node_clear(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_rb_tree_node *to_clear);
static struct col_avl_tree_node *node_copy(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_rb_tree_node *to_copy);
static size_t node_count(struct col_rb_tree_node *node);
static size_t node_depth(struct col_rb_tree_node *node);
static void node_insert(struct col_rb_tree_node *dest_tree, struct col_rb_tree_node *to_insert);
static enum col_rb_tree_node_color node_color(struct col_rb_tree_node *node);

/*
 *  This function is assumed to be called after a tree operation that causes a violation between grandparent->left 
 *  (parent) and grandparent->left->left (self). The rotation will go from:
 *        B
 *       / \
 *      R   B
 *     / \
 *    R   B
 *   / \
 *  B   B
 *  to:
 *        B
 *      /   \
 *    R       R
 *   / \     / \
 *  B   B   B   B
 */
static struct col_rb_tree_node *node_rot_ll(struct col_rb_tree_node *grandparent);

/*
 *  This function is assumed to be called after a tree operation that causes a violation between grandparent->left 
 *  (parent) and grandparent->left->right (self). The rotation will go from:
 *        B
 *       / \
 *      R   B
 *     / \
 *    B   R
 *       / \
 *      B   B
 *  to:
 *        B
 *      /   \
 *    R       R
 *   / \     / \
 *  B   B   B   B
 */
static struct col_rb_tree_node *node_rot_lr(struct col_rb_tree_node *grandparent);

/*
 *  This function is assumed to be called after a tree operation that causes a violation between grandparent->right 
 *  (parent) and grandparent->right->left (self). The rotation will go from:
 *        B
 *       / \
 *      B   R
 *         / \
 *        R   B
 *       / \
 *      B   B
 *  to:
 *        B
 *      /   \
 *    R       R
 *   / \     / \
 *  B   B   B   B
 */
static struct col_rb_tree_node *node_rot_rl(struct col_rb_tree_node *grandparent);

/*
 *  This function is assumed to be called after a tree operation that causes a violation between grandparent->right 
 *  (parent) and grandparent->right->right (self). The rotation will go from:
 *        B
 *       / \
 *      B   R
 *         / \
 *        B   R
 *           / \
 *          B   B
 *  to:
 *        B
 *      /   \
 *    R       R
 *   / \     / \
 *  B   B   B   B
 */
static struct col_rb_tree_node *node_rot_rr(struct col_rb_tree_node *grandparent);

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

size_t
col_rb_tree_count(
    struct col_rb_tree *self
)
{
    return node_count(self->root);
}

size_t
col_rb_tree_depth(
    struct col_rb_tree *self
)
{
    return node_depth(self->root);
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
    node_to_insert->color = COL_RB_TREE_NODE_COLOR_RED;
    node_to_insert->data = node_to_insert + 1;
    memcpy(node_to_insert + 1, to_insert, self->elem_metadata->elem_size);
    self->root = node_insert(self->root, node_to_insert);
}

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


static void node_clear(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_rb_tree_node *to_clear)
{
    if(!to_clear) return;
    node_clear(allocator, md, to_clear->left);
    node_clear(allocator, md, to_clear->right);
    col_elem_clr(allocator, md, to_clear->data);
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

static size_t node_count(struct col_rb_tree_node *node)
{
    if(!node) return 0;
    return node_count(node->left) + node_count(node->right) + 1;
}

static size_t node_depth(struct col_rb_tree_node *node)
{
    if(!node) return 0;
    size_t dl = node_depth(node->left);
    size_t dr = node_depth(node->right);
    if(dl > dr) return dl + 1;
    else return dr + 1;
}

static void node_insert(struct col_rb_tree_node *dest_tree, struct col_rb_tree_node *to_insert)
{

}

static enum col_rb_tree_node_color node_color(struct col_rb_tree_node *node)
{
    if(!node) return COL_RB_TREE_NODE_COLOR_BLACK;
    return node->color;
}

static struct col_rb_tree_node *node_rot_ll(struct col_rb_tree_node *grandparent)
{
    assert(grandparent);
    assert(grandparent->left);
    assert(grandparent->left->left);
    assert(node_color(grandparent) == COL_RB_TREE_NODE_COLOR_BLACK);
    assert(node_color(grandparent->left) == COL_RB_TREE_NODE_COLOR_RED);
    assert(node_color(grandparent->left->left) == COL_RB_TREE_NODE_COLOR_RED);
    assert(node_color(grandparent->right) == COL_RB_TREE_NODE_COLOR_BLACK);
    assert(node_color(grandparent->left->right) == COL_RB_TREE_NODE_COLOR_BLACK);
    assert(node_color(grandparent->left->left->left) == COL_RB_TREE_NODE_COLOR_BLACK);
    assert(node_color(grandparent->left->left->right) == COL_RB_TREE_NODE_COLOR_BLACK);
    struct col_rb_tree_node *grgrandparent = grandparent->parent;
    struct col_rb_tree_node *new_grandparent = grandparent->left;
    struct col_rb_tree_node *new_l = grandparent->left->left;
    struct col_rb_tree_node *new_r = grandparent;
    struct col_rb_tree_node *new_ll = grandparent->left->left->left;
    struct col_rb_tree_node *new_lr = grandparent->left->left->right;
    struct col_rb_tree_node *new_rl = grandparent->left->right;
    struct col_rb_tree_node *new_rr = grandparent->right;
    new_grandparent->parent = grgrandparent;
    new_grandparent->left = new_l;
    new_grandparent->right = new_r;
    new_grandparent->color = COL_RB_TREE_NODE_COLOR_BLACK;
    new_l->parent = new_grandparent;
    new_l->left = new_ll;
    new_l->right = new_lr;
    new_r->parent = new_grandparent;
    new_r->left = new_rl;
    new_r->right = new_rr;
    new_r->color = COL_RB_TREE_NODE_COLOR_RED;
    if(new_rl) new_rl->parent = new_r;
    return new_grandparent;
}
