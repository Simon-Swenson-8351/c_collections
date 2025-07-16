#include <stdbool.h>

#define RANGE_CAT_(a, b) a ## b
#define RANGE_CAT(a, b) RANGE_CAT_(a, b)

#ifdef RANGE_INTERNAL_DEBUG
  #define RANGE_INTERNAL_ASSERT(x) assert(x)
#else
  #define RANGE_INTERNAL_ASSERT(x)
#endif

#if !defined(RANGE_HEADER) && !defined(RANGE_IMPL)
  #error "Define either RANGE_HEADER or RANGE_IMPL to generate code"
#endif

#ifndef RANGE_DATA_TYPENAME
  #error "Define RANGE_DATA_TYPENAME to use the range macros"
#endif

#ifndef RANGE_DATA_COMPARE
  #error "Define RANGE_DATA_COMPARE to use range macros"
#endif

#if !defined(RANGE_DATA)

// RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
// RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR
// RANGE_DATA_TRIVIAL_BY_PTR
// RANGE_DATA_NONTRIVIAL_BY_PTR
#if !defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL) && !defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR) && !defined(RANGE_DATA_TRIVIAL_BY_PTR) && !defined(RANGE_DATA_NONTRIVIAL_BY_PTR)
  #error "Define one of RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL, " \
    "RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR, RANGE_DATA_TRIVIAL_BY_PTR, " \
    "RANGE_DATA_NONTRIVIAL_BY_PTR to define argument passing, copy, move, " \
    "and clear semantics"
#endif

#ifdef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
  #ifdef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR
    #error "Define only one of RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL, " \
      "RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR, RANGE_DATA_TRIVIAL_BY_PTR, " \
      "RANGE_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef RANGE_DATA_TRIVIAL_BY_PTR
    #error "Define only one of RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL, " \
      "RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR, RANGE_DATA_TRIVIAL_BY_PTR, " \
      "RANGE_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef RANGE_DATA_NONTRIVIAL_BY_PTR
    #error "Define only one of RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL, " \
      "RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR, RANGE_DATA_TRIVIAL_BY_PTR, " \
      "RANGE_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef RANGE_DATA_COPY
    #error "RANGE_DATA_COPY was specified for a trivial data type"
  #endif
  #ifdef RANGE_DATA_MOVE
    #error "RANGE_DATA_MOVE was specified for a trivial data type"
  #endif
  #ifdef RANGE_DATA_CLEAR
    #error "RANGE_DATA_CLEAR was specified for a trivial data type"
  #endif
#endif

#ifdef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR
  #ifdef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
    #error "Define only one of RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL, " \
      "RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR, RANGE_DATA_TRIVIAL_BY_PTR, " \
      "RANGE_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef RANGE_DATA_TRIVIAL_BY_PTR
    #error "Define only one of RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL, " \
      "RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR, RANGE_DATA_TRIVIAL_BY_PTR, " \
      "RANGE_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef RANGE_DATA_NONTRIVIAL_BY_PTR
    #error "Define only one of RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL, " \
      "RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR, RANGE_DATA_TRIVIAL_BY_PTR, " \
      "RANGE_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef RANGE_DATA_COPY
    #error "RANGE_DATA_COPY was specified for a trivial data type"
  #endif
  #ifdef RANGE_DATA_MOVE
    #error "RANGE_DATA_MOVE was specified for a trivial data type"
  #endif
  #ifdef RANGE_DATA_CLEAR
    #error "RANGE_DATA_CLEAR was specified for a trivial data type"
  #endif
#endif

