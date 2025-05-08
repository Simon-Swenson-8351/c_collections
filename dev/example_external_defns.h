#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

/*
 *  MACRO_ARG_datatype
 *  ifdef  MACRO_ARG_POD
 *      We can define default move, move many, copy, copy many, clear, clear many for the user
 *  ifndef MACRO_ARG_POD
 *      MACRO_ARG_datatype_mv
 *      MACRO_ARG_datatype_cp
 *      MACRO_ARG_datatype_clr
 *  ifdef  MACRO_ARG_datatype_cmp
 *      MACRO_ARG_datatype_cmp (use for binary search)
 *      ifdef  MACRO_ARG_datatype_eq
 *          MACRO_ARG_datatype_eq (use for linear search)
 *      ifndef MACRO_ARG_datatype_eq
 *          Implement MACRO_ARG_datatype_eq using MACRO_ARG_datatype_cmp
 *  ifndef MACRO_ARG_datatype_cmp
 *      Do not implement binary search
 *      ifdef  MACRO_ARG_datatype_eq
 *          MACRO_ARG_datatype_eq
 *      ifndef MACRO_ARG_datatype_eq
 *          Do not implement linear search
 * 
 *  Not required to be declared, but an argument to the macros:
 *  MACRO_ARG_dyn_ary_typename
 */

typedef struct
{
    char *str;
    int num_words;
} MACRO_ARG_datatype;

void MACRO_ARG_datatype_mv(MACRO_ARG_datatype *dest, MACRO_ARG_datatype *src);
bool MACRO_ARG_datatype_cp(MACRO_ARG_datatype *dest, MACRO_ARG_datatype *src);
void MACRO_ARG_datatype_clr(MACRO_ARG_datatype *to_clear);
int MACRO_ARG_datatype_cmp(MACRO_ARG_datatype *a, MACRO_ARG_datatype *b);
bool MACRO_ARG_datatype_eq(MACRO_ARG_datatype *a, MACRO_ARG_datatype *b);
