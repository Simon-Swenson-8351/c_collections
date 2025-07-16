#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define COLN_CAT(x, y) COLN_CAT_(x, y)
#define COLN_CAT_(x, y) x ## y

#if !defined(ARRAY_HEADER) && !defined(ARRAY_IMPL)
  #error "ARRAY_HEADER or ARRAY_IMPL must be defined"
#endif

#ifndef DATA_TYPENAME
  #error "Collection macros require DATA_TYPENAME"
#endif

#if !defined(DATA_PASS_BY_VAL) && !defined(DATA_PASS_BY_PTR)
  #error "Define either DATA_PASS_BY_VAL or DATA_PASS_BY_PTR"
#endif

#if defined(DATA_PASS_BY_VAL) && defined(DATA_PASS_BY_PTR)
  #error "Define only one of DATA_PASS_BY_VAL or DATA_PASS_BY_PTR"
#endif

#ifdef DATA_PASS_BY_VAL
  #define DATA_ARG(arg_name) DATA_TYPENAME arg_name
  #define DATA_ASSERT_ARG(arg_name)
  #define DATA_EQUALS_PTR_ARG(ptr, arg_val) \
    DATA_EQUALS(*(ptr), (arg_val))
  #define DATA_COMPARE_PTR_ARG(ptr, arg_val) \
    DATA_COMPARE(*(ptr), (arg_val))
  #define DATA_COMPARE_PTR_PTR(a, b) \
    DATA_COMPARE(*(a), *(b))
#else 
  #define DATA_ARG(arg_name) DATA_TYPENAME *arg_name
  #define DATA_ASSERT_ARG(arg_name) assert(arg_name)
  #define DATA_EQUALS_PTR_ARG(ptr, arg_ptr) \
    DATA_EQUALS((ptr), (arg_ptr))
  #define DATA_COMPARE_PTR_ARG(ptr, arg_ptr) \
    DATA_COMPARE((ptr), (arg_ptr))
  #define DATA_COMPARE_PTR_PTR(a, b) \
    DATA_COMPARE((a), (b))
#endif

#ifdef DATA_MOVE
  #ifdef DATA_PASS_BY_VAL
    #define DATA_SWAP_PTR_PTR(a, b) \
      do \
      { \
        DATA_TYPENAME swaptmp; \
        swaptmp = DATA_MOVE(*(a)); \
        *(a) = DATA_MOVE(*(b)); \
        *(b) = DATA_MOVE(swaptmp); \
      } while(0)
  #else 
    #define DATA_SWAP_PTR_PTR(a, b) \
      do \
      { \
        DATA_TYPENAME swaptmp; \
        DATA_MOVE(&swaptmp, (a)); \
        DATA_MOVE((a), (b)); \
        DATA_MOVE((b), &swaptmp); \
      } while(0)
  #endif
#else
  #define DATA_SWAP_PTR_PTR(a, b) \
    do \
    { \
      DATA_TYPENAME swaptmp; \
      swaptmp = *(a); \
      *(a) = *(b); \
      *(b) = swaptmp; \
    } while(0)
#endif

#if !defined(DATA_EQUALS) && defined(DATA_COMPARE)
#define DATA_EQUALS_SET
#define DATA_EQUALS(a, b) (DATA_COMPARE((a), (b)) == 0)
#endif

#ifndef ARRAY_TYPENAME 
#define ARRAY_TYPENAME_SET
#define ARRAY_TYPENAME COLN_CAT(DATA_TYPENAME, _array)
#endif

#ifdef DATA_EQUALS
  #define ARRAY_LINEAR_SEARCH_FNNAME COLN_CAT(ARRAY_TYPENAME, _linear_search)
  #define ARRAY_LINEAR_SEARCH_SIGN \
    ptrdiff_t ARRAY_LINEAR_SEARCH_FNNAME(DATA_TYPENAME *array, \
                                         size_t array_len, \
                                         DATA_ARG(to_find))
  #define ARRAY_LINEAR_SEARCH_CALL(array, array_len, to_find) \
    ARRAY_LINEAR_SEARCH_FNNAME((array), (array_len), (to_find))
  #define ARRAY_LINEAR_SEARCH_DECL ARRAY_LINEAR_SEARCH_SIGN;
  #define ARRAY_LINEAR_SEARCH_DEFN \
    ARRAY_LINEAR_SEARCH_SIGN \
    { \
      assert(array_len > 0 ? array != NULL : true); \
      DATA_ASSERT_ARG(to_find); \
      for(ptrdiff_t i = 0; i < (ptrdiff_t)array_len; i++) \
        if(DATA_EQUALS_PTR_ARG(array + i, to_find)) return i; \
      return -1; \
    }
#else
  #define ARRAY_LINEAR_SEARCH_FNNAME
  #define ARRAY_LINEAR_SEARCH_SIGN
  #define ARRAY_LINEAR_SEARCH_CALL(array, array_len, to_find)
  #define ARRAY_LINEAR_SEARCH_DECL
  #define ARRAY_LINEAR_SEARCH_DEFN
#endif

