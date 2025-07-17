#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "coln_result.h"

#define COLN_CAT_(a, b) a ## b
#define COLN_CAT(a, b) COLN_CAT_(a, b)

#ifndef COLN_INTERNAL_NDEBUG
  #define COLN_INTERNAL_ASSERT(x) assert(x)
#else
  #define COLN_INTERNAL_ASSERT(x)
#endif

#if !defined(HASH_TABLE_HEADER) && !defined(HASH_TABLE_IMPL)
  #error "HASH_TABLE_HEADER or HASH_TABLE_IMPL must be defined"
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

#ifndef DATA_HASH
  #error "Hash table requires a data hash function or macro, DATA_HASH, with " \
    "a signature of [size_t DATA_HASH(DATA_TYPENAME)] or " \
    "[size_t DATA_HASH(DATA_TYPENAME *)] depending on the specified passing " \
    "semantics"
#endif

#ifndef DATA_EQUALS
  #error "Hash table requires an equality check function or macro, DATA_EQUALS"\
    "with a signature of [bool DATA_EQUALS(DATA_TYPENAME, DATA_TYPENAME)] or " \
    "[bool DATA_EQUALS(DATA_TYPENAME *, DATA_TYPENAME *)] depending on the " \
    "specified passing semantics"
#endif

#if defined(DATA_PASS_BY_VAL)
  #define DATA_ARG(arg_name) DATA_TYPENAME arg_name
  #define DATA_ASSERT_ARG(arg_name)
  #ifdef DATA_MOVE
    #define DATA_SWAP_VAL_ARG(a, b) \
      do \
      { \
        DATA_TYPENAME tmp; \
        tmp = DATA_MOVE((a)); \
        (a) = DATA_MOVE((b)); \
        (b) = DATA_MOVE(tmp); \
      } while(0)
  #else
    #define DATA_SWAP_VAL_ARG(a, b) \
      do \
      { \
        DATA_TYPENAME tmp; \
        tmp = (a); \
        (a) = (b); \
        (b) = tmp; \
      } while(0)
  #endif
#elif defined(DATA_PASS_BY_PTR)
  #define DATA_ARG(arg_name) DATA_TYPENAME *arg_name
  #define DATA_ASSERT_ARG(arg_name) assert(arg_name)
  #ifdef DATA_MOVE
    #define DATA_SWAP_VAL_ARG(a, b) \
      do \
      { \
        DATA_TYPENAME tmp; \
        DATA_MOVE(&tmp, &(a)); \
        DATA_MOVE(&(a), (b)); \
        DATA_MOVE((b), &tmp); \
      } while(0)
  #else
    #define DATA_SWAP_VAL_ARG(a, b) \
      do \
      { \
        DATA_TYPENAME tmp; \
        tmp = (a); \
        (a) = *(b); \
        *(b) = tmp; \
      } while(0)
  #endif
#endif

#ifdef ALLOC_TYPENAME
  #define ALLOC_DECL(allocator) ALLOC_TYPENAME *allocator;
  #define ALLOC_ARG(allocator) , ALLOC_TYPENAME *allocator
  #define ALLOC_ASSIGN(lval, rval) ((lval) = (rval))
  #define ALLOC_ASSERT(expr) assert(expr)
  #ifndef ALLOC_ALLOC
    #error "Collection macros require an allocation function if an allocator " \
      "type is defined."
  #endif
  #ifndef ALLOC_FREE
    #define ALLOC_FREE_SET
    #define ALLOC_FREE(allocator, ptr_to_free)
  #endif
#else
  #define ALLOC_DECL(allocator)
  #define ALLOC_ARG(allocator)
  #define ALLOC_ASSIGN(lval, rval)
  #define ALLOC_ASSERT(expr)
  #define ALLOC_ALLOC_SET
  #define ALLOC_ALLOC(allocator, size_to_alloc) malloc(size_to_alloc)
  #define ALLOC_FREE_SET
  #define ALLOC_FREE(allocator, ptr_to_free) free(ptr_to_free)
  #define ALLOC_REALLOC_SET
  #define ALLOC_REALLOC(allocator, ptr_to_realloc, new_size) \
    realloc((ptr_to_realloc), (new_size))
#endif

#ifndef HASH_TABLE_TYPENAME
  #define HASH_TABLE_TYPENAME_SET
  #define HASH_TABLE_TYPENAME COLN_CAT(DATA_TYPENAME, _hash_table)
#endif

