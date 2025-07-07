#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "coln_result.h"

#define COLN_CAT_(a, b) a ## b
#define COLN_CAT(a, b) COLN_CAT_(a, b)

#ifdef COLN_INTERNAL_DEBUG
  #define COLN_INTERNAL_ASSERT(x) assert(x)
#else
  #define COLN_INTERNAL_ASSERT(x)
#endif

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
  #ifdef COLN_DATA_CLEAR 
    #error "Data was designated trivial, but a clear function was provided"
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
  #ifdef COLN_DATA_CLEAR 
    #error "Data was designated trivial, but a clear function was provided"
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
  #ifndef COLN_DATA_CLEAR 
    #error "Data was designated trivial, but a clear function was not provided"
  #endif 
#endif

#ifdef COLN_DATA_TRIVIAL_BY_VAL
  #define COLN_DATA_ARG(arg_name) COLN_DATA_TYPENAME arg_name
  #define COLN_DATA_ASSERT_ARG(arg_name)
#else
  #define COLN_DATA_ARG(arg_name) COLN_DATA_TYPENAME arg_name
  #define COLN_DATA_ASSERT_ARG(arg_name) assert(arg_name)
#endif

#if defined(COLN_DATA_TRIVIAL_BY_VAL)
  #define COLN_DATA_MOVE_PTR_FROM_ARG_WRAPPER(ptr, arg) (*(ptr) = arg)
#elif defined(COLN_DATA_TRIVIAL_BY_PTR)
  #define COLN_DATA_MOVE_PTR_FROM_ARG_WRAPPER(ptr, arg) (*(ptr) = *(arg))
#elif defined(COLN_DATA_NONTRIVIAL_BY_PTR)
  #define COLN_DATA_MOVE_PTR_FROM_ARG_WRAPPER(ptr, arg) \
    COLN_DATA_MOVE((ptr), (arg))
#endif

#if !defined(COLN_DATA_NONTRIVIAL_BY_PTR)
  #define COLN_DATA_MOVE_PTR_FROM_PTR_WRAPPER(dest, src) (*(dest) = *(src))
  #define COLN_DATA_MOVE_MANY(dest_ptr, src_ptr, len) memcpy((dest_ptr), (src_ptr), (len))
  #define COLN_DATA_CLEAR_MANY(to_clear_ptr, len)
#else
  #define COLN_DATA_MOVE_PTR_FROM_PTR_WRAPPER(dest, src) COLN_DATA_MOVE(dest, src)
  #define COLN_DATA_MOVE_MANY(dest_ptr, src_ptr, len) \
    do \
    { \
      for(ptrdiff_t i = 0; i < (count); i++) \
      { \
        COLN_DATA_MOVE((dest_ptr) + i, (src_ptr) + i); \
      } \
    } while(0)
  #define COLN_DATA_CLEAR_MANY(to_clear_ptr, len) \
    do \
    { \
      for(ptrdiff_t i = 0; i < (ptrdiff_t)len; i++) \
      { \
        COLN_DATA_CLEAR((to_clear_ptr) + i); \
      } \
    } \
    while(0)
#endif

#if !defined(COLN_DATA_NONTRIVIAL_BY_PTR)
  #define ARRAY_LIST_COPY_DATA_SNIPPET memcpy(dest->data, src->data, sizeof(COLN_DATA_TYPENAME) * src->len);
#elif defined(COLN_DATA_COPY)
  #define ARRAY_LIST_COPY_DATA_SNIPPET \
    for(ptrdiff_t i = 0; i < src->len; i++) \
    { \
      if(!COLN_DATA_COPY(dest->data + i, src->data + i)) \
      { \
        for(ptrdiff_t j = i - 1; j >= 0; j--) \
        { \
          COLN_DATA_CLEAR(dest->data + j); \
        } \
        COLN_FREE(src->allocator, dest->data); \
        return COLN_RESULT_COPY_ELEM_FAILED; \
      } \
    }
#else
  #define ARRAY_LIST_COPY_DATA_SNIPPET
#endif

