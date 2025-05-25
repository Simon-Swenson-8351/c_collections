#include "col_rb_tree.c"

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "col_elem.c"
#include "col_allocator.c"

static int cmp_fn(void *a, void *b);
static struct col_elem_metadata md =
{
    .cp_fn = NULL,
    .clr_fn = NULL,
    .eq_fn = NULL,
    .cmp_fn = cmp_fn,
    .size = sizeof(int),
    .alignment = alignof(int)
};

static void print_node(struct col_rb_tree_node *n, int indentation);
static void print_indentation(int indentation);

static struct col_rb_tree_node *node(int val, enum node_color color, struct col_rb_tree_node *left, struct col_rb_tree_node *right);
static bool node_eq(struct col_rb_tree_node *a, struct col_rb_tree_node *b);

static bool verify_tree(struct col_rb_tree *t);
static int verify_bdepth(struct col_rb_tree_node *n);
static bool verify_red_nadj(struct col_rb_tree_node *n);
static bool verify_order(struct col_rb_tree_node *n, struct col_elem_metadata *md);
static bool verify_backrefs(struct col_rb_tree_node *n);

#define TEST_INS(case, insertend_nodes, to_insert, expected_output_nodes) \
    int case(void) \
    { \
        struct col_rb_tree insertend; \
        col_rb_tree_init(&insertend, NULL, &md); \
        insertend.root = (insertend_nodes); \
    \
        struct col_rb_tree expected_output; \
        col_rb_tree_init(&expected_output, NULL, &md); \
        expected_output.root = (expected_output_nodes); \
    \
        int i = (to_insert); \
        enum col_result ins_result = col_rb_tree_insert(&insertend, &i); \
    \
        int result; \
        if(ins_result) \
        { \
            printf("Insert test case failed.\n"); \
            printf("ins_result = %d", (int)ins_result); \
            result = 1; \
        } \
        if(!node_eq(insertend.root, expected_output.root)) \
        { \
            printf("Insert test case failed.\n"); \
            printf("Insertend:\n\n"); \
            print_node(insertend.root, 1); \
            printf("\nExpected output:----------------------------------------------------------------\n\n"); \
            print_node(expected_output.root, 1); \
            printf("\n--------------------------------------------------------------------------------\n"); \
            result = 1; \
        } \
        else result = 0; \
        col_rb_tree_clear(&insertend); \
        col_rb_tree_clear(&expected_output); \
        return result; \
    }
#define TEST_RM(case, removend_nodes, to_remove, expected_output_nodes, expected_result) \
    int case(void) \
    { \
        struct col_rb_tree removend; \
        col_rb_tree_init(&removend, NULL, &md); \
        removend.root = (removend_nodes); \
    \
        struct col_rb_tree expected_output; \
        col_rb_tree_init(&expected_output, NULL, &md); \
        expected_output.root = (expected_output_nodes); \
    \
        int removed; \
        int i = (to_remove); \
        enum col_result removal_result = col_rb_tree_remove(&removend, &i, &removed); \
    \
        int result; \
        if(removal_result != expected_result) \
        { \
            printf("Removal test case failed.\n"); \
            printf("removal_result = %d", (int)removal_result); \
            result = 1; \
        } \
        if(!removal_result && removed != i) \
        { \
            printf("Removal test case failed.\n"); \
            printf("to_remove = %d, removed = %d\n", to_remove, removed); \
            result = 1; \
        } \
        if(!node_eq(removend.root, expected_output.root)) \
        { \
            printf("Removal test case failed.\n"); \
            printf("Removend:\n\n"); \
            print_node(removend.root, 1); \
            printf("\nExpected output:----------------------------------------------------------------\n\n"); \
            print_node(expected_output.root, 1); \
            printf("\n--------------------------------------------------------------------------------\n"); \
            result = 1; \
        } \
        else result = 0; \
        col_rb_tree_clear(&removend); \
        col_rb_tree_clear(&expected_output); \
        return result; \
    }

