#define COLN_CAT_(a, b) a ## b
#define COLN_CAT(a, b) COLN_CAT_(a, b)

#ifdef COLN_INTERNAL_DEBUG
#define COLN_INTERNAL_ASSERT(x) assert(x)
#else
#define COLN_INTERNAL_ASSERT(x)
#endif

#if !defined(COLN_HEADER) && !defined(COLN_IMPL)
#error "COLN_HEADER or COLN_IMPL must be defined"
#endif

#ifndef COLN_DATA_TYPE
#error "Collection macros require COLN_DATA_TYPE"
#endif

#ifndef COLN_DATA_COMPARE
#error "Red-black Tree requires a compare function"
#endif

#ifndef COLN_DATA_COPY
#define COLN_DATA_COPY(dest_ptr, src_ptr) (*(dest_ptr) = *(src_ptr), true)
#define COLN_DATA_COPY_MANY(dest_ptr, src_ptr, count) \
    (memcpy((dest_ptr), (src_ptr), sizeof(COLN_DATA_TYPE) * (count)), true)
#endif

#ifndef COLN_DATA_COPY_MANY
#define COLN_DATA_COPY_MANY COLN_CAT(COLN_DATA_TYPE, _colnhelper_copy_many)
#define COLN_DATA__PRIV__COPY_MANY_DECL \
    static bool COLN_DATA_COPY_MANY(COLN_DATA_TYPE *dest, \
                                    COLN_DATA_TYPE *src, \
                                    size_t count)
#define COLN_DATA__PRIV__COPY_MANY_DECLSC COLN_DATA__PRIV__COPY_MANY_DECL;
#define COLN_DATA__PRIV__COPY_MANY_DEFN \
    COLN_DATA__PRIV__COPY_MANY_DECL \
    { \
        for(ptrdiff_t i = 0; i < count; i++) \
        { \
            if(!COLN_DATA_COPY(dest + i, src + i)) \
            { \
                for(ptrdiff_t j = i - 1; j >= 0; j--) \
                { \
                    COLN_DATA_CLEAR(dest + j); \
                } \
                return false; \
            } \
        } \
        return true; \
    }
#else
#define COLN_DATA__PRIV__COPY_MANY_DECL
#define COLN_DATA__PRIV__COPY_MANY_DECLSC
#define COLN_DATA__PRIV__COPY_MANY_DEFN
#endif

#ifndef COLN_DATA_MOVE
#define COLN_DATA_MOVE(dest_ptr, src_ptr) (*(dest_ptr) = *(src_ptr))
#define COLN_DATA_MOVE_MANY(dest_ptr, src_ptr, count) \
    memcpy((dest_ptr), (src_ptr), sizeof(COLN_DATA_TYPE) * (count))
#endif

#ifndef COLN_DATA_MOVE_MANY
#define COLN_DATA_MOVE_MANY(dest_ptr, src_ptr, count) \
    do \
    { \
        for(ptrdiff_t i = 0; i < (count); i++) \
        { \
            COLN_DATA_MOVE((dest_ptr) + i, (src_ptr) + i); \
        } \
    } while(0)
#endif

#ifndef COLN_DATA_CLEAR
#define COLN_DATA_CLEAR(to_clear_ptr)
#define COLN_DATA_CLEAR_MANY(to_clear_ptr, count)
#endif

#ifndef COLN_DATA_CLEAR_MANY
#define COLN_DATA_CLEAR_MANY(to_clear, count)
    do \
    { \
        for(ptrdiff_t i = 0; i < count; i++) \
        { \
            COLN_DATA_CLEAR((to_clear) + i); \
        } \
    } while(0)
#endif

#ifndef COLN_TYPE
#define COLN_TYPE COLN_CAT(COLN_DATA_TYPE, _red_black_tree)
#endif

#ifdef COLN_ALLOC_TYPE
#define COLN_ALLOC_DECL(allocator) COLN_ALLOC_TYPE *allocator;
#define COLN_ALLOC_ARG(allocator) COLN_ALLOC_TYPE *allocator
#define COLN_ALLOC_ASSIGN(lval, rval) ((lval) = (rval))
#define COLN_ALLOC_ASSERT(expr) assert(expr)
#ifndef COLN_ALLOC
#error "Collection macros require an allocation function if an allocator type is defined."
#endif
#ifndef COLN_FREE
#define COLN_FREE(allocator, ptr_to_free)
#endif
#else
#define COLN_ALLOC_DECL(allocator)
#define COLN_ALLOC_ARG(allocator)
#define COLN_ALLOC_ASSIGN(lval, rval)
#define COLN_ALLOC_ASSERT(expr)
#define COLN_ALLOC(allocator, size_to_alloc) malloc(size_to_alloc)
#define COLN_FREE(allocator, ptr_to_free) free(ptr_to_free)
#endif

