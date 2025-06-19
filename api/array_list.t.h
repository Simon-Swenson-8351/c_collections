#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "coln_result.h"

#ifdef COLN_INTERNAL_DEBUG
#define COLN_INTERNAL_ASSERT(x) assert(x)
#else
#define COLN_INTERNAL_ASSERT(x)
#endif

#define COLN_CAT_(a, b) a ## b
#define COLN_CAT(a, b) COLN_CAT_(a, b)

#if !defined(COLN_HEADER) && !defined(COLN_IMPL)
#error "COLN_HEADER or COLN_IMPL must be defined"
#endif

#ifndef COLN_DATA_TYPE
#error "Collection macros require COLN_DATA_TYPE"
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
// We need the DECLSC below so that if COLN_DATA_COPY_MANY was already defined, 
// we don't have a stray semicolon at the top level of the file
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
#define COLN_TYPE COLN_CAT(COLN_DATA_TYPE, _array_list)
#endif

#ifdef COLN_ALLOC_TYPE
#define COLN_ALLOC_DECL(allocator) COLN_ALLOC_TYPE *allocator;
#define COLN_ALLOC_ARG(allocator) COLN_ALLOC_TYPE *allocator,
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

#define ARRAY_LIST_STRUCT_DEFN \
    typedef struct COLN_TYPE \
    { \
        COLN_ALLOC_DECL(allocator) \
        COLN_DATA_TYPE *data; \
        size_t len; \
        size_t cap; \
    } COLN_TYPE;

#define ARRAY_LIST_INIT_DECL \
    coln_result COLN_CAT(COLN_TYPE, _init)(COLN_TYPE *to_init, \
                                           COLN_ALLOC_ARG(allocator) \
                                           size_t initial_cap)
#define ARRAY_LIST_INIT_DEFN \
    ARRAY_LIST_INIT_DECL \
    { \
        assert(to_init); \
        COLN_ALLOC_ASSERT(allocator); \
        COLN_ALLOC_ASSIGN(to_init->allocator, allocator); \
        to_init->cap = initial_cap; \
        to_init->data = COLN_ALLOC( \
            allocator, \
            sizeof(COLN_DATA_TYPE) * to_init->cap); \
        if(!to_init->data) return COLN_RESULT_ALLOC_FAILED; \
        to_init->len = 0; \
        return COLN_RESULT_SUCCESS; \
    }

#define ARRAY_LIST_COPY_DECL \
    coln_result COLN_CAT(COLN_TYPE, _copy)(COLN_TYPE *dest, COLN_TYPE *src)
#define ARRAY_LIST_COPY_DEFN \
    ARRAY_LIST_COPY_DECL \
    { \
        assert(dest); \
        assert(src); \
        dest->data = COLN_ALLOC(src->allocator, \
                                sizeof(COLN_DATA_TYPE) * src->cap); \
        if(!dest->data) return COLN_RESULT_ALLOC_FAILED; \
        if(!COLN_DATA_COPY_MANY(dest->data, src->data, src->len)) \
        { \
            COLN_FREE(src->allocator, dest->data); \
            return COLN_RESULT_COPY_ELEM_FAILED; \
        } \
        COLN_ALLOC_ASSIGN(dest->allocator, src->allocator); \
        dest->len = src->len; \
        dest->cap = src->cap; \
        return COLN_RESULT_SUCCESS; \
    }

#define ARRAY_LIST_CLEAR_DECL \
    void COLN_CAT(COLN_TYPE, _clear)(COLN_TYPE *to_clear)
#define ARRAY_LIST_CLEAR_DEFN \
    ARRAY_LIST_CLEAR_DECL \
    { \
        COLN_DATA_CLEAR_MANY(to_clear->data, to_clear->len); \
        COLN_FREE(to_clear->allocator, to_clear->data); \
    }

#define ARRAY_LIST_PUSH_BACK_DECL \
    coln_result COLN_CAT(COLN_TYPE, _push_back)(COLN_TYPE *self, \
                                       COLN_DATA_TYPE *to_insert)
