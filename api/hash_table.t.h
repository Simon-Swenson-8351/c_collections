#include <stdbool.h>

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

#ifndef COLN_DATA_HASH
#error "Hash table requires a data hash function, COLN_DATA_HASH"
#endif

#ifndef COLN_DATA_COPY
#define COLN_DATA_COPY(dest_ptr, src_ptr) (*(dest_ptr) = *(src_ptr), true)
#define COLN_DATA_COPY_MANY(dest_ptr, src_ptr, count) \
    (memcpy((dest_ptr), (src_ptr), sizeof(COLN_DATA_TYPENAME) * (count)), true)
#endif

#ifndef COLN_DATA_COPY_MANY
#define HASH_TABLE__PRIV__DATA_COPY_MANY_FNNAME \
    COLN_CAT(HASH_TABLE_TYPENAME, _data_copy_many)
#define COLN_DATA_COPY_MANY HASH_TABLE__PRIV__DATA_COPY_MANY_FNNAME
#define HASH_TABLE__PRIV__DATA_COPY_MANY_SIGN \
    static bool HASH_TABLE__PRIV__DATA_COPY_MANY_FNNAME( \
        COLN_DATA_TYPENAME *dest, \
        COLN_DATA_TYPENAME *src, \
        size_t count)
#define HASH_TABLE__PRIV__DATA_COPY_MANY_DECL \
    HASH_TABLE__PRIV__DATA_COPY_MANY_SIGN;
#define HASH_TABLE__PRIV__DATA_COPY_MANY_DEFN \
    HASH_TABLE__PRIV__DATA_COPY_MANY_SIGN \
    { \
        for(ptrdiff_t i = 0; i < count; i++) \
        { \
            if(!COLN_DATA_COPY(dest + i, src + i)) \
            { \
                for(ptrdiff_t j = i - 1; j >= 0; j--) \
                { \
                    COLN_DATA_CLEAR(dest + j); \
                } \
                return false; \
            } \
        } \
        return true; \
    }
#else
#define HASH_TABLE__PRIV__DATA_COPY_MANY_FNNAME
#define HASH_TABLE__PRIV__DATA_COPY_MANY_SIGN
#define HASH_TABLE__PRIV__DATA_COPY_MANY_DECL
#define HASH_TABLE__PRIV__DATA_COPY_MANY_DEFN
#endif

#ifndef COLN_DATA_MOVE
#define COLN_DATA_MOVE(dest_ptr, src_ptr) (*(dest_ptr) = *(src_ptr))
#define COLN_DATA_MOVE_MANY(dest_ptr, src_ptr, count) \
    memcpy((dest_ptr), (src_ptr), sizeof(COLN_DATA_TYPENAME) * (count))
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

#ifndef COLN_DATA_CLEAR
#define COLN_DATA_CLEAR(to_clear_ptr)
#define COLN_DATA_CLEAR_MANY(to_clear_ptr, count)
#endif

#ifndef COLN_DATA_CLEAR_MANY
#define COLN_DATA_CLEAR_MANY(to_clear, count)
    do \
    { \
        for(ptrdiff_t i = 0; i < count; i++) \
        { \
            COLN_DATA_CLEAR((to_clear) + i); \
        } \
    } while(0)
#endif

#ifndef HASH_TABLE_TYPENAME
#define HASH_TABLE_TYPENAME COLN_CAT(COLN_DATA_TYPENAME, _red_black_tree)
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

#if defined(COLN_INTERNAL_DEBUG) && defined(COLN_ALLOC_TYPE)
#define COLN_ALLOC_INTERNAL_ASSERT(x) assert(x)
#else
#define COLN_ALLOC_INTERNAL_ASSERT(x)
#endif

#define HASH_TABLE_DEFN \
    typedef struct HASH_TABLE_TYPENAME \
    { \
        COLN_ALLOC_DECL(allocator) \
        HASH_TABLE_ENTRY_TYPENAME *entries; \
        COLN_DATA_TYPENAME *entries; \
        int *probe_seq_lens; \
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
        size_t hash_val; \
        int probe_seq_len; \
        COLN_DATA_TYPENAME data; \
    } HASH_TABLE_ENTRY_TYPENAME;

#define HASH_TABLE_INIT_FNNAME COLN_CAT(HASH_TABLE_TYPENAME, _init)
#define HASH_TABLE_INIT_SIGN \
    ColnResult HASH_TABLE_INIT_FNNAME(HASH_TABLE_TYPENAME *to_init \
                                      COLN_ALLOC_ARG(allocator), \
                                      int initial_cap_exp)
