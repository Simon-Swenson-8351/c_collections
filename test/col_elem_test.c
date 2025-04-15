#include "col_elem.c"

int(*TEST_FUNCTIONS)(void)[] =
{
    when_cp_is_called_with_cp_fn_ptr_defined_then_it_is_used,
    when_cp_is_called_without_cp_fn_ptr_defined_then_memcpy_is_used,
    
};

#define BEGIN printf("%s BEGAN\n", __func__)
#define PASS printf("%s \033[32mPASSED\033[0m\n", __func__)
#define FAIL(x) \
    do \
    { \
        printf("%s \033[31mFAILED\033[0m: %s\n", __func__, x); \
        return 1; \
    } while(0)

static int fail(char *reason)
{
    printf("Failed: %s\n", reason);
    return 1;
}

int main(int argc, char *argv[])
{
    printf("Begin test suite "__FILE__"\n");
    for(size_t i = 0; i < sizeof(TEST_FUNCTIONS)/sizeof(TEST_FUNCTIONS[0]); i++)
    {

    }
}

int WHEN_cp_is_called_WITH_cp_fn_ptr_defined_THEN_it_is_used(void)
{
    BEGIN;

    PASS;
}
