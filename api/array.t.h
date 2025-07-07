#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define COLN_CAT(x, y) COLN_CAT_(x, y)
#define COLN_CAT_(x, y) x ## y

#if !defined(COLN_HEADER) && !defined(COLN_IMPL)
#error "COLN_HEADER or COLN_IMPL must be defined"
#endif

#ifndef COLN_DATA_TYPENAME
#error "Collection macros require COLN_DATA_TYPENAME"
#endif

#if !defined(COLN_DATA_TRIVIAL_BY_VAL) && !defined(COLN_DATA_TRIVIAL_BY_PTR) && \
    !defined(COLN_DATA_NONTRIVIAL_BY_PTR)
  #error "Define one of COLN_DATA_TRIVIAL_BY_VAL, COLN_DATA_TRIVIAL_BY_PTR, " \
    "COLN_DATA_NONTRIVIAL_BY_PTR"
#endif

#ifdef COLN_DATA_TRIVIAL_BY_VAL
  #ifdef COLN_DATA_TRIVIAL_BY_PTR
    #error "Define only one of COLN_DATA_TRIVIAL_BY_VAL, " \
      "COLN_DATA_TRIVIAL_BY_PTR, COLN_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef COLN_DATA_NONTRIVIAL_BY_PTR
    #error "Define only one of COLN_DATA_TRIVIAL_BY_VAL, " \
      "COLN_DATA_TRIVIAL_BY_PTR, COLN_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef COLN_DATA_MOVE
    #error "Data was designated trivial, but a move function was provided"
  #endif
#endif

#ifdef COLN_DATA_TRIVIAL_BY_PTR
  #ifdef COLN_DATA_TRIVIAL_BY_VAL
    #error "Define only one of COLN_DATA_TRIVIAL_BY_VAL, " \
      "COLN_DATA_TRIVIAL_BY_PTR, COLN_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef COLN_DATA_NONTRIVIAL_BY_PTR
    #error "Define only one of COLN_DATA_TRIVIAL_BY_VAL, " \
      "COLN_DATA_TRIVIAL_BY_PTR, COLN_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef COLN_DATA_MOVE
    #error "Data was designated trivial, but a move function was provided"
  #endif
#endif

#ifdef COLN_DATA_NONTRIVIAL_BY_PTR
  #ifdef COLN_DATA_TRIVIAL_BY_VAL
    #error "Define only one of COLN_DATA_TRIVIAL_BY_VAL, " \
      "COLN_DATA_TRIVIAL_BY_PTR, COLN_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifdef COLN_DATA_TRIVIAL_BY_PTR
    #error "Define only one of COLN_DATA_TRIVIAL_BY_VAL, " \
      "COLN_DATA_TRIVIAL_BY_PTR, COLN_DATA_NONTRIVIAL_BY_PTR"
  #endif
  #ifndef COLN_DATA_MOVE
    #error "Data was designated nontrivial, but a move function was not provided"
  #endif
#endif

#ifdef COLN_DATA_TRIVIAL_BY_VAL
  #define COLN_DATA_ARG(arg_name) COLN_DATA_TYPENAME arg_name
  #define COLN_DATA_ASSERT_ARG(arg_name)
  #define COLN_DATA_EQUALS_PTR_TO_ARG_WRAPPER(array_offset_ptr, to_find_val) \
    COLN_DATA_EQUALS(*(array_offset_ptr), (to_find_val))
  #define COLN_DATA_COMPARE_PTR_TO_ARG_WRAPPER(array_offset_ptr, to_find_val) \
    COLN_DATA_COMPARE(*(array_offset_ptr), (to_find_val))
  #define COLN_DATA_COMPARE_PTR_TO_PTR_WRAPPER(array_offset_ptr_a, \
                                            array_offset_ptr_b) \
    COLN_DATA_COMPARE(*(array_offset_ptr_a), *(array_offset_ptr_b))
