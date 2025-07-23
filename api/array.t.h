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

#if defined(DATA_PASS_BY_VAL)
  #define DATA_ARG(arg_name) DATA_TYPENAME arg_name
  #define DATA_EQUALS_PTR_ARG(ptr, arg_val) DATA_EQUALS(*(ptr), (arg_val))
  #define DATA_COMPARE_PTR_ARG(ptr, arg_val) DATA_COMPARE(*(ptr), (arg_val))
  #define DATA_COMPARE_PTR_PTR(a, b) DATA_COMPARE(*(a), *(b))
  #define DATA_DIGIT_PTR(ptr, digit) DATA_DIGIT(*(ptr), (digit))
#elif defined(DATA_PASS_BY_PTR) 
  #define DATA_ARG(arg_name) DATA_TYPENAME *arg_name
  #define DATA_EQUALS_PTR_ARG(ptr, arg_ptr) DATA_EQUALS((ptr), (arg_ptr))
  #define DATA_COMPARE_PTR_ARG(ptr, arg_ptr) DATA_COMPARE((ptr), (arg_ptr))
  #define DATA_COMPARE_PTR_PTR(a, b) DATA_COMPARE((a), (b))
  #define DATA_DIGIT_PTR(ptr, digit) DATA_DIGIT((ptr), (digit))
#endif

#ifdef DATA_MOVE
  #if defined(DATA_PASS_BY_VAL)
    #define DATA_SWAP_PTR_PTR(a, b) \
      do \
      { \
        DATA_TYPENAME swaptmp; \
        swaptmp = DATA_MOVE(*(a));\
        *(a) = DATA_MOVE(*(b)); \
        *(b) = DATA_MOVE(swaptmp); \
      } while(0)
  #elif defined(DATA_PASS_BY_PTR) 
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
#endif

#ifdef DATA_COMPARE
  #define ARRAY_BINARY_SEARCH_FNNAME COLN_CAT(ARRAY_TYPENAME, _binary_search)
  #define ARRAY_BINARY_SEARCH_SIGN \
    ptrdiff_t ARRAY_BINARY_SEARCH_FNNAME(DATA_TYPENAME *array, \
                                         size_t array_len, \
                                         DATA_ARG(to_find))

  #define ARRAY_QUICK_SORT_FNNAME COLN_CAT(ARRAY_TYPENAME, _quick_sort)
  #define ARRAY_QUICK_SORT_SIGN \
    void ARRAY_QUICK_SORT_FNNAME(DATA_TYPENAME *array, size_t array_len)
#endif

#if defined(DATA_DIGIT_LEN) && defined(DATA_DIGIT)
  #define ARRAY_RADIX_SORT_RECURSIVE_FNNAME \
    COLN_CAT(ARRAY_TYPENAME, _radix_sort_recursive)
  #define ARRAY_RADIX_SORT_RECURSIVE_SIGN \
    void ARRAY_RADIX_SORT_RECURSIVE_FNNAME(DATA_TYPENAME *base, \
                                           size_t len, \
                                           int digit)

  #define ARRAY_RADIX_SORT_FNNAME COLN_CAT(ARRAY_TYPENAME, _radix_sort)
  #define ARRAY_RADIX_SORT_SIGN \
    void ARRAY_RADIX_SORT_FNNAME(DATA_TYPENAME *array, size_t array_len)
#endif

#if defined(DATA_COMPARE) && !defined(NDEBUG)
  #define ARRAY_ASSERT_SORTED_FNNAME COLN_CAT(ARRAY_TYPENAME, _assert_sorted)
  #define ARRAY_ASSERT_SORTED_SIGN \
    static void ARRAY_ASSERT_SORTED_FNNAME(DATA_TYPENAME *array, \
                                           size_t array_len)
#endif

#ifdef ARRAY_HEADER

  #ifdef DATA_EQUALS
    ARRAY_LINEAR_SEARCH_SIGN;
  #endif

  #ifdef DATA_COMPARE
    ARRAY_BINARY_SEARCH_SIGN;
    ARRAY_QUICK_SORT_SIGN;
  #endif

  #if defined(DATA_DIGIT_LEN) && defined(DATA_DIGIT)
    ARRAY_RADIX_SORT_SIGN;
  #endif

#endif

