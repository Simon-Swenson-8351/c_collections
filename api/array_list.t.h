#include "col_dyn_ary_priv.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "col_result.h"

#ifndef COLN_DATA_TYPE
#error "Collection macros require COLN_DATA_TYPE"
#endif

#ifndef COLN_DATA_COPY
#define COLN_DATA_COPY(dest_ptr, src_ptr) *dest_ptr = *src_ptr, true
#define COLN_DATA_COPY_MANY(dest_ptr, src_ptr, count) memcpy(dest_ptr, src_ptr, sizeof(COLN_DATA_TYPE) * (count)), true
#endif

#ifndef COLN_DATA_COPY_MANY
#define COLN_DATA_COPY_MANY COLN_DATA_TYPE ## _colnhelper_copy_many
#define COLN_DATA_COPY_MANY_DECL static bool COLN_DATA_COPY_MANY(COLN_DATA_TYPE *dest, COLN_DATA_TYPE *src, size_t count)
#define COLN_DATA_COPY_MANY_DEFN \
    COLN_DATA_COPY_MANY_DECL \
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
#define COLN_DATA_COPY_MANY_DECL
#define COLN_DATA_COPY_MANY_DEFN
#endif

#ifndef COLN_DATA_MOVE
#define COLN_DATA_MOVE(dest_ptr, src_ptr) *dest_ptr = *src_ptr
#define COLN_DATA_MOVE_MANY(dest_ptr, src_ptr, count) memcpy(dest_ptr, src_ptr, sizeof(COLN_DATA_TYPE) * (count))
#endif

#ifdef COLN_DATA_MOVE_MANY
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
#error "Collection macros require COLN_TYPE"
#endif

#ifndef COLN_ERR_HANDLING_TYPE
#define COLN_ERR_HANDLING_TYPE COLN_EHT_RET
#endif

#if COLN_ERR_HANDLING_TYPE == COLN_EHT_RET
#define COLN_CHECK(expr, return_result) \
    do \
    { \
        if(expr) \
        { \
            return return_result; \
        } \
    } while(0)
#define COLN_CHECK_CLEANUP(expr, return_result, cleanup_jump_label) \
    do \
    { \
        if(expr) \
        { \
            result = return_result; \
            goto cleanup_jump_label; \
        } \
    } while(0)
