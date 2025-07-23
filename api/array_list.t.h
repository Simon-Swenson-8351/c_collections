#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "coln_result.h"

#define COLN_CAT_(a, b) a ## b
#define COLN_CAT(a, b) COLN_CAT_(a, b)

#ifndef COLN_INTERNAL_NDEBUG
  #define COLN_INTERNAL_ASSERT(x) assert(x)
#else
  #define COLN_INTERNAL_ASSERT(x)
#endif

#if !defined(ARRAY_LIST_HEADER) && !defined(ARRAY_LIST_IMPL)
  #error "ARRAY_LIST_HEADER or ARRAY_LIST_IMPL must be defined"
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
  #define DATA_RESULT_RETURN_TYPE DATA_TYPENAME
  #define DATA_RESULT_ARG(result)
  #ifdef DATA_MOVE
    #define DATA_MOVE_PTR_FROM_ARG(ptr, arg) (*(ptr) = DATA_MOVE((arg)))
    #define DATA_MOVE_RESULT_FROM_PTR(result, ptr) \
      ((result) = DATA_MOVE(*(ptr)))
  #else
    #define DATA_MOVE_PTR_FROM_ARG(ptr, arg) (*(ptr) = (arg))
    #define DATA_MOVE_RESULT_FROM_PTR(result, ptr) \
      ((result) = *(ptr))
  #endif
  // We don't provide the simple versions of DATA_COPY_PTR_FROM_PTR or 
  // DATA_CLEAR_PTR because they are only used in the complicated version of 
  // ARRAY_LIST_COPY
  #define DATA_COPY_PTR_FROM_PTR(dest_ptr, src_ptr) \
    DATA_COPY((dest_ptr), *(src_ptr))
  #define DATA_CLEAR_PTR(ptr) DATA_CLEAR(*(ptr))
#elif defined(DATA_PASS_BY_PTR)
  #define DATA_ARG(arg_name) DATA_TYPENAME *arg_name
  #define DATA_RESULT_RETURN_TYPE void
  #define DATA_RESULT_ARG(result) , DATA_TYPENAME *result
  #ifdef DATA_MOVE
    #define DATA_MOVE_PTR_FROM_ARG(ptr, arg) DATA_MOVE((ptr), (arg))
    #define DATA_MOVE_RESULT_FROM_PTR(result, ptr) \
      DATA_MOVE((result), (ptr))
  #else
    #define DATA_MOVE_PTR_FROM_ARG(ptr, arg) (*(ptr) = *(arg))
    #define DATA_MOVE_RESULT_FROM_PTR(result, ptr) \
      (*(result) = *(ptr))
  #endif
  #define DATA_COPY_PTR_FROM_PTR(dest_ptr, src_ptr) \
    DATA_COPY((dest_ptr), (src_ptr))
  #define DATA_CLEAR_PTR(ptr) DATA_CLEAR(*(ptr))
#endif

#ifdef DATA_MOVE
  #if defined(DATA_PASS_BY_VAL)
    #define DATA_MOVE_PTR_FROM_PTR(ptr1, ptr2) \
      DATA_MOVE(*(ptr1), *(ptr2))
    #define DATA_MOVE_MANY(dest_base, src_base, len) \
      do \
      { \
        for(size_t i = 0; i < (len); i++) \
        { \
          (dest_base)[i] = DATA_MOVE((src_base)[i]); \
        } \
      } while(0)
  #elif defined(DATA_PASS_BY_PTR)
    #define DATA_MOVE_PTR_FROM_PTR(ptr1, ptr2) \
      DATA_MOVE((ptr1), (ptr2))
    #define DATA_MOVE_MANY(dest_base, src_base, len) \
      do \
      { \
        for(size_t i = 0; i < (len); i++) \
        { \
          DATA_MOVE((dest_base) + i, (src_base) + i); \
        } \
      } while(0)
  #endif
