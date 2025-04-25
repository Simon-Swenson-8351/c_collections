#pragma once

#include <stdbool.h>

struct TEMPL_alloc;

struct TEMPL_example_struct
{
    char *str;
    int num_words;
};

bool TEMPL_example_struct_cp(struct TEMPL_alloc *alloc, struct TEMPL_example_struct *dest, struct TEMPL_example_struct *src);
void TEMPL_example_struct_clr(struct TEMPL_alloc *alloc, struct TEMPL_example_struct *to_clear);
int TEMPL_example_struct_cmp(struct TEMPL_example_struct *a, struct TEMPL_example_struct *b);
bool TEMPL_example_struct_eq(struct TEMPL_example_struct *a, struct TEMPL_example_struct *b);