#define ARRAY_LIST_PUSH_BACK_DEFN \
    ARRAY_LIST_PUSH_BACK_DECL \
    { \
        assert(self); \
        assert(to_insert); \
        coln_result result; \
        if(self->len == self->cap && (result = ARRAY_LIST__PRIV__EXPAND(self))) return result; \
        COLN_DATA_MOVE(self->data + self->len, to_insert); \
        self->len++; \
        return COLN_RESULT_SUCCESS; \
    }

#define ARRAY_LIST_INSERT_AT_DECL \
    coln_result COLN_CAT(COLN_TYPE, _insert_at)(COLN_TYPE *self, \
                                       COLN_DATA_TYPE *to_insert, \
                                       ptrdiff_t index)
#define ARRAY_LIST_INSERT_AT_DEFN \
    ARRAY_LIST_INSERT_AT_DECL \
    { \
        assert(self); \
        assert(to_insert); \
        assert(index <= (ptrdiff_t)self->len); \
        coln_result result; \
        if(self->len == self->cap && (result = ARRAY_LIST__PRIV__EXPAND(self))) return result; \
        for(ptrdiff_t i = self->len; i > index; i--) \
            COLN_DATA_MOVE(self->data + i, self->data + (i - 1)); \
        COLN_DATA_MOVE(self->data + index, to_insert); \
        self->len++; \
        return COLN_RESULT_SUCCESS; \
    }

#define ARRAY_LIST_POP_BACK_DECL \
    void COLN_CAT(COLN_TYPE, _pop_back)(COLN_TYPE *self, \
                                COLN_DATA_TYPE *popped)
#define ARRAY_LIST_POP_BACK_DEFN \
    ARRAY_LIST_POP_BACK_DECL \
    { \
        assert(self); \
        assert(popped); \
        assert(self->len > 0); \
        COLN_DATA_MOVE(popped, self->data + self->len - 1); \
        self->len--; \
    }

#define ARRAY_LIST_REMOVE_AT_DECL \
    void COLN_CAT(COLN_TYPE, _remove_at)(COLN_TYPE *self, \
                                 COLN_DATA_TYPE *removed, \
                                 ptrdiff_t index)
#define ARRAY_LIST_REMOVE_AT_DEFN \
    ARRAY_LIST_REMOVE_AT_DECL \
    { \
        assert(self); \
        assert(removed); \
        assert(index < (ptrdiff_t)self->len); \
        COLN_DATA_MOVE(removed, self->data + index); \
        for(ptrdiff_t i = index; i + 1 < (ptrdiff_t)self->len; i++) \
            COLN_DATA_MOVE(self->data + i, self->data + i + 1); \
        self->len--; \
    }

#define ARRAY_LIST_CONCATENATE_DECL \
    coln_result COLN_CAT(COLN_TYPE, _concatenate)(COLN_TYPE *first, \
                                                  COLN_TYPE *second)
#define ARRAY_LIST_CONCATENATE_DEFN \
    ARRAY_LIST_CONCATENATE_DECL \
    { \
        assert(first); \
        assert(second); \
        if(first->cap < first->len + second->len) { \
            size_t new_cap = first->cap; \
            while(new_cap < first->len + second->len) new_cap <<= 1; \
            COLN_DATA_TYPE *new_buf = COLN_ALLOC( \
                first->allocator, \
                sizeof(COLN_DATA_TYPE) * new_cap); \
            if(!new_buf) return COLN_RESULT_ALLOC_FAILED; \
            COLN_DATA_MOVE_MANY(new_buf, first->data, first->len); \
            COLN_FREE(first->allocator, first->data); \
            first->data = new_buf; \
            first->cap = new_cap; \
        } \
        COLN_DATA_MOVE_MANY(first->data + first->len, \
                            second->data, \
                            second->len); \
        COLN_FREE(second->allocator, second->data); \
        first->len += second->len; \
        return COLN_RESULT_SUCCESS; \
    }