#define HASH_TABLE_INIT_DECL HASH_TABLE_INIT_SIGN;
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

#define HASH_TABLE_COPY_FNNAME COLN_CAT(HASH_TABLE_TYPENAME, _copy)
#define HASH_TABLE_COPY_SIGN \
    ColnResult HASH_TABLE_COPY_FNNAME(HASH_TABLE_TYPENAME *dest, \
                                      HASH_TABLE_TYPENAME *src)
#define HASH_TABLE_COPY_DECL HASH_TABLE_COPY_SIGN;
#define HASH_TABLE_COPY_DEFN \
    HASH_TABLE_COPY_SIGN \
    { \
        assert(dest); \
        assert(src); \
        if(!(to_init->entries = COLN_ALLOC( \
                allocator, \
                sizeof(HASH_TABLE_ENTRY_TYPENAME) * src->cap))) \
            return COLN_RESULT_ALLOC_FAILED; \
        for(ptrdiff_t i = 0; i < (ptrdiff_t)src->cap; i++) \
        { \
            if(src->entries[i].probe_seq_len < 0) continue; \
            if(!COLN_DATA_COPY(dest->entries + i, src->entries + i)) \
            { \
                for(ptrdiff_t j = i - 1; j >= 0; j--) \
                { \
                    COLN_CLEAR(dest->entries[j]); \
                } \
                COLN_FREE(src->allocator, dest->entries); \
                return COLN_RESULT_COPY_ELEM_FAILED; \
            } \
        } \
        memcpy(dest->probe_seq_lens, src->probe_seq_lens, col_ct_size); \
        COLN_ALLOC_ASSIGN(dest->allocator, src->allocator); \
        dest->cap = src->cap; \
        dest->count = src->count; \
        dest->max_probe_seq_len = src->max_probe_seq_len; \
        return COLN_RESULT_SUCCESS; \
    }

#define HASH_TABLE_CLEAR_FNNAME COLN_CAT(HASH_TABLE_TYPENAME, _clear)
#define HASH_TABLE_CLEAR_SIGN \
    void HASH_TABLE_CLEAR_FNNAME(HASH_TABLE_TYPENAME *to_clear)
#define HASH_TABLE_CLEAR_DECL HASH_TABLE_CLEAR_SIGN;
#define HASH_TABLE_CLEAR_DEFN \
    HASH_TABLE_CLEAR_SIGN \
    { \
        assert(to_clear); \
        for(intptr_t i = 0; i < to_clear->cap; i++) \
        { \
           if(to_clear->probe_seq_lens[i] < 0) continue; \
           COLN_DATA_CLEAR(&(to_clear->entries[i])); \
        } \
        COLN_FREE(to_clear->allocator, to_clear->entries); \
    }

#define HASH_TABLE_INSERT_FNNAME COLN_CAT(HASH_TABLE_TYPENAME, _insert)
#define HASH_TABLE_INSERT_SIGN \
    ColnResult HASH_TABLE_INSERT_FNNAME(HASH_TABLE_TYPENAME *hash_table, \
                                        COLN_DATA_TYPENAME *to_insert)
#define HASH_TABLE_INSERT_DECL HASH_TABLE_INSERT_SIGN;
#define HASH_TABLE_INSERT_DEFN \
    HASH_TABLE_INSERT_SIGN \
    { \
        assert(hash_table); \
        assert(to_insert); \
        float load_factor = (float)(hash_table->cap) / \
            (float)(hash_table->count + 1); \
        if((load_factor > MAX_LOAD_FACTOR) && \
                !HASH_TABLE__PRIV__EXPAND_FNNAME(hash_table)) \
            return COLN_RESULT_ALLOC_FAILED; \
        size_t ins_max_probe_seq = HASH_TABLE__PRIV__INTERNAL_INSERT_FNNAME( \
            hash_table->entries, \
            hash_table->probe_seq_lens, \
            hash_table->cap, \
            to_insert); \
        if(ins_max_probe_seq > hash_table->max_probe_seq_len) \
            hash_table->max_probe_seq_len = ins_max_probe_seq; \
        return COLN_RESULT_SUCCESS; \
    }

#define HASH_TABLE_SEARCH_FNNAME COLN_CAT(HASH_TABLE_TYPENAME, _search)
#define HASH_TABLE_SEARCH_SIGN \
    COLN_DATA_TYPENAME *HASH_TABLE_SEARCH_FNNAME( \
        HASH_TABLE_TYPENAME *hash_table, \
        COLN_DATA_TYPENAME *elem_to_find)