#ifdef COLN_ALLOC_TYPE
  #define COLN_ALLOC_DECL(allocator) COLN_ALLOC_TYPE *allocator;
  #define COLN_ALLOC_ARG(allocator) COLN_ALLOC_TYPE *allocator,
  #define COLN_ALLOC_ASSIGN(lval, rval) ((lval) = (rval))
  #define COLN_ALLOC_ASSERT(expr) assert((expr))
  #ifndef COLN_ALLOC
    #error "Collection macros require an allocation function if an allocator " \
      "type is defined."
  #endif
  #ifndef COLN_FREE
    #define COLN_FREE(allocator, ptr_to_free)
  #endif
#else
  #define COLN_ALLOC_DECL(allocator)
  #define COLN_ALLOC_ARG(allocator)
  #define COLN_ALLOC_ASSIGN(lval, rval)
  #define COLN_ALLOC_ASSERT(expr)
  #define COLN_ALLOC(allocator, size_to_alloc) malloc(size_to_alloc)
  #define COLN_FREE(allocator, ptr_to_free) free(ptr_to_free)
#endif

#ifndef ARRAY_LIST_TYPENAME
  #define ARRAY_LIST_TYPENAME_SET
  #define ARRAY_LIST_TYPENAME COLN_CAT(COLN_DATA_TYPENAME, _array_list)
#endif

#define ARRAY_LIST_STRUCT_DEFN \
  typedef struct ARRAY_LIST_TYPENAME \
  { \
    COLN_ALLOC_DECL(allocator) \
    COLN_DATA_TYPENAME *data; \
    size_t len; \
    size_t cap; \
  } ARRAY_LIST_TYPENAME;

#define ARRAY_LIST_INIT_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _init)
#define ARRAY_LIST_INIT_SIGN \
    coln_result ARRAY_LIST_INIT_FNNAME(ARRAY_LIST_TYPENAME *to_init, \
                                       COLN_ALLOC_ARG(allocator) \
                                       size_t initial_cap)
#define ARRAY_LIST_INIT_DECL ARRAY_LIST_INIT_SIGN;
#define ARRAY_LIST_INIT_DEFN \
    ARRAY_LIST_INIT_SIGN \
    { \
        assert(to_init); \
        COLN_ALLOC_ASSERT(allocator); \
        COLN_ALLOC_ASSIGN(to_init->allocator, allocator); \
        to_init->cap = initial_cap; \
        to_init->data = COLN_ALLOC( \
            allocator, \
            sizeof(COLN_DATA_TYPENAME) * to_init->cap); \
        if(!to_init->data) return COLN_RESULT_ALLOC_FAILED; \
        to_init->len = 0; \
        return COLN_RESULT_SUCCESS; \
    }

#if !defined(COLN_DATA_NONTRIVIAL_BY_PTR) || defined(COLN_DATA_COPY)
  #define ARRAY_LIST_COPY_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _copy)
  #define ARRAY_LIST_COPY_SIGN \
    coln_result ARRAY_LIST_COPY_FNNAME(ARRAY_LIST_TYPENAME *dest, \
                                       ARRAY_LIST_TYPENAME *src)
  #define ARRAY_LIST_COPY_CALL(dest, src) ARRAY_LIST_COPY_FNNAME((dest), (src))
  #define ARRAY_LIST_COPY_DECL ARRAY_LIST_COPY_SIGN;
  #define ARRAY_LIST_COPY_DEFN \
    ARRAY_LIST_COPY_SIGN \
    { \
      assert(dest); \
      assert(src); \
      dest->data = COLN_ALLOC(src->allocator, \
                              sizeof(COLN_DATA_TYPENAME) * src->cap); \
      if(!dest->data) return COLN_RESULT_ALLOC_FAILED; \
      ARRAY_LIST_COPY_DATA_SNIPPET \
      COLN_ALLOC_ASSIGN(dest->allocator, src->allocator); \
      dest->len = src->len; \
      dest->cap = src->cap; \
      return COLN_RESULT_SUCCESS; \
    }
#else
  #define ARRAY_LIST_COPY_FNNAME
  #define ARRAY_LIST_COPY_SIGN
  #define ARRAY_LIST_COPY_CALL(dest, src)
  #define ARRAY_LIST_COPY_DECL
  #define ARRAY_LIST_COPY_DEFN
#endif

