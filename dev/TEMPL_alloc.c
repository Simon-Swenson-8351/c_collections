#include "TEMPL_alloc.h"

void *TEMPL_alloc_malloc(struct TEMPL_alloc *self, size_t size)
{
    // do the alignment
    size = (8 - (size % 8)) % 8;
    if(size + self->used > sizeof(self->buffer)/sizeof(self->buffer[0])) return NULL;
    void *result = (void *)(self->buffer + self->used);
    self->used += size;
    return result;
}

void TEMPL_alloc_free(struct TEMPL_alloc *self, void *to_free)
{
    // no-op
}
