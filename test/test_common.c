#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int int_compare(int a, int b) { return (a < b ? -1 : (a == b ? 0 : 1)); }
bool int_equals(int a, int b) { return a == b; }

typedef struct vec4
{
  float data[4];
} vec4;

float vec4_mag(vec4 *v)
{
  assert(v);
  return sqrtf(v->data[0] * v->data[0] + 
               v->data[1] * v->data[1] + 
               v->data[2] * v->data[2] + 
               v->data[3] * v->data[3]);
}

int vec4_mag_compare(vec4 *a, vec4 *b)
{
  assert(a);
  assert(b);
  float a_mag = vec4_mag(a);
  float b_mag = vec4_mag(b);
  return (a_mag > b_mag ? 1 : (a_mag == b_mag ? 0 : -1));
}

bool vec4_equals(vec4 *a, vec4 *b)
{
  assert(a);
  assert(b);
  for(int i = 0; i < 4; i++) if(a->data[i] != b->data[i]) return false;
  return true;
}

typedef struct mat44
{
  float data[16];
} mat44;

#define MAT44_IDX(matrix, row, column) ((matrix).data[(row) * 4 + (column)])

bool mat44_equals(mat44 *a, mat44 *b)
{
  assert(a);
  assert(b);
  for(int i = 0; i < 16; i++) if(a->data[i] != b->data[i]) return false;
  return true;
}

typedef struct backrefd_struct backrefd_struct;
struct backrefd_struct
{
  backrefd_struct **backref;
  int value;
};

void backrefd_struct_move(backrefd_struct *dest, backrefd_struct *src)
{
  assert(dest);
  assert(src);
  *dest = *src;
  if(dest->backref) *(dest->backref) = dest;
}

int backrefd_struct_compare(backrefd_struct *a, backrefd_struct *b)
{
  assert(a);
  assert(b);
  return (a->value < b->value ? -1 : (a->value == b->value ? 0 : 1));
}

bool backrefd_struct_equals(backrefd_struct *a, backrefd_struct *b)
{
  assert(a);
  assert(b);
  return a->value == b->value;
}

typedef struct dyn_str
{
  char *data;
} dyn_str;

bool dyn_str_copy(dyn_str *dest, dyn_str src)
{
  assert(dest);
  assert(dest->data);
  assert(src.data);
  size_t buf_size = strlen(src.data) + 1;
  dest->data = malloc(buf_size);
  if(!(dest->data)) return false;
  memcpy(dest->data, src.data, buf_size);
  return true;
}

void dyn_str_clear(dyn_str to_clear)
{
  assert(to_clear.data);
  free(to_clear.data);
}

int dyn_str_cmp(dyn_str a, dyn_str b)
{
  assert(a.data);
  assert(b.data);
  return strcmp(a.data, b.data);
}

typedef struct aug_int_call_counts
{
  int move_count;
  int copy_count;
  int clear_count;
  int compare_count;
  int equals_count;
  int hash_count;
  int digit_count;
} aug_int_call_counts;

typedef struct aug_int
{
  int i;
  aug_int_call_counts *counter;
} aug_int;

aug_int aug_int_move_by_val(aug_int src)
{
  src.counter->move_count++;
  return src;
}

void aug_int_move_by_ptr(aug_int *dest, aug_int *src)
{
  src->counter->move_count++;
  *dest = *src;
}

bool aug_int_copy_by_val(aug_int *dest, aug_int src)
{
  assert(dest);
  src.counter->copy_count++;
  *dest = src;
  return true;
}

bool aug_int_copy_by_ptr(aug_int *dest, aug_int *src)
{
  assert(dest);
  assert(src);
  src->counter->copy_count++;
  *dest = *src;
  return true;
}

void aug_int_clear_by_val(aug_int to_clear)
{
  to_clear.counter->clear_count++;
}

void aug_int_clear_by_ptr(aug_int *to_clear)
{
  assert(to_clear);
  to_clear->counter->clear_count++;
}

int aug_int_compare_by_val(aug_int a, aug_int b)
{
  a.counter->compare_count++;
  b.counter->compare_count++;
  return (a.i > b.i) ? 1 : ((a.i == b.i) ? 0 : -1);
}

int aug_int_compare_by_ptr(aug_int *a, aug_int *b)
{
  assert(a);
  assert(b);
  a->counter->compare_count++;
  b->counter->compare_count++;
  return (a->i > b->i) ? 1 : ((a->i == b->i) ? 0 : -1);
}

bool aug_int_equals_by_val(aug_int a, aug_int b)
{
  a.counter->equals_count++;
  b.counter->equals_count++;
  return a.i == b.i;
}

bool aug_int_equals_by_ptr(aug_int *a, aug_int *b)
{
  assert(a);
  assert(b);
  a->counter->equals_count++;
  b->counter->equals_count++;
  return a->i == b->i;
}

size_t aug_int_hash_by_val(aug_int to_hash)
{
  to_hash.counter->hash_count++;
  return (size_t)(to_hash.i);
}

size_t aug_int_hash_by_ptr(aug_int *to_hash)
{
  assert(to_hash);
  to_hash->counter->hash_count++;
  return (size_t)(to_hash->i);
}

bool aug_int_digit_by_val(aug_int a, unsigned int digit)
{
  a.counter->digit_count++;
  return (a.i >> digit) & 1;
}

