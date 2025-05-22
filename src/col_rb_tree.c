#include "col_rb_tree_priv.h"

#include <stddef.h>
#include <string.h>

#include "col_allocator_priv.h"
#include "col_elem.h"

enum node_color
{
    NODE_COLOR_RED,
    NODE_COLOR_BLACK,

    NODE_COLOR__COUNT
};

enum node_child_pos
{
    NCP_LEFT,
    NCP_RIGHT,

    NCD__COUNT
};

enum node_color_combo
{
    NCC_RED_RED,
    NCC_BLACK_RED,
    NCC_RED_BLACK,
    NCC_BLACK_BLACK,
    
    NCC__COUNT
};

struct col_rb_tree_node
{
    struct col_rb_tree_node *parent;
    struct col_rb_tree_node *children[2];
    enum node_color color;
};

#define LEFT_CHILD(node) node->children[(ptrdiff_t)NCP_LEFT]
#define RIGHT_CHILD(node) node->children[(ptrdiff_t)NCP_RIGHT]

static void node_clear(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_rb_tree_node *to_clear);
static struct col_rb_tree_node *node_copy(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_rb_tree_node *to_copy);
static enum node_color node_color(struct col_rb_tree_node *node);
static void *node_data(struct col_rb_tree_node *node);
static void handle_red_violation(struct col_rb_tree_node *node);
static struct col_rb_tree_node *uncle(struct col_rb_tree_node *node);
static enum node_color_combo childrens_colors(struct col_rb_tree_node *node);

static void handle_black_violation(struct col_rb_tree *tree, struct col_rb_tree_node *parent, enum node_child_pos violator_pos);
static void handle_black_excess(struct col_rb_tree *tree, struct col_rb_tree_node *parent, enum node_child_pos exceeder_pos);
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
    if(!node_to_insert) return COL_RESULT_ALLOC_FAILED;
    memset(node_to_insert->children, 0x00, sizeof(node_to_insert->children));
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
            insertion_point = (*insertion_point)->children + NCP_LEFT;
        }
        else
        {
            // current <= to_insert
            parent = *insertion_point;
            insertion_point = (*insertion_point)->children + NCP_RIGHT;
        }
    }
    node_to_insert->parent = parent;
    *insertion_point = node_to_insert;
    handle_red_violation(node_to_insert);
    return COL_RESULT_SUCCESS;
}

enum col_result
col_rb_tree_search(
    struct col_rb_tree *self,
    void *elem_to_search,
    void **found_elem
)
{

    struct col_rb_tree_node *cur = self->root;
    while(cur)
    {
        int cmp_res = col_elem_cmp(self->elem_metadata, node_data(cur), elem_to_search);
        if(cmp_res < 0)
        {
            cur = right_child(cur);
        }
        else if(cmp_res == 0)
        {
            break;
        }
        else
        {
            cur = left_child(cur);
        }
    }
    if(!cur) return COL_RESULT_ELEM_NOT_FOUND;
    *found_elem = node_data(cur);
    return COL_RESULT_SUCCESS;
}

enum col_result
col_rb_tree_remove(
    struct col_rb_tree *self,
    void *elem_to_remove,
    void *removed_elem
)
{
    enum node_child_pos cur_pos;
    struct col_rb_tree_node **removal_point = &(self->root);
    while(*removal_point)
    {
        int cmp_res = col_elem_cmp(self->elem_metadata, node_data(*removal_point), elem_to_remove);
        if(cmp_res < 0)
        {
            cur_pos = NCP_RIGHT;
            removal_point = (*removal_point)->children + NCP_RIGHT;
        }
        else if(cmp_res == 0)
        {
            break;
        }
        else
        {
            cur_pos = NCP_LEFT;
            removal_point = (*removal_point)->children + NCP_LEFT;
        }
    }
    if(!(*removal_point)) return COL_RESULT_ELEM_NOT_FOUND;
    memcpy(removed_elem, node_data(*removal_point), self->elem_metadata->elem_size);
    if(left_child(*removal_point) && right_child(*removal_point))
    {
        struct col_rb_tree_node **succ_ptr = (*removal_point)->children + NCP_RIGHT;
        cur_pos = NCP_RIGHT;
        while(left_child(*succ_ptr))
        {
            succ_ptr = (*succ_ptr)->children + NCP_LEFT;
            cur_pos = NCP_LEFT;
        }
        memcpy(node_data(*removal_point), node_data(*succ_ptr), self->elem_metadata->elem_size);
        removal_point = succ_ptr;
    }
    // We want the above case where we replace with successor to fall-through so that a node is eventually removed.
    struct col_rb_tree_node *to_remove = *removal_point;
    if(left_child(to_remove))
    {
        *removal_point = left_child(*removal_point);
        to_remove->children[NCP_LEFT]->parent = to_remove->parent;
        to_remove->children[NCP_LEFT]->color = NODE_COLOR_BLACK;
    }
    else if(to_remove->right)
    {
        *removal_point = to_remove->right;
        to_remove->right->parent = to_remove->parent;
        to_remove->right->color = NODE_COLOR_BLACK;
    }
    else
    {
        *removal_point = NULL;
        if(node_color(to_remove) == NODE_COLOR_BLACK)
        {
            handle_black_violation(self, to_remove->parent, to_remove);
        }
    }

    col_allocator_free(self->allocator, to_remove);
    return COL_RESULT_SUCCESS;
}

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

