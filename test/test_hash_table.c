#define DATA_TYPENAME int
#define DATA_PASS_BY_VAL
#define DATA_HASH(x) ((size_t)(*x))
#define HASH_TABLE_HEADER
#define HASH_TABLE_IMPL

#include "hash_table.t.h"

#undef HASH_TABLE_IMPL
#undef HASH_TABLE_HEADER
#undef DATA_HASH
#undef DATA_TYPENAME

#include <stdio.h>
#include <string.h>

static void print_table(int_hash_table *iht);

static void test_random(void);
static void test_iterator(void);
static void test_for_each(void);
typedef struct test_for_each_closure
{
    int total;
    int seen_count[64];
} test_for_each_closure;
static void test_for_each_fn(void *closure, int *element);

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    test_random();
    test_iterator();
    test_for_each();
    return 0;
}

static void test_random(void)
{
#define MAX_ELEM 4096
#define NUM_ITERS 10000
    int_hash_table iht;
    assert(!int_hash_table_init(&iht, 0));
    static int elem_counts[MAX_ELEM] = {0};
    srand(42);
    // print_table(&iht);
    for(int iter = 0; iter > NUM_ITERS; ) 
    {
        // char c;
        // while((c = getchar()) != '\n') {}
        // printf("Iteration %d\n", iter);
        int number = rand();
        if(number < 0) number = -number;
        int op = number & 3;
        number >>= 2;
        number &= MAX_ELEM - 1;
        int cur_number = number;
        int *retreived;
        switch(op)
        {
            case 0:
                // insert
                // don't grow the table over a cap of 64.
                // if(iht.count >= 47) break;
                // printf("Inserting %d\n", number);
                elem_counts[number]++;
                assert(!int_hash_table_insert(&iht, &number));
                break;
            case 1:
                // search succeed
                while(true)
                {
                    if(elem_counts[cur_number] > 0) break;
                    cur_number = (cur_number + 1) & (MAX_ELEM - 1);
                    if(cur_number == number) goto loop_end;
                }
                // printf("Searching for %d (should be successful)\n", cur_number);
                retreived = int_hash_table_search(&iht, &cur_number);
                assert(retreived);
                assert(*retreived == cur_number);
                break;
            case 2:
                // search fail
                while(true)
                {
                    if(elem_counts[cur_number] == 0) break;
                    cur_number = (cur_number + 1) & (MAX_ELEM - 1);
                    if(cur_number == number) goto loop_end;
                }
                // printf("Searching for %d (should fail)\n", cur_number);
                retreived = int_hash_table_search(&iht, &cur_number);
                assert(!retreived);
                break;
            case 3:
                // remove
                while(true)
                {
                    if(elem_counts[cur_number] > 0) break;
                    cur_number = (cur_number + 1) & (MAX_ELEM - 1);
                    if(cur_number == number) goto loop_end;
                }
                int removed;
                // printf("Removing %d\n", cur_number);
                elem_counts[cur_number]--;
                assert(!int_hash_table_remove(&iht, &cur_number, &removed));
                assert(cur_number == removed);
                break;
        }
        // print_table(&iht);
        iter++;
loop_end:
        ;
    }
    int_hash_table_clear(&iht);
#undef NUM_ITERS
#undef MAX_ELEM
}