bool aug_int_digit_by_ptr(aug_int *a, unsigned int digit)
{
  assert(a);
  a->counter->digit_count++;
  return (a->i >> digit) & 1;
}

typedef struct linear_allocator
{
  uint8_t *buf;
  size_t buf_cap;
  size_t allocated;
} linear_allocator;

#define UNIVERSALIZE_SIZE(size) \
  (((size) + sizeof(void *) - 1) & ~(sizeof(void *) - 1))

bool linear_allocator_init(linear_allocator *to_init, size_t cap)
{
  assert(to_init);
  assert(cap > 0);
  cap = UNIVERSALIZE_SIZE(cap);
  to_init->buf = malloc(cap);
  if(!(to_init->buf)) return false;
  to_init->buf_cap = cap;
  to_init->allocated = 0;
  return true;
}

void linear_allocator_clear(linear_allocator *to_clear)
{
  assert(to_clear);
  free(to_clear->buf);
}

void *linear_allocator_alloc(linear_allocator *alloc, size_t to_alloc)
{
  assert(alloc);
  to_alloc = UNIVERSALIZE_SIZE(to_alloc);
  if(alloc->allocated + to_alloc > alloc->buf_cap) return NULL;
  void *result = alloc->buf + alloc->allocated;
  alloc->allocated += to_alloc;
  return result;
}

typedef struct slot_segment_header
{
  struct slot_segment_header *next_segment;
} slot_segment_header;
#define SLOT_SEGMENT_HEADER_SIZE UNIVERSALIZE_SIZE(sizeof(slot_segment_header))

typedef struct slot_header
{
  struct slot_header *next_free;
} slot_header;
#define SLOT_HEADER_SIZE UNIVERSALIZE_SIZE(sizeof(slot_header))

typedef struct dynamic_slot_allocator
{
  slot_segment_header *first_segment;
  slot_header *first_free;
  size_t slot_size;
  size_t used;
  size_t cap;
} dynamic_slot_allocator;

bool dynamic_slot_allocator_expand(dynamic_slot_allocator *to_expand,
                                   size_t slots_to_add)
{
  assert(to_expand);
  assert(slots_to_add > 0);
  size_t inc = SLOT_HEADER_SIZE + to_expand->slot_size;
  slot_segment_header *new_segment = malloc(SLOT_SEGMENT_HEADER_SIZE + 
    slots_to_add * inc);
  if(!new_segment) return false;
  new_segment->next_segment = to_expand->first_segment;
  to_expand->first_segment = new_segment;
  to_expand->cap += slots_to_add;
  uint8_t *base = (uint8_t *)new_segment + SLOT_SEGMENT_HEADER_SIZE;
  for(size_t i = 0; i < slots_to_add; i++)
  {
    slot_header *cur = (slot_header *)(base + i * inc);
    cur->next_free = to_expand->first_free;
    to_expand->first_free = cur;
  }
  return true;
}

bool dynamic_slot_allocator_init(dynamic_slot_allocator *to_init,
                                 size_t slot_size,
                                 size_t initial_cap)
{
  assert(to_init);
  assert(slot_size > 0);
  assert(initial_cap > 0);
  to_init->first_segment = NULL;
  to_init->first_free = NULL;
  to_init->slot_size = UNIVERSALIZE_SIZE(slot_size);
  to_init->used = 0;
  to_init->cap = 0;
  return dynamic_slot_allocator_expand(to_init, initial_cap);
}

void dynamic_slot_allocator_clear(dynamic_slot_allocator *to_clear)
{
  assert(to_clear);
  slot_segment_header *cur = to_clear->first_segment;
  while(cur)
  {
    slot_segment_header *next = cur->next_segment;
    free(cur);
    cur = next;
  }
}

void *dynamic_slot_allocator_alloc(dynamic_slot_allocator *alloc, size_t size)
{
  assert(size <= alloc->slot_size);
  if(!(alloc->first_free) && !dynamic_slot_allocator_expand(alloc, alloc->cap))
    return NULL;
  assert(alloc->first_free);
  void *result = (uint8_t *)(alloc->first_free) + SLOT_HEADER_SIZE;
  alloc->first_free = alloc->first_free->next_free;
  alloc->used++;
  return result;
}

#include <stdio.h>
void dynamic_slot_allocator_free(dynamic_slot_allocator *alloc, void *to_free)
{
  assert(alloc);
  assert(to_free);
  ((slot_header *)to_free)->next_free = alloc->first_free;
  alloc->first_free = (slot_header *)to_free;
  alloc->used--;
}

typedef struct stdalloc_wrapper
{
  int alloc_call_count;
  int realloc_call_count;
  int free_call_count;
} stdalloc_wrapper;

void *stdalloc_wrapper_alloc(stdalloc_wrapper *alloc, size_t size)
{
  assert(alloc);
  alloc->alloc_call_count++;
  return malloc(size);
}

void stdalloc_wrapper_free(stdalloc_wrapper *alloc, void *to_free)
{
  assert(alloc);
  alloc->free_call_count++;
  free(to_free);
}

void *stdalloc_wrapper_realloc(stdalloc_wrapper *alloc, void *old, size_t new_size)
{
  assert(alloc);
  alloc->realloc_call_count++;
  return realloc(old, new_size);
}

#undef UNIVERSALIZE_SIZE
