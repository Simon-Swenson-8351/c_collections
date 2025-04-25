#include "TEMPL_example_struct.h"

#include <string.h>

#include "TEMPL_alloc.h"

bool TEMPL_example_struct_cp(struct TEMPL_alloc *alloc, struct TEMPL_example_struct *dest, struct TEMPL_example_struct *src)
{
    dest->str = TEMPL_alloc_malloc(alloc, strlen(src->str) + 1);
    if(!dest->str) return false;
    memcpy(dest->str, src->str, strlen(src->str) + 1);
    dest->num_words = src->num_words;
}

void TEMPL_example_struct_clr(struct TEMPL_alloc *alloc, struct TEMPL_example_struct *to_clear)
{
    TEMPL_alloc_free(alloc, to_clear->str);
}

int TEMPL_example_struct_cmp(struct TEMPL_example_struct *a, struct TEMPL_example_struct *b)
{
    return strcmp(a->str, b->str);
}

bool TEMPL_example_struct_eq(struct TEMPL_example_struct *a, struct TEMPL_example_struct *b)
{
    return TEMPL_example_struct_cmp(a, b) == 0;
}