static void test_iterator(void)
{
    int_hash_table iht;
    assert(!int_hash_table_init(&iht, 5));
    int elements[] = { 1, 33, 5, 6, 7, 16, 48, 28, 29, 31, 33 };
    // When writing this test, we don't want to make any assumptions as to the 
    // internal order of the elements, so we just tally how many we've seen to 
    // check for consistency.
    int should_see_count[64] = { 0 };
    for(ptrdiff_t i = 0; i < (ptrdiff_t)(sizeof(elements)/sizeof(elements[0])); i++)
        should_see_count[elements[i]]++;
    for(ptrdiff_t i = 0; i < (ptrdiff_t)(sizeof(elements)/sizeof(elements[0])); i++)
        assert(!int_hash_table_insert(&iht, elements + i));
    print_table(&iht);
    int_hash_table_iter iter;
    assert(int_hash_table_iter_init(&iht, &iter));
    int seen_count[64] = { 0 };
    for(ptrdiff_t i = 0; i < (ptrdiff_t)(sizeof(elements)/sizeof(elements[0])); i++)
    {
        seen_count[*(iter.cur)]++;
        bool has_cur = int_hash_table_iter_next(&iter);
        if(i == sizeof(elements)/sizeof(elements[0]) - 1) assert(!has_cur);
        else assert(has_cur);
    }
    for(ptrdiff_t i = 0; i < (ptrdiff_t)(sizeof(seen_count)/sizeof(seen_count[0])); i++)
        assert(seen_count[i] == should_see_count[i]);
    int_hash_table_clear(&iht);
    int_hash_table_init(&iht, 6);
    assert(!int_hash_table_iter_init(&iht, &iter));
    int_hash_table_clear(&iht);
}

static void test_for_each(void)
{
    test_for_each_closure cl = { .total = 0, .seen_count = { 0 } };
    int_hash_table iht;
    assert(!int_hash_table_init(&iht, 5));
    int elements[] = { 1, 33, 5, 6, 7, 16, 48, 28, 29, 31, 33 };
    // When writing this test, we don't want to make any assumptions as to the 
    // internal order of the elements, so we just tally how many we've seen to 
    // check for consistency.
    int should_see_count[64] = { 0 };
    for(ptrdiff_t i = 0; i < (ptrdiff_t)(sizeof(elements)/sizeof(elements[0])); i++)
        should_see_count[elements[i]]++;
    for(ptrdiff_t i = 0; i < (ptrdiff_t)(sizeof(elements)/sizeof(elements[0])); i++)
        assert(!int_hash_table_insert(&iht, elements + i));
    print_table(&iht);
    int_hash_table_for_each(&iht, &cl, test_for_each_fn);
    assert(cl.total == 237);
    for(ptrdiff_t i = 0; i < (ptrdiff_t)(sizeof(should_see_count)/sizeof(should_see_count[0])); i++)
        assert(cl.seen_count[i] == should_see_count[i]);
    int_hash_table_clear(&iht);
}

static void test_for_each_fn(void *closure, int *element)
{
    test_for_each_closure *clcast = (test_for_each_closure *)closure;
    clcast->total += *element;
    clcast->seen_count[*element]++;
}

static void print_table(int_hash_table *iht)
{
    /* "i012: 2012 (psl: 001)    " */
    /* "xxxxxxxxxxxxxxxxxxxxx    " */
    /* "                         " */
    #define ROWS_LEN 24
    #define COLS_LEN 81
    #define ROW_IDX(idx) ((idx) % 24)
    #define COL_IDX(idx) (((idx) / 24) * 25)
    assert(iht);
    assert(iht->cap <= 64);
    char line_bufs[ROWS_LEN][COLS_LEN];
    memset(line_bufs, ' ', sizeof(line_bufs));
    for(int i = 0; i < ROWS_LEN; i++) line_bufs[i][COLS_LEN - 1] = '\0';
    for(ptrdiff_t i = 0; i < (ptrdiff_t)iht->cap; i++)
    {
        char *pos = &line_bufs[ROW_IDX(i)][COL_IDX(i)];
        if(iht->entries[i].probe_seq_len < 0)
            memcpy(pos, "xxxxxxxxxxxxxxxxxxxxx", 21);
        else
        {
            // The compiler expects us to retain and check the return value 
            // if truncation is possible, so that's why written is here.
            int written = snprintf(pos,
                                   22,
                                   "i%03ld: %04d (psl: %03d)",
                                   i,
                                   iht->entries[i].data,
                                   iht->entries[i].probe_seq_len);
            (void)written;
            pos[21] = ' ';
        }
    }
    for(int i = 0; i < ROWS_LEN; i++) printf("%s\n", &line_bufs[i][0]);
    #undef COL_IDX
    #undef ROW_IDX
    #undef COLS_LEN
    #undef ROWS_LEN
}
