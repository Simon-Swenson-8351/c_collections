#include "col_allocator_priv.h"

#include <stdlib.h>

#include "col_result.h"

enum col_result col_allocator_init(struct col_allocator *to_init, col_malloc_fn_td malloc_fn, col_free_fn_td free_fn, col_allocator_priv_td *priv_data)
{
    if(!to_init) return COL_RESULT_BAD_ARG;
    if(!malloc_fn) return COL_RESULT_BAD_ARG;
    to_init->malloc = malloc_fn;
    to_init->free = free_fn;
    to_init->priv = priv_data;
    return COL_RESULT_SUCCESS;
}

bool col_allocator_validate(struct col_allocator *to_validate)
{
    if(!to_validate) return false;
    return to_validate->malloc;
}

void *col_allocator_malloc(struct col_allocator *self, size_t size)
{
    if(!self) return malloc(size);
    return self->malloc(self, size);
}

void col_allocator_free(struct col_allocator *self, void *to_free)
{
    if(!self) free(to_free);
    else if(self->free) self->free(self, to_free);
}