#define HASH_TABLE_SEARCH_DECL HASH_TABLE_SEARCH_SIGN;
#define HASH_TABLE_SEARCH_DEFN \
    HASH_TABLE_SEARCH_SIGN \
    { \
        assert(hash_table); \
        assert(elem_to_find); \
        size_t idx_mask = hash_table->cap - 1; \
        size_t base_idx = COLN_DATA_HASH(elem_to_find) & idx_mask; \
        for(size_t misses = 0; \
                misses <= hash_table->max_probe_seq_len; \
                misses++) \
        { \
            size_t cur_idx = (base_idx + misses) & idx_mask; \
            if()
        } \
    }

#define HASH_TABLE__PRIV__NEW_BUF_FNNAME COLN_CAT(HASH_TABLE_TYPENAME, _new_buf)
#define HASH_TABLE__PRIV__NEW_BUF_SIGN \
    static bool HASH_TABLE__PRIV__NEW_BUF_FNNAME( \
        size_t cap \
        COLN_ALLOC_ARG(allocator), \
        COLN_DATA_TYPENAME **new_entries, \
        int **new_probe_seq_lens)
#ifdef COLN_ALLOC_TYPE
#define HASH_TABLE__PRIV__NEW_BUF_INVOC(cap, \
                                        allocator, \
                                        new_entries, \
                                        new_probe_seq_lens) \
    HASH_TABLE__PRIV__NEW_BUF_FNNAME((cap), \
                                     (allocator), \
                                     (new_entries), \
                                     (new_probe_seq_lens))
#else
#define HASH_TABLE__PRIV__NEW_BUF_INVOC(cap, \
                                        allocator, \
                                        new_entries, \
                                        new_probe_seq_lens) \
    HASH_TABLE__PRIV__NEW_BUF_FNNAME((cap), \
                                     (new_entries), \
                                     (new_probe_seq_lens))
#endif
#define HASH_TABLE__PRIV__NEW_BUF_DECL HASH_TABLE__PRIV__NEW_BUF_SIGN;
#define HASH_TABLE__PRIV__NEW_BUF_DEFN \
    HASH_TABLE__PRIV__NEW_BUF_SIGN \
    { \
        COLN_INTERNAL_ASSERT(cap > 0); \
        COLN_ALLOC_INTERNAL_ASSERT(allocator); \
        COLN_INTERNAL_ASSERT(new_entries); \
        COLN_INTERNAL_ASSERT(new_probe_seq_lens); \
        size_t alloc_size = sizeof(COLN_DATA_TYPENAME) * cap; \
        alloc_size = COLN_ALIGN(alignof(int), alloc_size); \
        size_t probe_seq_lens_offset = alloc_size; \
        alloc_size += sizeof(int) * cap; \
        *new_entries = COLN_ALLOC(allocator, alloc_size); \
        if(!(*new_entries)) return false;
        *new_probe_seq_lens = (int *) \
            ((unsigned char *)(*new_entries) + probe_seq_lens_offset); \
        return true;
    }

#define HASH_TABLE__PRIV__EXPAND_FNNAME COLN_CAT(HASH_TABLE_TYPENAME, _expand)
#define HASH_TABLE__PRIV__EXPAND_SIGN \
    static bool HASH_TABLE__PRIV__EXPAND_FNNAME(HASH_TABLE_TYPENAME *to_expand)
#define HASH_TABLE__PRIV__EXPAND_DECL HASH_TABLE__PRIV__EXPAND_SIGN;
#define HASH_TABLE__PRIV__EXPAND_DEFN \
    HASH_TABLE__PRIV__EXPAND_SIGN \
    { \
        COLN_INTERNAL_ASSERT(to_expand); \
        size_t new_cap = to_expand->cap << 1; \
        assert(new_cap > to_expand->cap); \
        COLN_DATA_TYPENAME *new_entries;
        int *new_probe_seq_lens;
        if(!HASH_TABLE__PRIV__NEW_BUF_INVOC(new_cap, \
                                        to_expand->allocator, \
                                        &new_entries, \
                                        &new_probe_seq_lens)) \
            return false; \
        size_t max_probe_seq_len = 0; \
        for(intptr_t i = 0; i < (intptr_t)(to_expand->cap); i++) \
        { \
            if(to_expand->probe_seq_lens[i] < 0) continue; \
            size_t ins_probe_seq_len = \
                HASH_TABLE__PRIV__INTERNAL_INSERT_FNNAME( \
                    new_entries, \
                    new_probe_seq_lens, \
                    new_cap, \
                    to_expand->entries + i); \
            if(ins_probe_seq_len > max_probe_seq_len) \
                max_probe_seq_len = ins_probe_seq_len; \
        } \
        COLN_FREE(to_expand->allocator, to_expand->entries); \
        to_expand->entries = new_entries; \
        to_expand->probe_seq_lens = new_probe_seq_lens; \
        to_expand->cap = new_cap; \
        to_expand->max_probe_seq_len = max_probe_seq_len; \
        return true; \
    }

