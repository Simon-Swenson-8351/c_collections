#include <stdbool.h>

#define RANGE_CAT_(a, b) a ## b
#define RANGE_CAT(a, b) RANGE_CAT_(a, b)

#ifdef RANGE_INTERNAL_DEBUG
#define RANGE_INTERNAL_ASSERT(x) assert(x)
#else
#define RANGE_INTERNAL_ASSERT(x)
#endif

#ifndef RANGE_DATA_TYPENAME
#error "Define RANGE_DATA_TYPENAME to use the range macros"
#endif

#if !defined(RANGE_HEADER) && !defined(RANGE_IMPL)
#error "Define either RANGE_HEADER or RANGE_IMPL to generate code"
#endif

#ifndef RANGE_DATA_COMPARE
#error "Define RANGE_DATA_COMPARE to use range macros"
#endif

#ifndef RANGE_TYPENAME
#define RANGE_TYPENAME RANGE_CAT(RANGE_DATA_TYPENAME, _range)
#endif

#define RANGE_INIT_FNNAME RANGE_CAT(RANGE_TYPENAME, _init)
#define RANGE_INIT_SIGN void RANGE_INIT_FNNAME(RANGE_TYPENAME *to_init, \
                                               RANGE_DATA_TYPENAME *low, \
                                               RANGE_DATA_TYPENAME *high)
#define RANGE_INIT_DECL RANGE_INIT_SIGN;

#ifdef RANGE_DATA_COPY
#define RANGE_COPY_FNNAME RANGE_CAT(RANGE_TYPENAME, _copy)
#define RANGE_COPY_SIGN \
    coln_result RANGE_COPY_FNNAME(RANGE_TYPENAME *dest, RANGE_TYPENAME *src)
#define RANGE_COPY_DECL RANGE_COPY_SIGN;
#else
#define RANGE_COPY_FNNAME
#define RANGE_COPY_SIGN
#define RANGE_COPY_DECL
#endif

#ifdef RANGE_DATA_MOVE
#define RANGE_MOVE_FNNAME RANGE_CAT(RANGE_TYPENAME, _move)
#define RANGE_MOVE_SIGN
    void RANGE_MOVE_FNNAME(RANGE_TYPENAME *dest, RANGE_TYPENAME *src)
#define RANGE_MOVE_DECL RANGE_MOVE_SIGN;
#else
#define RANGE_MOVE_FNNAME
#define RANGE_MOVE_SIGN
#define RANGE_MOVE_DECL
#endif

#ifdef RANGE_DATA_CLEAR
#define RANGE_CLEAR_FNNAME RANGE_CAT(RANGE_TYPENAME, _clear)
#define RANGE_CLEAR_SIGN void RANGE_CLEAR_FNNAME(RANGE_TYPENAME* to_clear)
#define RANGE_CLEAR_DECL RANGE_CLEAR_SIGN;
#else
#define RANGE_CLEAR_FNNAME
#define RANGE_CLEAR_SIGN
#define RANGE_CLEAR_DECL
#endif

#define RANGE_COMPARE_FNNAME RANGE_CAT(RANGE_TYPENAME, _compare)
#define RANGE_COMPARE_SIGN \
    range_comparison_result RANGE_COMPARE_FNNAME(RANGE_TYPENAME *a, \
                                                 RANGE_TYPENAME *b)
#define RANGE_COMPARE_DECL RANGE_COMPARE_SIGN;

#define RANGE_CONTAINS_FNNAME RANGE_CAT(RANGE_TYPENAME, _contains)
#define RANGE_CONTAINS_SIGN int RANGE_CONTAINS_FNNAME(RANGE_TYPENAME *range, RANGE_DATA_TYPENAME *elem)
#define RANGE_CONTAINS_DECL RANGE_CONTAINS_SIGN;

#define RANGE_TRY_COMBINE_FNNAME RANGE_CAT(RANGE_TYPENAME, _try_combine)
#define RANGE_TRY_COMBINE_SIGN \
    bool RANGE_TRY_COMBINE_FNNAME(RANGE_TYPENAME *a, RANGE_TYPENAME *b)
#define RANGE_TRY_COMBINE_DECL RANGE_TRY_COMBINE_SIGN;

#define RANGE_COMBINE_FNNAME RANGE_CAT(RANGE_TYPENAME, _combine)
#define RANGE_COMBINE_SIGN \
    void RANGE_COMBINE_FNNAME(RANGE_TYPENAME *a, RANGE_TYPENAME *b)
#define RANGE_COMBINE_DECL RANGE_COMBINE_SIGN;

#ifdef RANGE_HEADER

typedef struct RANGE_TYPENAME
{
    RANGE_DATA_TYPENAME low;
    RANGE_DATA_TYPENAME high;
} RANGE_TYPENAME;