#define RED_BLACK_NODE_COLOR_DEFN \
    typedef enum NodeColor \
    { \
        NC_RED, \
        NC_BLACK, \
        \
        NC__COUNT \
    } NodeColor;

#define RED_BLACK_NODE_DIR_DEFN \
    typedef enum NodeDir \
    { \
        ND_LEFT, \
        ND_RIGHT, \
        \
        ND__COUNT \
    } NodeDir;

#define RED_BLACK_NODE COLN_CAT(COLN_TYPE, _node)
#define RED_BLACK_NODE_DECL \
    typedef struct RED_BLACK_NODE RED_BLACK_NODE
#define RED_BLACK_NODE_DEFN \
    typedef struct RED_BLACK_NODE \
    { \
        struct RED_BLACK_NODE *parent; \
        struct RED_BLACK_NODE *children[2]; \
        NodeColor color; \
        COLN_DATA_TYPE data; \
    } RED_BLACK_NODE;

#define RED_BLACK_TREE_STRUCT_DEFN \
    typedef struct COLN_TYPE \
    { \
        COLN_ALLOC_DECL(allocator) \
        RED_BLACK_NODE_TYPE *root; \
    };

#define RED_BLACK_TREE_INIT_DECL \
    void COLN_CAT(COLN_TYPE, _init)(COLN_TYPE *to_init, \
                                    COLN_ALLOC_ARG(allocator))
#define RED_BLACK_TREE_INIT_DEFN \
    RED_BLACK_TREE_INIT_DECL \
    { \
        assert(to_init); \
        COLN_ALLOC_ASSERT(allocator); \
        COLN_ALLOC_ASSIGN(to_init->allocator, allocator);
        to_init->root = NULL;
    }

#define RED_BLACK_TREE_COPY_DECL \
    ColnResult COLN_CAT(COLN_TYPE, _copy)(COLN_TYPE *dest, COLN_TYPE *src)
#define RED_BLACK_TREE_COPY_DEFN \
    RED_BLACK_TREE_COPY_DECL \
    { \
        assert(dest); \
        assert(src); \
        COLN_ALLOC_ASSIGN(dest->allocator, src->allocator); \
        NodeCopyResult cr = RED_BLACK_NODE__PRIV__COPY(src->allocator, \
                                                       src->root); \
        if(cr.type != COLN_RESULT_SUCCESS) return cr.type; \
        dest->root = cr.value; \
        return COLN_RESULT_SUCCESS; \
    }

#define RED_BLACK_TREE_CLEAR_DECL \
    void COLN_CAT(COLN_TYPE, _clear)(COLN_TYPE *to_clear)
#define RED_BLACK_TREE_CLEAR_DEFN \
    RED_BLACK_TREE_CLEAR_DECL \
    { \
        RED_BLACK_NODE__PRIV__DESTROY(to_clear->allocator, to_clear->root); \
    }

#define RED_BLACK_TREE_INSERT_DECL \
    ColnResult COLN_CAT(COLN_TYPE, _insert)(COLN_TYPE *self, \
                                            COLN_DATA_TYPE *to_insert)
