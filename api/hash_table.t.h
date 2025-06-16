#include "coln_result.h"

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

#ifndef COLN_DATA_HASH
#error "Hash table requires a data hash function, COLN_DATA_HASH"
#endif

#ifndef COLN_DATA_COPY
#define COLN_DATA_COPY(dest_ptr, src_ptr) (*(dest_ptr) = *(src_ptr), true)
#define COLN_DATA_COPY_MANY(dest_ptr, src_ptr, count) \
    (memcpy((dest_ptr), (src_ptr), sizeof(COLN_DATA_TYPE) * (count)), true)
#endif

#ifndef COLN_DATA_COPY_MANY
#define COLN_DATA_COPY_MANY COLN_CAT(COLN_DATA_TYPE, _colnhelper_copy_many)
#define COLN_DATA__PRIV__COPY_MANY_SIGN \
    static bool COLN_DATA_COPY_MANY(COLN_DATA_TYPE *dest, \
                                    COLN_DATA_TYPE *src, \
                                    size_t count)
#define COLN_DATA__PRIV__COPY_MANY_DECL COLN_DATA__PRIV__COPY_MANY_SIGN;
#define COLN_DATA__PRIV__COPY_MANY_DEFN \
    COLN_DATA__PRIV__COPY_MANY_SIGN \
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
#define COLN_DATA__PRIV__COPY_MANY_SIGN
#define COLN_DATA__PRIV__COPY_MANY_DECL
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
#define COLN_ALLOC_ARG(allocator) , COLN_ALLOC_TYPE *allocator
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

#define HASH_TABLE_DEFN \
    typedef struct COLN_TYPE \
    { \
        COLN_ALLOC_DECL(allocator) \
        COLN_DATA_TYPE *entries; \
        int *collision_counts; \
        size_t count; \
        size_t cap; \
        int max_collision_count; \
    } COLN_TYPE;

#define HASH_TABLE_INIT_ID COLN_CAT(COLN_TYPE, _init)
#define HASH_TABLE_INIT_SIGN \
    ColnResult HASH_TABLE_INIT_ID(COLN_TYPE *to_init \
                                  COLN_ALLOC_ARG(allocator), \
                                  int initial_cap_exp)
#define HASH_TABLE_INIT_DECL HASH_TABLE_INIT_SIGN;
#define HASH_TABLE_INIT_DEFN \
    HASH_TABLE_INIT_SIGN \
    { \
        assert(to_init); \
        COLN_ALLOC_ASSERT(allocator); \
        to_init->cap = 1 << initial_cap_exp; \
        size_t alloc_size = sizeof(COLN_DATA_TYPE) * to_init->cap; \
        /* Handle potential alignment issues */ \
        alloc_size = (alloc_size + sizeof(int) - 1) & ~(sizeof(int) - 1); \
        size_t collision_count_offset = alloc_size; \
        size_t col_ct_size = sizeof(int) * to_init->cap; \
        alloc_size += col_ct_size; \
        to_init->entries = COLN_ALLOC( \
            allocator, \
            alloc_size); \
        if(!(to_init->entries)) return COLN_RESULT_ALLOC_FAILED; \
        COLN_ALLOC_ASSIGN(to_init->allocator, allocator); \
        to_init->collision_counts = (int *) \
            ((unsigned char *)(to_init->entries) + collision_count_offset); \
        to_init->count = 0; \
        to_init->max_collision_count = 0; \
        /* This will set the integer values to twos-complement -1. */ \
        memset(to_init->collision_counts, 0xff, col_ct_size); \
        return COLN_RESULT_SUCCESS; \
    }

#define HASH_TABLE_COPY_ID COLN_CAT(COLN_TYPE, _copy)
#define HASH_TABLE_COPY_SIGN \
    ColnResult HASH_TABLE_COPY_ID(COLN_TYPE *dest, COLN_TYPE *src)
#define HASH_TABLE_COPY_DECL HASH_TABLE_COPY_SIGN;
#define HASH_TABLE_COPY_DEFN \
    HASH_TABLE_COPY_SIGN \
    { \
        assert(dest); \
        assert(src); \
        dest->entries = COLN_ALLOC( \
            src->allocator, \
            sizeof(HASH_TABLE_ENTRY_ID) * src->cap); \
        if(!(dest->entries)) return COLN_RESULT_ALLOC_FAILED; \
        if(!COLN_DATA_COPY_MANY(dest->entries, src->entries)) \
        { \
            COLN_FREE(src->allocator, dest->entries); \
            return COLN_RESULT_COPY_ELEM_FAILED; \
        } \
        COLN_ALLOC_ASSIGN(dest->allocator, src->allocator); \
        dest->cap = src->cap; \
        dest->count = src->count; \
        dest->max_miss_count = src->max_miss_count; \
        return COLN_RESULT_SUCCESS; \
    }