#ifdef DATA_COMPARE
  #define ARRAY_BINARY_SEARCH_FNNAME COLN_CAT(ARRAY_TYPENAME, _binary_search)
  #define ARRAY_BINARY_SEARCH_SIGN \
    ptrdiff_t ARRAY_BINARY_SEARCH_FNNAME(DATA_TYPENAME *array, \
                                         size_t array_len, \
                                         DATA_ARG(to_find))
  #define ARRAY_BINARY_SEARCH_CALL(array, array_len, to_find) \
    ARRAY_BINARY_SEARCH_FNNAME((array), (array_len), (to_find))
  #define ARRAY_BINARY_SEARCH_DECL ARRAY_BINARY_SEARCH_SIGN;
  #define ARRAY_BINARY_SEARCH_DEFN \
    ARRAY_BINARY_SEARCH_SIGN \
    { \
      assert(array_len > 0 ? array != NULL : true); \
      DATA_ASSERT_ARG(to_find); \
      ARRAY_ASSERT_SORTED_CALL(array, array_len); \
      ptrdiff_t left = 0; \
      ptrdiff_t right = array_len; \
      while(left < right) \
      { \
        ptrdiff_t mid = left + ((right - left) >> 1); \
        int cmp_res = DATA_COMPARE_PTR_ARG(array + mid, to_find); \
        if(cmp_res > 0) right = mid; \
        else if(cmp_res == 0) return mid; \
        else left = mid + 1; \
      } \
      return -1; \
    }

  #define ARRAY_QUICK_SORT_FNNAME COLN_CAT(ARRAY_TYPENAME, _quick_sort)
  #define ARRAY_QUICK_SORT_SIGN \
    void ARRAY_QUICK_SORT_FNNAME(DATA_TYPENAME *array, size_t array_len)
  #define ARRAY_QUICK_SORT_CALL(array, array_len) \
    ARRAY_QUICK_SORT_FNNAME((array), (array_len))
  #define ARRAY_QUICK_SORT_DECL ARRAY_QUICK_SORT_SIGN;
  #define ARRAY_QUICK_SORT_DEFN \
    ARRAY_QUICK_SORT_SIGN \
    { \
      if(array_len < 2) return; \
      ptrdiff_t pivot_idx = array_len >> 1; \
      if(pivot_idx != (ptrdiff_t)array_len - 1) \
        DATA_SWAP_PTR_PTR(array + pivot_idx, array + array_len - 1); \
      pivot_idx = array_len - 1; \
      ptrdiff_t left_top = 0; \
      ptrdiff_t right_bot = pivot_idx; \
      while(left_top != right_bot) \
      { \
        int cmp_res = DATA_COMPARE_PTR_PTR(array + left_top, \
                                                           array + pivot_idx); \
        if(cmp_res > 0) \
        { \
          right_bot--; \
          DATA_SWAP_PTR_PTR(array + left_top, array + right_bot); \
        } \
        else \
        { \
          left_top++; \
        } \
      } \
      if(pivot_idx != right_bot) \
      { \
        DATA_SWAP_PTR_PTR(array + pivot_idx, array + right_bot); \
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

#if defined(DATA_COMPARE) && !defined(NDEBUG)
  #define ARRAY_ASSERT_SORTED_FNNAME COLN_CAT(ARRAY_TYPENAME, _assert_sorted)
  #define ARRAY_ASSERT_SORTED_SIGN \
    static void ARRAY_ASSERT_SORTED_FNNAME(DATA_TYPENAME *array, \
                                                  size_t array_len)
  #define ARRAY_ASSERT_SORTED_CALL(array, array_len) \
    ARRAY_ASSERT_SORTED_FNNAME((array), (array_len))
  #define ARRAY_ASSERT_SORTED_DECL ARRAY_ASSERT_SORTED_SIGN;
  #define ARRAY_ASSERT_SORTED_DEFN \
    ARRAY_ASSERT_SORTED_SIGN \
    { \
      assert(array_len > 0 ? array != NULL : true); \
      for(ptrdiff_t i = 1; i < (ptrdiff_t)array_len; i++) \
        assert(DATA_COMPARE_PTR_PTR(array + i - 1, array + i) < 0); \
    }
#else
  #define ARRAY_ASSERT_SORTED_FNNAME
  #define ARRAY_ASSERT_SORTED_SIGN
  #define ARRAY_ASSERT_SORTED_CALL(array, array_len)
  #define ARRAY_ASSERT_SORTED_DECL
  #define ARRAY_ASSERT_SORTED_DEFN
#endif

#ifdef ARRAY_HEADER
ARRAY_LINEAR_SEARCH_DECL
ARRAY_BINARY_SEARCH_DECL
ARRAY_QUICK_SORT_DECL
#endif

#ifdef ARRAY_IMPL
ARRAY_ASSERT_SORTED_DECL
ARRAY_LINEAR_SEARCH_DEFN
ARRAY_BINARY_SEARCH_DEFN
ARRAY_QUICK_SORT_DEFN
ARRAY_ASSERT_SORTED_DEFN
#endif

#undef ARRAY_ASSERT_SORTED_FNNAME
#undef ARRAY_ASSERT_SORTED_SIGN
#undef ARRAY_ASSERT_SORTED_CALL
#undef ARRAY_ASSERT_SORTED_DECL
#undef ARRAY_ASSERT_SORTED_DEFN

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

#ifdef DATA_EQUALS_SET
#undef DATA_EQUALS_SET
#undef DATA_EQUALS
#endif

#undef DATA_SWAP_PTR_PTR
#undef DATA_COMPARE_PTR_PTR
#undef DATA_COMPARE_PTR_ARG
#undef DATA_EQUALS_PTR_ARG
#undef DATA_ASSERT_ARG
#undef DATA_ARG

#undef COLN_CAT
#undef COLN_CAT_