#ifndef HASH_TABLE_MAX_COUNT
  #define HASH_TABLE_MAX_COUNT_SET
  #define HASH_TABLE_MAX_COUNT(cap) (((cap) * 3) >> 2)
#endif

#define HASH_TABLE_DEFN \
  typedef struct HASH_TABLE_TYPENAME \
  { \
    ALLOC_DECL(allocator) \
    HASH_TABLE_ENTRY_TYPENAME *entries; \
    size_t count; \
    size_t cap; \
    int max_probe_seq_len; \
  } HASH_TABLE_TYPENAME;

#define HASH_TABLE_ENTRY_TYPENAME COLN_CAT(HASH_TABLE_TYPENAME, _entry)
#define HASH_TABLE_ENTRY_DECL \
  typedef struct HASH_TABLE_ENTRY_TYPENAME HASH_TABLE_ENTRY_TYPENAME;
#define HASH_TABLE_ENTRY_DEFN \
  struct HASH_TABLE_ENTRY_TYPENAME \
  { \
    size_t hash; \
    int probe_seq_len; \
    DATA_TYPENAME data; \
  };

#define HASH_TABLE_INIT_SIGN \
  coln_result COLN_CAT(HASH_TABLE_TYPENAME, _init)( \
    HASH_TABLE_TYPENAME *to_init \
    ALLOC_ARG(allocator), \
    int initial_cap_exp)
#define HASH_TABLE_INIT_DEFN \
  HASH_TABLE_INIT_SIGN \
  { \
    assert(to_init); \
    ALLOC_ASSERT(allocator); \
    to_init->cap = 1 << initial_cap_exp; \
    assert(to_init->cap > 0); \
    if(!(to_init->entries = ALLOC_ALLOC( \
        allocator, \
        sizeof(HASH_TABLE_ENTRY_TYPENAME) * to_init->cap))) \
      return COLN_RESULT_ALLOC_FAILED; \
    for(ptrdiff_t i = 0; i < (ptrdiff_t)to_init->cap; i++) \
      to_init->entries[i].probe_seq_len = -1; \
    ALLOC_ASSIGN(to_init->allocator, allocator); \
    to_init->count = 0; \
    to_init->max_probe_seq_len = 0; \
    return COLN_RESULT_SUCCESS; \
  }

#ifdef DATA_NO_COPY
  #define HASH_TABLE_COPY_ELEMS_SNIPPET
  #define HASH_TABLE_COPY_SIGN
  #define HASH_TABLE_COPY_DEFN
#else
  #ifdef DATA_COPY
    
    #if defined(DATA_PASS_BY_VAL)
    #elif defined(DATA_PASS_BY_PTR)
    #endif
  #else
    #define HASH_TABLE_COPY_ELEMS_SNIPPET 
  #endif
  #define HASH_TABLE_COPY_SIGN \
    coln_result COLN_CAT(HASH_TABLE_TYPENAME, _copy)(HASH_TABLE_TYPENAME *dest, \
                                                     HASH_TABLE_TYPENAME *src)
  #define HASH_TABLE_COPY_DEFN \
    HASH_TABLE_COPY_SIGN \
    { \
      assert(dest); \
      assert(src); \
      if(!(dest->entries = ALLOC_ALLOC( \
          allocator, \
          sizeof(HASH_TABLE_ENTRY_TYPENAME) * src->cap))) \
        return COLN_RESULT_ALLOC_FAILED; \
      for(ptrdiff_t i = 0; i < (ptrdiff_t)src->cap; i++) \
      { \
        dest->entries[i].probe_seq_len = src->entries[i].probe_seq_len; \
        if(src->entries[i].probe_seq_len < 0) continue; \
        dest->entries[i].hash = src->entries[i].hash; \
        if(!DATA_COPY(&(dest->entries[i].data), \
                      &(src->entries[i].data))) \
            { \
                for(ptrdiff_t j = i - 1; j >= 0; j--) \
                    DATA_CLEAR(&(dest->entries[j].data)); \
                ALLOC_FREE(src->allocator, dest->entries); \
                return COLN_RESULT_COPY_ELEM_FAILED; \
            } \
        } \
        ALLOC_ASSIGN(dest->allocator, src->allocator); \
        dest->cap = src->cap; \
        dest->count = src->count; \
        dest->max_probe_seq_len = src->max_probe_seq_len; \
        return COLN_RESULT_SUCCESS; \
    }
#endif

