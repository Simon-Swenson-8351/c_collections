#include "example_dyn_ary.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "col_elem_priv.h"
#include "col_result.h"

static enum col_result expand(MACRO_ARG_dyn_ary_typename *dyn_ary);

enum col_result
MACRO_ARG_dyn_ary_typename_init(
    MACRO_ARG_dyn_ary_typename *to_init,
    struct col_allocator *allocator,
    size_t initial_cap,
    float growth_factor
)
{
    if(!to_init) return COL_RESULT_BAD_ARG;
    if(!allocator) return COL_RESULT_BAD_ARG;
    if(growth_factor <= 1.0) return COL_RESULT_BAD_ARG;
    to_init->allocator = allocator;
    if(initial_cap > 0)
    {
        to_init->data = allocator->malloc(allocator, sizeof(MACRO_ARG_datatype) * initial_cap);
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

enum col_result
MACRO_ARG_dyn_ary_typename_copy(
    MACRO_ARG_dyn_ary_typename *dest,
    MACRO_ARG_dyn_ary_typename *src
)
{
    if(!dest) return COL_RESULT_BAD_ARG;
    if(!src) return COL_RESULT_BAD_ARG;
    dest->allocator = src->allocator;
    dest->len = src->len;
    dest->cap = src->cap;
    dest->growth_factor = src->growth_factor;
    if(src->cap > 0)
    {
        dest->data = src->allocator->malloc(src->allocator, sizeof(MACRO_ARG_datatype) * src->cap);
        if(!dest->data) return COL_RESULT_ALLOC_FAILED;
    }
    else
    {
        dest->data = NULL;
    }
    return MACRO_ARG_datatype_cp_many(dest->data, src->data, src->len);
}

void
MACRO_ARG_dyn_ary_typename_clear(
    MACRO_ARG_dyn_ary_typename *to_clear
)
{
    TEMPLATE_datatype_clr_many(to_clear->data, to_clear->len);
    TEMPLATE_allocator_free(to_clear->data);
}

size_t
MACRO_ARG_dyn_ary_typename_len(
    MACRO_ARG_dyn_ary_typename *self
)
{
    return self->len;
}

enum col_result
MACRO_ARG_dyn_ary_typename_insert_at(
    MACRO_ARG_dyn_ary_typename *dyn_ary,
    TEMPLATE_datatype *to_insert,
    size_t index
)
{
    if(index > dyn_ary->len) return COL_RESULT_IDX_OOB;
    if(dyn_ary->len == dyn_ary->cap)
    {
        enum col_result result;
        if(result = expand(dyn_ary)) return result;
    }
    for(size_t i = dyn_ary->len; i > index; i--)
    {
        TEMPLATE_datatype_mv(dyn_ary->data + i, dyn_ary->data + i - 1);
        // memcpy(
        //     dyn_ary->data + dyn_ary->elem_metadata->elem_size * i,
        //     dyn_ary->data + dyn_ary->elem_metadata->elem_size * (i - 1),
        //     dyn_ary->elem_metadata->elem_size
        // );
    }
    TEMPLATE_datatype_mv(dyn_ary->data + index, to_insert);
    // memcpy(
    //     dyn_ary->data + dyn_ary->elem_metadata->elem_size * index,
    //     to_insert,
    //     dyn_ary->elem_metadata->elem_size
    // );
    dyn_ary->len++;
    return COL_RESULT_SUCCESS;
}

enum col_result
MACRO_ARG_dyn_ary_typename_push_back(
    MACRO_ARG_dyn_ary_typename *dyn_ary,
    TEMPLATE_datatype *to_insert
)
{
    return MACRO_ARG_dyn_ary_typename_insert_at(dyn_ary, to_insert, dyn_ary->len);
}

TEMPLATE_datatype *
MACRO_ARG_dyn_ary_typename_get(
    MACRO_ARG_dyn_ary_typename *dyn_ary,
    size_t idx
)
{
    if(idx >= dyn_ary->len) return NULL;
    return dyn_ary->data + idx;
}

enum col_result
MACRO_ARG_dyn_ary_typename_rm(
    MACRO_ARG_dyn_ary_typename *dyn_ary,
    size_t idx,
    TEMPLATE_datatype *removed_elem
)
{
    if(idx >= dyn_ary->len) return COL_RESULT_IDX_OOB;
    TEMPLATE_datatype_mv(removed_elem, dyn_ary->data + idx);
    // memcpy(
    //     removed_elem,
    //     dyn_ary->data + dyn_ary->elem_metadata->elem_size * idx,
    //     dyn_ary->elem_metadata->elem_size
    // );
    for(size_t i = idx; i + 1 < dyn_ary->len; i++)
    {
        TEMPLATE_datatype_mv(dyn_ary->data + i, dyn_ary->data + i + 1);
        // memcpy(
        //     dyn_ary->data + dyn_ary->elem_metadata->elem_size * i,
        //     dyn_ary->data + dyn_ary->elem_metadata->elem_size * (i + 1),
        //     dyn_ary->elem_metadata->elem_size
        // );
    }
    dyn_ary->len--;
    return COL_RESULT_SUCCESS;
}

enum col_result
MACRO_ARG_dyn_ary_typename_pop_back(
    MACRO_ARG_dyn_ary_typename *dyn_ary,
    TEMPLATE_datatype *removed_elem
)
{
    return MACRO_ARG_dyn_ary_typename_rm(dyn_ary, dyn_ary->len - 1, removed_elem);
}

enum col_result
MACRO_ARG_dyn_ary_typename_lin_search(
    MACRO_ARG_dyn_ary_typename *dyn_ary,
    TEMPLATE_datatype *elem,
    size_t *idx_if_found
)
{
    for(size_t i = 0; i < dyn_ary->len; i++)
    {
        if(TEMPLATE_datatype_eq(dyn_ary->elem_metadata, elem, dyn_ary->data + i))
        {
            *idx_if_found = i;
            return COL_RESULT_SUCCESS;
        }
    }
    *idx_if_found = SIZE_MAX;
    return COL_RESULT_ELEM_NOT_FOUND;
}

enum col_result
MACRO_ARG_dyn_ary_typename_bin_search(
    MACRO_ARG_dyn_ary_typename *dyn_ary,
    TEMPLATE_datatype *elem,
    size_t *idx_if_found
)
{
    size_t left = 0;
    size_t right = dyn_ary->len;
    while(left < right)
    {
        size_t mid = (right - left) / 2;
        int cmp_res = TEMPLATE_datatype_cmp(
            dyn_ary->elem_metadata,
            elem,
            dyn_ary->data + mid
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

enum col_result
MACRO_ARG_dyn_ary_typename_trim(
    MACRO_ARG_dyn_ary_typename *dyn_ary
)
{
    uint8_t *new_buf = TEMPLATE_allocator_malloc(
        dyn_ary->allocator,
        dyn_ary->len * sizeof(TEMPLATE_datatype)
    );
    if(!new_buf) return COL_RESULT_ALLOC_FAILED;
    TEMPLATE_datatype_mv_many(new_buf, dyn_ary->data, dyn_ary->len);
    TEMPLATE_allocator_free(dyn_ary->allocator, dyn_ary->data);
    dyn_ary->data = new_buf;
    dyn_ary->cap = dyn_ary->len;
    return COL_RESULT_SUCCESS;
}

enum col_result
MACRO_ARG_dyn_ary_typename_cat(
    MACRO_ARG_dyn_ary_typename *first,
    MACRO_ARG_dyn_ary_typename *second
)
{
    if(first->cap < first->len + second->len)
    {
        // grow if needed
        uint8_t *new_buf = TEMPLATE_allocator_malloc(first->allocator, sizeof(TEMPLATE_datatype) * (first->cap + second->cap));
        if(!new_buf) return COL_RESULT_ALLOC_FAILED;
        TEMPLATE_datatype_mv_many(new_buf, first->data, first->len);
        TEMPLATE_allocator_free(first->allocator, first->data);
        first->data = new_buf;
    }
    TEMPLATE_datatype_mv_many(first->data + first->len, second->data, second->len);
    TEMPLATE_allocator_free(second->allocator, second->data);
    first->len += second->len;
    first->cap += second->cap;
    return COL_RESULT_SUCCESS;
}

enum col_result
MACRO_ARG_dyn_ary_typename_sort_cmp(
    MACRO_ARG_dyn_ary_typename *to_sort,
    col_sort_cmp_fn sort_fn
)
{
    return sort_fn(to_sort->allocator, to_sort->elem_metadata, to_sort->data, to_sort->len);
}

static enum col_result expand(MACRO_ARG_dyn_ary_typename *dyn_ary)
{
    size_t new_cap = (size_t)((float)(dyn_ary->cap) * dyn_ary->growth_factor);
    if(new_cap == dyn_ary->cap) new_cap++;
    uint8_t *new_buf = TEMPLATE_allocator_malloc(dyn_ary->allocator, sizeof(TEMPLATE_datatype) * new_cap);
    if(!new_buf) return COL_RESULT_ALLOC_FAILED;
    TEMPLATE_datatype_mv_many(new_buf, dyn_ary->data, dyn_ary->len);
    TEMPLATE_allocator_free(dyn_ary->allocator, dyn_ary->data);
    dyn_ary->data = new_buf;
    dyn_ary->cap = new_cap;
    return COL_RESULT_SUCCESS;
}