#else
  #define DATA_MOVE_PTR_FROM_PTR(ptr1, ptr2) (*(ptr1) = *(ptr2))
  #define DATA_MOVE_MANY(dest_base, src_base, len) \
    memcpy((dest_base), (src_base), sizeof(DATA_TYPENAME) * (len))
#endif

#ifdef DATA_CLEAR
  #if defined(DATA_PASS_BY_VAL)
    #define DATA_CLEAR_MANY(base, len) \
      do \
      { \
        for(size_t i = 0; i < (len); i++) \
        { \
          DATA_CLEAR((base)[i]); \
        } \
      } \
      while(0)
  #elif defined(DATA_PASS_BY_PTR) 
    #define DATA_CLEAR_MANY(base, len) \
      do \
      { \
        for(size_t i = 0; i < (len); i++) \
        { \
          DATA_CLEAR((base) + i); \
        } \
      } \
      while(0)
  #endif
#else
  #define DATA_CLEAR_MANY(base, len)
#endif

#ifdef ALLOC_TYPENAME
  #define ALLOC_ARG(allocator) ALLOC_TYPENAME *allocator,
  #ifndef ALLOC_ALLOC
    #error "Collection macros require an allocation function if an allocator " \
      "type is defined."
  #endif
  #ifndef ALLOC_FREE
    #define ALLOC_FREE_SET
    #define ALLOC_FREE(allocator, ptr_to_free)
  #endif
#else
  #define ALLOC_ARG(allocator)
  #define ALLOC_ALLOC_SET
  #define ALLOC_ALLOC(allocator, size_to_alloc) malloc((size_to_alloc))
  #define ALLOC_FREE_SET
  #define ALLOC_FREE(allocator, ptr_to_free) free((ptr_to_free))
  #define ALLOC_REALLOC_SET
  #define ALLOC_REALLOC(allocator, ptr_to_realloc, new_size) \
    realloc((ptr_to_realloc), (new_size))
#endif

#ifndef ARRAY_LIST_TYPENAME
  #define ARRAY_LIST_TYPENAME_SET
  #define ARRAY_LIST_TYPENAME COLN_CAT(DATA_TYPENAME, _array_list)
#endif

#ifndef ARRAY_LIST_NEW_CAP
  #define ARRAY_LIST_NEW_CAP_SET
  #define ARRAY_LIST_NEW_CAP(old_cap) ((old_cap) << 1)
#endif

#define ARRAY_LIST_INIT_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _init)
#define ARRAY_LIST_INIT_SIGN \
  coln_result ARRAY_LIST_INIT_FNNAME(ARRAY_LIST_TYPENAME *to_init, \
                                     ALLOC_ARG(allocator) \
                                     size_t initial_cap)

#ifndef DATA_NO_COPY
  #define ARRAY_LIST_COPY_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _copy)
  #define ARRAY_LIST_COPY_SIGN \
    coln_result ARRAY_LIST_COPY_FNNAME(ARRAY_LIST_TYPENAME *dest, \
                                       ARRAY_LIST_TYPENAME *src)
#endif

#if defined(DATA_CLEAR) || !defined(ALLOC_TYPENAME) || !defined(ALLOC_FREE_SET)
  // Only emit a clear function if it does something
  #define ARRAY_LIST_CLEAR_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _clear)
  #define ARRAY_LIST_CLEAR_SIGN \
    void ARRAY_LIST_CLEAR_FNNAME(ARRAY_LIST_TYPENAME *to_clear)
#endif

#define ARRAY_LIST_PUSH_BACK_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _push_back)
#define ARRAY_LIST_PUSH_BACK_SIGN \
  coln_result ARRAY_LIST_PUSH_BACK_FNNAME(ARRAY_LIST_TYPENAME *array_list, \
                                          DATA_ARG(to_insert))

#define ARRAY_LIST_INSERT_AT_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _insert_at)
#define ARRAY_LIST_INSERT_AT_SIGN \
  coln_result ARRAY_LIST_INSERT_AT_FNNAME(ARRAY_LIST_TYPENAME *array_list, \
                                          DATA_ARG(to_insert), \
                                          ptrdiff_t index)