#ifdef RANGE_DATA_TRIVIAL_BY_PTR
  #ifdef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
    #error "Define only one of RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL, " \
      "RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR, RANGE_DATA_TRIVIAL_BY_PTR, " \
      "RANGE_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR
    #error "Define only one of RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL, " \
      "RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR, RANGE_DATA_TRIVIAL_BY_PTR, " \
      "RANGE_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef RANGE_DATA_NONTRIVIAL_BY_PTR
    #error "Define only one of RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL, " \
      "RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR, RANGE_DATA_TRIVIAL_BY_PTR, " \
      "RANGE_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef RANGE_DATA_COPY
    #error "RANGE_DATA_COPY was specified for a trivial data type"
  #endif
  #ifdef RANGE_DATA_MOVE
    #error "RANGE_DATA_MOVE was specified for a trivial data type"
  #endif
  #ifdef RANGE_DATA_CLEAR
    #error "RANGE_DATA_CLEAR was specified for a trivial data type"
  #endif
#endif

#ifdef RANGE_DATA_NONTRIVIAL_BY_PTR
  #ifdef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
    #error "Define only one of RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL, " \
      "RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR, RANGE_DATA_TRIVIAL_BY_PTR, " \
      "RANGE_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR
    #error "Define only one of RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL, " \
      "RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR, RANGE_DATA_TRIVIAL_BY_PTR, " \
      "RANGE_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef RANGE_DATA_NONTRIVIAL_BY_VAL
    #error "Define only one of RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL, " \
      "RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR, RANGE_DATA_TRIVIAL_BY_PTR, " \
      "RANGE_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifndef RANGE_DATA_COPY
    #error "Define RANGE_DATA_COPY for a nontrivial data type"
  #endif
  #ifndef RANGE_DATA_MOVE
    #error "Define RANGE_DATA_MOVE for a nontrivial data type"
  #endif
  #ifndef RANGE_DATA_CLEAR
    #error "Define RANGE_DATA_CLEAR for a nontrivial data type"
  #endif
#endif

#ifndef RANGE_TYPENAME
#define RANGE_TYPENAME RANGE_CAT(RANGE_DATA_TYPENAME, _range)
#endif

#define RANGE_INIT_FNNAME RANGE_CAT(RANGE_TYPENAME, _init)
#if defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL)
  #define RANGE_INIT_SIGN \
    RANGE_TYPENAME RANGE_INIT_FNNAME(RANGE_DATA_TYPENAME low, \
                                     RANGE_DATA_TYPENAME high)
#elif defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR)
  #define RANGE_INIT_SIGN \
    void RANGE_INIT_FNNAME(RANGE_TYPENAME *to_init, \
                           RANGE_DATA_TYPENAME low, \
                           RANGE_DATA_TYPENAME high)
#else
  #define RANGE_INIT_SIGN \
    void RANGE_INIT_FNNAME(RANGE_TYPENAME *to_init, \
                           RANGE_DATA_TYPENAME *low, \
                           RANGE_DATA_TYPENAME *high)
#endif
#define RANGE_INIT_DECL RANGE_INIT_SIGN;

#ifdef RANGE_DATA_NONTRIVIAL_BY_PTR
  #define RANGE_COPY_FNNAME RANGE_CAT(RANGE_TYPENAME, _copy)
  #define RANGE_COPY_SIGN \
    coln_result RANGE_COPY_FNNAME(RANGE_TYPENAME *dest, RANGE_TYPENAME *src)
  #define RANGE_COPY_DECL RANGE_COPY_SIGN;
#else
  #define RANGE_COPY_FNNAME
  #define RANGE_COPY_SIGN
  #define RANGE_COPY_DECL
#endif

#ifdef RANGE_DATA_NONTRIVIAL_BY_PTR
  #define RANGE_MOVE_FNNAME RANGE_CAT(RANGE_TYPENAME, _move)
  #define RANGE_MOVE_SIGN \
    void RANGE_MOVE_FNNAME(RANGE_TYPENAME *dest, RANGE_TYPENAME *src)
  #define RANGE_MOVE_DECL RANGE_MOVE_SIGN;
#else
  #define RANGE_MOVE_FNNAME
  #define RANGE_MOVE_SIGN
  #define RANGE_MOVE_DECL
