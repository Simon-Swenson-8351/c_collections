#include "col_rb_tree_priv.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "col_allocator_priv.h"
#include "col_elem.h"

enum node_color
{
    NC_RED,
    NC_BLACK,

    NC__COUNT
};

enum node_dir
{
    ND_LEFT,
    ND_RIGHT,

    ND__COUNT
};

struct col_rb_tree_node
{
    struct col_rb_tree_node *parent;
    struct col_rb_tree_node *children[2];
    enum node_color color;
};

#define LEFT_CHILD(node) ((node)->children[(ptrdiff_t)ND_LEFT])
#define RIGHT_CHILD(node) ((node)->children[(ptrdiff_t)ND_RIGHT])

static void node_clear(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_rb_tree_node *to_clear);
static struct col_rb_tree_node *node_copy(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_rb_tree_node *to_copy);
static enum node_color node_color(struct col_rb_tree_node *node);
static void *node_data(struct col_rb_tree_node *node, struct col_elem_metadata *md);

static void handle_red_violation(struct col_rb_tree *tree, struct col_rb_tree_node *node);
static void handle_black_violation(struct col_rb_tree *tree, struct col_rb_tree_node *parent, enum node_dir violator_pos);

static struct col_rb_tree_node *uncle(struct col_rb_tree_node *node);

static struct col_rb_tree_node **accessor(struct col_rb_tree *tree, struct col_rb_tree_node *node);

static void rotate(struct col_rb_tree_node **accessor, enum node_dir from_dir);
// Preconditions: node is not NULL, node->right is not NULL
static void rotate_from_rr(struct col_rb_tree_node **accessor);
// Preconditions: node is not NULL, node->left is not NULL
static void rotate_from_ll(struct col_rb_tree_node **accessor);
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
static void rotate_from_lr(struct col_rb_tree_node **accessor);
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
static void rotate_from_rl(struct col_rb_tree_node **accessor);

enum col_result
col_rb_tree_init(
    struct col_rb_tree *to_init,
    struct col_allocator *allocator,
    struct col_elem_metadata *elem_metadata
)
{
    assert(to_init);
    assert(elem_metadata);
    assert(elem_metadata->cmp_fn);
    to_init->allocator = allocator;
    to_init->elem_metadata = elem_metadata;
    to_init->root = NULL;
    return COL_RESULT_SUCCESS;
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
    if((dest->root = node_copy(src->allocator, src->elem_metadata, src->root)) == (void *)1) return COL_RESULT_ALLOC_FAILED;
    return COL_RESULT_SUCCESS;
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
    size_t size_to_allocate = sizeof(struct col_rb_tree_node);
    size_t remainder = size_to_allocate & (self->elem_metadata->alignment - 1);
    if(remainder) size_to_allocate += self->elem_metadata->alignment - remainder;
    size_to_allocate += self->elem_metadata->size;
    struct col_rb_tree_node *node_to_insert = col_allocator_malloc(self->allocator, size_to_allocate);
    if(!node_to_insert) return COL_RESULT_ALLOC_FAILED;
    memset(node_to_insert->children, 0x00, sizeof(node_to_insert->children));
    node_to_insert->color = NC_RED;
    memcpy(node_data(node_to_insert, self->elem_metadata), to_insert, self->elem_metadata->size);
    struct col_rb_tree_node *parent = NULL;
    struct col_rb_tree_node **insertion_point = &(self->root);
    while(*insertion_point)
    {
        // a node already occupies the insertion point, keep descending.
        if(col_elem_cmp(self->elem_metadata, node_data(*insertion_point, self->elem_metadata), node_data(node_to_insert, self->elem_metadata)) > 0)
        {
            // current > to_insert
            parent = *insertion_point;
            insertion_point = &LEFT_CHILD(*insertion_point);
        }
        else
        {
            // current <= to_insert
            parent = *insertion_point;
            insertion_point = &RIGHT_CHILD(*insertion_point);
        }
    }
    node_to_insert->parent = parent;
    *insertion_point = node_to_insert;
    handle_red_violation(self, node_to_insert);
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
        int cmp_res = col_elem_cmp(self->elem_metadata, node_data(cur, self->elem_metadata), elem_to_search);
        if(cmp_res < 0)
        {
            cur = RIGHT_CHILD(cur);
        }
        else if(cmp_res == 0)
        {
            break;
        }
        else
        {
            cur = LEFT_CHILD(cur);
        }
    }
    if(!cur) return COL_RESULT_ELEM_NOT_FOUND;
    *found_elem = node_data(cur, self->elem_metadata);
    return COL_RESULT_SUCCESS;
}

