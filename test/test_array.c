#include "test_common.c"

// Test all by-value code-gen.
#define COLN_DATA_TYPENAME int
#define COLN_DATA_PASS_BY_VAL
#define COLN_DATA_COMPARE int_compare
#define COLN_DATA_EQUALS int_equals
#define COLN_HEADER
#define COLN_IMPL

#include "array.t.h"

#undef COLN_IMPL
#undef COLN_HEADER
#undef COLN_DATA_EQUALS
#undef COLN_DATA_COMPARE
#undef COLN_DATA_PASS_BY_VAL
#undef COLN_DATA_TYPENAME

// Test conditional code-gen. Nothing should be generated here.
#define COLN_DATA_TYPENAME mat44
#define COLN_DATA_PASS_BY_PTR
#define COLN_HEADER
#define COLN_IMPL

#include "array.t.h"

#undef COLN_IMPL
#undef COLN_HEADER
#undef COLN_DATA_PASS_BY_VAL
#undef COLN_DATA_TYPENAME

// Test by-pointer code-gen without a custom move function.
#define COLN_DATA_TYPENAME vec4
#define COLN_DATA_PASS_BY_PTR
#define COLN_DATA_COMPARE vec4_mag_compare
#define COLN_HEADER
#define COLN_IMPL

#include "array.t.h"

#undef COLN_IMPL
#undef COLN_HEADER
#undef COLN_DATA_COMPARE
#undef COLN_DATA_PASS_BY_PTR
#undef COLN_DATA_TYPENAME

// Test by-pointer code-gen with a custom move function.
#define COLN_DATA_TYPENAME backrefd_struct
#define COLN_DATA_PASS_BY_PTR
#define COLN_DATA_MOVE backrefd_struct_move
#define COLN_DATA_COMPARE backrefd_struct_compare
#define COLN_DATA_EQUALS backrefd_struct_equals
#define COLN_HEADER
#define COLN_IMPL

#include "array.t.h"

#undef COLN_IMPL
#undef COLN_HEADER
#undef COLN_DATA_EQUALS
#undef COLN_DATA_COMPARE
#undef COLN_DATA_MOVE
#undef COLN_DATA_PASS_BY_PTR
#undef COLN_DATA_TYPENAME

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  int my_array[1024];
  ptrdiff_t needle_idx = 
    (ptrdiff_t)(sizeof(my_array)/sizeof(my_array[0]) / 4 * 3);
  int needle;
  srand(42);
  for(ptrdiff_t i = 0;
      i < (ptrdiff_t)(sizeof(my_array)/sizeof(my_array[0]));
      i++)
  {
    my_array[i] = rand();
  }
  needle = my_array[needle_idx];
  int bad_needle = rand();
  while(true)
  {
    ptrdiff_t i;
    for(i = 0; i < (ptrdiff_t)(sizeof(my_array)/sizeof(my_array[0])); i++)
    {
      if(bad_needle == my_array[i])
      {
        bad_needle = rand();
        break;
      }
    }
    if(i == sizeof(my_array)/sizeof(my_array[0])) break;
  }
  ptrdiff_t found_needle_idx = int_array_linear_search(
    my_array,
    sizeof(my_array)/sizeof(my_array[0]),
    needle);
  assert(found_needle_idx == needle_idx);
  ptrdiff_t found_bad_needle_idx = int_array_linear_search(
    my_array,
    sizeof(my_array)/sizeof(my_array[0]),
    bad_needle);
  assert(found_bad_needle_idx < 0);
  int_array_quick_sort(my_array, sizeof(my_array)/sizeof(my_array[0]));
  found_needle_idx = int_array_binary_search(
    my_array,
    sizeof(my_array)/sizeof(my_array[0]),
    needle);
  assert(my_array[found_needle_idx] == needle);
  found_bad_needle_idx = int_array_binary_search(
    my_array,
    sizeof(my_array)/sizeof(my_array[0]),
    bad_needle);
  assert(found_bad_needle_idx < 0);
  return 0;
}

#undef COLN_DATA_COMPARE