int test_init(void)
{
    struct col_rb_tree t;
    col_rb_tree_init(&t, NULL, &md);
    col_rb_tree_clear(&t);
    return 0;
}

int test_copy(void)
{
    struct col_rb_tree t1;
    col_rb_tree_init(&t1, NULL, &md);
    struct col_rb_tree t2;

    t1.root =
        node(0, NC_BLACK,
            node(-16, NC_BLACK, NULL, NULL),
            node(16, NC_RED, 
                node(8, NC_BLACK, NULL, NULL),
                node(24, NC_BLACK, 
                    node(20, NC_RED, NULL, NULL),
                    NULL)));

    col_rb_tree_copy(&t2, &t1);
    if(!node_eq(t1.root, t2.root)) return 1;
    // test a couple of nodes to see if they are at different memory addresses
    if(t1.root == t2.root) return 1;
    if(LEFT_CHILD(t1.root) == LEFT_CHILD(t2.root)) return 1;
    if(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(t1.root))) == LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(t2.root)))) return 1;

    col_rb_tree_clear(&t2);
    col_rb_tree_clear(&t1);
    return 0;
}

int test_clear(void)
{
    struct col_rb_tree t1;
    col_rb_tree_init(&t1, NULL, &md);
    t1.root =
        node(0, NC_BLACK,
            node(-16, NC_BLACK, NULL, NULL),
            node(16, NC_RED, 
                node(8, NC_BLACK, NULL, NULL),
                node(24, NC_BLACK, 
                    node(20, NC_RED, NULL, NULL),
                    NULL)));
    col_rb_tree_clear(&t1);
    return 0;
}

int test_random(void)
{
#define MAX_ELEM 4096
#define NUM_ITERS 1024
    static int elem_counts[MAX_ELEM] = {0};
    struct col_rb_tree prev;
    struct col_rb_tree t;
    col_rb_tree_init(&prev, NULL, &md);
    col_rb_tree_init(&t, NULL, &md);
    srand(42);
    for(int i = 0; i < NUM_ITERS; i++)
    {
        int number = rand();
        int insert = number & 1;
        number >>= 1;
        number %= MAX_ELEM;
        if(insert)
        {
            col_rb_tree_insert(&t, &number);
            elem_counts[number]++;
        }
        else
        {
            int j = number;
            do
            {
                if(elem_counts[j] > 0) break;
                j = (j + 1) % MAX_ELEM;
            } while (j != number);
            if(elem_counts[j] == 0) continue;
            int out;
            col_rb_tree_remove(&t, &number, &out);
            elem_counts[number]--;
        }
        if(!verify_tree(&t))
        {
            printf("Prev tree:\n\n");
            print_node(prev.root, 1);
            printf("\nCurrent tree:-------------------------------------------------------------------\n");
            print_node(t.root, 1);
            printf("\n");
            return 1;
        }
        col_rb_tree_clear(&prev);
        col_rb_tree_copy(&prev, &t);
    }
    col_rb_tree_clear(&prev);
    col_rb_tree_clear(&t);
    return 0;
#undef NUM_ITERS
#undef MAX_ELEM
}