#else 
  #define COLN_DATA_ARG(arg_name) COLN_DATA_TYPENAME *arg_name
  #define COLN_DATA_ASSERT_ARG(arg_name) assert(arg_name)
  #define COLN_DATA_EQUALS_PTR_TO_ARG_WRAPPER(array_offset_ptr, to_find_ptr) \
    COLN_DATA_EQUALS((array_offset_ptr), (to_find_val))
  #define COLN_DATA_COMPARE_PTR_TO_ARG_WRAPPER(array_offset_ptr, to_find_ptr) \
    COLN_DATA_COMPARE((array_offset_ptr), (to_find_val))
  #define COLN_DATA_COMPARE_PTR_TO_PTR_WRAPPER(array_offset_ptr_a, \
                                            array_offset_ptr_b) \
    COLN_DATA_COMPARE((array_offset_ptr_a), (array_offset_ptr_b))
#endif

#ifdef COLN_DATA_NONTRIVIAL_BY_PTR
  #define COLN_DATA_SWAP_BY_PTR(a, b) \
    do \
    { \
      COLN_DATA_TYPENAME swaptmp; \
      COLN_DATA_MOVE(&swaptmp, (a)); \
      COLN_DATA_MOVE((a), (b)); \
      COLN_DATA_MOVE((b), &swaptmp); \
    } while(false)
#else
  #define COLN_DATA_SWAP_BY_PTR(a, b) \
    do \
    { \
      COLN_DATA_TYPENAME swaptmp; \
      swaptmp = *(a); \
      *(a) = *(b); \
      *(b) = swaptmp; \
    } while(false)
#endif

#if !defined(COLN_DATA_EQUALS) && defined(COLN_DATA_COMPARE)
#define COLN_DATA_EQUALS_SET
#define COLN_DATA_EQUALS(a, b) (COLN_DATA_COMPARE((a), (b)) == 0)
#endif

#ifndef ARRAY_TYPENAME 
#define ARRAY_TYPENAME_SET
#define ARRAY_TYPENAME COLN_CAT(COLN_DATA_TYPENAME, _array)
#endif

#ifdef COLN_DATA_EQUALS
  #define ARRAY_LINEAR_SEARCH_FNNAME COLN_CAT(ARRAY_TYPENAME, _linear_search)
  #define ARRAY_LINEAR_SEARCH_SIGN \
    ptrdiff_t ARRAY_LINEAR_SEARCH_FNNAME(COLN_DATA_TYPENAME *array, \
                                         size_t array_len, \
                                         COLN_DATA_ARG(to_find))
  #define ARRAY_LINEAR_SEARCH_CALL(array, array_len, to_find) \
    ARRAY_LINEAR_SEARCH_FNNAME((array), (array_len), (to_find))
  #define ARRAY_LINEAR_SEARCH_DECL ARRAY_LINEAR_SEARCH_SIGN;
  #define ARRAY_LINEAR_SEARCH_DEFN \
    ARRAY_LINEAR_SEARCH_SIGN \
    { \
      assert(array_len > 0 ? array != NULL : true); \
      COLN_DATA_ASSERT_ARG(to_find); \
      for(ptrdiff_t i = 0; i < (ptrdiff_t)array_len; i++) \
        if(COLN_DATA_EQUALS_PTR_TO_ARG_WRAPPER(array + i, to_find)) return i; \
      return -1; \
    }
#else
  #define ARRAY_LINEAR_SEARCH_FNNAME
  #define ARRAY_LINEAR_SEARCH_SIGN
  #define ARRAY_LINEAR_SEARCH_CALL(array, array_len, to_find)
  #define ARRAY_LINEAR_SEARCH_DECL
  #define ARRAY_LINEAR_SEARCH_DEFN
#endif

