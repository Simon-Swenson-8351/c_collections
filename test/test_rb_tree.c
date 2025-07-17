#include "test_common.c"

#define DATA_TYPENAME int
#define DATA_PASS_BY_VAL
#define DATA_COMPARE int_compare
#define RB_TREE_TYPENAME int_red_black_tree
#define RB_TREE_HEADER
#define RB_TREE_IMPL

#include "rb_tree.t.h"

#undef RB_TREE_IMPL
#undef RB_TREE_HEADER
#undef RB_TREE_TYPENAME
#undef DATA_COMPARE
#undef DATA_PASS_BY_VAL
#undef DATA_TYPENAME

#define DATA_TYPENAME int
#define DATA_PASS_BY_VAL
#define DATA_COMPARE int_compare
#define ALLOC_TYPENAME dynamic_slot_allocator
#define ALLOC_ALLOC dynamic_slot_allocator_alloc
#define ALLOC_FREE dynamic_slot_allocator_free
#define RB_TREE_TYPENAME int_slot_allocd_red_black_tree
#define RB_TREE_HEADER
#define RB_TREE_IMPL

#include "rb_tree.t.h"

#undef RB_TREE_IMPL
#undef RB_TREE_HEADER
#undef RB_TREE_TYPENAME
#undef ALLOC_FREE
#undef ALLOC_ALLOC
#undef ALLOC_TYPENAME
#undef DATA_COMPARE
#undef DATA_PASS_BY_VAL
#undef DATA_TYPENAME

#define DATA_TYPENAME dyn_str
#define DATA_PASS_BY_PTR
#define DATA_COPY dyn_str_copy
#define DATA_CLEAR dyn_str_clear
#define DATA_COMPARE dyn_str_cmp
#define RB_TREE_HEADER
#define RB_TREE_IMPL

#include "rb_tree.t.h"

#undef RB_TREE_IMPL
#undef RB_TREE_HEADER
#undef DATA_COMPARE
#undef DATA_CLEAR
#undef DATA_COPY
#undef DATA_PASS_BY_PTR
#undef DATA_TYPENAME

// for test purposes, this ensures that the header usage of the collection .t.h 
// is isolated from the header usage of the tests
#include <stdio.h>
#include <stdlib.h>

#define RB_NODE_COLOR_RED 0 
#define RB_NODE_COLOR_BLACK 1 
#define RB_NODE_DIR_LEFT 0 
#define RB_NODE_DIR_RIGHT 1 
#define LEFT_CHILD(node) ((node)->children[RB_NODE_DIR_LEFT])
#define RIGHT_CHILD(node) ((node)->children[RB_NODE_DIR_RIGHT])
#define ACCESSOR(tree, node) \
  (!((node)->parent) ? \
    &((tree)->root) : \
    (LEFT_CHILD((node)->parent) == node ? \
      &LEFT_CHILD((node)->parent) : \
      &RIGHT_CHILD((node)->parent)))
#define COLOR(node) (node ? (node)->color : RB_NODE_COLOR_BLACK)

static void print_node(int_red_black_tree_node *n, int indentation);
static void print_indentation(int indentation);

static int_red_black_tree_node *node(int val,
                                     int color, 
                                     int_red_black_tree_node *left, 
                                     int_red_black_tree_node *right);
static bool node_eq(int_red_black_tree_node *a, int_red_black_tree_node *b);

static bool verify_tree(int_red_black_tree *t);
static int verify_bdepth(int_red_black_tree_node *n);
static bool verify_red_nadj(int_red_black_tree_node *n);
static bool verify_order(int_red_black_tree_node *n);
static bool verify_backrefs(int_red_black_tree_node *n);