TEST_INS(test_ins_root, NULL, 42, node(42, NC_BLACK, NULL, NULL))
TEST_INS(
    test_ins_dup,
    node(42, NC_BLACK,
        node(42, NC_RED, NULL, NULL),
        NULL),
    42,
    node(42, NC_BLACK,
        node(42, NC_RED, NULL, NULL),
        node(42, NC_RED, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ur_ll,
    node(32, NC_BLACK,
        node(0, NC_BLACK,
            node(-16, NC_RED, NULL, NULL),
            node(16, NC_RED, NULL, NULL)),
        node(48, NC_BLACK, NULL, NULL)),
    -32,
    node(32, NC_BLACK,
        node(0, NC_RED,
            node(-16, NC_BLACK, 
                node(-32, NC_RED, NULL, NULL),
                NULL),
            node(16, NC_BLACK, NULL, NULL)),
        node(48, NC_BLACK, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ur_lr,
    node(32, NC_BLACK,
        node(0, NC_BLACK,
            node(-16, NC_RED, NULL, NULL),
            node(16, NC_RED, NULL, NULL)),
        node(48, NC_BLACK, NULL, NULL)),
    -8,
    node(32, NC_BLACK,
        node(0, NC_RED,
            node(-16, NC_BLACK,
                NULL,
                node(-8, NC_RED, NULL, NULL)),
            node(16, NC_BLACK, NULL, NULL)),
        node(48, NC_BLACK, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ur_rl,
    node(32, NC_BLACK,
        node(0, NC_BLACK,
            node(-16, NC_RED, NULL, NULL),
            node(16, NC_RED, NULL, NULL)),
        node(48, NC_BLACK, NULL, NULL)),
    8,
    node(32, NC_BLACK,
        node(0, NC_RED,
            node(-16, NC_BLACK, NULL, NULL),
            node(16, NC_BLACK,
                node(8, NC_RED, NULL, NULL),
                NULL)),
        node(48, NC_BLACK, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ur_rr,
    node(32, NC_BLACK,
        node(0, NC_BLACK,
            node(-16, NC_RED, NULL, NULL),
            node(16, NC_RED, NULL, NULL)),
        node(48, NC_BLACK, NULL, NULL)),
    24,
    node(32, NC_BLACK,
        node(0, NC_RED,
            node(-16, NC_BLACK, NULL, NULL),
            node(16, NC_BLACK,
                NULL,
                node(24, NC_RED, NULL, NULL))),
        node(48, NC_BLACK, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ur_multiple,
    node(0, NC_BLACK,
        node(-32, NC_RED,
            node(-48, NC_BLACK,
                node(-52, NC_RED, NULL, NULL),
                node(-44, NC_RED, NULL, NULL)),
            node(-16, NC_BLACK, NULL, NULL)),
        node(32, NC_RED,
            node(16, NC_BLACK, NULL, NULL),
            node(48, NC_BLACK, NULL, NULL))),
    -46,
    node(0, NC_BLACK,
        node(-32, NC_BLACK,
            node(-48, NC_RED,
                node(-52, NC_BLACK, NULL, NULL),
                node(-44, NC_BLACK,
                    node(-46, NC_RED, NULL, NULL),
                    NULL)),
            node(-16, NC_BLACK, NULL, NULL)),
        node(32, NC_BLACK,
            node(16, NC_BLACK, NULL, NULL),
            node(48, NC_BLACK, NULL, NULL))))
TEST_INS(
    test_ins_with_pr_ub_ll,
    node(0, NC_BLACK,
        node(-2, NC_RED, NULL, NULL),
        NULL),
    -3,
    node(-2, NC_BLACK,
        node(-3, NC_RED, NULL, NULL),
        node(0, NC_RED, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ub_lr,
    node(0, NC_BLACK,
        node(-2, NC_RED, NULL, NULL),
        NULL),
    -1,
    node(-1, NC_BLACK,
        node(-2, NC_RED, NULL, NULL),
        node(0, NC_RED, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ub_rl,
    node(0, NC_BLACK,
        NULL,
        node(2, NC_RED, NULL, NULL)),
    1,
    node(1, NC_BLACK,
        node(0, NC_RED, NULL, NULL),
        node(2, NC_RED, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ub_rr,
    node(0, NC_BLACK,
        NULL,
        node(2, NC_RED, NULL, NULL)),
    3,
    node(2, NC_BLACK,
        node(0, NC_RED, NULL, NULL),
        node(3, NC_RED, NULL, NULL)))
TEST_RM(
    test_rm_root,
    node(0, NC_BLACK, NULL, NULL),
    0,
    NULL,
    COL_RESULT_SUCCESS)
TEST_RM(
    test_rm_missing,
    node(0, NC_BLACK, NULL, NULL),
    1,
    node(0, NC_BLACK, NULL, NULL),
    COL_RESULT_ELEM_NOT_FOUND)
TEST_RM(
    test_rm_int_w_2c,
    node(0, NC_BLACK,
        node(-4, NC_BLACK, NULL, NULL),
        node(4, NC_RED,
            node(2, NC_BLACK,
                node(1, NC_RED, NULL, NULL),
                NULL),
            node(6, NC_BLACK, NULL, NULL))),
    0,
    node(1, NC_BLACK,
        node(-4, NC_BLACK, NULL, NULL),
        node(4, NC_RED,
            node(2, NC_BLACK,
                NULL,
                NULL),
            node(6, NC_BLACK, NULL, NULL))),
    COL_RESULT_SUCCESS)
TEST_RM(
    test_rm_int_w_lc,
    node(0, NC_BLACK,
        node(-2, NC_BLACK, 
            node(-3, NC_RED, NULL, NULL),
            NULL),
        node(2, NC_BLACK, NULL, NULL)),
    -2,
    node(0, NC_BLACK,
        node(-3, NC_BLACK, NULL, NULL),
        node(2, NC_BLACK, NULL, NULL)),
    COL_RESULT_SUCCESS)
TEST_RM(
    test_rm_int_w_rc,
    node(0, NC_BLACK,
        node(-2, NC_BLACK, 
            NULL,
            node(-1, NC_RED, NULL, NULL)),
        node(2, NC_BLACK, NULL, NULL)),
    -2,
    node(0, NC_BLACK,
        node(-1, NC_BLACK, NULL, NULL),
        node(2, NC_BLACK, NULL, NULL)),
    COL_RESULT_SUCCESS)
TEST_RM(
    test_rm_r_leaf,
    node(0, NC_BLACK, 
        node(-1, NC_RED, NULL, NULL),
        node(1, NC_RED, NULL, NULL)),
    -1,
    node(0, NC_BLACK, 
        NULL,
        node(1, NC_RED, NULL, NULL)),
    COL_RESULT_SUCCESS)
TEST_RM(
    test_rm_blk_leaf_selfleft_parentred_siblingblack_cousinleftblack_cousinrightblack,
    node(0, NC_BLACK,
        node(-2, NC_BLACK, NULL, NULL),
        node(2, NC_RED, 
            node(1, NC_BLACK, NULL, NULL),
            node(3, NC_BLACK, NULL, NULL))),
    1,
    node(0, NC_BLACK,
        node(-2, NC_BLACK, NULL, NULL),
        node(2, NC_BLACK, 
            NULL,
            node(3, NC_RED, NULL, NULL))),
    COL_RESULT_SUCCESS)
TEST_RM(
    test_rm_blk_leaf_selfright_parentred_siblingblack_cousinleftblack_cousinrightblack,
    node(0, NC_BLACK,
        node(-2, NC_BLACK, NULL, NULL),
        node(2, NC_RED, 
            node(1, NC_BLACK, NULL, NULL),
            node(3, NC_BLACK, NULL, NULL))),
    3,
    node(0, NC_BLACK,
        node(-2, NC_BLACK, NULL, NULL),
        node(2, NC_BLACK, 
            node(1, NC_RED, NULL, NULL),
            NULL)),
    COL_RESULT_SUCCESS)
TEST_RM(
    test_rm_blk_leaf_selfleft_parentred_siblingblack_cousinleftred_cousinrightblack,
    node(0, NC_BLACK,
        node(-8, NC_BLACK, 
            node(-12, NC_BLACK, NULL, NULL), 
            node(-4, NC_BLACK, NULL, NULL)),
        node(8, NC_RED, 
            node(4, NC_BLACK,
                node(2, NC_BLACK, NULL, NULL), 
                node(6, NC_BLACK, NULL, NULL)),
            node(12, NC_BLACK,
                node(10, NC_RED, 
                    node(9, NC_BLACK, NULL, NULL),
                    node(11, NC_BLACK, NULL, NULL)),
                node(14, NC_BLACK, NULL, NULL)))),
    2,
    node(0, NC_BLACK,
        node(-8, NC_BLACK, 
            node(-12, NC_BLACK, NULL, NULL), 
            node(-4, NC_BLACK, NULL, NULL)),
        node(10, NC_RED,
            node(8, NC_BLACK,
                node(4, NC_BLACK,
                    NULL,
                    node(6, NC_RED, NULL, NULL)),
                node(9, NC_BLACK, NULL, NULL)),
            node(12, NC_BLACK,
                node(11, NC_BLACK, NULL, NULL),
                node(14, NC_BLACK, NULL, NULL)))),
    COL_RESULT_SUCCESS)
TEST_RM(
    test_rm_blk_leaf_selfleft_parentblack_siblingblack_cousinleftred_cousinrightblack,
    node(0, NC_BLACK,
        node(-8, NC_BLACK, 
            node(-12, NC_BLACK,
                node(-14, NC_BLACK, NULL, NULL),
                node(-10, NC_BLACK, NULL, NULL)), 
            node(-4, NC_BLACK,
                node(-6, NC_BLACK, NULL, NULL),
                node(-2, NC_BLACK, NULL, NULL))),
        node(8, NC_BLACK, 
            node(4, NC_BLACK,
                node(2, NC_BLACK, NULL, NULL), 
                node(6, NC_BLACK, NULL, NULL)),
            node(12, NC_BLACK,
                node(10, NC_RED, 
                    node(9, NC_BLACK, NULL, NULL),
                    node(11, NC_BLACK, NULL, NULL)),
                node(14, NC_BLACK, NULL, NULL)))),
    2,
    node(0, NC_BLACK,
        node(-8, NC_BLACK,
            node(-12, NC_BLACK,
                node(-14, NC_BLACK, NULL, NULL),
                node(-10, NC_BLACK, NULL, NULL)), 
            node(-4, NC_BLACK,
                node(-6, NC_BLACK, NULL, NULL),
                node(-2, NC_BLACK, NULL, NULL))),
        node(10, NC_BLACK,
            node(8, NC_BLACK,
                node(4, NC_BLACK,
                    NULL,
                    node(6, NC_RED, NULL, NULL)),
                node(9, NC_BLACK, NULL, NULL)),
            node(12, NC_BLACK,
                node(11, NC_BLACK, NULL, NULL),
                node(14, NC_BLACK, NULL, NULL)))),
    COL_RESULT_SUCCESS)
TEST_RM(
    test_rm_blk_leaf_selfright_parentblack_siblingblack_cousinleftred_cousinrightblack,
    node(0, NC_BLACK,
        node(-4, NC_BLACK,
            node(-6, NC_RED,
                node(-7, NC_BLACK, NULL, NULL),
                node(-5, NC_BLACK, NULL, NULL)),
            node(-2, NC_BLACK, NULL, NULL)),
        node(4, NC_BLACK,
            node(2, NC_BLACK, NULL, NULL),
            node(6, NC_BLACK, NULL, NULL))),
    6,
    node(-4, NC_BLACK,
        node(-6, NC_BLACK,
            node(-7, NC_BLACK, NULL, NULL),
            node(-5, NC_BLACK, NULL, NULL)),
        node(0, NC_BLACK,
            node(-2, NC_BLACK, NULL, NULL),
            node(4, NC_BLACK,
                node(2, NC_RED, NULL, NULL),
                NULL))),
    COL_RESULT_SUCCESS)
TEST_RM(
    test_rm_blk_leaf_selfleft_siblingred,
    node(0, NC_BLACK,
        node(-4, NC_BLACK, NULL, NULL),
        node(4, NC_RED,
            node(2, NC_BLACK,
                node(1, NC_RED, NULL, NULL),
                node(3, NC_RED, NULL, NULL)),
            node(6, NC_BLACK, NULL, NULL))),
    -4,
    node(4, NC_BLACK,
        node(1, NC_RED,
            node(0, NC_BLACK, NULL, NULL),
            node(2, NC_BLACK,
                NULL,
                node(3, NC_RED, NULL, NULL))),
        node(6, NC_BLACK, NULL, NULL)),
    COL_RESULT_SUCCESS)

struct test_case
{
    char const *name;
    int(*fn)(void);
};
#define TEST_CASE(fn_name) { .name = #fn_name, .fn = fn_name }
struct test_case TEST_CASES[] =
{
    TEST_CASE(test_init),
    TEST_CASE(test_copy),
    TEST_CASE(test_clear),
    TEST_CASE(test_ins_root),
    TEST_CASE(test_ins_dup),
    TEST_CASE(test_ins_with_pr_ur_ll),
    TEST_CASE(test_ins_with_pr_ur_lr),
    TEST_CASE(test_ins_with_pr_ur_rl),
    TEST_CASE(test_ins_with_pr_ur_rr),
    TEST_CASE(test_ins_with_pr_ur_multiple),
    TEST_CASE(test_ins_with_pr_ub_ll),
    TEST_CASE(test_ins_with_pr_ub_lr),
    TEST_CASE(test_ins_with_pr_ub_rl),
    TEST_CASE(test_ins_with_pr_ub_rr),
    TEST_CASE(test_rm_root),
    TEST_CASE(test_rm_missing),
    TEST_CASE(test_rm_int_w_2c),
    TEST_CASE(test_rm_int_w_lc),
    TEST_CASE(test_rm_int_w_rc),
    TEST_CASE(test_rm_r_leaf),
    TEST_CASE(test_rm_blk_leaf_selfleft_parentred_siblingblack_cousinleftblack_cousinrightblack),
    TEST_CASE(test_rm_blk_leaf_selfright_parentred_siblingblack_cousinleftblack_cousinrightblack),
    TEST_CASE(test_rm_blk_leaf_selfleft_parentred_siblingblack_cousinleftred_cousinrightblack),
    TEST_CASE(test_rm_blk_leaf_selfleft_parentblack_siblingblack_cousinleftred_cousinrightblack),
    TEST_CASE(test_rm_blk_leaf_selfright_parentblack_siblingblack_cousinleftred_cousinrightblack),
    TEST_CASE(test_rm_blk_leaf_selfleft_siblingred),
    //TEST_CASE(test_random)
};

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    int failures = 0;
    for(ptrdiff_t i = 0; i < (ptrdiff_t)(sizeof(TEST_CASES)/sizeof(TEST_CASES[0])); i++)
    {
        printf("\033[36mBEGINNING test %s\033[0m\n", TEST_CASES[i].name);
        int failed = TEST_CASES[i].fn();
        if(failed)
        {
            printf("\033[31mFAILED test %s\033[0m\n", TEST_CASES[i].name);
            failures++;
        }
        else
        {
            printf("\033[32mPASSED test %s\033[0m\n", TEST_CASES[i].name);
        }
    }
    return failures;
}

static int cmp_fn(void *a, void *b)
{
    if(*((int *)a) < *((int *)b)) return -1;
    else if(*((int *)a) == *((int *)b)) return 0;
    else return 1;
}

static void print_node(struct col_rb_tree_node *n, int indentation)
{
    if(!n) return;
    print_node(LEFT_CHILD(n), indentation + 1);
    print_indentation(indentation);
    printf("%s     %d\n", (n->color == NC_RED) ? "\033[31mRED\033[0m" : "\033[32mBLK\033[0m", *((int *)node_data(n, &md)));
    print_node(RIGHT_CHILD(n), indentation + 1);
}

static void print_indentation(int indentation)
{
    for(int i = 0; i < indentation; i++)
    {
        printf("    ");
    }
}

static struct col_rb_tree_node *node(int val, enum node_color color, struct col_rb_tree_node *left, struct col_rb_tree_node *right)
{
    struct col_rb_tree_node *result = malloc(sizeof(struct col_rb_tree_node) + sizeof(int));
    result->parent = NULL;
    *((int *)(result + 1)) = val;
    result->color = color;
    LEFT_CHILD(result) = left;
    if(left) left->parent = result;
    RIGHT_CHILD(result) = right;
    if(right) right->parent = result;
    return result;
}

static bool node_eq(struct col_rb_tree_node *a, struct col_rb_tree_node *b)
{
    if(!a && !b) return true;
    if(!a && b) return false;
    if(a && !b) return false;
    if(a->color != b->color) return false;
    if(*((int *)node_data(a, &md)) != *((int *)node_data(b, &md))) return false;
    return node_eq(LEFT_CHILD(a), LEFT_CHILD(b)) && node_eq(RIGHT_CHILD(a), RIGHT_CHILD(b));
}

static bool verify_tree(struct col_rb_tree *t)
{
    int bdepth = verify_bdepth(t->root);
    if(bdepth < 0)
    {
        printf("bdepth verification failed.\n");
        return false;
    }
    if(!verify_red_nadj(t->root))
    {
        printf("no adjacent red node verification failed.\n");
        return false;
    }
    if(!verify_order(t->root, t->elem_metadata))
    {
        printf("element order verification failed.\n");
        return false;
    }
    if(!verify_backrefs(t->root) || t->root->parent != NULL)
    {
        printf("backrefs verification failed.\n");
        return false;
    }
    return true;
}

static int verify_bdepth(struct col_rb_tree_node *n)
{
    if(!n) return 1;
    int lbd = verify_bdepth(LEFT_CHILD(n));
    int rbd = verify_bdepth(RIGHT_CHILD(n));
    if(lbd != rbd) return -1;
    if(n->color == NC_RED) return lbd;
    else return lbd + 1;
}

static bool verify_red_nadj(struct col_rb_tree_node *n)
{
    if(!n) return true;
    if(n->color == NC_RED)
    {
        if(LEFT_CHILD(n) && LEFT_CHILD(n)->color == NC_RED) return false;
        if(RIGHT_CHILD(n) && RIGHT_CHILD(n)->color == NC_RED) return false;
    }
    if(!verify_red_nadj(LEFT_CHILD(n))) return false;
    if(!verify_red_nadj(RIGHT_CHILD(n))) return false;
    return true;
}

static bool verify_order(struct col_rb_tree_node *n, struct col_elem_metadata *md)
{
    if(!n) return true;
    if(LEFT_CHILD(n) && col_elem_cmp(md, node_data(LEFT_CHILD(n), md), node_data(n, md)) > 0) return false;
    if(RIGHT_CHILD(n) && col_elem_cmp(md, node_data(RIGHT_CHILD(n), md), node_data(n, md)) < 0) return false;
    if(!verify_order(LEFT_CHILD(n), md)) return false;
    if(!verify_order(RIGHT_CHILD(n), md)) return false;
    return true;
}

static bool verify_backrefs(struct col_rb_tree_node *n)
{
    if(!n) return true;
    if(LEFT_CHILD(n) && LEFT_CHILD(n)->parent != n) return false;
    if(RIGHT_CHILD(n) && RIGHT_CHILD(n)->parent != n) return false;
    if(!verify_backrefs(LEFT_CHILD(n))) return false;
    if(!verify_backrefs(RIGHT_CHILD(n))) return false;
    return true;
}
