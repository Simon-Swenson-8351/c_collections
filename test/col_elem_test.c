#include "col_elem.c"

int(*TEST_FUNCTIONS)(void)[] =
{
    when_cp_is_called_with_cp_fn_ptr_defined_then_it_is_used,
    when_cp_is_called_without_cp_fn_ptr_defined_then_memcpy_is_used,
    
};

int main(int argc, char *argv[])
{

}