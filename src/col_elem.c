#include "col_elem_priv.h"

#include <inttypes.h>
#include <stdalign.h>
#include <string.h>

static bool int_eq_fn(void *a, void *b);
static int int_cmp_fn(void *a, void *b);
static bool uint_eq_fn(void *a, void *b);
static int uint_cmp_fn(void *a, void *b);
static bool float_eq_fn(void *a, void *b);
static int float_cmp_fn(void *a, void *b);
static bool double_eq_fn(void *a, void *b);
static int double_cmp_fn(void *a, void *b);
static bool ptr_eq_fn(void *a, void *b);
static int ptr_cmp_fn(void *a, void *b);

extern struct col_elem_metadata const col_elem_metadata_int =
{
    .eq_fn = int_eq_fn,
    .cmp_fn = int_cmp_fn,
    .size = sizeof(int),
    .alignment = alignof(int)
};
extern struct col_elem_metadata const col_elem_metadata_uint =
{
    .eq_fn = uint_eq_fn,
    .cmp_fn = uint_cmp_fn,
    .size = sizeof(unsigned int),
    .alignment = alignof(unsigned int)
};
extern struct col_elem_metadata const col_elem_metadata_float =
{
    .eq_fn = float_eq_fn,
    .cmp_fn = float_cmp_fn,
    .size = sizeof(float),
    .alignment = alignof(float)
};
extern struct col_elem_metadata const col_elem_metadata_double =
{
    .eq_fn = double_eq_fn,
    .cmp_fn = double_cmp_fn,
    .size = sizeof(double),
    .alignment = alignof(double)
};
extern struct col_elem_metadata const col_elem_metadata_ptr =
{
    .eq_fn = ptr_eq_fn,
    .cmp_fn = ptr_cmp_fn,
    .size = sizeof(void *),
    .alignment = alignof(void *)
};

bool col_elem_cp(struct col_elem_metadata *md, void *dest, void *src)
{
    if(md->cp_fn) return md->cp_fn(dest, src);
    memcpy(dest, src, md->size);
    return true;
}

bool col_elem_cp_many(struct col_elem_metadata *md, void *dest, void *src, size_t count)
{
    if(!md->cp_fn)
    {
        memcpy(dest, src, md->size * count);
        return true;
    }
    for(size_t i = 0; i < count; i++)
    {
        if(!md->cp_fn((uint8_t *)(dest) + md->size * i, (uint8_t *)(src) + md->size * i))
        {
            col_elem_clr_many(md, dest, i);
            return false;
        }
    }
    return true;
}

void col_elem_clr(struct col_elem_metadata *md, void *to_clear)
{
    if(md->clr_fn) md->clr_fn(to_clear);
}

void col_elem_clr_many(struct col_elem_metadata *md, void *to_clear, size_t count)
{
    if(!md->clr_fn) return;
    for(size_t i = 0; i < count; i++) md->clr_fn(to_clear);
}

bool col_elem_eq(struct col_elem_metadata *md, void *a, void *b)
{
    if(md->eq_fn) return md->eq_fn(a, b);
    if(md->cmp_fn) return md->cmp_fn(a, b) == 0;
    return memcmp(a, b, md->size) == 0;
}

int col_elem_cmp(struct col_elem_metadata *md, void *a, void *b)
{
    if(md->cmp_fn) return md->cmp_fn(a, b);
    return memcmp(a, b, md->size);
}

static bool int_eq_fn(void *a, void *b) { return *(int *)a == *(int *)b; }
static int int_cmp_fn(void *a, void *b)
{
    if(*(int *)a < *(int *)b) return -1;
    else if(*(int *)a == *(int *)b) return 0;
    else return 1;
}
static bool uint_eq_fn(void *a, void *b) { return *(unsigned int *)a == *(unsigned int *)b; }
static int uint_cmp_fn(void *a, void *b)
{
    if(*(unsigned int *)a < *(unsigned int *)b) return -1;
    else if(*(unsigned int *)a == *(unsigned int *)b) return 0;
    else return 1;
}
static bool float_eq_fn(void *a, void *b) { return *(float *)a == *(float *)b; }
static int float_cmp_fn(void *a, void *b)
{
    if(*(float *)a < *(float *)b) return -1;
    else if(*(float *)a == *(float *)b) return 0;
    else return 1;
}
static bool double_eq_fn(void *a, void *b) { return *(double *)a == *(double *)b; }
static int double_cmp_fn(void *a, void *b)
{
    if(*(double *)a < *(double *)b) return -1;
    else if(*(double *)a == *(double *)b) return 0;
    else return 1;
}
static bool ptr_eq_fn(void *a, void *b) { return (uintptr_t)*(void **)a == (uintptr_t)*(void **)b; }
static int ptr_cmp_fn(void *a, void *b)
{
    if((uintptr_t)*(void **)a < (uintptr_t)*(void **)b) return -1;
    else if((uintptr_t)*(void **)a == (uintptr_t)*(void **)b) return 0;
    else return 1;
}
