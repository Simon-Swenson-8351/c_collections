#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "coln_result.h"

#define COLN_CAT_(a, b) a ## b
#define COLN_CAT(a, b) COLN_CAT_(a, b)

#if !defined(COLN_HEADER) && !defined(COLN_IMPL)
#error "COLN_HEADER or COLN_IMPL must be defined"
#endif

#ifndef COLN_DATA_TYPENAME
#error "Collection macros require COLN_DATA_TYPENAME"
#endif

#ifndef COLN_DATA_HASH
#error "Hash table requires a data hash function, COLN_DATA_HASH"
#endif

#ifndef COLN_DATA_COPY
#define COLN_DATA_COPY(dest_ptr, src_ptr) (*(dest_ptr) = *(src_ptr), true)
#endif

#ifndef COLN_DATA_MOVE
#define COLN_DATA_MOVE(dest_ptr, src_ptr) (*(dest_ptr) = *(src_ptr))
#endif

#ifndef COLN_DATA_CLEAR
#define COLN_DATA_CLEAR(to_clear_ptr)
#endif

#ifndef COLN_DATA_SWAP
#define COLN_DATA_SWAP(a, b) \
    do \
    { \
        COLN_DATA_TYPENAME tmp; \
        COLN_DATA_MOVE(&tmp, (a)); \
        COLN_DATA_MOVE((a), (b)); \
        COLN_DATA_MOVE((b), &tmp); \
    } while(0)
#endif

#ifdef COLN_ALLOC_TYPE
#define COLN_ALLOC_DECL(allocator) COLN_ALLOC_TYPE *allocator;
#define COLN_ALLOC_ARG(allocator) , COLN_ALLOC_TYPE *allocator
#define COLN_ALLOC_ASSIGN(lval, rval) ((lval) = (rval))
#define COLN_ALLOC_ASSERT(expr) assert(expr)
#ifndef COLN_ALLOC
#error "Collection macros require an allocation function if an allocator type is defined."
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

#ifdef COLN_INTERNAL_DEBUG
#define COLN_INTERNAL_ASSERT(x) assert(x)
#else
#define COLN_INTERNAL_ASSERT(x)
#endif

#if defined(COLN_INTERNAL_DEBUG) && defined(COLN_ALLOC_TYPE)
#define COLN_ALLOC_INTERNAL_ASSERT(x) assert(x)
#else
#define COLN_ALLOC_INTERNAL_ASSERT(x)
#endif

#ifndef HASH_TABLE_MAX_LOAD_FACTOR
#define HASH_TABLE_MAX_LOAD_FACTOR 0.75
#endif

#ifndef HASH_TABLE_TYPENAME
#define HASH_TABLE_TYPENAME COLN_CAT(COLN_DATA_TYPENAME, _hash_table)
#endif
#define HASH_TABLE_DEFN \
    typedef struct HASH_TABLE_TYPENAME \
    { \
        COLN_ALLOC_DECL(allocator) \
        HASH_TABLE_ENTRY_TYPENAME *entries; \
        size_t count; \
        size_t cap; \
        int max_probe_seq_len; \
    } HASH_TABLE_TYPENAME;

#define HASH_TABLE_ENTRY_TYPENAME COLN_CAT(HASH_TABLE_TYPENAME, _entry)
#define HASH_TABLE_ENTRY_DECL \
    typedef struct HASH_TABLE_ENTRY_TYPENAME HASH_TABLE_ENTRY_TYPENAME;
#define HASH_TABLE_ENTRY_DEFN \
    typedef struct HASH_TABLE_ENTRY_TYPENAME \
    { \
        size_t hash; \
        int probe_seq_len; \
        COLN_DATA_TYPENAME data; \
    } HASH_TABLE_ENTRY_TYPENAME;