#define HASH_TABLE__PRIV__INTERNAL_INSERT_FNNAME \
    COLN_CAT(HASH_TABLE_TYPENAME, _internal_insert)
#define HASH_TABLE__PRIV__INTERNAL_INSERT_SIGN \
    static size_t HASH_TABLE__PRIV__INTERNAL_INSERT_FNNAME( \
        COLN_DATA_TYPENAME *entries, \
        int *probe_seq_lens, \
        size_t cap, \
        COLN_DATA_TYPENAME *to_insert)
#define HASH_TABLE__PRIV__INTERNAL_INSERT_DECL \
    HASH_TABLE__PRIV__INTERNAL_INSERT_SIGN;
#define HASH_TABLE__PRIV__INTERNAL_INSERT_DEFN \
    HASH_TABLE__PRIV__INTERNAL_INSERT_SIGN \
    { \
        COLN_INTERNAL_ASSERT(entries); \
        COLN_INTERNAL_ASSERT(probe_seq_lens); \
        COLN_INTERNAL_ASSERT(IS_POW_2(cap)); \
        COLN_INTERNAL_ASSERT(to_insert); \
        COLN_INTERNAL_ASSERT(HASH_TABLE__PRIV__HAS_SLOT_FNNAME(probe_seq_lens, \
                                                               cap)); \
        /* precond: the table has at least one open slot */ \
        /* should we test that here in an assert? */ \
        size_t mod_mask = cap - 1; \
        size_t index = COLN_DATA_HASH(to_insert) & mod_mask; \
        int misses = 0; \
        int result = 0; \
        while(true) \
        { \
            if(probe_seq_lens[index] < 0) \
            { \
                /* the slot is not in use */ \
                probe_seq_lens[index] = misses; \
                COLN_DATA_MOVE(entries + index, to_insert); \
                return result; \
            } \
            /* the slot is in use */ \
            if(misses > probe_seq_lens[index]) \
            { \
                /* robin hood */ \
                COLN_DATA_SWAP(entries + index, to_insert); \
                int swap_var = misses; \
                misses = probe_seq_lens[index]; \
                probe_seq_lens[index] = misses; \
            } \
            if(++misses > result) result = misses; \
            index = (index + 1) & mod_mask; \
        } \
    }

#ifdef COLN_INTERAL_DEBUG
#define HASH_TABLE__PRIV__HAS_SLOT_FNNAME COLN_CAT(HASH_TABLE_TYPENAME, \
                                                   _has_slot)
#define HASH_TABLE__PRIV__HAS_SLOT_SIGN \
    static bool HASH_TABLE__PRIV__HAS_SLOT_FNNAME(int *probe_seq_lens, \
                                                  size_t cap)
#define HASH_TABLE__PRIV__HAS_SLOT_DECL HASH_TABLE__PRIV__HAS_SLOT_SIGN;
#define HASH_TABLE__PRIV__HAS_SLOT_DEFN \
    HASH_TABLE__PRIV__HAS_SLOT_SIGN \
    { \
        for(ptrdiff_t i = 0; i < (ptrdiff_t)cap; i++) \
            if(probe_seq_lens[i] < 0) return true; \
        return false; \
    }
#else
#endif

#ifdef COLN_HEADER
#endif

#ifdef COLN_IMPL
#define COLN_ALIGN(dtype_alignment, given_bytes) \
    (((given_bytes) + (dtype_alignment) - 1) & ~((dtype_alignment) - 1))
#define MAX_LOAD_FACTOR 0.75
#define MAX_COLLISION_COUNT 15
#define IS_POW_2(x) ((x) && !((x) & ((x) - 1)))

#undef IS_POW_2
#undef MAX_COLLISION_COUNT
#undef MAX_LOAD_FACTOR
#undef COLN_ALIGN
#endif