#define ARRAY_LIST_POP_BACK_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _pop_back)
#define ARRAY_LIST_POP_BACK_SIGN \
  DATA_RESULT_RETURN_TYPE ARRAY_LIST_POP_BACK_FNNAME( \
    ARRAY_LIST_TYPENAME *array_list \
    DATA_RESULT_ARG(popped))

#define ARRAY_LIST_REMOVE_AT_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _remove_at)
#define ARRAY_LIST_REMOVE_AT_SIGN \
  DATA_RESULT_RETURN_TYPE ARRAY_LIST_REMOVE_AT_FNNAME( \
    ARRAY_LIST_TYPENAME *array_list, \
    size_t index \
    DATA_RESULT_ARG(removed))

#define ARRAY_LIST_CAT_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _cat)
#define ARRAY_LIST_CAT_SIGN \
    coln_result ARRAY_LIST_CAT_FNNAME(ARRAY_LIST_TYPENAME *first, \
                                      ARRAY_LIST_TYPENAME *second)

#define ARRAY_LIST_EXPAND_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _expand)
#define ARRAY_LIST_EXPAND_SIGN \
  static coln_result ARRAY_LIST_EXPAND_FNNAME( \
    ARRAY_LIST_TYPENAME *to_expand)

#ifdef ARRAY_LIST_HEADER

    typedef struct ARRAY_LIST_TYPENAME
    {
    #ifdef ALLOC_TYPENAME
      ALLOC_TYPENAME *allocator;
    #endif
      DATA_TYPENAME *data;
      size_t len;
      size_t cap;
    } ARRAY_LIST_TYPENAME;

    ARRAY_LIST_INIT_SIGN;

  #ifndef DATA_NO_COPY
    ARRAY_LIST_COPY_SIGN;
  #endif

  #if defined(DATA_CLEAR) || !defined(ALLOC_TYPENAME) || !defined(ALLOC_FREE_SET)
    ARRAY_LIST_CLEAR_SIGN;
  #endif

    ARRAY_LIST_PUSH_BACK_SIGN;
    ARRAY_LIST_INSERT_AT_SIGN;
    ARRAY_LIST_POP_BACK_SIGN;
    ARRAY_LIST_REMOVE_AT_SIGN;
    ARRAY_LIST_CAT_SIGN;

#endif