#define HASH_TABLE_INIT_SIGN \
    coln_result COLN_CAT(HASH_TABLE_TYPENAME, _init)( \
        HASH_TABLE_TYPENAME *to_init \
        COLN_ALLOC_ARG(allocator), \
        int initial_cap_exp)
#define HASH_TABLE_INIT_DEFN \
    HASH_TABLE_INIT_SIGN \
    { \
        assert(to_init); \
        COLN_ALLOC_ASSERT(allocator); \
        to_init->cap = 1 << initial_cap_exp; \
        assert(to_init->cap > 0); \
        if(!(to_init->entries = COLN_ALLOC( \
                allocator, \
                sizeof(HASH_TABLE_ENTRY_TYPENAME) * to_init->cap))) \
            return COLN_RESULT_ALLOC_FAILED; \
        for(intptr_t i = 0; i < (intptr_t)to_init->cap; i++) \
            to_init->entries[i].probe_seq_len = -1; \
        COLN_ALLOC_ASSIGN(to_init->allocator, allocator); \
        to_init->count = 0; \
        to_init->max_probe_seq_len = 0; \
        return COLN_RESULT_SUCCESS; \
    }

#define HASH_TABLE_COPY_SIGN \
    coln_result COLN_CAT(HASH_TABLE_TYPENAME, _copy)(HASH_TABLE_TYPENAME *dest, \
                                                    HASH_TABLE_TYPENAME *src)
#define HASH_TABLE_COPY_DEFN \
    HASH_TABLE_COPY_SIGN \
    { \
        assert(dest); \
        assert(src); \
        if(!(dest->entries = COLN_ALLOC( \
                allocator, \
                sizeof(HASH_TABLE_ENTRY_TYPENAME) * src->cap))) \
            return COLN_RESULT_ALLOC_FAILED; \
        for(ptrdiff_t i = 0; i < (ptrdiff_t)src->cap; i++) \
        { \
            dest->entries[i].probe_seq_len = src->entries[i].probe_seq_len; \
            if(src->entries[i].probe_seq_len < 0) continue; \
            dest->entries[i].hash = src->entries[i].hash; \
            if(!COLN_DATA_COPY(&(dest->entries[i].data), \
                               &(src->entries[i].data))) \
            { \
                for(ptrdiff_t j = i - 1; j >= 0; j--) \
                    COLN_DATA_CLEAR(&(dest->entries[j].data)); \
                COLN_FREE(src->allocator, dest->entries); \
                return COLN_RESULT_COPY_ELEM_FAILED; \
            } \
        } \
        COLN_ALLOC_ASSIGN(dest->allocator, src->allocator); \
        dest->cap = src->cap; \
        dest->count = src->count; \
        dest->max_probe_seq_len = src->max_probe_seq_len; \
        return COLN_RESULT_SUCCESS; \
    }

#define HASH_TABLE_CLEAR_SIGN \
    void COLN_CAT(HASH_TABLE_TYPENAME, _clear)(HASH_TABLE_TYPENAME *to_clear)
#define HASH_TABLE_CLEAR_DEFN \
    HASH_TABLE_CLEAR_SIGN \
    { \
        assert(to_clear); \
        for(intptr_t i = 0; i < (intptr_t)to_clear->cap; i++) \
        { \
            if(to_clear->entries[i].probe_seq_len >= 0) \
            { \
                COLN_DATA_CLEAR(&(to_clear->entries[i].data)); \
            } \
        } \
        COLN_FREE(to_clear->allocator, to_clear->entries); \
    }

#define HASH_TABLE_INSERT_SIGN \
    coln_result COLN_CAT(HASH_TABLE_TYPENAME, _insert)( \
        HASH_TABLE_TYPENAME *hash_table, \
        COLN_DATA_TYPENAME *to_insert)
