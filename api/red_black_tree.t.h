#define COLN_CAT_(a, b) a ## b
#define COLN_CAT(a, b) COLN_CAT_(a, b)

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
    };

#define RED_BLACK_NODE_TYPE COLN_CAT(COLN_DATA_TYPE, _red_black_node)
#define RED_BLACK_NODE_DECL \
    typedef struct RED_BLACK_NODE_TYPE RED_BLACK_NODE_TYPE
#define RED_BLACK_NODE_DEFN \
    typedef struct RED_BLACK_NODE_TYPE \
    { \
        struct RED_BLACK_NODE_TYPE *parent;
        struct RED_BLACK_NODE_TYPE *children[2];
        NodeColor color;
        COLN_DATA_TYPE data;
    } RED_BLACK_NODE_TYPE;

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
        NodeCopyResult cr = RED_BLACK_NODE_COPY_CALL(src->allocator, \
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
        RED_BLACK_NODE_DESTROY_CALL(to_clear->allocator, to_clear->root); \
    }

#define RED_BLACK_TREE_INSERT_DECL \
    ColnResult COLN_CAT(COLN_TYPE, _insert)(COLN_TYPE *self, \
                                            COLN_DATA_TYPE *to_insert)
#define RED_BLACK_TREE_INSERT_DEFN \
    RED_BLACK_TREE_INSERT_DECL \
    { \
        assert(self); \
        assert(to_insert); \
        RED_BLACK_NODE_TYPE *node_to_insert = \
            COLN_ALLOC(self->allocator, sizeof(RED_BLACK_NODE_TYPE)); \
        if(!node_to_insert) return COLN_RESULT_ALLOC_FAILED; \
        memset(node_to_insert->children, 0, sizeof(node_to_insert->children)); \
        node_to_insert->color = NC_RED; \
        COLN_DATA_MOVE(&(node_to_insert->data), to_insert); \
        RED_BLACK_NODE_TYPE *parent = NULL; \
        RED_BLACK_NODE_TYPE **insertion_point = &(self->root); \
        while(*insertion_point) \
        { \
            parent = *insertion_point; \
            if(COLN_ELEM_COMPARE( \
                (*insertion_point)->data, \
                node_to_insert->data) > 0) \
            { \
                insertion_point = &LEFT_CHILD(*insertion_point); \
            } \
            else \
            { \
                insertion_point = &RIGHT_CHILD(*insertion_point); \
            } \
        } \
        node_to_insert->parent = parent; \
        *insertion_point = node_to_insert; \
        handle_red_violation(self, node_to_insert); \
        return COLN_RESULT_SUCCESS; \
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