static void handle_red_violation(struct col_rb_tree_node *node)
{
    // node is red and node->parent is red
    while(node_color(node->parent) == NODE_COLOR_RED)
    {
        // Note: Grandparent exists since parent is red and the root cannot be red.
        if(node_color(uncle(node)) == NODE_COLOR_RED)
        {
            node->parent->parent->color = NODE_COLOR_RED;
            node->parent->parent->left->color = NODE_COLOR_BLACK;
            node->parent->parent->right->color = NODE_COLOR_BLACK;
            node = node->parent->parent;
        }
        else
        {
            /*
             *  gp is black
             *      parent is red
             *          cur is red
             *              c1 is black, since the red violations of the subtree would have been cleaned up in a previous iteration
             *              c2 is black, since the red violations of the subtree would have been cleaned up in a previous iteration
             *          sibling is black
             *      uncle is black
             * 
             *  Note that a rotation on gp, parent, cur will result in the lower level of the tree (grandchild level)
             *  being c1, c2, sibling, and uncle, which are all black. Therefore, setting the next level up (child 
             *  level) to red will not cause a violation farther down the tree.
             */
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
            return;
        }
    }
}

static struct col_rb_tree_node *uncle(struct col_rb_tree_node *node)
{
    return (node->parent == node->parent->parent->left) ? node->parent->parent->right : node->parent->parent->left;
}

static void handle_black_violation(struct col_rb_tree *tree, struct col_rb_tree_node *parent, struct col_rb_tree_node *violator)
{
    assert(tree);
    assert(violator);
    while(true)
    {
        if(!parent) return;
        struct col_rb_tree_node *sibling = (parent->left == violator) ? parent->right : parent->left;
        if(node_color(parent) == NODE_COLOR_RED)
        {
            if(node_color(sibling->left) == NODE_COLOR_RED)
            {
                if(violator == parent->left) parent = rotate_from_rl(tree, parent);
                else parent = rotate_from_l(tree, parent);
                parent->color = NODE_COLOR_RED;
                parent->left->color = NODE_COLOR_BLACK;
                parent->right->color = NODE_COLOR_BLACK;
                return;
            }
            else if(node_color(sibling->right) == NODE_COLOR_RED)
            {
                if(violator == parent->left) parent = rotate_from_r(tree, parent);
                else parent = rotate_from_lr(tree, parent);
                parent->color = NODE_COLOR_RED;
                parent->left->color = NODE_COLOR_BLACK;
                parent->right->color = NODE_COLOR_BLACK;
                return;
            }
            else
            {
                parent->color = NODE_COLOR_BLACK;
                sibling->color = NODE_COLOR_RED;
                return;
            }
        }
        else
        {
            if(node_color(sibling) == NODE_COLOR_RED)
            {
                if(violator == parent->left)
                {
                    sibling->color = NODE_COLOR_BLACK;
                    parent = rotate_from_r(tree, parent);
                    handle_black_excess(tree, parent->left, parent->left->right);
                    return;
                }
                else
                {
                    sibling->color = NODE_COLOR_BLACK;
                    parent = rotate_from_l(tree, parent);
                    handle_black_excess(tree, parent->right, parent->right->left);
                    return;
                }
            }
            else
            {
                if(node_color(sibling->left) == NODE_COLOR_RED)
                {
                    if(violator == parent->left) parent = rotate_from_rl(tree, parent);
                    else parent = rotate_from_l(tree, parent);
                    parent->color = NODE_COLOR_BLACK;
                    parent->left->color = NODE_COLOR_BLACK;
                    parent->right->color = NODE_COLOR_BLACK;
                    return;
                }
                else if(node_color(sibling->right) == NODE_COLOR_RED)
                {
                    if(violator == parent->left) parent = rotate_from_r(tree, parent);
                    else parent = rotate_from_lr(tree, parent);
                    parent->color = NODE_COLOR_BLACK;
                    parent->left->color = NODE_COLOR_BLACK;
                    parent->right->color = NODE_COLOR_BLACK;
                    return;
                }
                else
                {
                    sibling->color = NODE_COLOR_RED;
                    violator = parent;
                    parent = parent->parent;
                    continue;
                }
            }
        }
    }
}

static void handle_black_excess(struct col_rb_tree *tree, struct col_rb_tree_node *parent, enum node_child_pos exceeder_pos)
{
    assert(tree);
    assert(parent);
    enum node_color_combo gc_colors;
    switch(exceeder_pos)
    {
        case NCP_LEFT:
            gc_colors = childrens_colors(parent->left);
            switch(gc_colors)
            {
                case NCC_RED_RED:
                    // intentional fall-through
                case NCC_BLACK_RED:
                    parent = rotate_from_lr(tree, parent);
                    goto fixup_red_colors;
                    break;
                case NCC_RED_BLACK:
                    parent = rotate_from_l(tree, parent);
                    goto fixup_red_colors;
                    break;
                case NCC_BLACK_BLACK:
                    parent->left->color = NODE_COLOR_RED;
                    break;
            }
            break;
        case NCP_RIGHT:
            gc_colors = childrens_colors(parent->right);
            switch(gc_colors)
            {
                case NCC_RED_RED:
                    // intentional fall-through
                case NCC_BLACK_RED:
                    parent = rotate_from_lr(tree, parent);
                    goto fixup_red_colors;
                    break;
                case NCC_RED_BLACK:
                    parent = rotate_from_l(tree, parent);
                    goto fixup_red_colors;
                    break;
                case NCC_BLACK_BLACK:
                    parent->left->color = NODE_COLOR_RED;
                    break;
            }
    }
    return;
fixup_red_colors:
    parent->color = NODE_COLOR_RED;
    parent->left->color = NODE_COLOR_BLACK;
    parent->right->color = NODE_COLOR_BLACK;
}

static enum node_color_combo childrens_colors(struct col_rb_tree_node *node)
{
    assert(node);
    return (node_color(node->children + NCP_LEFT) << NCP_LEFT) | (node_color(node->children + NCP_RIGHT) << NCP_RIGHT);
}