#ifdef COLN_DATA_COMPARE
  #define ARRAY_BINARY_SEARCH_FNNAME COLN_CAT(ARRAY_TYPENAME, _binary_search)
  #define ARRAY_BINARY_SEARCH_SIGN \
    ptrdiff_t ARRAY_BINARY_SEARCH_FNNAME(COLN_DATA_TYPENAME *array, \
                                         size_t array_len, \
                                         COLN_DATA_ARG(to_find))
  #define ARRAY_BINARY_SEARCH_CALL(array, array_len, to_find) \
    ARRAY_BINARY_SEARCH_FNNAME((array), (array_len), (to_find))
  #define ARRAY_BINARY_SEARCH_DECL ARRAY_BINARY_SEARCH_SIGN;
  #define ARRAY_BINARY_SEARCH_DEFN \
    ARRAY_BINARY_SEARCH_SIGN \
    { \
      assert(array_len > 0 ? array != NULL : true); \
      COLN_DATA_ASSERT_ARG(to_find); \
      ARRAY__PRIV__ASSERT_SORTED_CALL(array, array_len); \
      ptrdiff_t left = 0; \
      ptrdiff_t right = array_len; \
      while(left < right) \
      { \
        ptrdiff_t mid = left + ((right - left) >> 1); \
        int cmp_res = COLN_DATA_COMPARE_PTR_TO_ARG_WRAPPER(array + mid, to_find); \
        if(cmp_res > 0) right = mid; \
        else if(cmp_res == 0) return mid; \
        else left = mid + 1; \
      } \
      return -1; \
    }

  #define ARRAY_QUICK_SORT_FNNAME COLN_CAT(ARRAY_TYPENAME, _quick_sort)
  #define ARRAY_QUICK_SORT_SIGN \
    void ARRAY_QUICK_SORT_FNNAME(COLN_DATA_TYPENAME *array, size_t array_len)
  #define ARRAY_QUICK_SORT_CALL(array, array_len) \
    ARRAY_QUICK_SORT_FNNAME((array), (array_len))
  #define ARRAY_QUICK_SORT_DECL ARRAY_QUICK_SORT_SIGN;
  #define ARRAY_QUICK_SORT_DEFN \
    ARRAY_QUICK_SORT_SIGN \
    { \
      if(array_len < 2) return; \
      ptrdiff_t pivot_idx = array_len >> 1; \
      if(pivot_idx != (ptrdiff_t)array_len - 1) \
        COLN_DATA_SWAP_BY_PTR(array + pivot_idx, array + array_len - 1); \
      pivot_idx = array_len - 1; \
      ptrdiff_t left_top = 0; \
      ptrdiff_t right_bot = pivot_idx; \
      while(left_top != right_bot) \
      { \
        int cmp_res = COLN_DATA_COMPARE_PTR_TO_PTR_WRAPPER(array + left_top, \
                                                           array + pivot_idx); \
        if(cmp_res > 0) \
        { \
          right_bot--; \
          COLN_DATA_SWAP_BY_PTR(array + left_top, array + right_bot); \
        } \
        else \
        { \
          left_top++; \
        } \
      } \
      if(pivot_idx != right_bot) \
      { \
        COLN_DATA_SWAP_BY_PTR(array + pivot_idx, array + right_bot); \
      } \
      right_bot++; \
      ARRAY_QUICK_SORT_CALL(array, left_top); \
      ARRAY_QUICK_SORT_CALL(array + right_bot, array_len - right_bot); \
    }
#else
  #define ARRAY_BINARY_SEARCH_FNNAME
  #define ARRAY_BINARY_SEARCH_SIGN
  #define ARRAY_BINARY_SEARCH_CALL(array, array_len, to_find)
  #define ARRAY_BINARY_SEARCH_DECL
  #define ARRAY_BINARY_SEARCH_DEFN

  #define ARRAY_QUICK_SORT_FNNAME
  #define ARRAY_QUICK_SORT_SIGN
  #define ARRAY_QUICK_SORT_CALL(array, array_len)
  #define ARRAY_QUICK_SORT_DECL
  #define ARRAY_QUICK_SORT_DEFN
#endif