#define ARRAY_LIST_CLEAR_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _clear)
#define ARRAY_LIST_CLEAR_SIGN \
  void ARRAY_LIST_CLEAR_FNNAME(ARRAY_LIST_TYPENAME *to_clear)
#define ARRAY_LIST_CLEAR_CALL(to_clear) ARRAY_LIST_CLEAR_FNNAME(to_clear)
#define ARRAY_LIST_CLEAR_DECL ARRAY_LIST_CLEAR_SIGN;
#define ARRAY_LIST_CLEAR_DEFN \
  ARRAY_LIST_CLEAR_SIGN \
  { \
    COLN_DATA_CLEAR_MANY(to_clear->data, to_clear->len); \
    COLN_FREE(to_clear->allocator, to_clear->data); \
  }

#define ARRAY_LIST_PUSH_BACK_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _push_back)
#define ARRAY_LIST_PUSH_BACK_SIGN \
  coln_result ARRAY_LIST_PUSH_BACK_FNNAME(ARRAY_LIST_TYPENAME *self, \
                                          COLN_DATA_ARG(to_insert))
#define ARRAY_LIST_PUSH_BACK_CALL(array_list_ptr, to_insert) \
  ARRAY_LIST_PUSH_BACK_FNNAME((array_list_ptr), (to_insert))
#define ARRAY_LIST_PUSH_BACK_DECL ARRAY_LIST_PUSH_BACK_SIGN;
#define ARRAY_LIST_PUSH_BACK_DEFN \
  ARRAY_LIST_PUSH_BACK_SIGN \
  { \
    assert(self); \
    COLN_DATA_ASSERT_ARG(to_insert); \
    coln_result result; \
    if(self->len == self->cap && (result = ARRAY_LIST__PRIV__EXPAND(self))) \
      return result; \
    COLN_DATA_MOVE_PTR_FROM_ARG_WRAPPER(self->data + self->len, to_insert); \
    self->len++; \
    return COLN_RESULT_SUCCESS; \
  }

#define ARRAY_LIST_INSERT_AT_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _insert_at)
#define ARRAY_LIST_INSERT_AT_SIGN \
  coln_result ARRAY_LIST_INSERT_AT_FNNAME(ARRAY_LIST_TYPENAME *self, \
                                          COLN_DATA_ARG(to_insert), \
                                          ptrdiff_t index)
#define ARRAY_LIST_INSERT_AT_DECL ARRAY_LIST_INSERT_AT_SIGN;
#define ARRAY_LIST_INSERT_AT_DEFN \
  ARRAY_LIST_INSERT_AT_SIGN \
  { \
    assert(self); \
    COLN_DATA_ASSERT_ARG(to_insert); \
    assert(index >= 0 && index <= (ptrdiff_t)self->len); \
    coln_result result; \
    if(self->len == self->cap && (result = ARRAY_LIST__PRIV__EXPAND(self))) \
      return result; \
    for(ptrdiff_t i = self->len; i > index; i--) \
      COLN_DATA_MOVE_PTR_FROM_PTR_WRAPPER(self->data + i, self->data + (i - 1)); \
    COLN_DATA_MOVE_PTR_FROM_ARG_WRAPPER(self->data + index, to_insert); \
    self->len++; \
    return COLN_RESULT_SUCCESS; \
  }

#define ARRAY_LIST_POP_BACK_FNNAME COLN_CAT(ARRAY_LIST_TYPENAME, _pop_back)
#if defined(COLN_DATA_TRIVIAL_BY_VAL)
  #define ARRAY_LIST_POP_BACK_SIGN \
    COLN_DATA_TYPENAME ARRAY_LIST_POP_BACK_FNNAME(ARRAY_LIST_TYPENAME *array_list)
  #define ARRAY_LIST_POP_BACK_CALL(array_list_ptr, popped) \
    ((popped) = ARRAY_LIST_POP_BACK_FNNAME((array_list_ptr)))
  #define ARRAY_LIST_POP_BACK_RESULT_WRAPPER(removed_ptr, popped) \
    return *(removed_ptr)
