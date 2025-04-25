#pragma once

struct TEMPL_example_struct
{
    char *str;
    int num_words;
};

struct TEMPL_allocator
{
    struct TEMPL_example_struct buffer[64];
    unsigned int idx;
};

bool TEMPL_example_struct_cp(struct TEMPL_allocator *alloc, struct TEMPL_example_struct *dest, struct TEMPL_example_struct *src);
void TEMPL_example_struct_clr(struct TEMPL_allocator *alloc, struct TEMPL_example_struct *to_clear);
int TEMPL_example_struct_cmp(void *a, void *b);

bool TEMPL_example_struct_cp(void *alloc, void *dest, void *src)
{
    *(struct TEMPL_example_struct *)dest = *(struct TEMPL_example_struct *)src;
}
void TEMPL_example_struct_clr(void *alloc, void *to_clear)
{
    // no-op
}
int TEMPL_example_struct_cmp(void *a, void *b)
{
    struct TEMPL_example_struct *a_int = (struct TEMPL_example_struct *)a;
    struct TEMPL_example_struct *b_int = (struct TEMPL_example_struct *)b;
    if(a_int->a > b_int->a)
    {
        return 1;
    }
    else if(a_int->a == b_int->a)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}
