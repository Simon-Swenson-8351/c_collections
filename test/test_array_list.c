#include "test_common.c"

void linear_allocator_free_dummy(linear_allocator *alloc, void *to_free)
{
  (void)alloc;
  (void)to_free;
}

#define DATA_TYPENAME int
#define DATA_PASS_BY_VAL
#define ARRAY_LIST_TYPENAME int_array_list
#define ARRAY_LIST_HEADER
#define ARRAY_LIST_IMPL

#include "array_list.t.h"

#undef ARRAY_LIST_IMPL
#undef ARRAY_LIST_HEADER
#undef ARRAY_LIST_TYPENAME
#undef DATA_PASS_BY_VAL
#undef DATA_TYPENAME

#define DATA_TYPENAME mat44
#define DATA_PASS_BY_PTR
#define ALLOC_TYPENAME linear_allocator
#define ALLOC_ALLOC linear_allocator_alloc
#define ALLOC_FREE linear_allocator_free_dummy
#define ARRAY_LIST_TYPENAME mat44_array_list
#define ARRAY_LIST_HEADER
#define ARRAY_LIST_IMPL

#include "array_list.t.h"

#undef ARRAY_LIST_IMPL
#undef ARRAY_LIST_HEADER
#undef ARRAY_LIST_TYPENAME
#undef ALLOC_FREE
#undef ALLOC_ALLOC
#undef ALLOC_TYPENAME
#undef DATA_PASS_BY_PTR
#undef DATA_TYPENAME

#define DATA_TYPENAME dyn_str
#define DATA_PASS_BY_VAL
#define DATA_COPY dyn_str_copy
#define DATA_CLEAR dyn_str_clear
#define ARRAY_LIST_TYPENAME dyn_str_array_list
#define ARRAY_LIST_HEADER
#define ARRAY_LIST_IMPL

#include "array_list.t.h"

#undef ARRAY_LIST_IMPL
#undef ARRAY_LIST_HEADER
#undef ARRAY_LIST_TYPENAME
#undef DATA_CLEAR
#undef DATA_COPY
#undef DATA_PASS_BY_VAL
#undef DATA_TYPENAME

#define DATA_TYPENAME backrefd_struct
#define DATA_PASS_BY_PTR
#define DATA_MOVE backrefd_struct_move
#define ARRAY_LIST_HEADER
#define ARRAY_LIST_IMPL

#include "array_list.t.h"

#undef ARRAY_LIST_IMPL
#undef ARRAY_LIST_HEADER
#undef DATA_MOVE
#undef DATA_PASS_BY_PTR
#undef DATA_TYPENAME

#include <assert.h>
#include <stdio.h>


void test_trivial_data_by_val(void)
{
  int_array_list my_al;
  assert(int_array_list_init(&my_al, 4) == COLN_RESULT_SUCCESS);
  int tmp = 20;
  assert(int_array_list_push_back(&my_al, tmp) == COLN_RESULT_SUCCESS);
  assert(my_al.data[0] == 20);
  tmp = 10;
  assert(int_array_list_insert_at(&my_al, tmp, 0) == COLN_RESULT_SUCCESS);
  assert(my_al.data[0] == 10);
  assert(my_al.data[1] == 20);
  tmp = 30;
  assert(int_array_list_push_back(&my_al, tmp) == COLN_RESULT_SUCCESS);
  assert(my_al.data[2] == 30);
  tmp = 40;
  assert(int_array_list_push_back(&my_al, tmp) == COLN_RESULT_SUCCESS);
  assert(my_al.data[3] == 40);
  tmp = 50;
  assert(int_array_list_push_back(&my_al, tmp) == COLN_RESULT_SUCCESS);
  assert(my_al.data[4] == 50);
  tmp = 70;
  assert(int_array_list_push_back(&my_al, tmp) == COLN_RESULT_SUCCESS);
  assert(my_al.data[5] == 70);
  tmp = 80;
  assert(int_array_list_push_back(&my_al, tmp) == COLN_RESULT_SUCCESS);
  assert(my_al.data[6] == 80);
  tmp = 90;
  assert(int_array_list_push_back(&my_al, tmp) == COLN_RESULT_SUCCESS);
  assert(my_al.data[7] == 90);
  tmp = 100;
  assert(int_array_list_insert_at(&my_al, tmp, 8) == COLN_RESULT_SUCCESS);
  assert(my_al.data[8] == 100);
  tmp = 60;
  assert(int_array_list_insert_at(&my_al, tmp, 5) == COLN_RESULT_SUCCESS);
  assert(my_al.len == 10);
  for(ptrdiff_t i = 0; i < 10; i++) assert(my_al.data[i] == (i + 1) * 10);
  int_array_list my_al2;
  assert(int_array_list_copy(&my_al2, &my_al) == COLN_RESULT_SUCCESS);
  assert(my_al2.len == 10);
  for(ptrdiff_t i = 0; i < 10; i++) assert(my_al.data[i] == my_al2.data[i]);
  tmp = int_array_list_remove_at(&my_al, 0);
  assert(tmp == 10);
  assert(my_al.data[0] == 20);
  tmp = int_array_list_remove_at(&my_al, 8);
  assert(tmp == 100);
  tmp = int_array_list_remove_at(&my_al, 3);
  assert(tmp == 50);
  tmp = int_array_list_pop_back(&my_al);
  assert(tmp == 90);
  tmp = int_array_list_pop_back(&my_al);
  assert(tmp == 80);
  tmp = 42;
  int_array_list_push_back(&my_al, tmp);
  tmp = 9999;
  int_array_list_push_back(&my_al, tmp);
  assert(int_array_list_cat(&my_al2, &my_al) == COLN_RESULT_SUCCESS);
  for(ptrdiff_t i = 0; i < 10; i++) assert(my_al2.data[i] == (i + 1) * 10);
  assert(my_al2.data[10] == 20);
  assert(my_al2.data[11] == 30);
  assert(my_al2.data[12] == 40);
  assert(my_al2.data[13] == 60);
  assert(my_al2.data[14] == 70);
  assert(my_al2.data[15] == 42);
  assert(my_al2.data[16] == 9999);
  int_array_list_clear(&my_al2);
}

void test_with_alloc(void)
{
  #define POP_MAT(mat, mult) \
    do \
    { \
      for(int row = 0; row < 4; row++) \
        for(int col = 0; col < 4; col++) \
          MAT44_IDX(mat, row, col) = mult * (4 * row + col); \
    } while(0)
  linear_allocator alloc;
  assert(linear_allocator_init(&alloc, sizeof(mat44) * 4));
  mat44_array_list my_al;
  assert(!mat44_array_list_init(&my_al, &alloc, 1));
  mat44 tmp;
  POP_MAT(tmp, 2);
  assert(!mat44_array_list_push_back(&my_al, &tmp));
  POP_MAT(tmp, 3);
  assert(!mat44_array_list_push_back(&my_al, &tmp));
  POP_MAT(tmp, 5);
  assert(mat44_array_list_push_back(&my_al, &tmp) == COLN_RESULT_ALLOC_FAILED);
  mat44_array_list_clear(&my_al);
  linear_allocator_clear(&alloc);
  #undef POP_MAT
}

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  test_trivial_data_by_val();
  test_with_alloc();
  return 0;
}
