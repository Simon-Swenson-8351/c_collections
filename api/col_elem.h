#pragma once

#include <stdbool.h>
#include <stddef.h>

struct col_allocator;

/**
 * Copies an element from src to dest
 * @pre allocator is optional
 * @pre dest points to a buffer large enough to store an element of the type for which this function is written
 * @pre src points to a valid instance of an element of the type for which this function is written
 */
typedef bool (*col_elem_cp_fn)(struct col_allocator *allocator, void *dest, void *src);

/**
 * Clears the given element
 * @pre allocator is optional
 * @pre to_clear points to a valid instance of an element of the type for which this function is written
 */
typedef void (*col_elem_clr_fn)(struct col_allocator *allocator, void *to_clear);

/**
 * Determines whether two elements are equal
 * @pre a, b point to valid instances of elements of the type for which this function is written
 */
typedef bool (*col_elem_eq_fn)(void *a, void *b);

/**
 * Compares two elements
 * @pre a, b point to valid instances of elements of the type for which this function is written
 * @return -1 if a < b, 0 if a == b, 1 if a > b
 */
typedef int (*col_elem_cmp_fn)(void *a, void *b);

/**
 * Functions and data used by collections for their internal operations
 * 
 * cp_fn, clr_fn are used for data/buffer operations. eq_fn is used to for searching. cmp_fn is used for sorting and 
 * searching sorted collections. If cp_fn is defined, clr_fn should probably also be defined. (Refer to C++'s "Rule of 
 * Three" for the rationale.)
 */
struct col_elem_metadata
{
    col_elem_cp_fn  cp_fn;     ///< optional, if not present, will use memcpy on copy
    col_elem_clr_fn clr_fn;    ///< optional, if not present, will do nothing on clear
    col_elem_eq_fn  eq_fn;     ///< optional, if not present, will use memcmp
    col_elem_cmp_fn cmp_fn;    ///< optional, if not present, will use memcmp
    size_t          elem_size; ///< required, must be > 0
};

/**
 * Copies an element from src to dest using the given metadata struct
 * 
 * If md->cp_fn is NULL, memcpy will be used.
 * @pre allocator is optional
 * @pre md is required
 * @pre dest points to a buffer large enough to store an element of the type for which this function is written
 * @pre src points to a valid instance of an element of the type for which this function is written
 */
bool col_elem_cp(struct col_allocator *allocator,
                 struct col_elem_metadata *md,
                 void *dest,
                 void *src);

/**
 * Copies n elements from src to dest using the given metadata struct
 * 
 * If md->cp_fn is NULL, memcpy will be used.
 * @pre allocator is optional
 * @pre md is required
 * @pre dest points to a buffer large enough to store n elements of the type for which this function is written
 * @pre src points to n valid instances of elements of the type for which this function is written
 */
bool col_elem_cp_many(struct col_allocator *allocator,
                      struct col_elem_metadata *md,
                      void *dest,
                      void *src,
                      size_t count);

/**
 * Clears the given element using the given metadata struct
 * 
 * If md->clr_fn is NULL, no-op will be performed.
 * @pre allocator is optional
 * @pre md is required
 * @pre to_clear points to a valid instance of an element of the type for which this function is written
 */
void col_elem_clr(struct col_allocator *allocator,
                  struct col_elem_metadata *md,
                  void *to_clear);

/**
 * Clears n given elements using the given metadata struct
 * 
 * If md->clr_fn is NULL, no-op will be performed.
 * @pre allocator is optional
 * @pre md is required
 * @pre to_clear points to n valid instances of an element of the type for which this function is written
 */
void col_elem_clr_many(struct col_allocator *allocator,
                       struct col_elem_metadata *md,
                       void *to_clear,
                       size_t count);

/**
 * Determines whether two elements are equal
 * 
 * If md->eq_fn is NULL and md->cmp_fn is not NULL, md->cmp_fn will be used.
 * If md->eq_fn is NULL and md->cmp_fn is NULL, memcmp will be used.
 * @pre md is required
 * @pre a, b point to valid instances of elements of the type for which this function is written
 */
bool col_elem_eq(struct col_elem_metadata *md,
                 void *a,
                 void *b);

/**
 * Compares two elements
 * 
 * If md->cmp_fn is NULL, memcmp will be used.
 * @pre md is required
 * @pre a, b point to valid instances of elements of the type for which this function is written
 * @return -1 if a < b, 0 if a == b, 1 if a > b
 */
int col_elem_cmp(struct col_elem_metadata *md,
                 void *a,
                 void *b);
