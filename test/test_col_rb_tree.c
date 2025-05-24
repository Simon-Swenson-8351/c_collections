#include "col_rb_tree.c"

#include <assert.h>
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
    .elem_size = sizeof(int)
};

enum test_case_type
{
    TCT_INS,
    TCT_REM,

    TCT__COUNT
};

static void print_tree(struct col_rb_tree *t);
static void print_node(struct col_rb_tree_node *n, int indentation);
static void print_indentation(int indentation);

static struct col_rb_tree_node *node(int val, enum node_color color, struct col_rb_tree_node *left, struct col_rb_tree_node *right);
static bool node_eq(struct col_rb_tree_node *a, struct col_rb_tree_node *b);

#define FAIL(msg) 

int test_init(void);
int test_copy(void);
int test_clear(void);
int test_ins_root(void);
int test_ins_with_pr_ur(void);

struct test_case
{
    char const *name;
    int(*fn)(void);
};
#define TEST_CASE(fn_name) { .name = #fn_name, .fn = fn_name }
static struct test_case TEST_CASES[] =
{
    TEST_CASE(test_init),
    TEST_CASE(test_copy),
    TEST_CASE(test_clear),
    TEST_CASE(test_ins_root),
    TEST_CASE(test_ins_with_pr_ur),
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

int test_ins_root(void)
{
    struct col_rb_tree t;
    col_rb_tree_init(&t, NULL, &md);

    struct col_rb_tree expect_out;
    col_rb_tree_init(&expect_out, NULL, &md);
    expect_out.root = node(2, NC_BLACK, NULL, NULL);

    int i = 2;
    col_rb_tree_insert(&t, &i);

    if(!node_eq(t.root, expect_out.root)) return 1;
    col_rb_tree_clear(&t);
    col_rb_tree_clear(&expect_out);
    return 0;
}

int test_ins_with_pr_ur(void)
{
    struct col_rb_tree t;
    col_rb_tree_init(&t, NULL, &md);
    t.root = 
        node(32, NC_BLACK,
            node(0, NC_BLACK,
                node(-16, NC_RED, NULL, NULL),
                node(16, NC_RED, NULL, NULL)),
            node(48, NC_BLACK, NULL, NULL));

    struct col_rb_tree expect_out;
    col_rb_tree_init(&expect_out, NULL, &md);
    expect_out.root =
        node(32, NC_BLACK,
            node(0, NC_RED,
                node(-16, NC_BLACK, 
                    node(-32, NC_RED, NULL, NULL),
                    NULL),
                node(16, NC_BLACK, NULL, NULL)),
            node(48, NC_BLACK, NULL, NULL));

    int to_ins = -32;
    col_rb_tree_insert(&t, &to_ins);
    if(!node_eq(t.root, expect_out.root)) return 1;
    col_rb_tree_clear(&t);
    col_rb_tree_clear(&expect_out);
    return 0;
}

static int cmp_fn(void *a, void *b)
{
    if(*((int *)a) < *((int *)b)) return -1;
    else if(*((int *)a) == *((int *)b)) return 0;
    else return 1;
}

static void print_tree(struct col_rb_tree *t) { print_node(t->root, 0); }

static void print_node(struct col_rb_tree_node *n, int indentation)
{
    if(!n) return;
    print_node(LEFT_CHILD(n), indentation + 1);
    print_indentation(indentation);
    printf("%s     %d\n", (n->color == NC_RED) ? "\033[31mRED\033[0m" : "\033[32mBLK\033[0m", *((int *)node_data(n)));
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
    if(*((int *)node_data(a)) != *((int *)node_data(b))) return false;
    return node_eq(LEFT_CHILD(a), LEFT_CHILD(b)) && node_eq(RIGHT_CHILD(a), RIGHT_CHILD(b));
}