#define HASH_TABLE_CLEAR_SIGN \
    void COLN_CAT(HASH_TABLE_TYPENAME, _clear)(HASH_TABLE_TYPENAME *to_clear)
#define HASH_TABLE_CLEAR_DEFN \
    HASH_TABLE_CLEAR_SIGN \
    { \
        assert(to_clear); \
        for(ptrdiff_t i = 0; i < (ptrdiff_t)to_clear->cap; i++) \
        { \
            if(to_clear->entries[i].probe_seq_len >= 0) \
            { \
                DATA_CLEAR(&(to_clear->entries[i].data)); \
            } \
        } \
        ALLOC_FREE(to_clear->allocator, to_clear->entries); \
    }

#define HASH_TABLE_INSERT_SIGN \
    coln_result COLN_CAT(HASH_TABLE_TYPENAME, _insert)( \
        HASH_TABLE_TYPENAME *hash_table, \
        DATA_TYPENAME *to_insert)
#define HASH_TABLE_INSERT_DEFN \
    HASH_TABLE_INSERT_SIGN \
    { \
        assert(hash_table); \
        assert(to_insert); \
        if((hash_table->count + 1 > HASH_TABLE_MAX_COUNT(hash_table->cap)) && \
                !HASH_TABLE_EXPAND_FNNAME(hash_table)) \
            return COLN_RESULT_ALLOC_FAILED; \
        size_t hash = DATA_HASH(to_insert); \
        int ins_max_probe_seq = HASH_TABLE_INTERNAL_INSERT_FNNAME( \
            hash_table->entries, \
            hash_table->cap, \
            to_insert, \
            hash); \
        if(ins_max_probe_seq > hash_table->max_probe_seq_len) \
            hash_table->max_probe_seq_len = ins_max_probe_seq; \
        hash_table->count++; \
        return COLN_RESULT_SUCCESS; \
    }

#define HASH_TABLE_SEARCH_SIGN \
    DATA_TYPENAME *COLN_CAT(HASH_TABLE_TYPENAME, _search)( \
        HASH_TABLE_TYPENAME *hash_table, \
        DATA_TYPENAME *elem_to_find)
#define HASH_TABLE_SEARCH_DEFN \
    HASH_TABLE_SEARCH_SIGN \
    { \
        assert(hash_table); \
        assert(elem_to_find); \
        size_t idx_mask = hash_table->cap - 1; \
        size_t base_idx = hash & idx_mask; \
        for(int misses = 0; \
                misses <= hash_table->max_probe_seq_len; \
                misses++) \
        { \
            size_t i = (base_idx + misses) & idx_mask; \
            if(hash_table->entries[i].probe_seq_len < 0) return NULL; \
            if(DATA_EQUALS_VAL_ARG(hash_table->entries[i].data, elem_to_find)) \
                return &(hash_table->entries[i].data); \
        } \
        return NULL; \
    }

#define HASH_TABLE_REMOVE_SIGN \
    coln_result COLN_CAT(HASH_TABLE_TYPENAME, _remove)( \
        HASH_TABLE_TYPENAME *hash_table, \
        DATA_TYPENAME *to_remove, \
        DATA_TYPENAME *removed)
#define HASH_TABLE_REMOVE_DEFN \
    HASH_TABLE_REMOVE_SIGN \
    { \
        assert(hash_table); \
        assert(to_remove); \
        assert(removed); \
        size_t idx_mask = hash_table->cap - 1; \
        size_t hash = DATA_HASH(to_remove); \
        size_t base_idx = hash & idx_mask; \
        size_t cur; \
        for(int misses = 0; true; misses++) \
        { \
            if(misses > hash_table->max_probe_seq_len) \
                return COLN_RESULT_ELEM_NOT_FOUND; \
            cur = (base_idx + misses) & idx_mask; \
            if(hash_table->entries[cur].probe_seq_len < 0) \
                return COLN_RESULT_ELEM_NOT_FOUND; \
            if(hash == hash_table->entries[cur].hash) break; \
        } \
        DATA_MOVE(removed, &(hash_table->entries[cur].data)); \
        size_t next = (cur + 1) & idx_mask; \
        while(hash_table->entries[next].probe_seq_len > 0) \
        { \
            DATA_MOVE(&(hash_table->entries[cur].data), \
                           &(hash_table->entries[next].data)); \
            hash_table->entries[cur].probe_seq_len = \
                hash_table->entries[next].probe_seq_len - 1; \
            hash_table->entries[cur].hash = \
                hash_table->entries[next].hash; \
            cur = next; \
            next = (next + 1) & idx_mask; \
        } \
        hash_table->entries[cur].probe_seq_len = -1; \
        hash_table->count--; \
        return COLN_RESULT_SUCCESS; \
    }