RANGE_INIT_DECL
RANGE_COPY_DECL
RANGE_MOVE_DECL
RANGE_CLEAR_DECL
RANGE_COMPARE_DECL
RANGE_CONTAINS_DECL
RANGE_TRY_COMBINE_DECL
RANGE_COMBINE_DECL

#endif

#ifdef RANGE_IMPL

RANGE_INIT_SIGN
{
    assert(to_init);
    assert(low);
    assert(high);
    assert(RANGE_DATA_COMPARE(low, high)) <= 0);
#ifdef RANGE_DATA_MOVE
    RANGE_DATA_MOVE(&(to_init->low), low);
    RANGE_DATA_MOVE(&(to_init->high), high);
#else
    to_init->low = *low;
    to_init->high = *high;
#endif
}

#ifdef RANGE_DATA_COPY
RANGE_COPY_SIGN
{
    assert(dest);
    assert(src);
    if(!RANGE_DATA_COPY(&(dest->low), &(src->low)))
        return COLN_RESULT_COPY_ELEM_FAILED;
    if(!RANGE_DATA_COPY(&(dest->high), &(src->high)))
    {
        RANGE_DATA_CLEAR(&(dest->low));
        return COLN_RESULT_COPY_ELEM_FAILED;
    }
    return COLN_RESULT_SUCCESS;
}
#endif

#ifdef RANGE_DATA_MOVE
RANGE_MOVE_SIGN
{
    assert(dest);
    assert(src);
    RANGE_DATA_MOVE(&(dest->low), &(src->low));
    RANGE_DATA_MOVE(&(dest->high, &(src->high)));
}
#endif

#ifdef RANGE_DATA_CLEAR
RANGE_CLEAR_SIGN
{
    assert(to_clear);
    RANGE_DATA_CLEAR(&(to_clear->low));
    RANGE_DATA_CLEAR(&(to_clear->high));
}
#endif

RANGE_COMPARE_SIGN
{
    assert(a);
    assert(b);
    int cmp_ahigh_blow = RANGE_DATA_COMPARE(&(a->high), &(b->low));
    if(cmp_ahigh_blow < 0)
    {
#ifdef RANGE_DATA_SUCCESSOR
        RANGE_DATA_TYPENAME ahigh_succ;
        RANGE_DATA_SUCCESSOR(&ahigh_succ, &(a->high));
        if(RANGE_DATA_COMPARE(&ahigh_succ, &(b->low)) == 0)
            return 0;
#endif
        return -1;
    }
    else if(cmp_ahigh_blow == 0)
    {
        return 0;
    }
    else
    {
        // a->high > b->low
        int cmp_alow_bhigh = RANGE_DATA_COMPARE(&(a->low), &(b->high));
        if(cmp_alow_bhigh > 0)
        {
#ifdef RANGE_DATA_SUCCESSOR
            RANGE_DATA_TYPENAME bhigh_succ;
            RANGE_DATA_SUCCESSOR(&bhigh_succ, &(b->high));
            if(RANGE_DATA_COMPARE(&(a->low), &bhigh_succ) == 0)
                return 0;
#endif
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

RANGE_CONTAINS_SIGN
{
    assert(range);
    assert(elem);
    return RANGE_DATA_COMPARE(elem, &(range->low)) >= 0 &&
        RANGE_DATA_COMPARE(elem, &(range->high)) <= 0;
}

RANGE_TRY_COMBINE_SIGN
{
    assert(a);
    assert(b);
    if(RANGE_COMPARE_FNNAME(a, b) != 0) return false;
    RANGE_COMBINE_FNNAME(a, b);
    return true;
}

RANGE_COMBINE_SIGN
{
    assert(a);
    assert(b);
    assert(RANGE_COMPARE_FNNAME(a, b) == 0);
    if(RANGE_DATA_COMPARE(&(a->low), &(b->low)) > 0)
    {
        #ifdef RANGE_DATA_CLEAR
            RANGE_DATA_CLEAR(&(a->low));
        #endif
        #ifdef RANGE_DATA_MOVE
            RANGE_DATA_MOVE(&(a->low), &(b->low));
        #else
            a->low = b->low;
        #endif
    }
    else
    {
        #ifdef RANGE_DATA_CLEAR
            RANGE_DATA_CLEAR(&(b->low));
        #endif
    }
    if(RANGE_DATA_COMPARE(&(a->high), &(b->high)) < 0)
    {
        #ifdef RANGE_DATA_CLEAR
            RANGE_DATA_CLEAR(&(a->high));
        #endif
        #ifdef RANGE_DATAMOVE
            RANGE_DATA_MOVE(&(a->high), &(b->high));
        #else
            a->high = b->high;
        #endif
    }
    else
    {
        #ifdef RANGE_DATA_CLEAR
            RANGE_DATA_CLEAR(&(b->high));
        #endif
    }
}

#endif
