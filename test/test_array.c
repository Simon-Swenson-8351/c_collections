#include "test_common.c"

// Test all by-value code-gen.
#define DATA_TYPENAME int
#define DATA_PASS_BY_VAL
#define DATA_COMPARE int_compare
#define DATA_EQUALS int_equals
#define DATA_DIGIT_LEN (sizeof(int) * 8)
#define DATA_DIGIT(data, digit) (((data) >> (digit)) & 1)
#define ARRAY_HEADER
#define ARRAY_IMPL

#include "array.t.h"

#undef ARRAY_IMPL
#undef ARRAY_HEADER
#undef DATA_EQUALS
#undef DATA_COMPARE
#undef DATA_PASS_BY_VAL
#undef DATA_TYPENAME

// Test conditional code-gen. Nothing should be generated here.
#define DATA_TYPENAME mat44
#define DATA_PASS_BY_PTR
#define ARRAY_HEADER
#define ARRAY_IMPL

#include "array.t.h"

#undef ARRAY_IMPL
#undef ARRAY_HEADER
#undef DATA_PASS_BY_VAL
#undef DATA_TYPENAME

// Test by-pointer code-gen without a custom move function.
#define DATA_TYPENAME vec4
#define DATA_PASS_BY_PTR
#define DATA_COMPARE vec4_mag_compare
#define ARRAY_HEADER
#define ARRAY_IMPL

#include "array.t.h"

#undef ARRAY_IMPL
#undef ARRAY_HEADER
#undef DATA_COMPARE
#undef DATA_PASS_BY_PTR
#undef DATA_TYPENAME

// Test by-pointer code-gen with a custom move function.
#define DATA_TYPENAME backrefd_struct
#define DATA_PASS_BY_PTR
#define DATA_MOVE backrefd_struct_move
#define DATA_COMPARE backrefd_struct_compare
#define DATA_EQUALS backrefd_struct_equals
#define ARRAY_HEADER
#define ARRAY_IMPL

#include "array.t.h"

#undef ARRAY_IMPL
#undef ARRAY_HEADER
#undef DATA_EQUALS
#undef DATA_COMPARE
#undef DATA_MOVE
#undef DATA_PASS_BY_PTR
#undef DATA_TYPENAME

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  int my_array[1024];
  int my_array2[1024];
  size_t needle_idx = 
    (sizeof(my_array)/sizeof(my_array[0])) * 3 / 4;
  int needle;
  srand(42);
  for(size_t i = 0; i < sizeof(my_array)/sizeof(my_array[0]); i++)
  {
    my_array[i] = rand();
    my_array2[i] = my_array[i];
  }
  needle = my_array[needle_idx];
  int bad_needle = rand();
  while(true)
  {
    size_t i;
    for(i = 0; i < sizeof(my_array)/sizeof(my_array[0]); i++)
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
  assert(found_needle_idx == (ptrdiff_t)needle_idx);
  ptrdiff_t found_bad_needle_idx = int_array_linear_search(
    my_array,
    sizeof(my_array)/sizeof(my_array[0]),
    bad_needle);
  assert(found_bad_needle_idx < 0);
  int_array_quick_sort(my_array, sizeof(my_array)/sizeof(my_array[0]));
  int_array_radix_sort(my_array2, sizeof(my_array)/sizeof(my_array[0]));
  found_needle_idx = int_array_binary_search(
    my_array,
    sizeof(my_array)/sizeof(my_array[0]),
    needle);
  assert(my_array[found_needle_idx] == needle);
  found_needle_idx = int_array_binary_search(
    my_array2,
    sizeof(my_array)/sizeof(my_array[0]),
    needle);
  assert(my_array2[found_needle_idx] == needle);
  found_bad_needle_idx = int_array_binary_search(
    my_array,
    sizeof(my_array)/sizeof(my_array[0]),
    bad_needle);
  assert(found_bad_needle_idx < 0);
  found_bad_needle_idx = int_array_binary_search(
    my_array2,
    sizeof(my_array)/sizeof(my_array[0]),
    bad_needle);
  assert(found_bad_needle_idx < 0);
  return 0;
}
