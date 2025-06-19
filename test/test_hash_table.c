#define COLN_DATA_TYPENAME int
#define COLN_DATA_HASH(x) ((size_t)(*x))
#define COLN_HEADER
#define COLN_IMPL
#define COLN_INTERNAL_DEBUG

#include "hash_table.t.h"

#undef COLN_IMPL
#undef COLN_HEADER
#undef COLN_DATA_HASH
#undef COLN_DATA_TYPENAME

#include <stdio.h>
#include <string.h>

static void print_table(int_hash_table *iht);

int main(int argc, char **argv)
{
#define MAX_ELEM 4096
    (void)argc;
    (void)argv;
    int_hash_table iht;
    assert(!int_hash_table_init(&iht, 0));
    static int elem_counts[MAX_ELEM] = {0};
    srand(42);
    // print_table(&iht);
    int iter = 0;
    while(true)
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
    }
    int_hash_table_clear(&iht);
    return 0;
#undef MAX_ELEM
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
    for(intptr_t i = 0; i < (intptr_t)iht->cap; i++)
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