#define RED_BLACK_TREE_INSERT_DEFN \
    RED_BLACK_TREE_INSERT_DECL \
    { \
        assert(self); \
        assert(to_insert); \
        RED_BLACK_NODE *node_to_insert = \
            COLN_ALLOC(self->allocator, sizeof(RED_BLACK_NODE)); \
        if(!node_to_insert) return COLN_RESULT_ALLOC_FAILED; \
        memset(node_to_insert->children, 0, sizeof(node_to_insert->children)); \
        node_to_insert->color = NC_RED; \
        COLN_DATA_MOVE(&(node_to_insert->data), to_insert); \
        RED_BLACK_NODE *parent = NULL; \
        RED_BLACK_NODE **insertion_point = &(self->root); \
        while(*insertion_point) \
        { \
            parent = *insertion_point; \
            if(COLN_ELEM_COMPARE( \
                    &((*insertion_point)->data), \
                    &(node_to_insert->data) > 0) \
                insertion_point = &LEFT_CHILD(*insertion_point); \
            else \
                insertion_point = &RIGHT_CHILD(*insertion_point); \
        } \
        node_to_insert->parent = parent; \
        *insertion_point = node_to_insert; \
        RED_BLACK_TREE__PRIV__HANDLE_RED_VIOLATION(self, node_to_insert); \
        return COLN_RESULT_SUCCESS; \
    }

#define RED_BLACK_TREE_SEARCH_DECL \
    COLN_DATA_TYPE *COLN_CAT(COLN_TYPE, _search)(COLN_TYPE *self, \
                                                 COLN_DATA_TYPE *elem_to_search)
#define RED_BLACK_TREE_SEARCH_DEFN \
    RED_BLACK_TREE_SEARCH_DECL \
    { \
        assert(self); \
        assert(elem_to_search); \
        RED_BLACK_NODE *cur = self->root; \
        while(cur) \
        { \
            int cmp_res = COLN_ELEM_COMPARE(&(cur->data), elem_to_search); \
            if(cmp_res < 0) cur = RIGHT_CHILD(cur); \
            else if(cmp_res == 0) return &(cur->data); \
            else cur = LEFT_CHILD(cur); \
        } \
        return NULL; \
    }

#define RED_BLACK_TREE_REMOVE_DECL \
    ColnResult COLN_CAT(COLN_TYPE, _remove)(COLN_TYPE *self, \
                                            COLN_DATA_TYPE *elem_to_remove, \
                                            COLN_DATA_TYPE *removed_elem)
#define RED_BLACK_TREE_REMOVE_DEFN \
    RED_BLACK_TREE_REMOVE_DECL \
    { \
        RED_BLACK_NODE_DIR cur_pos; \
        RED_BLACK_NODE **removal_point = &(self->root); \
        while(*removal_point) \
        { \
            int cmp_res = COLN_ELEM_COMPARE(&((*removal_point)->data), \
                                            elem_to_remove); \
            if(cmp_res < 0) \
            { \
                cur_pos = ND_RIGHT; \
                removal_point = &RIGHT_CHILD(*removal_point); \
            } \
            else if(cmp_res == 0) \
            { \
                break; \
            } \
            else \
            { \
                cur_pos = ND_LEFT; \
                removal_point = &LEFT_CHILD(*removal_point); \
            } \
        }
        if(!(*removal_point)) return COLN_RESULT_ELEM_NOT_FOUND; \
        COLN_ELEM_MOVE(removed_elem, &((*removal_point)->data)); \
        if(LEFT_CHILD(*removal_point) && RIGHT_CHILD(*removal_point)) \
        { \
            RED_BLACK_NODE **succ_ptr = &RIGHT_CHILD(*removal_point); \
            cur_pos = ND_RIGHT; \
            while(LEFT_CHILD(*succ_ptr)) \
            { \
                succ_ptr = &LEFT_CHILD(*succ_ptr); \
                cur_pos = ND_LEFT; \
            } \
            COLN_DATA_MOVE(&((*removal_point)->data), &((*succ_ptr)->data)); \
            removal_point = succ_ptr;
        } \
        /*  We want the above case where we replace with successor to */ \
        /* fall-through so that a node is eventually removed. */ \
        RED_BLACK_NODE *to_remove = *removal_point; \
        if(LEFT_CHILD(to_remove)) \
        { \
            *removal_point = LEFT_CHILD(*removal_point); \
            LEFT_CHILD(to_remove)->parent = to_remove->parent; \
            LEFT_CHILD(to_remove)->color = NC_BLACK; \
        } \
        else if(RIGHT_CHILD(to_remove)) \
        { \
            *removal_point = RIGHT_CHILD(to_remove); \
            RIGHT_CHILD(to_remove)->parent = to_remove->parent; \
            RIGHT_CHILD(to_remove)->color = NC_BLACK; \
        } \
        else \
        { \
            *removal_point = NULL; \
            if(RED_BLACK_NODE__PRIV__COLOR(to_remove) == NC_BLACK) \
                RED_BLACK_TRE__PRIV__HANDLE_BLACK_VIOLATION( \
                    self, \
                    to_remove->parent, \
                    cur_pos); \
        } \
        COLN_FREE(self->allocator, to_remove); \
        return COLN_RESULT_SUCCESS; \
    }

#define RED_BLACK_NODE__PRIV__DESTROY COLN_CAT(RED_BLACK_NODE, _clear)
#ifdef COLN_ALLOC_TYPE
#define RED_BLACK_NODE__PRIV__DESTROY_INVOC(to_destroy, allocator) \
    RED_BLACK_NODE__PRIV__DESTROY(to_destroy, allocator)
#else
#define RED_BLACK_NODE__PRIV__DESTROY_INVOC(to_destroy, allocator) \
    RED_BLACK_NODE__PRIV__DESTROY(to_destroy)
#endif
#define RED_BLACK_NODE__PRIV__DESTROY_DECL \
    static void RED_BLACK_NODE__PRIV__DESTROY(RED_BLACK_NODE *to_destroy, \
                                              COLN_ALLOC_ARG(allocator))
#define RED_BLACK_NODE__PRIV__DESTROY_DEFN \
    RED_BLACK_NODE__PRIV__DESTROY_DECL \
    { \
        if(!to_destroy) return; \
        RED_BLACK_NODE__PRIV__DESTROY_INVOC(LEFT_CHILD(to_destroy), \
                                            allocator); \
        RED_BLACK_NODE__PRIV__DESTROY_INVOC(RIGHT_CHILD(to_destroy), \
                                            allocator); \
        COLN_ELEM_CLEAR(&(to_destroy->data)); \
        COLN_FREE(allocator, to_destroy); \
    }

#define RED_BLACK_NODE__PRIV__COPY COLN_CAT(RED_BLACK_NODE, _copy)
#ifdef COLN_ALLOC_TYPE
#define RED_BLACK_NODE__PRIV__COPY_INVOC(new_dest, src, allocator) \
    RED_BLACK_NODE__PRIV__COPY(new_dest, src, allocator) 
#else
#define RED_BLACK_NODE__PRIV__COPY_INVOC(new_dest, src, allocator) \
    RED_BLACK_NODE__PRIV__COPY(new_dest, src) 
#endif
#define RED_BLACK_NODE__PRIV__COPY_DECL \
    static ColnResult RED_BLACK_NODE__PRIV__COPY( \
        RED_BLACK_NODE **new_dest, \
        RED_BLACK_NODE *src \
        COLN_ALLOC_ARG(allocator))
#define RED_BLACK_NODE__PRIV__COPY_DEFN \
    RED_BLACK_NODE__PRIV__COPY_DECL \
    { \
        COLN_INTERNAL_ASSERT(new_dest); \
        COLN_INTERNAL_ASSERT(src); \
        COLN_ALLOC_ASSERT(allocator); \
        if(!to_copy) \
        { \
            *new_dest = NULL; \
            return COLN_RESULT_SUCCESS; \
        } \
        ColnResult result; \
        *new_dest = COLN_ALLOC(allocator, sizeof(RED_BLACK_NODE)); \
        if(!(*new_dest)) \
        { \
            result = COLN_RESULT_ALLOC_FAILED; \
            goto fail_on_alloc; \
        } \
        if(!COLN_DATA_COPY(&((*new_dest)->data)), &(src->data)) \
        { \
            result = COLN_RESULT_COPY_ELEM_FAILED; \
            goto fail_on_elem_cp; \
        } \
        if(result = RED_BLACK_NODE__PRIV__COPY_INVOC(&LEFT_CHILD(*new_dest), \
                                                     LEFT_CHILD(src), \
                                                     allocator) \
            goto fail_on_left; \
        if(result = RED_BLACK_NODE__PRIV__COPY_INVOC(&RIGHT_CHILD(*new_dest), \
                                                     RIGHT_CHILD(src), \
                                                     allocator) \
            goto fail_on_right; \
        (*new_dest)->color = to_copy->color; \
        return COLN_RESULT_SUCCESS; \
    fail_on_right: \
        RED_BLACK_NODE__PRIV__DESTROY_INVOC(LEFT_CHILD(*new_dest), \
                                            allocator); \
    fail_on_left: \
        COLN_ELEM_CLEAR(&((*new_dest)->data)); \
    fail_on_elem_cp: \
        COLN_FREE(allocator, *new_dest); \
        *new_dest = NULL; \
    fail_on_alloc: \
        return result; \
    }

#define RED_BLACK_NODE__PRIV__COLOR COLN_CAT(RED_BLACK_NODE, _color)
#define RED_BLACK_NODE__PRIV__COLOR_DECL \
    static NodeColor RED_BLACK_NODE__PRIV__COLOR(RED_BLACK_NODE *node)
#define RED_BLACK_NODE__PRIV__COLOR_DEFN \
    RED_BLACK_NODE__PRIV__COLOR_DECL
    { \
        if(!node) return NC_BLACK; \
        return node->color; \
    }

#define RED_BLACK_NODE__PRIV__ROTATE COLN_CAT(RED_BLACK_NODE, _rotate)
#define RED_BLACK_NODE__PRIV__ROTATE_DECL \
    static void RED_BLACK_NODE__PRIV__ROTATE(RED_BLACK_NODE **accessor, \
                                             NodeDir from_dir)
#define RED_BLACK_NODE__PRIV__ROTATE_DEFN \
    RED_BLACK_NODE__PRIV__ROTATE_DECL \
    { \
        COLN_INTERNAL_ASSERT(accessor); \
        COLN_INTERNAL_ASSERT(*accessor); \
        COLN_INTERNAL_ASSERT((*accessor)->children[(ptrdiff_t)from_dir]); \
        RED_BLACK_NODE *old_top = *accessor; \
        RED_BLACK_NODE *new_top = (*accessor)->children[(ptrdiff_t)from_dir]; \
        *accessor = new_top; \
        new_top->parent = old_top->parent; \
        old_top->children[(ptrdiff_t)from_dir] = \
            new_top->children[1 - (ptrdiff_t)from_dir]; \
        if(old_top->children[(ptrdiff_t)from_dir]) \
            old_top->children[(ptrdiff_t)from_dir]->parent = old_top; \
        new_top->children[1 - (ptrdiff_t)from_dir] = old_top; \
        old_top->parent = new_top; \
    }

#define RED_BLACK_NODE__PRIV__ROTATE_FROM_RR COLN_CAT( \
    RED_BLACK_NODE, \
    _rotate_from_rr)
#define RED_BLACK_NODE__PRIV__ROTATE_FROM_RR_DECL \
    static void RED_BLACK_NODE__PRIV__ROTATE_FROM_RR(RED_BLACK_NODE **accessor)
#define RED_BLACK_NODE__PRIV__ROTATE_FROM_RR_DEFN \
    RED_BLACK_NODE__PRIV__ROTATE_FROM_RR_DECL { rotate(accessor, ND_RIGHT); }

#define RED_BLACK_NODE__PRIV__ROTATE_FROM_LL COLN_CAT( \
    RED_BLACK_NODE, \
    _rotate_from_ll)
#define RED_BLACK_NODE__PRIV__ROTATE_FROM_LL_DECL \
    static void RED_BLACK_NODE__PRIV__ROTATE_FROM_LL(RED_BLACK_NODE **accessor)
#define RED_BLACK_NODE__PRIV__ROTATE_FROM_LL_DEFN \
    RED_BLACK_NODE__PRIV__ROTATE_FROM_LL_DECL { rotate(accessor, ND_LEFT); }

#define RED_BLACK_NODE__PRIV__ROTATE_FROM_LR COLN_CAT( \
    RED_BLACK_NODE, \
    _rotate_from_lr)