#define TEST_INS(case, insertend_nodes, to_insert, expected_output_nodes) \
    int case(void) \
    { \
        int_red_black_tree insertend; \
        int_red_black_tree_init(&insertend); \
        insertend.root = (insertend_nodes); \
    \
        int_red_black_tree expected_output; \
        int_red_black_tree_init(&expected_output); \
        expected_output.root = (expected_output_nodes); \
    \
        int i = (to_insert); \
        coln_result ins_result = int_red_black_tree_insert(&insertend, i); \
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
            printf("\nExpected output:---------------------------------------" \
                "-------------------------\n\n"); \
            print_node(expected_output.root, 1); \
            printf("\n-------------------------------------------------------" \
                "-------------------------\n"); \
            result = 1; \
        } \
        else result = 0; \
        int_red_black_tree_clear(&insertend); \
        int_red_black_tree_clear(&expected_output); \
        return result; \
    }
#define TEST_RM(case, removend_nodes, to_remove, expected_output_nodes, expected_result) \
    int case(void) \
    { \
        int_red_black_tree removend; \
        int_red_black_tree_init(&removend); \
        removend.root = (removend_nodes); \
    \
        int_red_black_tree expected_output; \
        int_red_black_tree_init(&expected_output); \
        expected_output.root = (expected_output_nodes); \
    \
        int removed; \
        int i = (to_remove); \
        coln_result removal_result = int_red_black_tree_remove(&removend, \
                                                              i, \
                                                              &removed); \
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
            printf("\nExpected output:---------------------------------------" \
                "-------------------------\n\n"); \
            print_node(expected_output.root, 1); \
            printf("\n-------------------------------------------------------" \
                "-------------------------\n"); \
            result = 1; \
        } \
        else result = 0; \
        int_red_black_tree_clear(&removend); \
        int_red_black_tree_clear(&expected_output); \
        return result; \
    }

int test_init(void)
{
    int_red_black_tree t;
    int_red_black_tree_init(&t);
    int_red_black_tree_clear(&t);
    return 0;
}

int test_copy(void)
{
    int_red_black_tree t1;
    int_red_black_tree_init(&t1);
    int_red_black_tree t2;

    t1.root =
        node(0, RB_NODE_COLOR_BLACK,
            node(-16, RB_NODE_COLOR_BLACK, NULL, NULL),
            node(16, RB_NODE_COLOR_RED, 
                node(8, RB_NODE_COLOR_BLACK, NULL, NULL),
                node(24, RB_NODE_COLOR_BLACK, 
                    node(20, RB_NODE_COLOR_RED, NULL, NULL),
                    NULL)));

    int_red_black_tree_copy(&t2, &t1);
    if(!node_eq(t1.root, t2.root)) return 1;
    // test a couple of nodes to see if they are at different memory addresses
    if(t1.root == t2.root) return 1;
    if(LEFT_CHILD(t1.root) == LEFT_CHILD(t2.root)) return 1;
    if(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(t1.root))) == 
            LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(t2.root))))
        return 1;

    int_red_black_tree_clear(&t2);
    int_red_black_tree_clear(&t1);
    return 0;
}

int test_clear(void)
{
    int_red_black_tree t1;
    int_red_black_tree_init(&t1);
    t1.root =
        node(0, RB_NODE_COLOR_BLACK,
            node(-16, RB_NODE_COLOR_BLACK, NULL, NULL),
            node(16, RB_NODE_COLOR_RED, 
                node(8, RB_NODE_COLOR_BLACK, NULL, NULL),
                node(24, RB_NODE_COLOR_BLACK, 
                    node(20, RB_NODE_COLOR_RED, NULL, NULL),
                    NULL)));
    int_red_black_tree_clear(&t1);
    return 0;
}

