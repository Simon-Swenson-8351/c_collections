#define COLN_DATA_TYPENAME int
#define COLN_DATA_HASH(x) ((size_t)(x))
#define COLN_HEADER
#define COLN_IMPL

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
    (void)argc;
    (void)argv;
    int_hash_table iht;
    assert(!int_hash_table_init(&iht, 6));
    print_table(&iht);
    int_hash_table_clear(&iht);
    return 0;
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
            snprintf(pos,
                     21,
                     "i%03ld: %04d (psl: %03d)",
                     i,
                     iht->entries[i].data,
                     iht->entries[i].probe_seq_len);
        }
    }
    for(int i = 0; i < ROWS_LEN; i++) printf("%s\n", &line_bufs[i][0]);
    #undef COL_IDX
    #undef ROW_IDX
    #undef COLS_LEN
    #undef ROWS_LEN
}