#define HASH_TABLE_FOR_EACH_SIGN \
    void COLN_CAT(HASH_TABLE_TYPENAME, _for_each)( \
        HASH_TABLE_TYPENAME *hash_table, \
        void *capture, \
        void(*lambda)(void *capture, \
                      DATA_TYPENAME *data))
#define HASH_TABLE_FOR_EACH_DEFN \
    HASH_TABLE_FOR_EACH_SIGN \
    { \
        for(ptrdiff_t i = 0; i < (ptrdiff_t)hash_table->cap; i++) \
        { \
            if(hash_table->entries[i].probe_seq_len < 0) continue; \
            lambda(capture, &(hash_table->entries[i].data)); \
        } \
    }

#define HASH_TABLE_ITER_TYPENAME COLN_CAT(HASH_TABLE_TYPENAME, _iter)
#define HASH_TABLE_ITER_DEFN \
    typedef struct HASH_TABLE_ITER_TYPENAME \
    { \
        DATA_TYPENAME *end_invalid; \
        DATA_TYPENAME *cur; \
    } HASH_TABLE_ITER_TYPENAME;

#define HASH_TABLE_ITER_INIT_SIGN \
    bool COLN_CAT(HASH_TABLE_ITER_TYPENAME, _init)( \
        HASH_TABLE_TYPENAME *hash_table, \
        HASH_TABLE_ITER_TYPENAME *to_init)
#define HASH_TABLE_ITER_INIT_DEFN \
    HASH_TABLE_ITER_INIT_SIGN \
    { \
        assert(hash_table); \
        assert(to_init); \
        to_init->end_invalid = \
            &((hash_table->entries + hash_table->cap)->data); \
        to_init->cur = &(hash_table->entries[-1].data); \
        return HASH_TABLE_ITER_NEXT_FNNAME(to_init); \
    }

#define HASH_TABLE_ITER_NEXT_FNNAME COLN_CAT(HASH_TABLE_ITER_TYPENAME, _next) 
#define HASH_TABLE_ITER_NEXT_SIGN \
    bool HASH_TABLE_ITER_NEXT_FNNAME(HASH_TABLE_ITER_TYPENAME *iter)
#define HASH_TABLE_ITER_NEXT_DEFN \
    HASH_TABLE_ITER_NEXT_SIGN \
    { \
        assert(iter); \
        assert(iter->cur != iter->end_invalid); \
        while(true) \
        { \
            iter->cur = (DATA_TYPENAME *) \
                ((unsigned char *)(iter->cur) + \
                    sizeof(HASH_TABLE_ENTRY_TYPENAME)); \
            if(iter->cur == iter->end_invalid) return false; \
            int cur_probe_seq_len = ((HASH_TABLE_ENTRY_TYPENAME *) \
                ((unsigned char *)(iter->cur) - \
                    offsetof(HASH_TABLE_ENTRY_TYPENAME, data)) \
                )->probe_seq_len; \
            if(cur_probe_seq_len >= 0) return true; \
        } \
    }

#define HASH_TABLE_EXPAND_FNNAME COLN_CAT(HASH_TABLE_TYPENAME, _expand)
#define HASH_TABLE_EXPAND_SIGN \
    static bool HASH_TABLE_EXPAND_FNNAME(HASH_TABLE_TYPENAME *to_expand)
#define HASH_TABLE_EXPAND_DEFN \
    HASH_TABLE_EXPAND_SIGN \
    { \
        COLN_INTERNAL_ASSERT(to_expand); \
        size_t new_cap = to_expand->cap << 1; \
        HASH_TABLE_ENTRY_TYPENAME *new_entries = ALLOC_ALLOC( \
            to_expand->allocator, \
            sizeof(HASH_TABLE_ENTRY_TYPENAME) * new_cap); \
        if(!new_entries) return false; \
        for(ptrdiff_t i = 0; i < (ptrdiff_t)new_cap; i++) \
            new_entries[i].probe_seq_len = -1; \
        int max_probe_seq_len = 0; \
        for(ptrdiff_t i = 0; i < (ptrdiff_t)(to_expand->cap); i++) \
        { \
            if(to_expand->entries[i].probe_seq_len < 0) continue; \
            int ins_probe_seq_len = HASH_TABLE_INTERNAL_INSERT_FNNAME( \
                    new_entries, \
                    new_cap, \
                    &(to_expand->entries[i].data), \
                    to_expand->entries[i].hash); \
            if(ins_probe_seq_len > max_probe_seq_len) \
                max_probe_seq_len = ins_probe_seq_len; \
        } \
        ALLOC_FREE(to_expand->allocator, to_expand->entries); \
        to_expand->entries = new_entries; \
        to_expand->cap = new_cap; \
        to_expand->max_probe_seq_len = max_probe_seq_len; \
        return true; \
    }