#define ARRAY_LIST__PRIV__EXPAND COLN_CAT(COLN_TYPE, _expand)
#define ARRAY_LIST__PRIV__EXPAND_DECL \
    static coln_result ARRAY_LIST__PRIV__EXPAND(COLN_TYPE *to_expand)
#define ARRAY_LIST__PRIV__EXPAND_DEFN \
    ARRAY_LIST__PRIV__EXPAND_DECL \
    { \
        COLN_INTERNAL_ASSERT(to_expand); \
        size_t new_cap = to_expand->cap << 1; \
        COLN_DATA_TYPE *new_buf = COLN_ALLOC(to_expand->allocator, \
                                             sizeof(COLN_DATA_TYPE) * new_cap); \
        if(!new_buf) return COLN_RESULT_ALLOC_FAILED; \
        COLN_DATA_MOVE_MANY(new_buf, to_expand->data, to_expand->len); \
        COLN_FREE(to_expand->allocator, to_expand->data); \
        to_expand->data = new_buf; \
        to_expand->cap = new_cap; \
        return COLN_RESULT_SUCCESS; \
    }

#ifdef COLN_HEADER
ARRAY_LIST_STRUCT_DEFN
ARRAY_LIST_INIT_DECL;
ARRAY_LIST_COPY_DECL;
ARRAY_LIST_CLEAR_DECL;
ARRAY_LIST_PUSH_BACK_DECL;
ARRAY_LIST_INSERT_AT_DECL;
ARRAY_LIST_POP_BACK_DECL;
ARRAY_LIST_REMOVE_AT_DECL;
ARRAY_LIST_CONCATENATE_DECL;
#endif

#ifdef COLN_IMPL
COLN_DATA__PRIV__COPY_MANY_DECLSC
ARRAY_LIST__PRIV__EXPAND_DECL;
ARRAY_LIST_INIT_DEFN
ARRAY_LIST_COPY_DEFN
ARRAY_LIST_CLEAR_DEFN
ARRAY_LIST_PUSH_BACK_DEFN
ARRAY_LIST_INSERT_AT_DEFN
ARRAY_LIST_POP_BACK_DEFN
ARRAY_LIST_REMOVE_AT_DEFN
ARRAY_LIST_CONCATENATE_DEFN
COLN_DATA__PRIV__COPY_MANY_DEFN
ARRAY_LIST__PRIV__EXPAND_DEFN
#endif

#undef ARRAY_LIST__PRIV__EXPAND_DEFN
#undef ARRAY_LIST__PRIV__EXPAND_DECL
#undef ARRAY_LIST_CONCATENATE_DEFN
#undef ARRAY_LIST_CONCATENATE_DECL
#undef ARRAY_LIST_REMOVE_AT_DEFN
#undef ARRAY_LIST_REMOVE_AT_DECL
#undef ARRAY_LIST_POP_BACK_DEFN
#undef ARRAY_LIST_POP_BACK_DECL
#undef ARRAY_LIST_INSERT_AT_DEFN
#undef ARRAY_LIST_INSERT_AT_DECL
#undef ARRAY_LIST_PUSH_BACK_DEFN
#undef ARRAY_LIST_PUSH_BACK_DECL
#undef ARRAY_LIST_CLEAR_DEFN
#undef ARRAY_LIST_CLEAR_DECL
#undef ARRAY_LIST_COPY_DEFN
#undef ARRAY_LIST_COPY_DECL
#undef ARRAY_LIST_INIT_DEFN
#undef ARRAY_LIST_INIT_DECL
#undef ARRAY_LIST_STRUCT_DEFN
#undef COLN_ALLOC_ASSERT
#undef COLN_ALLOC_ASSIGN
#undef COLN_ALLOC_ARG
#undef COLN_ALLOC_DECL
#undef COLN_FREE
#undef COLN_ALLOC
#undef COLN_ALLOC_TYPE
#undef COLN_TYPE
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