#endif

#ifdef RANGE_DATA_NONTRIVIAL_BY_PTR
  #define RANGE_CLEAR_FNNAME RANGE_CAT(RANGE_TYPENAME, _clear)
  #define RANGE_CLEAR_SIGN void RANGE_CLEAR_FNNAME(RANGE_TYPENAME* to_clear)
  #define RANGE_CLEAR_DECL RANGE_CLEAR_SIGN;
#else
  #define RANGE_CLEAR_FNNAME
  #define RANGE_CLEAR_SIGN
  #define RANGE_CLEAR_DECL
#endif

#define RANGE_COMPARE_FNNAME RANGE_CAT(RANGE_TYPENAME, _compare)
#if defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL)
  #define RANGE_COMPARE_SIGN \
    int RANGE_COMPARE_FNNAME(RANGE_TYPENAME a, RANGE_TYPENAME b)
#else
  #define RANGE_COMPARE_SIGN \
    int RANGE_COMPARE_FNNAME(RANGE_TYPENAME *a, RANGE_TYPENAME *b)
#endif
#define RANGE_COMPARE_DECL RANGE_COMPARE_SIGN;

#define RANGE_CONTAINS_FNNAME RANGE_CAT(RANGE_TYPENAME, _contains)
#if defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL)
  #define RANGE_CONTAINS_SIGN \
    int RANGE_CONTAINS_FNNAME(RANGE_TYPENAME range, RANGE_DATA_TYPENAME elem)
#elif defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR)
  #define RANGE_CONTAINS_SIGN \
    int RANGE_CONTAINS_FNNAME(RANGE_TYPENAME *range, RANGE_DATA_TYPENAME elem)
#else
  #define RANGE_CONTAINS_SIGN \
    int RANGE_CONTAINS_FNNAME(RANGE_TYPENAME *range, RANGE_DATA_TYPENAME *elem)
#endif
#define RANGE_CONTAINS_DECL RANGE_CONTAINS_SIGN;

#define RANGE_CAN_COMBINE_FNNAME RANGE_CAT(RANGE_TYPENAME, _can_combine)
#ifdef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
  #define RANGE_CAN_COMBINE_SIGN \
    bool RANGE_CAN_COMBINE_FNNAME(RANGE_TYPENAME a, RANGE_TYPENAME b)
#else
  #define RANGE_CAN_COMBINE_SIGN \
    bool RANGE_CAN_COMBINE_FNNAME(RANGE_TYPENAME *a, RANGE_TYPENAME *b)
#endif
#define RANGE_CAN_COMBINE_DECL RANGE_CAN_COMBINE_SIGN;

#define RANGE_TRY_COMBINE_FNNAME RANGE_CAT(RANGE_TYPENAME, _try_combine)
#ifdef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
  #define RANGE_TRY_COMBINE_SIGN \
    bool RANGE_TRY_COMBINE_FNNAME(RANGE_TYPENAME *a, RANGE_TYPENAME b)
#else
  #define RANGE_TRY_COMBINE_SIGN \
    bool RANGE_TRY_COMBINE_FNNAME(RANGE_TYPENAME *a, RANGE_TYPENAME *b)
#endif
#define RANGE_TRY_COMBINE_DECL RANGE_TRY_COMBINE_SIGN;

#define RANGE_COMBINE_FNNAME RANGE_CAT(RANGE_TYPENAME, _combine)
#ifdef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
  #define RANGE_COMBINE_SIGN \
    RANGE_TYPENAME RANGE_COMBINE_FNNAME(RANGE_TYPENAME a, RANGE_TYPENAME b)
#else
  #define RANGE_COMBINE_SIGN \
    void RANGE_COMBINE_FNNAME(RANGE_TYPENAME *a, RANGE_TYPENAME *b)
#endif
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
RANGE_CAN_COMBINE_DECL
RANGE_TRY_COMBINE_DECL
RANGE_COMBINE_DECL