#define RED_BLACK_NODE__PRIV__ROTATE_FROM_LR_DECL \
    static void RED_BLACK_NODE__PRIV__ROTATE_FROM_LR(RED_BLACK_NODE **accessor)
#define RED_BLACK_NODE__PRIV__ROTATE_FROM_LR_DEFN \
    RED_BLACK_NODE__PRIV__ROTATE_FROM_LR_DECL \
    { \
        COLN_INTERNAL_ASSERT(accessor); \
        COLN_INTERNAL_ASSERT(*accessor); \
        rotate(&LEFT_CHILD(*accessor), ND_RIGHT); \
        rotate(accessor, ND_LEFT); \
    }

#define RED_BLACK_NODE__PRIV__ROTATE_FROM_RL COLN_CAT( \
    RED_BLACK_NODE, \
    _rotate_from_rl)
#define RED_BLACK_NODE__PRIV__ROTATE_FROM_RL_DECL \
    static void RED_BLACK_NODE__PRIV__ROTATE_FROM_RL(RED_BLACK_NODE **accessor) 
#define RED_BLACK_NODE__PRIV__ROTATE_FROM_RL_DEFN \
    RED_BLACK_NODE__PRIV__ROTATE_FROM_RL_DECL \
    { \
        COLN_INTERNAL_ASSERT(accessor); \
        COLN_INTERNAL_ASSERT(*accessor); \
        rotate(&RIGHT_CHILD(*accessor), ND_LEFT); \
        rotate(accessor, ND_RIGHT); \
    }

