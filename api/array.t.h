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

#ifndef COLN_DATA_TYPE
#error "Collection macros require COLN_DATA_TYPE"
#endif

#ifndef COLN_TYPE
#define COLN_TYPE COLN_DATA_TYPE _array
#endif

#ifndef COLN_DATA_MOVE
#define COLN_DATA_MOVE(dest, src) (*(dest) = *(src))
#define COLN_DATA_MOVE_MANY(dest, src, len) memcpy((dest), (src), sizeof(COLN_DATA_TYPE) * (len))
#endif

#ifndef COLN_DATA_MOVE_MANY
#define COLN_DATA_MOVE_MANY(dest_ptr, src_ptr, count) \
    do \
    { \
        for(ptrdiff_t i = 0; i < (count); i++) \
        { \
            COLN_DATA_MOVE((dest_ptr) + i, (src_ptr) + i); \
        } \
    } while(0)
#endif

#ifndef COLN_DATA_SWAP
#define COLN_DATA_SWAP(a, b) \
    do \
    { \
        COLN_DATA_TYPE tmp; \
        COLN_DATA_MOVE(&tmp, (a)); \
        COLN_DATA_MOVE((a), (b)); \
        COLN_DATA_MOVE((b), &tmp); \
    } while(0)
#endif

#if !defined(COLN_DATA_EQUALS) && defined(COLN_DATA_COMPARE)
#define COLN_DATA_EQUALS(a, b) (COLN_DATA_COMPARE((a), (b)) == 0)
#endif

#ifdef COLN_DATA_EQUALS
#define ARRAY_LINEAR_SEARCH_DECL \
    ptrdiff_t COLN_CAT(COLN_TYPE, _linear_search)(COLN_DATA_TYPE *array, \
                                                  size_t array_len, \
                                                  COLN_DATA_TYPE *to_find)
#define ARRAY_LINEAR_SEARCH_DEFN \
    ARRAY_LINEAR_SEARCH_DECL \
    { \
        assert(array_len > 0 ? array != NULL : true); \
        assert(to_find); \
        for(ptrdiff_t i = 0; i < (ptrdiff_t)array_len; i++) \
            if(COLN_DATA_EQUALS(array + i, to_find)) return i; \
        return -1; \
    }
#else
#define ARRAY_LINEAR_SEARCH_DECL
#define ARRAY_LINEAR_SEARCH_DEFN
#endif

#ifdef COLN_DATA_COMPARE
#ifndef NDEBUG
#define ARRAY_ASSERT_SORTED COLN_CAT(COLN_TYPE, _assert_sorted)
#define ARRAY__PRIV__ASSERT_SORTED_DECL \
    static void ARRAY_ASSERT_SORTED(COLN_DATA_TYPE *array, \
                                    size_t array_len)
#define ARRAY__PRIV__ASSERT_SORTED_DEFN \
    ARRAY__PRIV__ASSERT_SORTED_DECL \
    { \
        assert(array_len > 0 ? array != NULL : true); \
        for(ptrdiff_t i = 1; i < (ptrdiff_t)array_len; i++) \
            assert(COLN_DATA_COMPARE(array + i - 1, array + i) < 0); \
    }
#else
#define ARRAY_ASSERT_SORTED(self)
#define ARRAY__PRIV__ASSERT_SORTED_DECL
#define ARRAY__PRIV__ASSERT_SORTED_DEFN
#endif
#define ARRAY_BINARY_SEARCH_DECL \
    ptrdiff_t COLN_CAT(COLN_TYPE, _binary_search)(COLN_DATA_TYPE *array, \
                                                  size_t array_len, \
                                                  COLN_DATA_TYPE *to_find)
