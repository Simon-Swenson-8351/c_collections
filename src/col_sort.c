#include "col_sort_priv.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "col_allocator_priv.h"
#include "col_elem_priv.h"

static void swap(void *a, void *b, void *swap_buf, size_t elem_size);
static void quick_recursive(struct col_allocator     *allocator,
                            struct col_elem_metadata *md,
                            void                     *array_base,
                            size_t                    count,
                            void                     *swap_buf);

enum col_result col_sort_quick(struct col_allocator *allocator, struct col_elem_metadata *md, void *array_base, size_t count)
{
    if(count < 2) return COL_RESULT_SUCCESS;
    uint8_t *swap_buf = col_allocator_malloc(allocator, md->size);
    if(!swap_buf) return COL_RESULT_ALLOC_FAILED;
    quick_recursive(allocator, md, array_base, count, swap_buf);
    col_allocator_free(allocator, swap_buf);
    return COL_RESULT_SUCCESS;
}

enum col_result col_sort_cmp_std_qsort(struct col_allocator *allocator, struct col_elem_metadata *md, void *array_base, size_t count)
{
    if(!md->cmp_fn) return COL_RESULT_CMP_FN_MISSING;
    qsort(array_base, count, md->size, md->cmp_fn);
}

static void swap(void *a, void *b, void *swap_buf, size_t elem_size)
{
    memcpy(swap_buf, a, elem_size);
    memcpy(a, b, elem_size);
    memcpy(b, swap_buf, elem_size);
}

static void quick_recursive(struct col_allocator*     allocator,
                            struct col_elem_metadata* md,
                            void*                     array_base,
                            size_t                    count,
                            void*                     swap_buf)
{
    if(count < 2) return;
    size_t pivot_idx = count / 2;
    if(pivot_idx != count - 1)
    {
        swap((uint8_t *)array_base + pivot_idx   * md->size,
             (uint8_t *)array_base + (count - 1) * md->size,
             swap_buf,
             md->size);
    }
    pivot_idx = count - 1;
    size_t left_top = 0;
    size_t right_top = pivot_idx;
    while(left_top != right_top)
    {
        int cmp_res = col_elem_cmp(md,
                                  (uint8_t *)array_base + left_top  * md->size,
                                  (uint8_t *)array_base + pivot_idx * md->size);
        if(cmp_res > 0)
        {
            right_top--;
            swap((uint8_t *)array_base + left_top  * md->size,
                 (uint8_t *)array_base + right_top * md->size,
                 swap_buf,
                 md->size);
        }
        else
        {
            left_top++;
        }
    }
    if(pivot_idx != right_top)
    {
        swap((uint8_t *)array_base + pivot_idx * md->size,
             (uint8_t *)array_base + right_top * md->size,
             swap_buf,
             md->size);
        right_top++;
    }
    quick_recursive(allocator, md, array_base, left_top, swap_buf);
    quick_recursive(allocator, md, (uint8_t *)array_base + right_top * md->size, count - right_top, swap_buf);
}