#endif

#ifdef RANGE_IMPL

RANGE_INIT_SIGN
{
  #ifndef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
    assert(to_init);
  #endif
  assert(low);
  assert(high);
  assert(RANGE_DATA_COMPARE(low, high)) <= 0);
  #if defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL)
      return (RANGE_TYPENAME){ .low = low, .high = high };
  #elif defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR)
      to_init->low = low;
      to_init->high = high;
  #elif defined(RANGE_DATA_TRIVIAL_BY_PTR)
      to_init->low = *low;
      to_init->high = *high;
  #else
      RANGE_DATA_MOVE(&(to_init->low), low);
      RANGE_DATA_MOVE(&(to_init->high), high);
  #endif
}

#ifdef RANGE_DATA_NONTRIVIAL_BY_PTR
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

#ifdef RANGE_DATA_NONTRIVIAL_BY_PTR
  RANGE_MOVE_SIGN
  {
    assert(dest);
    assert(src);
    RANGE_DATA_MOVE(&(dest->low), &(src->low));
    RANGE_DATA_MOVE(&(dest->high, &(src->high)));
  }
#endif

#ifdef RANGE_DATA_NONTRIVIAL_BY_PTR
  RANGE_CLEAR_SIGN
  {
    assert(to_clear);
    RANGE_DATA_CLEAR(&(to_clear->low));
    RANGE_DATA_CLEAR(&(to_clear->high));
  }
#endif