#define HASH_TABLE_INTERNAL_INSERT_FNNAME \
    COLN_CAT(HASH_TABLE_TYPENAME, _internal_insert)
#define HASH_TABLE_INTERNAL_INSERT_SIGN \
    static int HASH_TABLE_INTERNAL_INSERT_FNNAME( \
        HASH_TABLE_ENTRY_TYPENAME *entries, \
        size_t cap, \
        DATA_TYPENAME *to_insert, \
        size_t to_insert_hash)
#define HASH_TABLE_INTERNAL_INSERT_DEFN \
    HASH_TABLE_INTERNAL_INSERT_SIGN \
    { \
        COLN_INTERNAL_ASSERT(entries); \
        COLN_INTERNAL_ASSERT(IS_POW_2(cap)); \
        COLN_INTERNAL_ASSERT(to_insert); \
        COLN_INTERNAL_ASSERT(HASH_TABLE_HAS_SLOT_INVOC(entries, \
                                                              cap)); \
        size_t mod_mask = cap - 1; \
        int misses = 0; \
        int result = 0; \
        for(size_t index = to_insert_hash & mod_mask; \
                true; \
                index = (index + 1) & mod_mask) \
        { \
            if(entries[index].probe_seq_len < 0) \
            { \
                /* the slot is not in use */ \
                entries[index].hash = to_insert_hash; \
                entries[index].probe_seq_len = misses; \
                DATA_MOVE(&(entries[index].data), to_insert); \
                return result; \
            } \
            /* the slot is in use */ \
            if(misses > entries[index].probe_seq_len) \
            { \
                /* Robin Hood */ \
                /* We're homeless and poorer than the data at the current */ \
                /* index, steal its home. */ \
                DATA_SWAP_VAL_ARG(entries[index].data, to_insert); \
                int ps_swap_var = misses; \
                misses = entries[index].probe_seq_len; \
                entries[index].probe_seq_len = ps_swap_var; \
                size_t hash_swap_var = to_insert_hash; \
                to_insert_hash = entries[index].hash; \
                entries[index].hash = hash_swap_var; \
            } \
            /* If we consider the case where we just stole a slot, pretend */ \
            /* that it just missed its slot instead. Stealing a slot */ \
            /* can be considered a miss for the element we just stole from. */ \
            if(++misses > result) result = misses; \
        } \
    }

#ifdef COLN_INTERAL_DEBUG
#define HASH_TABLE_HAS_SLOT_FNNAME COLN_CAT(HASH_TABLE_TYPENAME, \
                                                   _has_slot)
#define HASH_TABLE_HAS_SLOT_INVOC(entries, cap) \
    HASH_TABLE_HAS_SLOT_FNNAME((entries), (cap))
#define HASH_TABLE_HAS_SLOT_SIGN \
    static bool HASH_TABLE_HAS_SLOT_FNNAME( \
        HASH_TABLE_ENTRY_TYPENAME *entries, \
        size_t cap)
#define HASH_TABLE_HAS_SLOT_DECL HASH_TABLE_HAS_SLOT_SIGN;
#define HASH_TABLE_HAS_SLOT_DEFN \
    HASH_TABLE_HAS_SLOT_SIGN \
    { \
        for(ptrdiff_t i = 0; i < (ptrdiff_t)cap; i++) \
            if(entries[i].probe_seq_len < 0) return true; \
        return false; \
    }
#else
#define HASH_TABLE_HAS_SLOT_FNNAME
#define HASH_TABLE_HAS_SLOT_INVOC(entries, cap) true
#define HASH_TABLE_HAS_SLOT_SIGN
#define HASH_TABLE_HAS_SLOT_DECL
#define HASH_TABLE_HAS_SLOT_DEFN
#endif