#elif defined(COLN_DATA_TRIVIAL_BY_PTR)
  #define ARRAY_LIST_POP_BACK_SIGN \
    void ARRAY_LIST_POP_BACK_FNNAME(ARRAY_LIST_TYPENAME *array_list, \
                                    COLN_DATA_TYPENAME *popped)
  #define ARRAY_LIST_POP_BACK_CALL(array_list_ptr, popped) \
    ARRAY_LIST_POP_BACK_FNNAME((array_list_ptr), (popped))
  #define ARRAY_LIST_POP_BACK_RESULT_WRAPPER(removed_ptr, popped) \
    (*(popped) = *(removed_ptr))
#else
  #define ARRAY_LIST_POP_BACK_SIGN \
    void ARRAY_LIST_POP_BACK_FNNAME(ARRAY_LIST_TYPENAME *array_list, \
                                    COLN_DATA_TYPENAME *popped)
  #define ARRAY_LIST_POP_BACK_CALL(array_list_ptr, popped) \
    ARRAY_LIST_POP_BACK_FNNAME((array_list_ptr), (popped))
  #define ARRAY_LIST_POP_BACK_RESULT_WRAPPER(removed_ptr, popped) \
    COLN_DATA_MOVE((popped), (removed_ptr))
#endif
#define ARRAY_LIST_POP_BACK_DECL ARRAY_LIST_POP_BACK_SIGN;
#define ARRAY_LIST_POP_BACK_DEFN \
  ARRAY_LIST_POP_BACK_SIGN \
  { \
    assert(array_list); \
    COLN_DATA_ASSERT_ARG(popped); \
    assert(array_list->len > 0); \
    array_list->len--; \
    ARRAY_LIST_POP_BACK_RESULT_WRAPPER(array_list->data + array_list->len, \
                                       popped); \
  }

#define ARRAY_LIST_REMOVE_AT_DECL \
    void COLN_CAT(ARRAY_LIST_TYPENAME, _remove_at)(ARRAY_LIST_TYPENAME *self, \
                                 COLN_DATA_TYPENAME *removed, \
                                 ptrdiff_t index)
#define ARRAY_LIST_REMOVE_AT_DEFN \
    ARRAY_LIST_REMOVE_AT_DECL \
    { \
        assert(self); \
        assert(removed); \
        assert(index < (ptrdiff_t)self->len); \
        COLN_DATA_MOVE(removed, self->data + index); \
        for(ptrdiff_t i = index; i + 1 < (ptrdiff_t)self->len; i++) \
            COLN_DATA_MOVE(self->data + i, self->data + i + 1); \
        self->len--; \
    }

#define ARRAY_LIST_CONCATENATE_DECL \
    coln_result COLN_CAT(ARRAY_LIST_TYPENAME, _concatenate)(ARRAY_LIST_TYPENAME *first, \
                                                  ARRAY_LIST_TYPENAME *second)
#define ARRAY_LIST_CONCATENATE_DEFN \
    ARRAY_LIST_CONCATENATE_DECL \
    { \
        assert(first); \
        assert(second); \
        if(first->cap < first->len + second->len) { \
            size_t new_cap = first->cap; \
            while(new_cap < first->len + second->len) new_cap <<= 1; \
            COLN_DATA_TYPENAME *new_buf = COLN_ALLOC( \
                first->allocator, \
                sizeof(COLN_DATA_TYPENAME) * new_cap); \
            if(!new_buf) return COLN_RESULT_ALLOC_FAILED; \
            COLN_DATA_MOVE_MANY(new_buf, first->data, first->len); \
            COLN_FREE(first->allocator, first->data); \
            first->data = new_buf; \
            first->cap = new_cap; \
        } \
        COLN_DATA_MOVE_MANY(first->data + first->len, \
                            second->data, \
                            second->len); \
        COLN_FREE(second->allocator, second->data); \
        first->len += second->len; \
        return COLN_RESULT_SUCCESS; \
    }

#define ARRAY_LIST__PRIV__EXPAND COLN_CAT(ARRAY_LIST_TYPENAME, _expand)
#define ARRAY_LIST__PRIV__EXPAND_DECL \
    static coln_result ARRAY_LIST__PRIV__EXPAND(ARRAY_LIST_TYPENAME *to_expand)