#define ARRAY_BINARY_SEARCH_DEFN \
    ARRAY_BINARY_SEARCH_DECL \
    { \
        assert(array_len > 0 ? array != NULL : true); \
        assert(to_find); \
        ARRAY_ASSERT_SORTED(array, array_len); \
        ptrdiff_t left = 0; \
        ptrdiff_t right = array_len; \
        while(left < right) \
        { \
            ptrdiff_t mid = left + ((right - left) >> 1); \
            int cmp_res = COLN_DATA_COMPARE(to_find, array + mid); \
            if(cmp_res < 0) right = mid; \
            else if(cmp_res == 0) return mid; \
            else left = mid + 1; \
        } \
        return -1; \
    }
#define ARRAY_QUICK_SORT COLN_CAT(COLN_TYPE, _quick_sort)
#define ARRAY_QUICK_SORT_DECL \
    void ARRAY_QUICK_SORT(COLN_DATA_TYPE *array, size_t array_len)
#define ARRAY_QUICK_SORT_DEFN \
    ARRAY_QUICK_SORT_DECL \
    { \
        if(array_len < 2) return; \
        ptrdiff_t pivot_idx = array_len >> 1; \
        if(pivot_idx != (ptrdiff_t)array_len - 1) \
            COLN_DATA_SWAP(array + pivot_idx, array + array_len - 1); \
        pivot_idx = array_len - 1; \
        ptrdiff_t left_top = 0; \
        ptrdiff_t right_bot = pivot_idx; \
        while(left_top != right_bot) \
        { \
            int cmp_res = COLN_DATA_COMPARE(array + left_top, \
                                            array + pivot_idx); \
            if(cmp_res > 0) \
            { \
                right_bot--; \
                COLN_DATA_SWAP(array + left_top, array + right_bot); \
            } \
            else \
            { \
                left_top++; \
            } \
        } \
        if(pivot_idx != right_bot) \
        { \
            COLN_DATA_SWAP(array + pivot_idx, array + right_bot); \
        } \
        right_bot++; \
        ARRAY_QUICK_SORT(array, left_top); \
        ARRAY_QUICK_SORT(array + right_bot, array_len - right_bot); \
    }
#else
#define ARRAY_ASSERT_SORTED
#define ARRAY__PRIV__ASSERT_SORTED_DECL
#define ARRAY__PRIV__ASSERT_SORTED_DEFN
#define ARRAY_BINARY_SEARCH_DECL
#define ARRAY_BINARY_SEARCH_DEFN
#define ARRAY_QUICK_SORT
#define ARRAY_QUICK_SORT_DECL
#define ARRAY_QUICK_SORT_DEFN
#endif

#ifdef COLN_HEADER
ARRAY_LINEAR_SEARCH_DECL;
ARRAY_BINARY_SEARCH_DECL;
ARRAY_QUICK_SORT_DECL;
#endif

#ifdef COLN_IMPL
ARRAY__PRIV__ASSERT_SORTED_DECL;
ARRAY_LINEAR_SEARCH_DEFN
ARRAY_BINARY_SEARCH_DEFN
ARRAY_QUICK_SORT_DEFN
ARRAY__PRIV__ASSERT_SORTED_DEFN
#endif

#undef ARRAY_QUICK_SORT_DEFN
#undef ARRAY_QUICK_SORT_DECL
#undef ARRAY_QUICK_SORT
#undef ARRAY_BINARY_SEARCH_DEFN
#undef ARRAY_BINARY_SEARCH_DECL
#undef ARRAY__PRIV__ASSERT_SORTED_DEFN
#undef ARRAY__PRIV__ASSERT_SORTED_DECL
#undef ARRAY_ASSERT_SORTED
#undef ARRAY_LINEAR_SEARCH_DEFN
#undef ARRAY_LINEAR_SEARCH_DECL
#undef COLN_DATA_EQUALS
#undef COLN_DATA_COMPARE
#undef COLN_DATA_SWAP
#undef COLN_DATA_MOVE_MANY
#undef COLN_DATA_MOVE
#undef COLN_TYPE
#undef COLN_DATA_TYPE
#undef COLN_IMPL
#undef COLN_HEADER
#undef COLN_CAT_
#undef COLN_CAT
