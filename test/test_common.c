#include <assert.h>
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

bool dyn_str_copy(dyn_str *dest, dyn_str *src)
{
  assert(dest);
  assert(dest->data);
  assert(src);
  assert(src->data);
  size_t buf_size = strlen(src->data) + 1;
  dest->data = malloc(buf_size);
  if(!(dest->data)) return false;
  memcpy(dest->data, src->data, buf_size);
  return true;
}

void dyn_str_clear(dyn_str *to_clear)
{
  assert(to_clear);
  assert(to_clear->data);
  free(to_clear->data);
}

int dyn_str_cmp(dyn_str *a, dyn_str *b)
{
  assert(a);
  assert(a->data);
  assert(b);
  assert(b->data);
  return strcmp(a->data, b->data);
}