#define ARRAY_LIST__PRIV__EXPAND_DEFN \
    ARRAY_LIST__PRIV__EXPAND_DECL \
    { \
        COLN_INTERNAL_ASSERT(to_expand); \
        size_t new_cap = to_expand->cap << 1; \
        COLN_DATA_TYPENAME *new_buf = COLN_ALLOC(to_expand->allocator, \
                                             sizeof(COLN_DATA_TYPENAME) * new_cap); \
        if(!new_buf) return COLN_RESULT_ALLOC_FAILED; \
        COLN_DATA_MOVE_MANY(new_buf, to_expand->data, to_expand->len); \
        COLN_FREE(to_expand->allocator, to_expand->data); \
        to_expand->data = new_buf; \
        to_expand->cap = new_cap; \
        return COLN_RESULT_SUCCESS; \
    }

#ifdef COLN_HEADER
ARRAY_LIST_STRUCT_DEFN
ARRAY_LIST_INIT_DECL;
ARRAY_LIST_COPY_DECL;
ARRAY_LIST_CLEAR_DECL;
ARRAY_LIST_PUSH_BACK_DECL;
ARRAY_LIST_INSERT_AT_DECL;
ARRAY_LIST_POP_BACK_DECL;
ARRAY_LIST_REMOVE_AT_DECL;
ARRAY_LIST_CONCATENATE_DECL;
#endif

#ifdef COLN_IMPL
COLN_DATA__PRIV__COPY_MANY_DECLSC
ARRAY_LIST__PRIV__EXPAND_DECL;
ARRAY_LIST_INIT_DEFN
ARRAY_LIST_COPY_DEFN
ARRAY_LIST_CLEAR_DEFN
ARRAY_LIST_PUSH_BACK_DEFN
ARRAY_LIST_INSERT_AT_DEFN
ARRAY_LIST_POP_BACK_DEFN
ARRAY_LIST_REMOVE_AT_DEFN
ARRAY_LIST_CONCATENATE_DEFN
COLN_DATA__PRIV__COPY_MANY_DEFN
ARRAY_LIST__PRIV__EXPAND_DEFN
#endif

#undef ARRAY_LIST__PRIV__EXPAND_DEFN
#undef ARRAY_LIST__PRIV__EXPAND_DECL
#undef ARRAY_LIST_CONCATENATE_DEFN
#undef ARRAY_LIST_CONCATENATE_DECL
#undef ARRAY_LIST_REMOVE_AT_DEFN
#undef ARRAY_LIST_REMOVE_AT_DECL
#undef ARRAY_LIST_POP_BACK_DEFN
#undef ARRAY_LIST_POP_BACK_DECL
#undef ARRAY_LIST_INSERT_AT_DEFN
#undef ARRAY_LIST_INSERT_AT_DECL
#undef ARRAY_LIST_PUSH_BACK_DEFN
#undef ARRAY_LIST_PUSH_BACK_DECL
#undef ARRAY_LIST_CLEAR_DEFN
#undef ARRAY_LIST_CLEAR_DECL
#undef ARRAY_LIST_COPY_DEFN
#undef ARRAY_LIST_COPY_DECL
#undef ARRAY_LIST_INIT_DEFN
#undef ARRAY_LIST_INIT_DECL
#undef ARRAY_LIST_STRUCT_DEFN
#undef COLN_ALLOC_ASSERT
#undef COLN_ALLOC_ASSIGN
#undef COLN_ALLOC_ARG
#undef COLN_ALLOC_DECL
#undef COLN_FREE
#undef COLN_ALLOC
#undef COLN_ALLOC_TYPE
#undef ARRAY_LIST_TYPENAME
#undef COLN_DATA_CLEAR_MANY
#undef COLN_DATA_CLEAR
#undef COLN_DATA_MOVE_MANY
#undef COLN_DATA_MOVE
#undef COLN_DATA__PRIV__COPY_MANY_DEFN
#undef COLN_DATA__PRIV__COPY_MANY_DECLSC
#undef COLN_DATA__PRIV__COPY_MANY_DECL
#undef COLN_DATA_COPY_MANY
#undef COLN_DATA_COPY
#undef COLN_DATA_TYPENAME
#undef COLN_HEADER
#undef COLN_IMPL
#undef COLN_CAT
#undef COLN_CAT_
