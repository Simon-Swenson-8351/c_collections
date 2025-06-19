#define COLN_DATA_TYPE int
#define COLN_TYPE int_array_list
#define COLN_HEADER
#define COLN_IMPL

#include "array_list.t.h"

#include <assert.h>

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  int_array_list my_al;
  assert(int_array_list_init(&my_al, 4) == COLN_RESULT_SUCCESS);
  int tmp = 20;
  assert(int_array_list_push_back(&my_al, &tmp) == COLN_RESULT_SUCCESS);
  assert(my_al.data[0] == 20);
  tmp = 10;
  assert(int_array_list_insert_at(&my_al, &tmp, 0) == COLN_RESULT_SUCCESS);
  assert(my_al.data[0] == 10);
  assert(my_al.data[1] == 20);
  tmp = 30;
  assert(int_array_list_push_back(&my_al, &tmp) == COLN_RESULT_SUCCESS);
  assert(my_al.data[2] == 30);
  tmp = 40;
  assert(int_array_list_push_back(&my_al, &tmp) == COLN_RESULT_SUCCESS);
  assert(my_al.data[3] == 40);
  tmp = 50;
  assert(int_array_list_push_back(&my_al, &tmp) == COLN_RESULT_SUCCESS);
  assert(my_al.data[4] == 50);
  tmp = 70;
  assert(int_array_list_push_back(&my_al, &tmp) == COLN_RESULT_SUCCESS);
  assert(my_al.data[5] == 70);
  tmp = 80;
  assert(int_array_list_push_back(&my_al, &tmp) == COLN_RESULT_SUCCESS);
  assert(my_al.data[6] == 80);
  tmp = 90;
  assert(int_array_list_push_back(&my_al, &tmp) == COLN_RESULT_SUCCESS);
  assert(my_al.data[7] == 90);
  tmp = 100;
  assert(int_array_list_insert_at(&my_al, &tmp, 8) == COLN_RESULT_SUCCESS);
  assert(my_al.data[8] == 100);
  tmp = 60;
  assert(int_array_list_insert_at(&my_al, &tmp, 5) == COLN_RESULT_SUCCESS);
  assert(my_al.len == 10);
  for(ptrdiff_t i = 0; i < 10; i++) assert(my_al.data[i] == (i + 1) * 10);
  int_array_list my_al2;
  assert(int_array_list_copy(&my_al2, &my_al) == COLN_RESULT_SUCCESS);
  assert(my_al2.len == 10);
  for(ptrdiff_t i = 0; i < 10; i++) assert(my_al.data[i] == my_al2.data[i]);
  int_array_list_remove_at(&my_al, &tmp, 0);
  assert(tmp == 10);
  assert(my_al.data[0] == 20);
  int_array_list_remove_at(&my_al, &tmp, 8);
  assert(tmp == 100);
  int_array_list_remove_at(&my_al, &tmp, 3);
  assert(tmp == 50);
  int_array_list_pop_back(&my_al, &tmp);
  assert(tmp == 90);
  int_array_list_pop_back(&my_al, &tmp);
  assert(tmp == 80);
  tmp = 42;
  int_array_list_push_back(&my_al, &tmp);
  tmp = 9999;
  int_array_list_push_back(&my_al, &tmp);
  assert(int_array_list_concatenate(&my_al2, &my_al) == COLN_RESULT_SUCCESS);
  for(ptrdiff_t i = 0; i < 10; i++) assert(my_al2.data[i] == (i + 1) * 10);
  assert(my_al2.data[10] == 20);
  assert(my_al2.data[11] == 30);
  assert(my_al2.data[12] == 40);
  assert(my_al2.data[13] == 60);
  assert(my_al2.data[14] == 70);
  assert(my_al2.data[15] == 42);
  assert(my_al2.data[16] == 9999);
  int_array_list_clear(&my_al2);
  return 0;
}