#define RED_BLACK_TREE__PRIV__HANDLE_RED_VIOLATION COLN_CAT( \
    RED_BLACK_TREE, \
    _handle_red_violation)
#define RED_BLACK_TREE__PRIV__HANDLE_RED_VIOLATION_DECL \
    static void RED_BLACK_TREE__PRIV__HANDLE_RED_VIOLATION(COLN_TYPE *tree, \
                                                           RED_BLACK_NODE *node) 
#define RED_BLACK_TREE__PRIV__HANDLE_RED_VIOLATION_DEFN \
    RED_BLACK_TREE__PRIV__HANDLE_RED_VIOLATION_DECL \
    { \
        /* node is red and node->parent is red */ \
        while(true) \
        { \
            if(!(node->parent)) \
            { \
                node->color = NC_BLACK; \
                return; \
            } \
            else if(node->parent->color == NC_BLACK) return; \
            /* Note: Grandparent exists since parent is red and the root */ \
            /* cannot be red. */ \
            if(node_color(uncle(node)) == NC_RED) \
            { \
                node->parent->parent->color = NC_RED; \
                LEFT_CHILD(node->parent->parent)->color = NC_BLACK; \
                RIGHT_CHILD(node->parent->parent)->color = NC_BLACK; \
                node = node->parent->parent; \
            } \
            else \
            { \
                /*  gp is black */ \
                /*      parent is red */ \
                /*          cur is red */ \
                /*              c1 is black, since the red violations of */ \
                /*              the subtree would have been cleaned up in a */ \
                /*              previous iteration */ \
                /*              c2 is black, since the red violations of */ \
                /*              the subtree would have been cleaned up in a */ \
                /*              previous iteration */ \
                /*          sibling is black */ \
                /*      uncle is black */ \
                \
                /*  Note that a rotation on gp, parent, cur will result in */ \
                /*  the lower level of the tree (grandchild level) being */ \
                /*  c1, c2, sibling, and uncle, which are all black. */ \
                /*  Therefore, setting the next level up (child level) to */ \
                /*  red will not cause a violation farther down the tree. */ \
                RED_BLACK_NODE **gp_accessor = \
                    RED_BLACK_NODE__PRIV__ACCESSOR(tree, \
                                                   node->parent->parent); \
                if(node == LEFT_CHILD(node->parent)) \
                { \
                    if(node->parent == LEFT_CHILD(node->parent->parent)) \
                        RED_BLACK_NODE__PRIV__ROTATE_FROM_LL(gp_accessor); \
                    else RED_BLACK_NODE__PRIV__ROTATE_FROM_RL(gp_accessor); \
                } \
                else /* node == node->parent->right */ \
                { \
                    if(node->parent == LEFT_CHILD(node->parent->parent)) \
                        RED_BLACK_NODE__PRIV__ROTATE_FROM_LR(gp_accessor); \
                    else RED_BLACK_NODE__PRIV__ROTATE_FROM_RR(gp_accessor); \
                } \
                node = *gp_accessor; \
                node->color = NC_BLACK; \
                LEFT_CHILD(node)->color = NC_RED; \
                RIGHT_CHILD(node)->color = NC_RED; \
                return; \
            } \
        } \
    }

