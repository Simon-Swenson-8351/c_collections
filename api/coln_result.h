#pragma once

#include <inttypes.h>
#include <stddef.h>

typedef enum ColnResult
{
    COLN_RESULT_SUCCESS,
    COLN_RESULT_ALLOC_FAILED,
    COLN_RESULT_BAD_ARG,
    COLN_RESULT_COPY_ELEM_FAILED,
    COLN_RESULT_IDX_OOB,
    COLN_RESULT_EQ_FN_MISSING,
    COLN_RESULT_CMP_FN_MISSING,
    COLN_RESULT_ELEM_NOT_FOUND,
    COLN_RESULT_OP_REQUIRES_SORTING,

    COL_RESULT__LEN
} ColnResult;
extern char const * const RESULT_STRS[];