enum col_result
col_rb_tree_remove(
    struct col_rb_tree *self,
    void *elem_to_remove,
    void *removed_elem
)
{
    enum node_dir cur_pos;
    struct col_rb_tree_node **removal_point = &(self->root);
    while(*removal_point)
    {
        int cmp_res = col_elem_cmp(self->elem_metadata, node_data(*removal_point, self->elem_metadata), elem_to_remove);
        if(cmp_res < 0)
        {
            cur_pos = ND_RIGHT;
            removal_point = &RIGHT_CHILD(*removal_point);
        }
        else if(cmp_res == 0)
        {
            break;
        }
        else
        {
            cur_pos = ND_LEFT;
            removal_point = &LEFT_CHILD(*removal_point);
        }
    }
    if(!(*removal_point)) return COL_RESULT_ELEM_NOT_FOUND;
    memcpy(removed_elem, node_data(*removal_point, self->elem_metadata), self->elem_metadata->size);
    if(LEFT_CHILD(*removal_point) && RIGHT_CHILD(*removal_point))
    {
        struct col_rb_tree_node **succ_ptr = &RIGHT_CHILD(*removal_point);
        cur_pos = ND_RIGHT;
        while(LEFT_CHILD(*succ_ptr))
        {
            succ_ptr = &LEFT_CHILD(*succ_ptr);
            cur_pos = ND_LEFT;
        }
        memcpy(node_data(*removal_point, self->elem_metadata), node_data(*succ_ptr, self->elem_metadata), self->elem_metadata->size);
        removal_point = succ_ptr;
    }
    // We want the above case where we replace with successor to fall-through so that a node is eventually removed.
    struct col_rb_tree_node *to_remove = *removal_point;
    if(LEFT_CHILD(to_remove))
    {
        *removal_point = LEFT_CHILD(*removal_point);
        LEFT_CHILD(to_remove)->parent = to_remove->parent;
        LEFT_CHILD(to_remove)->color = NC_BLACK;
    }
    else if(RIGHT_CHILD(to_remove))
    {
        *removal_point = RIGHT_CHILD(to_remove);
        RIGHT_CHILD(to_remove)->parent = to_remove->parent;
        RIGHT_CHILD(to_remove)->color = NC_BLACK;
    }
    else
    {
        *removal_point = NULL;
        if(node_color(to_remove) == NC_BLACK) handle_black_violation(self, to_remove->parent, cur_pos);
    }

    col_allocator_free(self->allocator, to_remove);
    return COL_RESULT_SUCCESS;
}

static void node_clear(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_rb_tree_node *to_clear)
{
    if(!to_clear) return;
    node_clear(allocator, md, LEFT_CHILD(to_clear));
    node_clear(allocator, md, RIGHT_CHILD(to_clear));
    col_elem_clr(md, node_data(to_clear, md));
    col_allocator_free(allocator, to_clear);
}

