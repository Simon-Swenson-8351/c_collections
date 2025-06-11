#pragma once

#include <inttypes.h>
#include <stddef.h>

typedef enum ColnResult
{
    COLN_RESULT_SUCCESS,
    COLN_RESULT_ALLOC_FAILED,
    COLN_RESULT_COPY_ELEM_FAILED,
    COLN_RESULT_ELEM_NOT_FOUND,

    COL_RESULT__LEN
} ColnResult;