#define RED_BLACK_TREE__PRIV__HANDLE_BLACK_VIOLATION COLN_CAT( \
    COLN_TYPE, \
    _handle_black_violation)
#define RED_BLACK_TREE__PRIV__HANDLE_BLACK_VIOLATION_DECL \
    static void RED_BLACK_TREE__PRIV__HANDLE_BLACK_VIOLATION( \
        COLN_TYPE *tree, \
        RED_BLACK_NODE *parent, \
        NodeDir violator_pos)
#define RED_BLACK_TREE__PRIV__HANDLE_BLACK_VIOLATION_DEFN \
    RED_BLACK_TREE__PRIV__HANDLE_BLACK_VIOLATION_DECL \
    { \
        while(parent) \
        { \
            RED_BLACK_NODE *sibling = (violator_pos == ND_LEFT) ? \
                RIGHT_CHILD(parent) : \
                LEFT_CHILD(parent); \
            if(RED_BLACK_NODE__PRIV__COLOR(parent) == NC_RED) \
            { \
                if(RED_BLACK_NODE__PRIV__COLOR(LEFT_CHILD(sibling)) == \
                        NC_BLACK && \
                    RED_BLACK_NODE__PRIV__COLOR(RIGHT_CHILD(sibling)) == \
                        NC_BLACK) \
                { \
                    parent->color = NC_BLACK; \
                    sibling->color = NC_RED; \
                    return; \
                } \
                else \
                { \
                    RED_BLACK_NODE **parent_accessor = \
                        RED_BLACK_NODE__PRIV__ACCESSOR(tree, parent); \
                    if(violator_pos == ND_LEFT) \
                    { \
                        /* sibling pos is ND_RIGHT */ \
                        if(RED_BLACK_NODE__PRIV__COLOR(LEFT_CHILD(sibling)) == \
                                NC_RED) \
                            RED_BLACK_NODE__PRIV__ROTATE_FROM_RL( \
                                parent_accessor); \
                        else RED_BLACK_NODE__PRIV__ROTATE_FROM_RR( \
                            parent_accessor); \
                    } \
                    else \
                    { \
                        /* sibling pos is ND_LEFT */ \
                        if(RED_BLACK_NODE__PRIV__COLOR(LEFT_CHILD(sibling)) == \
                                NC_RED) \
                            RED_BLACK_NODE__PRIV__ROTATE_FROM_LL( \
                                parent_accessor); \
                        else RED_BLACK_NODE__PRIV__ROTATE_FROM_LR( \
                            parent_accessor); \
                    } \
                    parent = *parent_accessor; \
                    parent->color = NC_RED; \
                    LEFT_CHILD(parent)->color = NC_BLACK; \
                    RIGHT_CHILD(parent)->color = NC_BLACK; \
                    return; \
                } \
            } \
            else /* node_color(parent) == NODE_COLOR_BLACK */ \
            { \
                if(RED_BLACK_NODE__PRIV__COLOR(sibling) == NC_RED) \
                { \
                    parent->color = NC_RED; \
                    sibling->color = NC_BLACK; \
                    RED_BLACK_NODE **parent_accessor = \
                        RED_BLACK_NODE__PRIV__ACCESSOR(tree, parent); \
                    if(violator_pos == ND_LEFT) \
                        RED_BLACK_NODE__PRIV__ROTATE_FROM_RR(parent_accessor); \
                    else RED_BLACK_NODE__PRIV__ROTATE_FROM_LL(parent_accessor);\
                    /* This moves the redness in sibling to be the parent */ \
                    /* of the violator so that case 1 above can run next */ \
                    /* iteration and terminate. */ \
                } \
                else \
                { \
                    if(RED_BLACK_NODE__PRIV__COLOR(LEFT_CHILD(sibling)) == \
                            NC_BLACK && \
                        RED_BLACK_NODE__PRIV__COLOR(RIGHT_CHILD(sibling)) == \
                            NC_BLACK) \
                    { \
                        sibling->color = NC_RED; \
                        if(parent->parent && parent == \
                                LEFT_CHILD(parent->parent)) \
                            violator_pos = ND_LEFT; \
                        else violator_pos = ND_RIGHT; \
                        parent = parent->parent; \
                    } \
                    else \
                    { \
                        RED_BLACK_NODE **parent_accessor = \
                            RED_BLACK_NODE__PRIV__ACCESSOR(tree, parent); \
                        if(violator_pos == ND_LEFT) \
                        { \
                            /* sibling pos is ND_RIGHT */ \
                            if(RED_BLACK_NODE__PRIV__COLOR(LEFT_CHILD(sibling))\
                                    == NC_RED) \
                                RED_BLACK_NODE__PRIV__ROTATE_FROM_RL( \
                                    parent_accessor); \
                            else RED_BLACK_NODE__PRIV__ROTATE_FROM_RR( \
                                parent_accessor); \
                        } \
                        else \
                        { \
                            /* sibling pos is ND_LEFT */ \
                            if(RED_BLACK_NODE__PRIV__COLOR(LEFT_CHILD(sibling))\
                                    == NC_RED) \
                                RED_BLACK_NODE__PRIV__ROTATE_FROM_LL( \
                                    parent_accessor); \
                            else RED_BLACK_NODE__PRIV__ROTATE_FROM_LR( \
                                parent_accessor); \
                        } \
                        parent = *parent_accessor; \
                        parent->color = NC_BLACK; \
                        LEFT_CHILD(parent)->color = NC_BLACK; \
                        RIGHT_CHILD(parent)->color = NC_BLACK; \
                        return; \
                    } \
                } \
            } \
        } \
    }