#define COLN_CHECK_EVAL COLN_CHECK
#define COLN_RETURN_TYPE coln_result
#define COLN_RETURN_VAR_DECL coln_result result
#define COLN_RETURN_VAR_ASSIGN(x) result = x
#define COLN_RETURN_STMT(x) return x
#elif COLN_ERR_HANDLING_TYPE == COLN_EHT_EXIT
#define COLN_CHECK(expr, return_result) \
    do { \
        if(expr) \
        { \
            fprintf(stderr, #expr "\n"); \
            exit(1); \
        } \
    } while(0)
#define COLN_CHECK_CLEANUP(expr, return_result, cleanup_jump_label) \
    do { \
        if(expr) \
        { \
            fprintf(stderr, #expr "\n"); \
            exit(1); \
        } \
    } while(0)
#define COLN_CHECK_EVAL COLN_CHECK
#define COLN_RETURN_TYPE void
#define COLN_RETURN_VAR_DECL
#define COLN_RETURN_VAR_ASSIGN(x) x
#define COLN_RETURN_STMT(x) return
#elif COLN_ERR_HANDLING_TYPE == COLN_EHT_NO_CHECK
#define COLN_CHECK(expr, return_result)
#define COLN_CHECK_CLEANUP(expr, return_result, cleanup_jump_label)
#define COLN_CHECK_EVAL(expr, return_result, cleanup_jump_label) expr
#define COLN_RETURN_TYPE void
#define COLN_RETURN_VAR_DECL
#define COLN_RETURN_VAR_ASSIGN(x) x
#define COLN_RETURN_STMT(x) return
#else
#error "Collection macros were given an unknown COLN_ERR_HANDLING_TYPE"
#endif

#ifdef COLN_ALLOC_TYPE
#define COLN_ALLOC_DECL COLN_ALLOC_TYPE *allocator;
#define COLN_ALLOC_ARG COLN_ALLOC_TYPE *allocator,
#define COLN_ALLOC_ASSIGN(self) self->allocator = allocator
#define COLN_ALLOC_REASSIGN(self, other) self->allocator = other->allocator
#ifndef COLN_ALLOC
#error "Collection macros require an allocation function if an allocator type is defined."
#endif
#ifndef COLN_FREE
#define COLN_FREE(allocator, ptr_to_free)
#endif
#else
#define COLN_ALLOC_DECL
#define COLN_ALLOC_ARG
#define COLN_ALLOC_ASSIGN(self)
#define COLN_ALLOC_REASSIGN(self, other)
#define COLN_ALLOC(allocator, size_to_alloc) malloc(size_to_alloc)
#define COLN_FREE(allocator, ptr_to_free) free(ptr_to_free)
#endif

#define ARY_LIST_STRUCT_DEFN \
    typedef struct COLN_TYPE \
    { \
        COLN_ALLOC_DECL \
        COLN_DATA_TYPE *data; \
        size_t len; \
        size_t cap; \
    } COLN_TYPE;

#define ARRAY_LIST_INIT_DECL \
    COLN_RETURN_TYPE COLN_TYPE ## _init(COLN_TYPE *to_init, \
                                        COLN_ALLOC_ARG
                                        unsigned int initial_cap_exp)
#define ARRAY_LIST_INIT_DEFN \
    ARRAY_LIST_INIT_DECL \
    { \
        COLN_CHECK(!to_init, COLN_RESULT_BAD_ARG); \
        COLN_CHECK(!allocator, COLN_RESULT_BAD_ARG); \
        COLN_ALLOC_ASSIGN(to_init); \
        to_init->cap = 1 << initial_cap_exp;
        to_init->data = COLN_ALLOC( \
            allocator, \
            sizeof(COLN_DATA_TYPE) * to_init->cap); \
        COLN_CHECK(!to_init->data, COLN_RESULT_ALLOC_FAILED, end); \
        to_init->len = 0;
        COLN_RETURN_STMT(COLN_RESULT_SUCCESS);
    }

#define ARRAY_LIST_COPY_DECL \
    COLN_RETURN_TYPE COLN_TYPE ## _copy(COLN_TYPE *dest, COLN_TYPE *src)
#define ARRAY_LIST_COPY_DEFN \
    ARRAY_LIST_COPY_DECL \
    { \
        COLN_CHECK(!dest, COLN_RESULT_BAD_ARG); \
        COLN_CHECK(!src, COLN_RESULT_BAD_ARG); \
        COLN_ALLOC_REASSIGN(dest, src); \
        dest->len = src->len; \
        dest->cap = src->cap; \
        dest->data = COLN_ALLOC(src->allocator, \
                                   sizeof(COLN_DATA_TYPE) * src->cap); \
        COLN_CHECK(!dest->data, COLN_RESULT_ALLOC_FAILED); \
        COLN_CHECK_EVAL( \
            !COLN_DATA_COPY_MANY(dest->data, src->data, src->len), \
            COLN_RESULT_ALLOC_FAILED); \
        COLN_RETURN_STMT(COLN_RESULT_SUCCESS); \
    }

#define ARRAY_LIST_CLEAR_DECL \
    void COLN_TYPE ## _clear(COLN_TYPE *to_clear)
#define ARRAY_LIST_CLEAR_DEFN \
    ARRAY_LIST_CLEAR_DECL \
    { \
        COLN_DATA_CLEAR_MANY(to_clear->data, to_clear->len); \
        COLN_ALLOC_FREE(to_clear->data); \
    }

#define ARRAY_LIST_PUSH_BACK_DECL \
    COLN_RETURN_TYPE COLN_TYPE ## _push_back(COLN_TYPE *self, COLN_DATA_TYPE *to_insert)
#define ARRAY_LIST_PUSH_BACK_DEFN \
    ARRAY_LIST_PUSH_BACK_DECL \
    { \
        if(self->len == self->cap) \
        { \
            COL_RETURN_VAR_ASSIGN(expand(self)); \
    
    }

#define COLN_H \
    ARY_LIST_STRUCT_DEFN

static enum col_result expand(struct col_dyn_ary *dyn_ary);

enum col_result col_dyn_ary_init(struct col_dyn_ary *to_init, struct col_allocator *allocator, struct col_elem_metadata *elem_metadata, size_t initial_cap, float growth_factor)
{
    if(!to_init) return COL_RESULT_BAD_ARG;
    if(!allocator) return COL_RESULT_BAD_ARG;
    if(!elem_metadata) return COL_RESULT_BAD_ARG;
    if(growth_factor <= 1.0) return COL_RESULT_BAD_ARG;
    to_init->allocator = allocator;
    to_init->elem_metadata = elem_metadata;
    if(initial_cap > 0)
    {
        to_init->data = col_allocator_malloc(allocator, elem_metadata->elem_size * initial_cap);
        if(!to_init->data) return COL_RESULT_ALLOC_FAILED;
    }
    else
    {
        to_init->data = NULL;
    }
    to_init->len = 0;
    to_init->cap = initial_cap;
    to_init->growth_factor = growth_factor;
    return COL_RESULT_SUCCESS;
}

enum col_result col_dyn_ary_copy(struct col_dyn_ary *dest, struct col_dyn_ary *src)
{
    if(!dest) return COL_RESULT_BAD_ARG;
    if(!src) return COL_RESULT_BAD_ARG;
    dest->allocator = src->allocator;
    dest->elem_metadata = src->elem_metadata;
    dest->len = src->len;
    dest->cap = src->cap;
    dest->growth_factor = src->growth_factor;
    if(src->cap > 0)
    {
        dest->data = col_allocator_malloc(src->allocator, src->elem_metadata->elem_size * src->cap);
        if(!dest->data) return COL_RESULT_ALLOC_FAILED;
    }
    else
    {
        dest->data = NULL;
    }
    return col_elem_cp_many(src->elem_metadata, dest->data, src->data, src->len);
}

void col_dyn_ary_clear(struct col_dyn_ary *to_clear)
{
    col_elem_clr_many(to_clear->elem_metadata, to_clear->data, to_clear->len);
    free(to_clear->data);
}

size_t col_dyn_ary_len(struct col_dyn_ary *self)
{
    return self->len;
}

enum col_result col_dyn_ary_insert_at(struct col_dyn_ary *dyn_ary, void *to_insert, size_t index)
{
    if(index > dyn_ary->len) return COL_RESULT_IDX_OOB;
    if(dyn_ary->len == dyn_ary->cap)
    {
        enum col_result result;
        if(result = expand(dyn_ary)) return result;
    }
    for(size_t i = dyn_ary->len; i > index; i--)
    {
        memcpy(
            dyn_ary->data + dyn_ary->elem_metadata->elem_size * i,
            dyn_ary->data + dyn_ary->elem_metadata->elem_size * (i - 1),
            dyn_ary->elem_metadata->elem_size
        );
    }
    memcpy(
        dyn_ary->data + dyn_ary->elem_metadata->elem_size * index,
        to_insert,
        dyn_ary->elem_metadata->elem_size
    );
    dyn_ary->len++;
    return COL_RESULT_SUCCESS;
}

enum col_result col_dyn_ary_push_back(struct col_dyn_ary *dyn_ary, void *to_insert)
{
    return col_dyn_ary_insert_at(dyn_ary, to_insert, dyn_ary->len);
}

void *col_dyn_ary_get(struct col_dyn_ary const *dyn_ary, size_t idx)
{
    if(idx >= dyn_ary->len) return NULL;
    return dyn_ary->data + dyn_ary->elem_metadata->elem_size * idx;
}

enum col_result col_dyn_ary_rm(struct col_dyn_ary *dyn_ary, size_t idx, void *removed_elem)
{
    if(idx >= dyn_ary->len) return COL_RESULT_IDX_OOB;
    memcpy(
        removed_elem,
        dyn_ary->data + dyn_ary->elem_metadata->elem_size * idx,
        dyn_ary->elem_metadata->elem_size
    );
    for(size_t i = idx; i + 1 < dyn_ary->len; i++)
    {
        memcpy(
            dyn_ary->data + dyn_ary->elem_metadata->elem_size * i,
            dyn_ary->data + dyn_ary->elem_metadata->elem_size * (i + 1),
            dyn_ary->elem_metadata->elem_size
        );
    }
    dyn_ary->len--;
    return COL_RESULT_SUCCESS;
}

enum col_result col_dyn_ary_pop_back(struct col_dyn_ary *dyn_ary, void *removed_elem)
{
    return col_dyn_ary_rm(dyn_ary, dyn_ary->len - 1, removed_elem);
}

enum col_result col_dyn_ary_lin_search(struct col_dyn_ary *dyn_ary, void *elem, size_t *idx_if_found)
{
    for(size_t i = 0; i < dyn_ary->len; i++)
    {
        if(col_elem_eq(dyn_ary->elem_metadata, elem, dyn_ary->data + dyn_ary->elem_metadata->elem_size * i))
        {
            *idx_if_found = i;
            return COL_RESULT_SUCCESS;
        }
    }
    *idx_if_found = SIZE_MAX;
    return COL_RESULT_ELEM_NOT_FOUND;
}

enum col_result col_dyn_ary_bin_search(struct col_dyn_ary *dyn_ary, void *elem, size_t *idx_if_found)
{
    size_t left = 0;
    size_t right = dyn_ary->len;
    while(left < right)
    {
        size_t mid = (right - left) / 2;
        int cmp_res = col_elem_cmp(
            dyn_ary->elem_metadata,
            elem,
            dyn_ary->data + dyn_ary->elem_metadata->elem_size * mid
        );
        if(cmp_res < 0)
        {
            right = mid;
        }
        else if(cmp_res == 0)
        {
            *idx_if_found = mid;
            return COL_RESULT_SUCCESS;
        }
        else
        {
            left = mid + 1;
        }
    }
    *idx_if_found = SIZE_MAX;
    return COL_RESULT_ELEM_NOT_FOUND;
}

enum col_result col_dyn_ary_trim(struct col_dyn_ary *dyn_ary)
{
    uint8_t *new_buf = col_allocator_malloc(
        dyn_ary->allocator,
        dyn_ary->len * dyn_ary->elem_metadata->elem_size
    );
    if(!new_buf) return COL_RESULT_ALLOC_FAILED;
    memcpy(new_buf, dyn_ary->data, dyn_ary->len * dyn_ary->elem_metadata->elem_size);
    col_allocator_free(dyn_ary->allocator, dyn_ary->data);
    dyn_ary->data = new_buf;
    dyn_ary->cap = dyn_ary->len;
    return COL_RESULT_SUCCESS;
}

enum col_result col_dyn_ary_cat(struct col_dyn_ary *first, struct col_dyn_ary *second)
{
    if(first->cap < first->len + second->len)
    {
        // grow if needed
        uint8_t *new_buf = col_allocator_malloc(first->allocator, first->elem_metadata->elem_size * (first->cap + second->cap));
        if(!new_buf) return COL_RESULT_ALLOC_FAILED;
        memcpy(new_buf, first->data, first->elem_metadata->elem_size * first->len);
        col_allocator_free(first->allocator, first->data);
        first->data = new_buf;
    }
    memcpy(first->data + first->len * first->elem_metadata->elem_size, second->data, second->len * second->elem_metadata->elem_size);
    col_allocator_free(second->allocator, second->data);
    first->len += second->len;
    first->cap += second->cap;
    return COL_RESULT_SUCCESS;
}

enum col_result col_dyn_ary_sort_cmp(struct col_dyn_ary *to_sort, col_sort_cmp_fn sort_fn)
{
    return sort_fn(to_sort->allocator, to_sort->elem_metadata, to_sort->data, to_sort->len);
}

static enum col_result expand(struct col_dyn_ary *dyn_ary)
{
    size_t new_cap = (size_t)((float)(dyn_ary->cap) * dyn_ary->growth_factor);
    if(new_cap == dyn_ary->cap) new_cap++;
    uint8_t *new_buf = col_allocator_malloc(dyn_ary->allocator, dyn_ary->elem_metadata->elem_size * new_cap);
    if(!new_buf) return COL_RESULT_ALLOC_FAILED;
    memcpy(new_buf, dyn_ary->data, dyn_ary->len * dyn_ary->elem_metadata->elem_size);
    col_allocator_free(dyn_ary->allocator, dyn_ary->data);
    dyn_ary->data = new_buf;
    dyn_ary->cap = new_cap;
    return COL_RESULT_SUCCESS;
}
