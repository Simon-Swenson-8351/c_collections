#include "col_elem_priv.h"

#include <inttypes.h>
#include <string.h>

bool col_elem_cp(struct col_elem_metadata *md, void *dest, void *src)
{
    if(md->cp_fn) return md->cp_fn(md, dest, src);
    memcpy(dest, src, md->elem_size);
    return true;
}

bool col_elem_cp_many(struct col_elem_metadata *md, void *dest, void *src, size_t count)
{
    if(!md->cp_fn)
    {
        memcpy(dest, src, md->elem_size * count);
        return true;
    }
    for(size_t i = 0; i < count; i++)
    {
        if(!md->cp_fn(md, (uint8_t *)(dest) + md->elem_size * i, (uint8_t *)(src) + md->elem_size * i))
        {
            col_elem_clr_many(md, dest, i);
            return false;
        }
    }
    return true;
}

void col_elem_clr(struct col_elem_metadata *md, void *to_clear)
{
    if(md->clr_fn) return md->clr_fn(md, to_clear);
}

void col_elem_clr_many(struct col_elem_metadata *md, void *to_clear, size_t count)
{
    if(!md->clr_fn) return;
    for(size_t i = 0; i < count; i++) md->clr_fn(md, to_clear);
}

bool col_elem_eq(struct col_elem_metadata *md, void *a, void *b)
{
    if(md->eq_fn) return md->eq_fn(md, a, b);
    if(md->cmp_fn) return md->cmp_fn(md, a, b) == 0;
    return memcmp(a, b, md->elem_size) == 0;
}

int col_elem_cmp(struct col_elem_metadata *md, void *a, void *b)
{
    if(md->cmp_fn) return md->cmp_fn(md, a, b);
    return memcmp(a, b, md->elem_size);
}
