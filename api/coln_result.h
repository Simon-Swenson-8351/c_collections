#pragma once

typedef enum coln_result
{
    COLN_RESULT_SUCCESS,
    COLN_RESULT_ALLOC_FAILED,
    COLN_RESULT_COPY_ELEM_FAILED,
    COLN_RESULT_ELEM_NOT_FOUND,

    COLN_RESULT__LEN
} coln_result;