#ifdef HASH_TABLE_HEADER
HASH_TABLE_ENTRY_DECL
HASH_TABLE_DEFN
HASH_TABLE_ITER_DEFN
HASH_TABLE_INIT_SIGN;
HASH_TABLE_COPY_SIGN;
HASH_TABLE_CLEAR_SIGN;
HASH_TABLE_INSERT_SIGN;
HASH_TABLE_SEARCH_SIGN;
HASH_TABLE_REMOVE_SIGN;
HASH_TABLE_FOR_EACH_SIGN;
HASH_TABLE_ITER_INIT_SIGN;
HASH_TABLE_ITER_NEXT_SIGN;
#endif

#ifdef HASH_TABLE_IMPL
#define IS_POW_2(x) ((x) && !((x) & ((x) - 1)))
HASH_TABLE_ENTRY_DEFN
HASH_TABLE_EXPAND_SIGN;
HASH_TABLE_INTERNAL_INSERT_SIGN;
HASH_TABLE_HAS_SLOT_DECL
HASH_TABLE_INIT_DEFN
HASH_TABLE_COPY_DEFN
HASH_TABLE_CLEAR_DEFN
HASH_TABLE_INSERT_DEFN
HASH_TABLE_SEARCH_DEFN
HASH_TABLE_REMOVE_DEFN
HASH_TABLE_FOR_EACH_DEFN
HASH_TABLE_ITER_INIT_DEFN
HASH_TABLE_ITER_NEXT_DEFN
HASH_TABLE_EXPAND_DEFN
HASH_TABLE_INTERNAL_INSERT_DEFN
HASH_TABLE_HAS_SLOT_DEFN
#undef IS_POW_2
#endif

#undef HASH_TABLE_HAS_SLOT_DEFN
#undef HASH_TABLE_HAS_SLOT_DECL
#undef HASH_TABLE_HAS_SLOT_SIGN
#undef HASH_TABLE_HAS_SLOT_INVOC
#undef HASH_TABLE_HAS_SLOT_FNNAME
#undef HASH_TABLE_INTERNAL_INSERT_DEFN
#undef HASH_TABLE_INTERNAL_INSERT_SIGN
#undef HASH_TABLE_INTERNAL_INSERT_FNNAME
#undef HASH_TABLE_EXPAND_DEFN
#undef HASH_TABLE_EXPAND_SIGN
#undef HASH_TABLE_EXPAND_FNNAME
#undef HASH_TABLE_ITER_NEXT_DEFN
#undef HASH_TABLE_ITER_NEXT_SIGN
#undef HASH_TABLE_ITER_NEXT_FNNAME
#undef HASH_TABLE_ITER_INIT_DEFN
#undef HASH_TABLE_ITER_INIT_SIGN
#undef HASH_TABLE_ITER_DEFN
#undef HASH_TABLE_ITER_TYPENAME
#undef HASH_TABLE_FOR_EACH_DEFN
#undef HASH_TABLE_FOR_EACH_SIGN
#undef HASH_TABLE_REMOVE_DEFN
#undef HASH_TABLE_REMOVE_SIGN
#undef HASH_TABLE_SEARCH_DEFN
#undef HASH_TABLE_SEARCH_SIGN
#undef HASH_TABLE_INSERT_DEFN
#undef HASH_TABLE_INSERT_SIGN
#undef HASH_TABLE_CLEAR_DEFN
#undef HASH_TABLE_CLEAR_SIGN
#undef HASH_TABLE_COPY_DEFN
#undef HASH_TABLE_COPY_SIGN
#undef HASH_TABLE_INIT_DEFN
#undef HASH_TABLE_INIT_SIGN
#undef HASH_TABLE_ENTRY_DEFN
#undef HASH_TABLE_ENTRY_DECL
#undef HASH_TABLE_ENTRY_TYPENAME
#undef HASH_TABLE_DEFN
#undef HASH_TABLE_TYPENAME
#undef HASH_TABLE_MAX_COUNT
#undef COLN_INTERNAL_ASSERT
#undef ALLOC_FREE
#undef ALLOC_ALLOC
#undef ALLOC_ASSERT
#undef ALLOC_ASSIGN
#undef ALLOC_ARG
#undef ALLOC_DECL
#undef ALLOC_TYPENAME
#undef DATA_SWAP
#undef DATA_CLEAR
#undef DATA_MOVE
#undef DATA_COPY
#undef DATA_HASH
#undef DATA_TYPENAME
#undef HASH_TABLE_IMPL
#undef HASH_TABLE_HEADER
#undef COLN_CAT
#undef COLN_CAT_
