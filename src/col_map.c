#include "col_map_priv.h"

#include "col_pair_priv.h"
#include "col_rb_tree_priv.h"
#include "col_result_priv.h"

typedef struct col_pair col_map_entry_priv;

static bool entry_cp(void *dest, void *src);
static void entry_clr(void *to_clear);
static int entry_cmp(void *a, void *b);

enum col_result
col_map_init(
    struct col_map *to_init,
    struct col_allocator *allocator,
    struct col_elem_metadata *key_md,
    struct col_elem_metadata *value_md
)
{
    enum col_result result;
    

    if((result = col_rb_tree_init(&(to_init->rbt), allocator, )))
    return COL_RESULT_SUCCESS;
}
