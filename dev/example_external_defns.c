#include "example_external_defns.h"

#include <string.h>

void MACRO_ARG_datatype_mv(MACRO_ARG_datatype *dest, MACRO_ARG_datatype *src)
{
    *dest = *src;
    src->str = NULL;
}

bool MACRO_ARG_datatype_cp(MACRO_ARG_datatype *dest, MACRO_ARG_datatype *src)
{
    dest->str = malloc(strlen(src->str) + 1);
    if(!dest->str) return false;
    memcpy(dest->str, src->str, strlen(src->str) + 1);
    dest->num_words = src->num_words;
    return true;
}

void MACRO_ARG_datatype_clr(MACRO_ARG_datatype *to_clear)
{
    free(to_clear->str);
}

int MACRO_ARG_datatype_cmp(MACRO_ARG_datatype *a, MACRO_ARG_datatype *b)
{
    return strcmp(a->str, b->str);
}

bool MACRO_ARG_datatype_eq(MACRO_ARG_datatype *a, MACRO_ARG_datatype *b)
{
    return MACRO_ARG_datatype_cmp(a, b) == 0;
}
