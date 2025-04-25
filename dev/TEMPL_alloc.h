#pragma once

#include <inttypes.h>
#include <stddef.h>

struct TEMPL_alloc
{
    uint8_t buffer[1024];
    int used;
};

void *TEMPL_alloc_malloc(struct TEMPL_alloc *self, size_t size);
void TEMPL_alloc_free(struct TEMPL_alloc *self, void *to_free);
