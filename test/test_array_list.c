#include "test_common.c"

#define COLN_DATA_TYPENAME int
#define COLN_DATA_PASS_BY_VAL
#define ARRAY_LIST_TYPENAME int_array_list
#define COLN_HEADER
#define COLN_IMPL

#include "array_list.t.h"

#undef COLN_IMPL
#undef COLN_HEADER
#undef ARRAY_LIST_TYPENAME
#undef COLN_DATA_PASS_BY_VAL
#undef COLN_DATA_TYPENAME

#define COLN_DATA_TYPENAME mat44
#define COLN_DATA_PASS_BY_PTR
#define ARRAY_LIST_TYPENAME mat44_array_list
#define COLN_HEADER
#define COLN_IMPL

#include "array_list.t.h"

#undef COLN_IMPL
#undef COLN_HEADER
#undef ARRAY_LIST_TYPENAME
#undef COLN_DATA_TRIVIAL_BY_PTR
#undef COLN_DATA_TYPENAME

#define COLN_DATA_TYPENAME dyn_str
#define COLN_DATA_PASS_BY_PTR
#define COLN_DATA_COPY dyn_str_copy
#define COLN_DATA_CLEAR dyn_str_clear
#define ARRAY_LIST_TYPENAME dyn_str_array_list
#define COLN_HEADER
#define COLN_IMPL

#include "array_list.t.h"

#undef COLN_IMPL
#undef COLN_HEADER
#undef ARRAY_LIST_TYPENAME
#undef COLN_DATA_CLEAR
#undef COLN_DATA_COPY
#undef COLN_DATA_PASS_BY_PTR
#undef COLN_DATA_TYPENAME

#define COLN_DATA_TYPENAME backrefd_struct
#define COLN_DATA_PASS_BY_PTR
#define COLN_DATA_MOVE backrefd_struct_move
#define COLN_HEADER
#define COLN_IMPL

#include "array_list.t.h"

#undef COLN_IMPL
#undef COLN_HEADER
#undef COLN_DATA_MOVE
#undef COLN_DATA_PASS_BY_PTR
#undef COLN_DATA_TYPENAME

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

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  test_trivial_data_by_val();
  return 0;
}