#define RED_BLACK_NODE__PRIV__UNCLE COLN_CAT(RED_BLACK_NODE, _uncle)
#define RED_BLACK_NODE__PRIV__UNCLE_DECL \
    static RED_BLACK_NODE *RED_BLACK_NODE__PRIV__UNCLE(RED_BLACK_NODE *node)
#define RED_BLACK_NODE__PRIV__UNCLE_DEFN \
    RED_BLACK_NODE__PRIV__UNCLE_DECL \
    { \
        COLN_INTERNAL_ASSERT(node); \
        COLN_INTERNAL_ASSERT(node->parent); \
        COLN_INTERNAL_ASSERT(node->parent->parent); \
        return (node->parent == LEFT_CHILD(node->parent->parent)) ? \
            RIGHT_CHILD(node->parent->parent) : \
            LEFT_CHILD(node->parent->parent); \
    }

#define RED_BLACK_NODE__PRIV__ACCESSOR COLN_CAT(RED_BLACK_NODE, _accessor)
#define RED_BLACK_NODE__PRIV__ACCESSOR_DECL \
    static RED_BLACK_NODE **RED_BLACK_NODE__PRIV__ACCESSOR( \
        COLN_TYPE *tree, \
        RED_BLACK_NODE *node)
#define RED_BLACK_NODE__PRIV__ACCESSOR_DEFN \
    RED_BLACK_NODE__PRIV__ACCESSOR \
    { \
        if(!(node->parent)) return &(tree->root); \
        else if(LEFT_CHILD(node->parent) == node) \
            return &LEFT_CHILD(node->parent); \
        else return &RIGHT_CHILD(node->parent); \
    }

