#include "col_allocator_priv.h"

#include <stdlib.h>

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