#ifdef ARRAY_LIST_IMPL

    ARRAY_LIST_EXPAND_SIGN;

    ARRAY_LIST_INIT_SIGN
    {
      assert(to_init);
    #ifdef ALLOC_TYPENAME
      assert(allocator);
    #endif
      assert(initial_cap > 0);
      to_init->data = ALLOC_ALLOC(allocator,
                                  sizeof(DATA_TYPENAME) * initial_cap);
      if(!to_init->data) return COLN_RESULT_ALLOC_FAILED;
    #ifdef ALLOC_TYPENAME
      to_init->allocator = allocator;
    #endif
      to_init->len = 0;
      to_init->cap = initial_cap;
      return COLN_RESULT_SUCCESS;
    }

  #ifndef DATA_NO_COPY
    ARRAY_LIST_COPY_SIGN
    {
      assert(dest);
      assert(src);
      dest->data = ALLOC_ALLOC(src->allocator,
                               sizeof(DATA_TYPENAME) * src->cap);
      if(!dest->data) return COLN_RESULT_ALLOC_FAILED;
    #ifdef DATA_COPY
      for(size_t i = 0; i < src->len; i++)
      {
        if(!DATA_COPY_PTR_FROM_PTR(dest->data + i, src->data + i))
        {
        #ifdef DATA_CLEAR
          for(ptrdiff_t j = (ptrdiff_t)(i - 1); j >= 0; j--)
          {
            DATA_CLEAR_PTR(dest->data + j);
          }
        #endif
          ALLOC_FREE(src->allocator, dest->data);
          return COLN_RESULT_COPY_ELEM_FAILED;
        }
      }
    #else
      memcpy(dest->data, src->data, sizeof(DATA_TYPENAME) * src->len);
    #endif
    #ifdef ALLOC_TYPENAME
      dest->allocator = src->allocator;
    #endif
      dest->len = src->len;
      dest->cap = src->cap;
      return COLN_RESULT_SUCCESS;
    }
  #endif

  #if defined(DATA_CLEAR) || !defined(ALLOC_TYPENAME) || !defined(ALLOC_FREE_SET)
    ARRAY_LIST_CLEAR_SIGN
    {
      DATA_CLEAR_MANY(to_clear->data, to_clear->len);
      ALLOC_FREE(to_clear->allocator, to_clear->data);
    }
  #endif

    ARRAY_LIST_PUSH_BACK_SIGN
    {
      assert(array_list);
    #ifdef DATA_PASS_BY_PTR
      assert(to_insert);
    #endif
      coln_result result;
      if(array_list->len == array_list->cap &&
          (result = ARRAY_LIST_EXPAND_FNNAME(array_list)))
        return result;
      DATA_MOVE_PTR_FROM_ARG(array_list->data + array_list->len, to_insert);
      array_list->len++;
      return COLN_RESULT_SUCCESS;
    }

    ARRAY_LIST_INSERT_AT_SIGN
    {
      assert(array_list);
    #ifdef DATA_PASS_BY_PTR
      assert(to_insert);
    #endif
      assert(index >= 0 && index <= (ptrdiff_t)array_list->len);
      coln_result result;
      if(array_list->len == array_list->cap &&
          (result = ARRAY_LIST_EXPAND_FNNAME(array_list)))
        return result;
      for(ptrdiff_t i = array_list->len; i > index; i--)
        DATA_MOVE_PTR_FROM_PTR(array_list->data + i,
                               array_list->data + (i - 1));
      DATA_MOVE_PTR_FROM_ARG(array_list->data + index, to_insert);
      array_list->len++;
      return COLN_RESULT_SUCCESS;
    }

    ARRAY_LIST_POP_BACK_SIGN
    {
      assert(array_list);
    #ifdef DATA_PASS_BY_PTR
      assert(popped);
    #endif
      assert(array_list->len > 0);
      array_list->len--;
    #ifdef DATA_PASS_BY_VAL
      DATA_TYPENAME popped;
    #endif
      DATA_MOVE_RESULT_FROM_PTR(popped, array_list->data + array_list->len);
    #ifdef DATA_PASS_BY_VAL
      return popped;
    #endif
    }

    ARRAY_LIST_REMOVE_AT_SIGN
    {
      assert(array_list);
      assert(index < array_list->len);
    #ifdef DATA_PASS_BY_PTR
      assert(removed);
    #endif
    #ifdef DATA_PASS_BY_VAL
      DATA_TYPENAME removed;
    #endif
      DATA_MOVE_RESULT_FROM_PTR(removed, array_list->data + index);
      for(size_t i = index; i + 1 < array_list->len; i++)
        DATA_MOVE_PTR_FROM_PTR(array_list->data + i, array_list->data + i + 1);
      array_list->len--;
    #ifdef DATA_PASS_BY_VAL
      return removed;
    #endif
    }

    ARRAY_LIST_CAT_SIGN
    {
      assert(first);
      assert(second);
      if(first->cap < first->len + second->len) {
        size_t new_cap = first->cap + second->cap;
        DATA_TYPENAME *new_buf = ALLOC_ALLOC(first->allocator,
                                             sizeof(DATA_TYPENAME) * new_cap);
        if(!new_buf) return COLN_RESULT_ALLOC_FAILED;
        DATA_MOVE_MANY(new_buf, first->data, first->len);
        ALLOC_FREE(first->allocator, first->data);
        first->data = new_buf;
        first->cap = new_cap;
      }
      DATA_MOVE_MANY(first->data + first->len, second->data, second->len);
      ALLOC_FREE(second->allocator, second->data);
      first->len += second->len;
      return COLN_RESULT_SUCCESS;
    }

    ARRAY_LIST_EXPAND_SIGN
    {
      COLN_INTERNAL_ASSERT(to_expand);
      size_t new_cap = ARRAY_LIST_NEW_CAP(to_expand->cap);
    #if defined(ALLOC_REALLOC) && !defined(DATA_MOVE)
      // We can just use realloc if available since it's a simple move
      DATA_TYPENAME *new_buf = ALLOC_REALLOC(to_expand->allocator,
                                             to_expand->data,
                                             sizeof(DATA_TYPENAME) * new_cap);
      if(!new_buf) return COLN_RESULT_ALLOC_FAILED;
    #else
      DATA_TYPENAME *new_buf = ALLOC_ALLOC(to_expand->allocator,
                                           sizeof(DATA_TYPENAME) * new_cap);
      if(!new_buf) return COLN_RESULT_ALLOC_FAILED;
      DATA_MOVE_MANY(new_buf, to_expand->data, to_expand->len);
      ALLOC_FREE(to_expand->allocator, to_expand->data);
    #endif
      to_expand->data = new_buf;
      to_expand->cap = new_cap;
      return COLN_RESULT_SUCCESS;
    }