#ifdef ARRAY_IMPL
  
  #if defined(DATA_COMPARE) && !defined(NDEBUG)
    ARRAY_ASSERT_SORTED_SIGN;
  #endif

  #if defined(DATA_DIGIT_LEN) && defined(DATA_DIGIT)
    ARRAY_RADIX_SORT_RECURSIVE_SIGN;
  #endif

  #ifdef DATA_EQUALS
    ARRAY_LINEAR_SEARCH_SIGN
    {
      assert(array_len > 0 ? array != NULL : true);
    #ifdef DATA_PASS_BY_PTR
      assert(to_find);
    #endif
      for(ptrdiff_t i = 0; i < (ptrdiff_t)array_len; i++)
        if(DATA_EQUALS_PTR_ARG(array + i, to_find)) return i;
      return -1;
    }
  #endif

  #ifdef DATA_COMPARE
    ARRAY_BINARY_SEARCH_SIGN
    {
      assert(array_len > 0 ? array != NULL : true);
    #ifdef DATA_PASS_BY_PTR
      assert(to_find);
    #endif
      ARRAY_ASSERT_SORTED_FNNAME(array, array_len);
      ptrdiff_t left = 0;
      ptrdiff_t right = array_len;
      while(left < right)
      {
        ptrdiff_t mid = left + ((right - left) >> 1);
        int cmp_res = DATA_COMPARE_PTR_ARG(array + mid, to_find);
        if(cmp_res > 0) right = mid;
        else if(cmp_res == 0) return mid;
        else left = mid + 1;
      }
      return -1;
    }

    ARRAY_QUICK_SORT_SIGN
    {
      if(array_len < 2) return;
      ptrdiff_t pivot_idx = array_len >> 1;
      if(pivot_idx != (ptrdiff_t)array_len - 1)
        DATA_SWAP_PTR_PTR(array + pivot_idx, array + array_len - 1);
      pivot_idx = array_len - 1;
      ptrdiff_t left_top = 0;
      ptrdiff_t right_bot = pivot_idx;
      while(left_top != right_bot)
      {
        int cmp_res = DATA_COMPARE_PTR_PTR(array + left_top, array + pivot_idx);
        if(cmp_res > 0)
        {
          right_bot--;
          DATA_SWAP_PTR_PTR(array + left_top, array + right_bot);
        }
        else
        {
          left_top++;
        }
      }
      if(pivot_idx != right_bot)
        DATA_SWAP_PTR_PTR(array + pivot_idx, array + right_bot);
      right_bot++;
      ARRAY_QUICK_SORT_FNNAME(array, left_top);
      ARRAY_QUICK_SORT_FNNAME(array + right_bot, array_len - right_bot);
    }
  #endif

  #if defined(DATA_DIGIT_LEN) && defined(DATA_DIGIT)
    ARRAY_RADIX_SORT_RECURSIVE_SIGN
    {
      if(len < 2) return;
      if(digit < 0) return;
      size_t low = 0;
      size_t high = len;
      while(low < high)
      {
        if(DATA_DIGIT_PTR(base + low, (unsigned int)digit))
        {
          if(low < high - 1) DATA_SWAP_PTR_PTR(base + low, base + high - 1);
          high--;
        }
        else
        {
          low++;
        }
      }
      ARRAY_RADIX_SORT_RECURSIVE_FNNAME(base, low, digit - 1);
      ARRAY_RADIX_SORT_RECURSIVE_FNNAME(base + low, len - low, digit - 1);
    }

    ARRAY_RADIX_SORT_SIGN
    {
      ARRAY_RADIX_SORT_RECURSIVE_FNNAME(array, array_len, DATA_DIGIT_LEN - 1);
    }
  #endif

  #if defined(DATA_COMPARE) && !defined(NDEBUG)
    ARRAY_ASSERT_SORTED_SIGN
    {
      assert(array_len > 0 ? array != NULL : true);
      for(size_t i = 1; i < array_len; i++)
        assert(DATA_COMPARE_PTR_PTR(array + i - 1, array + i) < 0);
    }
  #endif
#endif

#undef ARRAY_ASSERT_SORTED_FNNAME
#undef ARRAY_ASSERT_SORTED_SIGN
#undef ARRAY_RADIX_SORT_RECURSIVE_FNNAME
#undef ARRAY_RADIX_SORT_RECURSIVE_SIGN
#undef ARRAY_RADIX_SORT_FNNAME
#undef ARRAY_RADIX_SORT_SIGN
#undef ARRAY_QUICK_SORT_FNNAME
#undef ARRAY_QUICK_SORT_SIGN
#undef ARRAY_BINARY_SEARCH_FNNAME
#undef ARRAY_BINARY_SEARCH_SIGN
#undef ARRAY_LINEAR_SEARCH_FNNAME
#undef ARRAY_LINEAR_SEARCH_SIGN

#ifdef ARRAY_TYPENAME_SET
#undef ARRAY_TYPENAME_SET
#undef ARRAY_TYPENAME
#endif

#ifdef DATA_EQUALS_SET
#undef DATA_EQUALS_SET
#undef DATA_EQUALS
#endif

#undef DATA_SWAP_PTR_PTR
#undef DATA_DIGIT_PTR
#undef DATA_COMPARE_PTR_PTR
#undef DATA_COMPARE_PTR_ARG
#undef DATA_EQUALS_PTR_ARG
#undef DATA_ARG

#undef COLN_CAT
#undef COLN_CAT_