#define HASH_TABLE_INSERT_DEFN \
    HASH_TABLE_INSERT_SIGN \
    { \
        assert(hash_table); \
        assert(to_insert); \
        float load_factor = (float)(hash_table->count + 1) / \
            (float)(hash_table->cap); \
        if((load_factor > HASH_TABLE_MAX_LOAD_FACTOR) && \
                !HASH_TABLE__PRIV__EXPAND_FNNAME(hash_table)) \
            return COLN_RESULT_ALLOC_FAILED; \
        size_t hash = COLN_DATA_HASH(to_insert); \
        int ins_max_probe_seq = HASH_TABLE__PRIV__INTERNAL_INSERT_FNNAME( \
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
    COLN_DATA_TYPENAME *COLN_CAT(HASH_TABLE_TYPENAME, _search)( \
        HASH_TABLE_TYPENAME *hash_table, \
        COLN_DATA_TYPENAME *elem_to_find)
#define HASH_TABLE_SEARCH_DEFN \
    HASH_TABLE_SEARCH_SIGN \
    { \
        assert(hash_table); \
        assert(elem_to_find); \
        size_t idx_mask = hash_table->cap - 1; \
        size_t hash = COLN_DATA_HASH(elem_to_find); \
        size_t base_idx = hash & idx_mask; \
        for(int misses = 0; \
                misses <= hash_table->max_probe_seq_len; \
                misses++) \
        { \
            size_t i = (base_idx + misses) & idx_mask; \
            if(hash_table->entries[i].probe_seq_len < 0) return NULL; \
            if(hash == hash_table->entries[i].hash) \
                return &(hash_table->entries[i].data); \
        } \
        return NULL; \
    }

#define HASH_TABLE_REMOVE_SIGN \
    coln_result COLN_CAT(HASH_TABLE_TYPENAME, _remove)( \
        HASH_TABLE_TYPENAME *hash_table, \
        COLN_DATA_TYPENAME *to_remove, \
        COLN_DATA_TYPENAME *removed)
#define HASH_TABLE_REMOVE_DEFN \
    HASH_TABLE_REMOVE_SIGN \
    { \
        assert(hash_table); \
        assert(to_remove); \
        assert(removed); \
        size_t idx_mask = hash_table->cap - 1; \
        size_t hash = COLN_DATA_HASH(to_remove); \
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
        COLN_DATA_MOVE(removed, &(hash_table->entries[cur].data)); \
        size_t next = (cur + 1) & idx_mask; \
        while(hash_table->entries[next].probe_seq_len > 0) \
        { \
            COLN_DATA_MOVE(&(hash_table->entries[cur].data), \
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
                      COLN_DATA_TYPENAME *data))
#define HASH_TABLE_FOR_EACH_DEFN \
    HASH_TABLE_FOR_EACH_SIGN \
    { \
        for(intptr_t i = 0; i < (intptr_t)hash_table->cap; i++) \
        { \
            if(hash_table->entries[i].probe_seq_len < 0) continue; \
            lambda(capture, &(hash_table->entries[i].data)); \
        } \
    }

#define HASH_TABLE_ITER_TYPENAME COLN_CAT(HASH_TABLE_TYPENAME, _iter)
#define HASH_TABLE_ITER_DEFN \
    typedef struct HASH_TABLE_ITER_TYPENAME \
    { \
        COLN_DATA_TYPENAME *end_invalid; \
        COLN_DATA_TYPENAME *cur; \
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
            iter->cur = (COLN_DATA_TYPENAME *) \
                ((uint8_t *)(iter->cur) + sizeof(HASH_TABLE_ENTRY_TYPENAME)); \
            if(iter->cur == iter->end_invalid) return false; \
            int cur_probe_seq_len = ((HASH_TABLE_ENTRY_TYPENAME *) \
                ((uint8_t *)(iter->cur) - \
                    offsetof(HASH_TABLE_ENTRY_TYPENAME, data)) \
                )->probe_seq_len; \
            if(cur_probe_seq_len >= 0) return true; \
        } \
    }

#define HASH_TABLE__PRIV__EXPAND_FNNAME COLN_CAT(HASH_TABLE_TYPENAME, _expand)
#define HASH_TABLE__PRIV__EXPAND_SIGN \
    static bool HASH_TABLE__PRIV__EXPAND_FNNAME(HASH_TABLE_TYPENAME *to_expand)
#define HASH_TABLE__PRIV__EXPAND_DEFN \
    HASH_TABLE__PRIV__EXPAND_SIGN \
    { \
        COLN_INTERNAL_ASSERT(to_expand); \
        size_t new_cap = to_expand->cap << 1; \
        HASH_TABLE_ENTRY_TYPENAME *new_entries = COLN_ALLOC( \
            to_expand->allocator, \
            sizeof(HASH_TABLE_ENTRY_TYPENAME) * new_cap); \
        if(!new_entries) return false; \
        for(intptr_t i = 0; i < (intptr_t)new_cap; i++) \
            new_entries[i].probe_seq_len = -1; \
        int max_probe_seq_len = 0; \
        for(intptr_t i = 0; i < (intptr_t)(to_expand->cap); i++) \
        { \
            if(to_expand->entries[i].probe_seq_len < 0) continue; \
            int ins_probe_seq_len = HASH_TABLE__PRIV__INTERNAL_INSERT_FNNAME( \
                    new_entries, \
                    new_cap, \
                    &(to_expand->entries[i].data), \
                    to_expand->entries[i].hash); \
            if(ins_probe_seq_len > max_probe_seq_len) \
                max_probe_seq_len = ins_probe_seq_len; \
        } \
        COLN_FREE(to_expand->allocator, to_expand->entries); \
        to_expand->entries = new_entries; \
        to_expand->cap = new_cap; \
        to_expand->max_probe_seq_len = max_probe_seq_len; \
        return true; \
    }

#define HASH_TABLE__PRIV__INTERNAL_INSERT_FNNAME \
    COLN_CAT(HASH_TABLE_TYPENAME, _internal_insert)
#define HASH_TABLE__PRIV__INTERNAL_INSERT_SIGN \
    static int HASH_TABLE__PRIV__INTERNAL_INSERT_FNNAME( \
        HASH_TABLE_ENTRY_TYPENAME *entries, \
        size_t cap, \
        COLN_DATA_TYPENAME *to_insert, \
        size_t to_insert_hash)
#define HASH_TABLE__PRIV__INTERNAL_INSERT_DEFN \
    HASH_TABLE__PRIV__INTERNAL_INSERT_SIGN \
    { \
        COLN_INTERNAL_ASSERT(entries); \
        COLN_INTERNAL_ASSERT(IS_POW_2(cap)); \
        COLN_INTERNAL_ASSERT(to_insert); \
        COLN_INTERNAL_ASSERT(HASH_TABLE__PRIV__HAS_SLOT_INVOC(entries, \
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
                COLN_DATA_MOVE(&(entries[index].data), to_insert); \
                return result; \
            } \
            /* the slot is in use */ \
            if(misses > entries[index].probe_seq_len) \
            { \
                /* Robin Hood */ \
                /* We're homeless and poorer than the data at the current */ \
                /* index, steal its home. */ \
                COLN_DATA_SWAP(&(entries[index].data), to_insert); \
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
#define HASH_TABLE__PRIV__HAS_SLOT_FNNAME COLN_CAT(HASH_TABLE_TYPENAME, \
                                                   _has_slot)
#define HASH_TABLE__PRIV__HAS_SLOT_INVOC(entries, cap) \
    HASH_TABLE__PRIV__HAS_SLOT_FNNAME((entries), (cap))
#define HASH_TABLE__PRIV__HAS_SLOT_SIGN \
    static bool HASH_TABLE__PRIV__HAS_SLOT_FNNAME( \
        HASH_TABLE_ENTRY_TYPENAME *entries, \
        size_t cap)
#define HASH_TABLE__PRIV__HAS_SLOT_DECL HASH_TABLE__PRIV__HAS_SLOT_SIGN;
#define HASH_TABLE__PRIV__HAS_SLOT_DEFN \
    HASH_TABLE__PRIV__HAS_SLOT_SIGN \
    { \
        for(ptrdiff_t i = 0; i < (ptrdiff_t)cap; i++) \
            if(entries[i].probe_seq_len < 0) return true; \
        return false; \
    }
#else
#define HASH_TABLE__PRIV__HAS_SLOT_FNNAME
#define HASH_TABLE__PRIV__HAS_SLOT_INVOC(entries, cap) true
#define HASH_TABLE__PRIV__HAS_SLOT_SIGN
#define HASH_TABLE__PRIV__HAS_SLOT_DECL
#define HASH_TABLE__PRIV__HAS_SLOT_DEFN
#endif

#ifdef COLN_HEADER
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

#ifdef COLN_IMPL
#define COLN_ALIGN(dtype_alignment, given_bytes) \
    (((given_bytes) + (dtype_alignment) - 1) & ~((dtype_alignment) - 1))
#define IS_POW_2(x) ((x) && !((x) & ((x) - 1)))
HASH_TABLE_ENTRY_DEFN
HASH_TABLE__PRIV__EXPAND_SIGN;
HASH_TABLE__PRIV__INTERNAL_INSERT_SIGN;
HASH_TABLE__PRIV__HAS_SLOT_DECL
HASH_TABLE_INIT_DEFN
HASH_TABLE_COPY_DEFN
HASH_TABLE_CLEAR_DEFN
HASH_TABLE_INSERT_DEFN
HASH_TABLE_SEARCH_DEFN
HASH_TABLE_REMOVE_DEFN
HASH_TABLE_FOR_EACH_DEFN
HASH_TABLE_ITER_INIT_DEFN
HASH_TABLE_ITER_NEXT_DEFN
HASH_TABLE__PRIV__EXPAND_DEFN
HASH_TABLE__PRIV__INTERNAL_INSERT_DEFN
HASH_TABLE__PRIV__HAS_SLOT_DEFN
#undef IS_POW_2
#undef COLN_ALIGN
#endif

#undef HASH_TABLE__PRIV__HAS_SLOT_DEFN
#undef HASH_TABLE__PRIV__HAS_SLOT_DECL
#undef HASH_TABLE__PRIV__HAS_SLOT_SIGN
#undef HASH_TABLE__PRIV__HAS_SLOT_INVOC
#undef HASH_TABLE__PRIV__HAS_SLOT_FNNAME
#undef HASH_TABLE__PRIV__INTERNAL_INSERT_DEFN
#undef HASH_TABLE__PRIV__INTERNAL_INSERT_SIGN
#undef HASH_TABLE__PRIV__INTERNAL_INSERT_FNNAME
#undef HASH_TABLE__PRIV__EXPAND_DEFN
#undef HASH_TABLE__PRIV__EXPAND_SIGN
#undef HASH_TABLE__PRIV__EXPAND_FNNAME
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
#undef HASH_TABLE_MAX_LOAD_FACTOR
#undef COLN_ALLOC_INTERNAL_ASSERT
#undef COLN_INTERNAL_ASSERT
#undef COLN_INTERNAL_DEBUG
#undef COLN_FREE
#undef COLN_ALLOC
#undef COLN_ALLOC_ASSERT
#undef COLN_ALLOC_ASSIGN
#undef COLN_ALLOC_ARG
#undef COLN_ALLOC_DECL
#undef COLN_ALLOC_TYPE
#undef COLN_DATA_SWAP
#undef COLN_DATA_CLEAR
#undef COLN_DATA_MOVE
#undef COLN_DATA_COPY
#undef COLN_DATA_HASH
#undef COLN_DATA_TYPENAME
#undef COLN_IMPL
#undef COLN_HEADER
#undef COLN_CAT
#undef COLN_CAT_