#endif

#undef ARRAY_LIST_EXPAND_SIGN
#undef ARRAY_LIST_EXPAND_FNNAME
#undef ARRAY_LIST_CAT_SIGN
#undef ARRAY_LIST_CAT_FNNAME
#undef ARRAY_LIST_REMOVE_AT_SIGN
#undef ARRAY_LIST_REMOVE_AT_FNNAME
#undef ARRAY_LIST_POP_BACK_SIGN
#undef ARRAY_LIST_POP_BACK_FNNAME
#undef ARRAY_LIST_INSERT_AT_SIGN
#undef ARRAY_LIST_INSERT_AT_FNNAME
#undef ARRAY_LIST_PUSH_BACK_SIGN
#undef ARRAY_LIST_PUSH_BACK_FNNAME
#if defined(DATA_CLEAR) || !defined(ALLOC_TYPENAME) || !defined(ALLOC_FREE_SET)
  #undef ARRAY_LIST_CLEAR_SIGN
  #undef ARRAY_LIST_CLEAR_FNNAME
#endif
#ifndef DATA_NO_COPY
  #undef ARRAY_LIST_COPY_SIGN
  #undef ARRAY_LIST_COPY_FNNAME
#endif
#undef ARRAY_LIST_INIT_SIGN
#undef ARRAY_LIST_INIT_FNNAME
#undef ARRAY_LIST_STRUCT_DEFN

#ifdef ARRAY_LIST_TYPENAME_SET
  #undef ARRAY_LIST_TYPENAME_SET
  #undef ARRAY_LIST_TYPENAME
#endif

#ifdef ALLOC_REALLOC_SET
  #undef ALLOC_REALLOC_SET
  #undef ALLOC_REALLOC
#endif
#ifdef ALLOC_FREE_SET
  #undef ALLOC_FREE_SET
  #undef ALLOC_FREE
#endif
#ifdef ALLOC_ALLOC_SET
  #undef ALLOC_ALLOC_SET
  #undef ALLOC_ALLOC
#endif
#undef ALLOC_ARG

#undef DATA_CLEAR_MANY
#undef DATA_MOVE_MANY
#undef DATA_MOVE_PTR_FROM_PTR
#undef DATA_CLEAR_PTR
#undef DATA_COPY_PTR_FROM_PTR
#undef DATA_MOVE_RESULT_FROM_PTR
#undef DATA_MOVE_PTR_FROM_ARG
#undef DATA_RESULT_ARG
#undef DATA_RESULT_RETURN_TYPE
#undef DATA_ARG
#undef COLN_INTERNAL_ASSERT
#undef COLN_CAT
#undef COLN_CAT_