static struct col_rb_tree_node *node_copy(struct col_allocator *allocator, struct col_elem_metadata *md, struct col_rb_tree_node *to_copy)
{
    if(!to_copy) return NULL;
    size_t size_to_allocate = sizeof(struct col_rb_tree_node);
    size_t remainder = size_to_allocate & (md->alignment - 1);
    if(remainder) size_to_allocate += md->alignment - remainder;
    size_to_allocate += md->size;
    struct col_rb_tree_node *result = col_allocator_malloc(allocator, size_to_allocate);
    if(!result) goto fail_on_alloc;
    if(!col_elem_cp(md, result + 1, to_copy + 1)) goto fail_on_elem_cp;
    if((LEFT_CHILD(result) = node_copy(allocator, md, LEFT_CHILD(to_copy))) == (void *)1) goto fail_on_left;
    if((RIGHT_CHILD(result) = node_copy(allocator, md, RIGHT_CHILD(to_copy))) == (void *)1) goto fail_on_right;
    result->color = to_copy->color;
    return result;
fail_on_right:
    node_clear(allocator, md, LEFT_CHILD(result));
fail_on_left:
    col_elem_clr(md, result + 1);
fail_on_elem_cp:
    col_allocator_free(allocator, result);
fail_on_alloc:
    return (void *)1;
}

static enum node_color node_color(struct col_rb_tree_node *node)
{
    if(!node) return NC_BLACK;
    return node->color;
}

static void rotate(struct col_rb_tree_node **accessor, enum node_dir from_dir)
{
    assert(accessor);
    assert(*accessor);
    assert((*accessor)->children[(ptrdiff_t)from_dir]);
    struct col_rb_tree_node *old_top = *accessor;
    struct col_rb_tree_node *new_top = (*accessor)->children[(ptrdiff_t)from_dir];
    *accessor = new_top;
    new_top->parent = old_top->parent;
    old_top->children[(ptrdiff_t)from_dir] = new_top->children[1 - (ptrdiff_t)from_dir];
    if(old_top->children[(ptrdiff_t)from_dir]) old_top->children[(ptrdiff_t)from_dir]->parent = old_top;
    new_top->children[1 - (ptrdiff_t)from_dir] = old_top;
    old_top->parent = new_top;
}

static void rotate_from_rr(struct col_rb_tree_node **accessor)
{
    rotate(accessor, ND_RIGHT);
}

static void rotate_from_ll(struct col_rb_tree_node **accessor)
{
    rotate(accessor, ND_LEFT);
}

static void rotate_from_lr(struct col_rb_tree_node **accessor)
{
    rotate(&LEFT_CHILD(*accessor), ND_RIGHT);
    rotate(accessor, ND_LEFT);
}

static void rotate_from_rl(struct col_rb_tree_node **accessor)
{
    rotate(&RIGHT_CHILD(*accessor), ND_LEFT);
    rotate(accessor, ND_RIGHT);
}

static void *node_data(struct col_rb_tree_node *node, struct col_elem_metadata *md)
{
    assert(node);
    size_t offset = sizeof(struct col_rb_tree_node);
    size_t remainder = offset & (md->alignment - 1);
    if(remainder) offset += md->alignment - remainder;
    return (void *)(((uint8_t *)(node)) + offset);
}

static void handle_red_violation(struct col_rb_tree *tree, struct col_rb_tree_node *node)
{
    // node is red and node->parent is red
    while(true)
    {
        if(!(node->parent))
        {
            node->color = NC_BLACK;
            return;
        }
        else if(node->parent->color == NC_BLACK) return;
        // Note: Grandparent exists since parent is red and the root cannot be red.
        if(node_color(uncle(node)) == NC_RED)
        {
            node->parent->parent->color = NC_RED;
            LEFT_CHILD(node->parent->parent)->color = NC_BLACK;
            RIGHT_CHILD(node->parent->parent)->color = NC_BLACK;
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
            struct col_rb_tree_node **gp_accessor = accessor(tree, node->parent->parent);
            if(node == LEFT_CHILD(node->parent))
            {
                if(node->parent == LEFT_CHILD(node->parent->parent)) rotate_from_ll(gp_accessor);
                else rotate_from_rl(gp_accessor);
            }
            else // node == node->parent->right
            {
                if(node->parent == LEFT_CHILD(node->parent->parent)) rotate_from_lr(gp_accessor);
                else rotate_from_rr(gp_accessor);
            }
            node = *gp_accessor;
            node->color = NC_BLACK;
            LEFT_CHILD(node)->color = NC_RED;
            RIGHT_CHILD(node)->color = NC_RED;
            return;
        }
    }
}

