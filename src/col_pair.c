#include "col_pair_priv.h"

#include <stdalign.h>
#include <string.h>

#include "col_elem_priv.h"

size_t
col_pair_size(
    struct col_elem_metadata *first_md,
    struct col_elem_metadata *second_md
)
{
    assert(first_md);
    assert(second_md);
    size_t bytes_to_allocate = sizeof(struct col_pair);
    size_t remainder = bytes_to_allocate & (first_md->alignment - 1);
    if(remainder) bytes_to_allocate += first_md->alignment - remainder;
    bytes_to_allocate += first_md->size;
    size_t remainder = bytes_to_allocate & (second_md->alignment - 1);
    if(remainder) bytes_to_allocate += second_md->alignment - remainder;
    bytes_to_allocate += second_md->size;
    return bytes_to_allocate;
}

enum col_result
col_pair_init(
    struct col_pair *to_init,
    struct col_elem_metadata *first_md,
    struct col_elem_metadata *second_md,
    void *first,
    void *second
)
{
    assert(to_init);
    assert(first_md);
    assert(second_md);
    assert(first);
    assert(second);
    to_init->md1 = first_md;
    to_init->md2 = second_md;
    memcpy(col_pair_first(to_init), first, first_md->size);
    memcpy(col_pair_second(to_init), second, first_md->size);
    return COL_RESULT_SUCCESS;
}

enum col_result col_pair_copy(struct col_pair *dest, struct col_pair *src)
{
    assert(dest);
    assert(src);
    dest->md1 = src->md1;
    dest->md2 = src->md2;
    if(!col_elem_cp(src->md1, col_pair_first(dest), col_pair_first(src))) return COL_RESULT_COPY_ELEM_FAILED;
    if(!col_elem_cp(src->md2, col_pair_second(dest), col_pair_second(src)))
    {
        col_elem_clr(src->md1, col_pair_first(dest));
        return COL_RESULT_COPY_ELEM_FAILED;
    }
}

void col_pair_clear(struct col_pair *to_clear)
{
    col_elem_clr(to_clear->md1, col_pair_first(to_clear));
    col_elem_clr(to_clear->md2, col_pair_second(to_clear));
}

void *col_pair_first(struct col_pair *pair)
{
    uint8_t *pos = (uint8_t *)pair;
    pos += sizeof(struct col_pair);
    size_t remainder = (size_t)pos & (pair->md1->alignment - 1);
    if(remainder) pos += pair->md1->alignment - remainder;
    return pos;
}

void *col_pair_second(struct col_pair *pair)
{
    uint8_t *pos = (uint8_t *)pair;
    pos += sizeof(struct col_pair);
    size_t remainder = (size_t)pos & (pair->md1->alignment - 1);
    if(remainder) pos += pair->md1->alignment - remainder;
    pos += sizeof(pair->md2->size);
    size_t remainder = (size_t)pos & (pair->md2->alignment - 1);
    if(remainder) pos += pair->md2->alignment - remainder;
    return pos;
}
