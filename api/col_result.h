#pragma once

#include <inttypes.h>
#include <stddef.h>

enum col_result
{
    COL_RESULT_SUCCESS,
    COL_RESULT_ALLOC_FAILED,
    COL_RESULT_BAD_ARG,
    COL_RESULT_COPY_ELEM_FAILED,
    COL_RESULT_IDX_OOB,
    COL_RESULT_EQ_FN_MISSING,
    COL_RESULT_CMP_FN_MISSING,
    COL_RESULT_ELEM_NOT_FOUND,
    COL_RESULT_OP_REQUIRES_SORTING,

    COL_RESULT__LEN
};
extern char const * const RESULT_STRS[];