#if defined(COLN_DATA_COMPARE) && !defined(NDEBUG)
  #define ARRAY__PRIV__ASSERT_SORTED_FNNAME COLN_CAT(ARRAY_TYPENAME, _assert_sorted)
  #define ARRAY__PRIV__ASSERT_SORTED_SIGN \
    static void ARRAY__PRIV__ASSERT_SORTED_FNNAME(COLN_DATA_TYPENAME *array, \
                                                  size_t array_len)
  #define ARRAY__PRIV__ASSERT_SORTED_CALL(array, array_len) \
    ARRAY__PRIV__ASSERT_SORTED_FNNAME((array), (array_len))
  #define ARRAY__PRIV__ASSERT_SORTED_DECL ARRAY__PRIV__ASSERT_SORTED_SIGN;
  #define ARRAY__PRIV__ASSERT_SORTED_DEFN \
    ARRAY__PRIV__ASSERT_SORTED_SIGN \
    { \
      assert(array_len > 0 ? array != NULL : true); \
      for(ptrdiff_t i = 1; i < (ptrdiff_t)array_len; i++) \
        assert(COLN_DATA_COMPARE(array + i - 1, array + i) < 0); \
    }
#else
  #define ARRAY__PRIV__ASSERT_SORTED_FNNAME
  #define ARRAY__PRIV__ASSERT_SORTED_SIGN
  #define ARRAY__PRIV__ASSERT_SORTED_CALL(array, array_len)
  #define ARRAY__PRIV__ASSERT_SORTED_DECL
  #define ARRAY__PRIV__ASSERT_SORTED_DEFN
#endif

#ifdef COLN_HEADER
ARRAY_LINEAR_SEARCH_DECL
ARRAY_BINARY_SEARCH_DECL
ARRAY_QUICK_SORT_DECL
#endif

#ifdef COLN_IMPL
ARRAY__PRIV__ASSERT_SORTED_DECL
ARRAY_LINEAR_SEARCH_DEFN
ARRAY_BINARY_SEARCH_DEFN
ARRAY_QUICK_SORT_DEFN
ARRAY__PRIV__ASSERT_SORTED_DEFN
#endif

#undef ARRAY__PRIV__ASSERT_SORTED_FNNAME
#undef ARRAY__PRIV__ASSERT_SORTED_SIGN
#undef ARRAY__PRIV__ASSERT_SORTED_CALL
#undef ARRAY__PRIV__ASSERT_SORTED_DECL
#undef ARRAY__PRIV__ASSERT_SORTED_DEFN

#undef ARRAY_QUICK_SORT_FNNAME
#undef ARRAY_QUICK_SORT_SIGN
#undef ARRAY_QUICK_SORT_CALL
#undef ARRAY_QUICK_SORT_DECL
#undef ARRAY_QUICK_SORT_DEFN

#undef ARRAY_BINARY_SEARCH_FNNAME
#undef ARRAY_BINARY_SEARCH_SIGN
#undef ARRAY_BINARY_SEARCH_CALL
#undef ARRAY_BINARY_SEARCH_DECL
#undef ARRAY_BINARY_SEARCH_DEFN

#undef ARRAY_LINEAR_SEARCH_FNNAME
#undef ARRAY_LINEAR_SEARCH_SIGN
#undef ARRAY_LINEAR_SEARCH_CALL
#undef ARRAY_LINEAR_SEARCH_DECL
#undef ARRAY_LINEAR_SEARCH_DEFN

#ifdef ARRAY_TYPENAME_SET
#undef ARRAY_TYPENAME_SET
#undef ARRAY_TYPENAME
#endif

#ifdef COLN_DATA_EQUALS_SET
#undef COLN_DATA_EQUALS_SET
#undef COLN_DATA_EQUALS
#endif

#undef COLN_DATA_SWAP_BY_PTR
#undef COLN_DATA_COMPARE_PTR_TO_PTR_WRAPPER
#undef COLN_DATA_COMPARE_PTR_TO_ARG_WRAPPER
#undef COLN_DATA_EQUALS_PTR_TO_ARG_WRAPPER
#undef COLN_DATA_ASSERT_ARG
#undef COLN_DATA_ARG

#undef COLN_CAT
#undef COLN_CAT_