RANGE_COMPARE_SIGN
{
  #ifndef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
    assert(a);
    assert(b);
  #endif
  int cmp_ahigh_blow =
    #if defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL)
      RANGE_DATA_COMPARE(a.high, b.low);
    #elif defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR)
      RANGE_DATA_COMPARE(a->high, b->low);
    #else
      RANGE_DATA_COMPARE(&(a->high), &(b->low));
    #endif
  if(cmp_ahigh_blow < 0)
  {
    return -1;
  }
  else if(cmp_ahigh_blow == 0)
  {
    return 0;
  }
  else
  {
    // a->high > b->low
    int cmp_alow_bhigh =
      #if defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL)
        RANGE_DATA_COMPARE(a.low, b.high);
      #elif defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR)
        RANGE_DATA_COMPARE(a->low, b->high);
      #else
        RANGE_DATA_COMPARE(&(a->low), &(b->high));
      #endif
    if(cmp_alow_bhigh > 0)
    {
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
  #ifndef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
    assert(range);
  #endif
  #if defined(RANGE_DATA_TRIVIAL_BY_PTR) || defined(RANGE_DATA_NONTRIVIAL_BY_PTR)
    assert(elem);
  #endif
  #if defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL)
    return RANGE_DATA_COMPARE(elem, range.low) >= 0 &&
      RANGE_DATA_COMPARE(elem, range.high) <= 0;
  #elif defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR)
    return RANGE_DATA_COMPARE(elem, range->low) >= 0 &&
      RANGE_DATA_COMPARE(elem, range->high) <= 0;
  #else
    return RANGE_DATA_COMPARE(elem, &(range->low)) >= 0 &&
      RANGE_DATA_COMPARE(elem, &(range->high)) <= 0;
  #endif
}

RANGE_CAN_COMBINE_SIGN
{
  #ifndef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
    assert(a);
    assert(b);
  #endif
  #ifndef RANGE_DATA_SUCCESSOR
    return RANGE_COMPARE_FNNAME(a, b) == 0;
  #else
    int cmp_ahigh_blow =
      #if defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL)
        RANGE_DATA_COMPARE(a.high, b.low);
      #elif defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR)
        RANGE_DATA_COMPARE(a->high, b->low);
      #else
        RANGE_DATA_COMPARE(&(a->high), &(b->low));
      #endif
    if(cmp_ahigh_blow < 0)
    {
      #if defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL)
        if(RANGE_DATA_COMPARE(RANGE_DATA_SUCCESSOR(a.high), b.low) == 0)
          return true;
      #elif defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR)
        if(RANGE_DATA_COMPARE(RANGE_DATA_SUCCESSOR(a->high), b->low) == 0)
          return true;
      #else
        RANGE_DATA_TYPENAME succ;
        RANGE_DATA_SUCCESSOR(&succ, &(a->high));
        if(RANGE_DATA_COMPARE(&succ, &(b->low)) == 0) return true;
      #endif
      return false;
    }
    else if(cmp_ahigh_blow == 0)
    {
      return true;
    }
    else
    {
      // a->high > b->low
      int cmp_alow_bhigh =
        #if defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL)
          RANGE_DATA_COMPARE(a.low, b.high);
        #elif defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR)
          RANGE_DATA_COMPARE(a->low, b->high);
        #else
          RANGE_DATA_COMPARE(&(a->low), &(b->high));
        #endif
      if(cmp_alow_bhigh > 0)
      {
        #if defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL)
          if(RANGE_DATA_COMPARE(RANGE_DATA_SUCCESSOR(b.high), a.low) == 0)
            return true;
        #elif defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR)
          if(RANGE_DATA_COMPARE(RANGE_DATA_SUCCESSOR(b->high), a->low) == 0)
            return true;
        #else
          RANGE_DATA_TYPENAME succ;
          RANGE_DATA_SUCCESSOR(&succ, &(b->high));
          if(RANGE_DATA_COMPARE(&succ, &(a->low)) == 0) return true;
        #endif
        return false;
      }
      else
      {
        return true;
      }
    }
  #endif
}

RANGE_TRY_COMBINE_SIGN
{
  assert(a);
  #ifndef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
    assert(b);
  #endif
  #ifdef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
    if(!RANGE_CAN_COMBINE_FNNAME(*a, b)) return false;
    *a = RANGE_COMBINE_FNNAME(*a, b);
  #else
    if(!RANGE_CAN_COMBINE_FNNAME(a, b)) return false;
    RANGE_COMBINE_FNNAME(a, b);
  #endif
  return true;
}

RANGE_COMBINE_SIGN
{
  #ifndef RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL
    assert(a);
    assert(b);
  #endif
  assert(RANGE_CAN_COMBINE_FNNAME(a, b));
  #if defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_VAL)
    return (RANGE_TYPENAME)
      {
        .low = (RANGE_DATA_COMPARE(a.low, b.low) < 0) ? a.low : b.low, 
        .high = (RANGE_DATA_COMPARE(a.high, b.high) > 0) ? a.high : b.high
      };
  #elif defined(RANGE_DATA_TRIVIAL_BY_VAL_RANGE_BY_PTR)
    if(RANGE_DATA_COMPARE(a->low, b->low) > 0) a->low = b->low;
    if(RANGE_DATA_COMPARE(a->high, b->high) < 0) a->high = b->high;
  #elif defined(RANGE_DATA_TRIVIAL_BY_PTR)
    if(RANGE_DATA_COMPARE(&(a->low), &(b->low)) > 0) a->low = b->low;
    if(RANGE_DATA_COMPARE(&(a->high), &(b->high)) < 0) a->high = b->high;
  #else
    if(RANGE_DATA_COMPARE(&(a->low), &(b->low)) > 0)
    {
      RANGE_DATA_CLEAR(&(a->low));
      RANGE_DATA_MOVE(&(a->low), &(b->low));
    }
    else
    {
      RANGE_DATA_CLEAR(&(b->low));
    }
    if(RANGE_DATA_COMPARE(&(a->high), &(b->high)) < 0)
    {
      RANGE_DATA_CLEAR(&(a->high));
      RANGE_DATA_MOVE(&(a->high), &(b->high));
    }
    else
    {
      RANGE_DATA_CLEAR(&(b->high));
    }
  #endif
}

#endif