int test_random(void)
{
#define MAX_ELEM 4096
#define NUM_ITERS 1024
    static int elem_counts[MAX_ELEM] = {0};
    int_red_black_tree prev;
    int_red_black_tree t;
    int_red_black_tree_init(&prev);
    int_red_black_tree_init(&t);
    srand(42);
    for(int i = 0; i < NUM_ITERS; i++)
    {
        int number = rand();
        int insert = number & 1;
        number >>= 1;
        number %= MAX_ELEM;
        if(insert)
        {
            int_red_black_tree_insert(&t, number);
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
            int_red_black_tree_remove(&t, number, &out);
            elem_counts[number]--;
        }
        if(!verify_tree(&t))
        {
            printf("Prev tree:\n\n");
            print_node(prev.root, 1);
            printf("\nCurrent tree:--------------------------------------------"
                "-----------------------\n");
            print_node(t.root, 1);
            printf("\n");
            return 1;
        }
        int_red_black_tree_clear(&prev);
        int_red_black_tree_copy(&prev, &t);
    }
    int_red_black_tree_clear(&prev);
    int_red_black_tree_clear(&t);
    return 0;
#undef NUM_ITERS
#undef MAX_ELEM
}

TEST_INS(test_ins_root, NULL, 42, node(42, RB_NODE_COLOR_BLACK, NULL, NULL))
TEST_INS(
    test_ins_dup,
    node(42, RB_NODE_COLOR_BLACK,
        node(42, RB_NODE_COLOR_RED, NULL, NULL),
        NULL),
    42,
    node(42, RB_NODE_COLOR_BLACK,
        node(42, RB_NODE_COLOR_RED, NULL, NULL),
        node(42, RB_NODE_COLOR_RED, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ur_ll,
    node(32, RB_NODE_COLOR_BLACK,
        node(0, RB_NODE_COLOR_BLACK,
            node(-16, RB_NODE_COLOR_RED, NULL, NULL),
            node(16, RB_NODE_COLOR_RED, NULL, NULL)),
        node(48, RB_NODE_COLOR_BLACK, NULL, NULL)),
    -32,
    node(32, RB_NODE_COLOR_BLACK,
        node(0, RB_NODE_COLOR_RED,
            node(-16, RB_NODE_COLOR_BLACK, 
                node(-32, RB_NODE_COLOR_RED, NULL, NULL),
                NULL),
            node(16, RB_NODE_COLOR_BLACK, NULL, NULL)),
        node(48, RB_NODE_COLOR_BLACK, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ur_lr,
    node(32, RB_NODE_COLOR_BLACK,
        node(0, RB_NODE_COLOR_BLACK,
            node(-16, RB_NODE_COLOR_RED, NULL, NULL),
            node(16, RB_NODE_COLOR_RED, NULL, NULL)),
        node(48, RB_NODE_COLOR_BLACK, NULL, NULL)),
    -8,
    node(32, RB_NODE_COLOR_BLACK,
        node(0, RB_NODE_COLOR_RED,
            node(-16, RB_NODE_COLOR_BLACK,
                NULL,
                node(-8, RB_NODE_COLOR_RED, NULL, NULL)),
            node(16, RB_NODE_COLOR_BLACK, NULL, NULL)),
        node(48, RB_NODE_COLOR_BLACK, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ur_rl,
    node(32, RB_NODE_COLOR_BLACK,
        node(0, RB_NODE_COLOR_BLACK,
            node(-16, RB_NODE_COLOR_RED, NULL, NULL),
            node(16, RB_NODE_COLOR_RED, NULL, NULL)),
        node(48, RB_NODE_COLOR_BLACK, NULL, NULL)),
    8,
    node(32, RB_NODE_COLOR_BLACK,
        node(0, RB_NODE_COLOR_RED,
            node(-16, RB_NODE_COLOR_BLACK, NULL, NULL),
            node(16, RB_NODE_COLOR_BLACK,
                node(8, RB_NODE_COLOR_RED, NULL, NULL),
                NULL)),
        node(48, RB_NODE_COLOR_BLACK, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ur_rr,
    node(32, RB_NODE_COLOR_BLACK,
        node(0, RB_NODE_COLOR_BLACK,
            node(-16, RB_NODE_COLOR_RED, NULL, NULL),
            node(16, RB_NODE_COLOR_RED, NULL, NULL)),
        node(48, RB_NODE_COLOR_BLACK, NULL, NULL)),
    24,
    node(32, RB_NODE_COLOR_BLACK,
        node(0, RB_NODE_COLOR_RED,
            node(-16, RB_NODE_COLOR_BLACK, NULL, NULL),
            node(16, RB_NODE_COLOR_BLACK,
                NULL,
                node(24, RB_NODE_COLOR_RED, NULL, NULL))),
        node(48, RB_NODE_COLOR_BLACK, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ur_multiple,
    node(0, RB_NODE_COLOR_BLACK,
        node(-32, RB_NODE_COLOR_RED,
            node(-48, RB_NODE_COLOR_BLACK,
                node(-52, RB_NODE_COLOR_RED, NULL, NULL),
                node(-44, RB_NODE_COLOR_RED, NULL, NULL)),
            node(-16, RB_NODE_COLOR_BLACK, NULL, NULL)),
        node(32, RB_NODE_COLOR_RED,
            node(16, RB_NODE_COLOR_BLACK, NULL, NULL),
            node(48, RB_NODE_COLOR_BLACK, NULL, NULL))),
    -46,
    node(0, RB_NODE_COLOR_BLACK,
        node(-32, RB_NODE_COLOR_BLACK,
            node(-48, RB_NODE_COLOR_RED,
                node(-52, RB_NODE_COLOR_BLACK, NULL, NULL),
                node(-44, RB_NODE_COLOR_BLACK,
                    node(-46, RB_NODE_COLOR_RED, NULL, NULL),
                    NULL)),
            node(-16, RB_NODE_COLOR_BLACK, NULL, NULL)),
        node(32, RB_NODE_COLOR_BLACK,
            node(16, RB_NODE_COLOR_BLACK, NULL, NULL),
            node(48, RB_NODE_COLOR_BLACK, NULL, NULL))))
TEST_INS(
    test_ins_with_pr_ub_ll,
    node(0, RB_NODE_COLOR_BLACK,
        node(-2, RB_NODE_COLOR_RED, NULL, NULL),
        NULL),
    -3,
    node(-2, RB_NODE_COLOR_BLACK,
        node(-3, RB_NODE_COLOR_RED, NULL, NULL),
        node(0, RB_NODE_COLOR_RED, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ub_lr,
    node(0, RB_NODE_COLOR_BLACK,
        node(-2, RB_NODE_COLOR_RED, NULL, NULL),
        NULL),
    -1,
    node(-1, RB_NODE_COLOR_BLACK,
        node(-2, RB_NODE_COLOR_RED, NULL, NULL),
        node(0, RB_NODE_COLOR_RED, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ub_rl,
    node(0, RB_NODE_COLOR_BLACK,
        NULL,
        node(2, RB_NODE_COLOR_RED, NULL, NULL)),
    1,
    node(1, RB_NODE_COLOR_BLACK,
        node(0, RB_NODE_COLOR_RED, NULL, NULL),
        node(2, RB_NODE_COLOR_RED, NULL, NULL)))
TEST_INS(
    test_ins_with_pr_ub_rr,
    node(0, RB_NODE_COLOR_BLACK,
        NULL,
        node(2, RB_NODE_COLOR_RED, NULL, NULL)),
    3,
    node(2, RB_NODE_COLOR_BLACK,
        node(0, RB_NODE_COLOR_RED, NULL, NULL),
        node(3, RB_NODE_COLOR_RED, NULL, NULL)))
TEST_RM(
    test_rm_root,
    node(0, RB_NODE_COLOR_BLACK, NULL, NULL),
    0,
    NULL,
    COLN_RESULT_SUCCESS)
TEST_RM(
    test_rm_missing,
    node(0, RB_NODE_COLOR_BLACK, NULL, NULL),
    1,
    node(0, RB_NODE_COLOR_BLACK, NULL, NULL),
    COLN_RESULT_ELEM_NOT_FOUND)
TEST_RM(
    test_rm_int_w_2c,
    node(0, RB_NODE_COLOR_BLACK,
        node(-4, RB_NODE_COLOR_BLACK, NULL, NULL),
        node(4, RB_NODE_COLOR_RED,
            node(2, RB_NODE_COLOR_BLACK,
                node(1, RB_NODE_COLOR_RED, NULL, NULL),
                NULL),
            node(6, RB_NODE_COLOR_BLACK, NULL, NULL))),
    0,
    node(1, RB_NODE_COLOR_BLACK,
        node(-4, RB_NODE_COLOR_BLACK, NULL, NULL),
        node(4, RB_NODE_COLOR_RED,
            node(2, RB_NODE_COLOR_BLACK,
                NULL,
                NULL),
            node(6, RB_NODE_COLOR_BLACK, NULL, NULL))),
    COLN_RESULT_SUCCESS)
TEST_RM(
    test_rm_int_w_lc,
    node(0, RB_NODE_COLOR_BLACK,
        node(-2, RB_NODE_COLOR_BLACK, 
            node(-3, RB_NODE_COLOR_RED, NULL, NULL),
            NULL),
        node(2, RB_NODE_COLOR_BLACK, NULL, NULL)),
    -2,
    node(0, RB_NODE_COLOR_BLACK,
        node(-3, RB_NODE_COLOR_BLACK, NULL, NULL),
        node(2, RB_NODE_COLOR_BLACK, NULL, NULL)),
    COLN_RESULT_SUCCESS)
TEST_RM(
    test_rm_int_w_rc,
    node(0, RB_NODE_COLOR_BLACK,
        node(-2, RB_NODE_COLOR_BLACK, 
            NULL,
            node(-1, RB_NODE_COLOR_RED, NULL, NULL)),
        node(2, RB_NODE_COLOR_BLACK, NULL, NULL)),
    -2,
    node(0, RB_NODE_COLOR_BLACK,
        node(-1, RB_NODE_COLOR_BLACK, NULL, NULL),
        node(2, RB_NODE_COLOR_BLACK, NULL, NULL)),
    COLN_RESULT_SUCCESS)
TEST_RM(
    test_rm_r_leaf,
    node(0, RB_NODE_COLOR_BLACK, 
        node(-1, RB_NODE_COLOR_RED, NULL, NULL),
        node(1, RB_NODE_COLOR_RED, NULL, NULL)),
    -1,
    node(0, RB_NODE_COLOR_BLACK, 
        NULL,
        node(1, RB_NODE_COLOR_RED, NULL, NULL)),
    COLN_RESULT_SUCCESS)
TEST_RM(
    test_rm_blk_leaf_selfleft_parentred_siblingblack_cousinleftblack_cousinrightblack,
    node(0, RB_NODE_COLOR_BLACK,
        node(-2, RB_NODE_COLOR_BLACK, NULL, NULL),
        node(2, RB_NODE_COLOR_RED, 
            node(1, RB_NODE_COLOR_BLACK, NULL, NULL),
            node(3, RB_NODE_COLOR_BLACK, NULL, NULL))),
    1,
    node(0, RB_NODE_COLOR_BLACK,
        node(-2, RB_NODE_COLOR_BLACK, NULL, NULL),
        node(2, RB_NODE_COLOR_BLACK, 
            NULL,
            node(3, RB_NODE_COLOR_RED, NULL, NULL))),
    COLN_RESULT_SUCCESS)
TEST_RM(
    test_rm_blk_leaf_selfright_parentred_siblingblack_cousinleftblack_cousinrightblack,
    node(0, RB_NODE_COLOR_BLACK,
        node(-2, RB_NODE_COLOR_BLACK, NULL, NULL),
        node(2, RB_NODE_COLOR_RED, 
            node(1, RB_NODE_COLOR_BLACK, NULL, NULL),
            node(3, RB_NODE_COLOR_BLACK, NULL, NULL))),
    3,
    node(0, RB_NODE_COLOR_BLACK,
        node(-2, RB_NODE_COLOR_BLACK, NULL, NULL),
        node(2, RB_NODE_COLOR_BLACK, 
            node(1, RB_NODE_COLOR_RED, NULL, NULL),
            NULL)),
    COLN_RESULT_SUCCESS)
TEST_RM(
    test_rm_blk_leaf_selfleft_parentred_siblingblack_cousinleftred_cousinrightblack,
    node(0, RB_NODE_COLOR_BLACK,
        node(-8, RB_NODE_COLOR_BLACK, 
            node(-12, RB_NODE_COLOR_BLACK, NULL, NULL), 
            node(-4, RB_NODE_COLOR_BLACK, NULL, NULL)),
        node(8, RB_NODE_COLOR_RED, 
            node(4, RB_NODE_COLOR_BLACK,
                node(2, RB_NODE_COLOR_BLACK, NULL, NULL), 
                node(6, RB_NODE_COLOR_BLACK, NULL, NULL)),
            node(12, RB_NODE_COLOR_BLACK,
                node(10, RB_NODE_COLOR_RED, 
                    node(9, RB_NODE_COLOR_BLACK, NULL, NULL),
                    node(11, RB_NODE_COLOR_BLACK, NULL, NULL)),
                node(14, RB_NODE_COLOR_BLACK, NULL, NULL)))),
    2,
    node(0, RB_NODE_COLOR_BLACK,
        node(-8, RB_NODE_COLOR_BLACK, 
            node(-12, RB_NODE_COLOR_BLACK, NULL, NULL), 
            node(-4, RB_NODE_COLOR_BLACK, NULL, NULL)),
        node(10, RB_NODE_COLOR_RED,
            node(8, RB_NODE_COLOR_BLACK,
                node(4, RB_NODE_COLOR_BLACK,
                    NULL,
                    node(6, RB_NODE_COLOR_RED, NULL, NULL)),
                node(9, RB_NODE_COLOR_BLACK, NULL, NULL)),
            node(12, RB_NODE_COLOR_BLACK,
                node(11, RB_NODE_COLOR_BLACK, NULL, NULL),
                node(14, RB_NODE_COLOR_BLACK, NULL, NULL)))),
    COLN_RESULT_SUCCESS)
TEST_RM(
    test_rm_blk_leaf_selfleft_parentblack_siblingblack_cousinleftred_cousinrightblack,
    node(0, RB_NODE_COLOR_BLACK,
        node(-8, RB_NODE_COLOR_BLACK, 
            node(-12, RB_NODE_COLOR_BLACK,
                node(-14, RB_NODE_COLOR_BLACK, NULL, NULL),
                node(-10, RB_NODE_COLOR_BLACK, NULL, NULL)), 
            node(-4, RB_NODE_COLOR_BLACK,
                node(-6, RB_NODE_COLOR_BLACK, NULL, NULL),
                node(-2, RB_NODE_COLOR_BLACK, NULL, NULL))),
        node(8, RB_NODE_COLOR_BLACK, 
            node(4, RB_NODE_COLOR_BLACK,
                node(2, RB_NODE_COLOR_BLACK, NULL, NULL), 
                node(6, RB_NODE_COLOR_BLACK, NULL, NULL)),
            node(12, RB_NODE_COLOR_BLACK,
                node(10, RB_NODE_COLOR_RED, 
                    node(9, RB_NODE_COLOR_BLACK, NULL, NULL),
                    node(11, RB_NODE_COLOR_BLACK, NULL, NULL)),
                node(14, RB_NODE_COLOR_BLACK, NULL, NULL)))),
    2,
    node(0, RB_NODE_COLOR_BLACK,
        node(-8, RB_NODE_COLOR_BLACK,
            node(-12, RB_NODE_COLOR_BLACK,
                node(-14, RB_NODE_COLOR_BLACK, NULL, NULL),
                node(-10, RB_NODE_COLOR_BLACK, NULL, NULL)), 
            node(-4, RB_NODE_COLOR_BLACK,
                node(-6, RB_NODE_COLOR_BLACK, NULL, NULL),
                node(-2, RB_NODE_COLOR_BLACK, NULL, NULL))),
        node(10, RB_NODE_COLOR_BLACK,
            node(8, RB_NODE_COLOR_BLACK,
                node(4, RB_NODE_COLOR_BLACK,
                    NULL,
                    node(6, RB_NODE_COLOR_RED, NULL, NULL)),
                node(9, RB_NODE_COLOR_BLACK, NULL, NULL)),
            node(12, RB_NODE_COLOR_BLACK,
                node(11, RB_NODE_COLOR_BLACK, NULL, NULL),
                node(14, RB_NODE_COLOR_BLACK, NULL, NULL)))),
    COLN_RESULT_SUCCESS)
TEST_RM(
    test_rm_blk_leaf_selfright_parentblack_siblingblack_cousinleftred_cousinrightblack,
    node(0, RB_NODE_COLOR_BLACK,
        node(-4, RB_NODE_COLOR_BLACK,
            node(-6, RB_NODE_COLOR_RED,
                node(-7, RB_NODE_COLOR_BLACK, NULL, NULL),
                node(-5, RB_NODE_COLOR_BLACK, NULL, NULL)),
            node(-2, RB_NODE_COLOR_BLACK, NULL, NULL)),
        node(4, RB_NODE_COLOR_BLACK,
            node(2, RB_NODE_COLOR_BLACK, NULL, NULL),
            node(6, RB_NODE_COLOR_BLACK, NULL, NULL))),
    6,
    node(-4, RB_NODE_COLOR_BLACK,
        node(-6, RB_NODE_COLOR_BLACK,
            node(-7, RB_NODE_COLOR_BLACK, NULL, NULL),
            node(-5, RB_NODE_COLOR_BLACK, NULL, NULL)),
        node(0, RB_NODE_COLOR_BLACK,
            node(-2, RB_NODE_COLOR_BLACK, NULL, NULL),
            node(4, RB_NODE_COLOR_BLACK,
                node(2, RB_NODE_COLOR_RED, NULL, NULL),
                NULL))),
    COLN_RESULT_SUCCESS)
TEST_RM(
    test_rm_blk_leaf_selfleft_siblingred,
    node(0, RB_NODE_COLOR_BLACK,
        node(-4, RB_NODE_COLOR_BLACK, NULL, NULL),
        node(4, RB_NODE_COLOR_RED,
            node(2, RB_NODE_COLOR_BLACK,
                node(1, RB_NODE_COLOR_RED, NULL, NULL),
                node(3, RB_NODE_COLOR_RED, NULL, NULL)),
            node(6, RB_NODE_COLOR_BLACK, NULL, NULL))),
    -4,
    node(4, RB_NODE_COLOR_BLACK,
        node(1, RB_NODE_COLOR_RED,
            node(0, RB_NODE_COLOR_BLACK, NULL, NULL),
            node(2, RB_NODE_COLOR_BLACK,
                NULL,
                node(3, RB_NODE_COLOR_RED, NULL, NULL))),
        node(6, RB_NODE_COLOR_BLACK, NULL, NULL)),
    COLN_RESULT_SUCCESS)

int test_with_allocator(void)
{
  dynamic_slot_allocator alloc;
  if(!dynamic_slot_allocator_init(&alloc, 
                                  sizeof(int_slot_allocd_red_black_tree_node),
                                  4))
    return 1;
  int_slot_allocd_red_black_tree tree;
  int_slot_allocd_red_black_tree_init(&tree, &alloc);
  int_slot_allocd_red_black_tree_insert(&tree, 11);
  int_slot_allocd_red_black_tree_insert(&tree, 2);
  int_slot_allocd_red_black_tree_insert(&tree, 5);
  int_slot_allocd_red_black_tree_insert(&tree, 3);
  int_slot_allocd_red_black_tree_insert(&tree, 7);
  if(alloc.used != 5) return 1;
  int_slot_allocd_red_black_tree tree2;
  int_slot_allocd_red_black_tree_copy(&tree2, &tree);
  if(alloc.used != 10) return 1;
  int_slot_allocd_red_black_tree_remove(&tree2, 5, NULL);
  if(alloc.used != 9) return 1;
  int_slot_allocd_red_black_tree_clear(&tree);
  if(alloc.used != 4) return 1;
  int_slot_allocd_red_black_tree_clear(&tree2);
  dynamic_slot_allocator_clear(&alloc);
  return 0;
}

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
    TEST_CASE(test_with_allocator),
    TEST_CASE(test_random)
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

static int cmp_fn(int *a, int *b)
{
    return *a - *b;
}

static void print_node(int_red_black_tree_node *n, int indentation)
{
    if(!n) return;
    print_node(LEFT_CHILD(n), indentation + 1);
    print_indentation(indentation);
    printf("%s     %d\n", (n->color == RB_NODE_COLOR_RED) ? "\033[31mRED\033[0m" : "\033[32mBLK\033[0m", n->data);
    print_node(RIGHT_CHILD(n), indentation + 1);
}

static void print_indentation(int indentation)
{
    for(int i = 0; i < indentation; i++)
    {
        printf("    ");
    }
}

static int_red_black_tree_node *node(int val,
                                     int color,
                                     int_red_black_tree_node *left,
                                     int_red_black_tree_node *right)
{
    int_red_black_tree_node *result = malloc(sizeof(int_red_black_tree_node));
    result->parent = NULL;
    result->data = val;
    result->color = color;
    LEFT_CHILD(result) = left;
    if(left) left->parent = result;
    RIGHT_CHILD(result) = right;
    if(right) right->parent = result;
    return result;
}

static bool node_eq(int_red_black_tree_node *a, int_red_black_tree_node *b)
{
    if(!a && !b) return true;
    if(!a && b) return false;
    if(a && !b) return false;
    if(a->color != b->color) return false;
    if(a->data != b->data) return false;
    return node_eq(LEFT_CHILD(a), LEFT_CHILD(b)) && 
        node_eq(RIGHT_CHILD(a), RIGHT_CHILD(b));
}

static bool verify_tree(int_red_black_tree *t)
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
    if(!verify_order(t->root))
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

static int verify_bdepth(int_red_black_tree_node *n)
{
    if(!n) return 1;
    int lbd = verify_bdepth(LEFT_CHILD(n));
    int rbd = verify_bdepth(RIGHT_CHILD(n));
    if(lbd != rbd) return -1;
    if(n->color == RB_NODE_COLOR_RED) return lbd;
    else return lbd + 1;
}

static bool verify_red_nadj(int_red_black_tree_node *n)
{
    if(!n) return true;
    if(n->color == RB_NODE_COLOR_RED)
    {
        if(LEFT_CHILD(n) && LEFT_CHILD(n)->color == RB_NODE_COLOR_RED) return false;
        if(RIGHT_CHILD(n) && RIGHT_CHILD(n)->color == RB_NODE_COLOR_RED) return false;
    }
    if(!verify_red_nadj(LEFT_CHILD(n))) return false;
    if(!verify_red_nadj(RIGHT_CHILD(n))) return false;
    return true;
}

static bool verify_order(int_red_black_tree_node *n)
{
    if(!n) return true;
    if(LEFT_CHILD(n) && cmp_fn(&(LEFT_CHILD(n)->data), &(n->data)) > 0)
        return false;
    if(RIGHT_CHILD(n) && cmp_fn(&(RIGHT_CHILD(n)->data), &(n->data)) < 0)
        return false;
    if(!verify_order(LEFT_CHILD(n))) return false;
    if(!verify_order(RIGHT_CHILD(n))) return false;
    return true;
}

static bool verify_backrefs(int_red_black_tree_node *n)
{
    if(!n) return true;
    if(LEFT_CHILD(n) && LEFT_CHILD(n)->parent != n) return false;
    if(RIGHT_CHILD(n) && RIGHT_CHILD(n)->parent != n) return false;
    if(!verify_backrefs(LEFT_CHILD(n))) return false;
    if(!verify_backrefs(RIGHT_CHILD(n))) return false;
    return true;
}