#ifdef COLN_HEADER
RED_BLACK_NODE_DECL;
RED_BLACK_TREE_STRUCT_DEFN;
RED_BLACK_TREE_INIT_DECL;
RED_BLACK_TREE_COPY_DECL;
RED_BLACK_TREE_CLEAR_DECL;
#endif

#ifdef COLN_IMPL
RED_BLACK_NODE_COLOR_DEFN
RED_BLACK_NODE_DIR_DEFN
RED_BLACK_NODE_DEFN
#define LEFT_CHILD(node) ((node)->children[(ptrdiff_t)ND_LEFT])
#define RIGHT_CHILD(node) ((node)->children[(ptrdiff_t)ND_RIGHT])
RED_BLACK_TREE_INIT_DEFN
RED_BLACK_TREE_COPY_DEFN
RED_BLACK_TREE_CLEAR_DEFN

#undef LEFT_CHILD
#undef RIGHT_CHILD
#endif

#undef RED_BLACK_TREE_CLEAR_DEFN
#undef RED_BLACK_TREE_CLEAR_DECL
#undef RED_BLACK_TREE_COPY_DEFN
#undef RED_BLACK_TREE_COPY_DECL
#undef RED_BLACK_TREE_INIT_DEFN
#undef RED_BLACK_TREE_INIT_DECL
#undef RED_BLACK_NODE_RIGHT_CHILD
#undef RED_BLACK_NODE_LEFT_CHILD
#undef RED_BLACK_TREE_STRUCT_DEFN
#undef RED_BLACK_NODE_DEFN
#undef RED_BLACK_NODE_DECL
#undef RED_BLACK_NODE_TYPE
#undef RED_BLACK_NODE_DIR_DEFN
#undef RED_BLACK_NODE_COLOR_DEFN
#undef COLN_ALLOC_ASSERT
#undef COLN_ALLOC_ASSIGN
#undef COLN_ALLOC_ARG
#undef COLN_ALLOC_DECL
#undef COLN_FREE
#undef COLN_ALLOC
#undef COLN_ALLOC_TYPE
#undef COLN_TYPE
#undef COLN_DATA_COMPARE
#undef COLN_DATA_CLEAR_MANY
#undef COLN_DATA_CLEAR
#undef COLN_DATA_MOVE_MANY
#undef COLN_DATA_MOVE
#undef COLN_DATA__PRIV__COPY_MANY_DEFN
#undef COLN_DATA__PRIV__COPY_MANY_DECLSC
#undef COLN_DATA__PRIV__COPY_MANY_DECL
#undef COLN_DATA_COPY_MANY
#undef COLN_DATA_COPY
#undef COLN_DATA_TYPE
#undef COLN_HEADER
#undef COLN_IMPL
#undef COLN_CAT
#undef COLN_CAT_