static struct col_rb_tree_node *uncle(struct col_rb_tree_node *node)
{
    return (node->parent == LEFT_CHILD(node->parent->parent)) ? RIGHT_CHILD(node->parent->parent) : LEFT_CHILD(node->parent->parent);
}

static void handle_black_violation(struct col_rb_tree *tree, struct col_rb_tree_node *parent, enum node_dir violator_pos)
{
    while(parent)
    {
        struct col_rb_tree_node *sibling = (violator_pos == ND_LEFT) ? RIGHT_CHILD(parent) : LEFT_CHILD(parent);
        if(node_color(parent) == NC_RED)
        {
            if(node_color(LEFT_CHILD(sibling)) == NC_BLACK && node_color(RIGHT_CHILD(sibling)) == NC_BLACK)
            {
                parent->color = NC_BLACK;
                sibling->color = NC_RED;
                return;
            }
            else
            {
                struct col_rb_tree_node **parent_accessor = accessor(tree, parent);
                if(violator_pos == ND_LEFT)
                {
                    // sibling pos is ND_RIGHT
                    if(node_color(LEFT_CHILD(sibling)) == NC_RED) rotate_from_rl(parent_accessor);
                    else rotate_from_rr(parent_accessor);
                }
                else
                {
                    // sibling pos is ND_LEFT
                    if(node_color(LEFT_CHILD(sibling)) == NC_RED) rotate_from_ll(parent_accessor);
                    else rotate_from_lr(parent_accessor);
                }
                parent = *parent_accessor;
                parent->color = NC_RED;
                LEFT_CHILD(parent)->color = NC_BLACK;
                RIGHT_CHILD(parent)->color = NC_BLACK;
                return;
            }
        }
        else // node_color(parent) == NODE_COLOR_BLACK
        {
            if(node_color(sibling) == NC_RED)
            {
                parent->color = NC_RED;
                sibling->color = NC_BLACK;
                struct col_rb_tree_node **parent_accessor = accessor(tree, parent);
                if(violator_pos == ND_LEFT) rotate_from_rr(parent_accessor);
                else rotate_from_ll(parent_accessor);
                // This moves the redness in sibling to be the parent of the violator so that case 1 above can run next 
                // iteration and terminate.
            }
            else
            {
                if(node_color(LEFT_CHILD(sibling)) == NC_BLACK && node_color(RIGHT_CHILD(sibling)) == NC_BLACK)
                {
                    sibling->color = NC_RED;
                    if(parent->parent && parent == LEFT_CHILD(parent->parent)) violator_pos = ND_LEFT;
                    else violator_pos = ND_RIGHT;
                    parent = parent->parent;
                }
                else
                {
                    struct col_rb_tree_node **parent_accessor = accessor(tree, parent);
                    if(violator_pos == ND_LEFT)
                    {
                        // sibling pos is ND_RIGHT
                        if(node_color(LEFT_CHILD(sibling)) == NC_RED) rotate_from_rl(parent_accessor);
                        else rotate_from_rr(parent_accessor);
                    }
                    else
                    {
                        // sibling pos is ND_LEFT
                        if(node_color(LEFT_CHILD(sibling)) == NC_RED) rotate_from_ll(parent_accessor);
                        else rotate_from_lr(parent_accessor);
                    }
                    parent = *parent_accessor;
                    parent->color = NC_BLACK;
                    LEFT_CHILD(parent)->color = NC_BLACK;
                    RIGHT_CHILD(parent)->color = NC_BLACK;
                    return;
                }
            }
        }
    }
}

static struct col_rb_tree_node **accessor(struct col_rb_tree *tree, struct col_rb_tree_node *node)
{
    if(!(node->parent)) return &(tree->root);
    else if(LEFT_CHILD(node->parent) == node) return &LEFT_CHILD(node->parent);
    else return &RIGHT_CHILD(node->parent);
}
