#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "coln_result.h"

#define COLN_CAT_(a, b) a ## b
#define COLN_CAT(a, b) COLN_CAT_(a, b)

#ifndef COLN_INTERNAL_NDEBUG
  #define COLN_INTERNAL_ASSERT(x) assert(x)
#else
  #define COLN_INTERNAL_ASSERT(x)
#endif

#if !defined(RB_TREE_HEADER) && !defined(RB_TREE_IMPL)
  #error "RB_TREE_HEADER or RB_TREE_IMPL must be defined"
#endif

#ifndef DATA_TYPENAME
  #error "Collection macros require DATA_TYPENAME"
#endif

#if !defined(DATA_PASS_BY_VAL) && !defined(DATA_PASS_BY_PTR)
  #error "Define either DATA_PASS_BY_VAL or DATA_PASS_BY_PTR"
#endif

#if defined(DATA_PASS_BY_VAL) && defined(DATA_PASS_BY_PTR)
  #error "Define only one of DATA_PASS_BY_VAL or DATA_PASS_BY_PTR"
#endif

#ifndef DATA_COMPARE
  #error "Red-black Tree requires a compare function or macro, DATA_COMPARE, "
    "with a signature of [int DATA_COMPARE(DATA_TYPENAME, DATA_TYPENAME)] or " \
    "[int DATA_COMPARE(DATA_TYPENAME *, DATA_TYPENAME *)] depending on the " \
    "specified passing semantics"
#endif

#if defined(DATA_PASS_BY_VAL)
  #define DATA_ARG(arg_name) DATA_TYPENAME arg_name
  #define DATA_ASSERT_ARG(arg_name)
  #ifdef DATA_MOVE
    #define DATA_MOVE_VAL_FROM_ARG(val, arg) \
      ((val) = DATA_MOVE((arg)))
  #else
    #define DATA_MOVE_VAL_FROM_ARG(val, arg) \
      ((val) = (arg))
  #endif
  #define DATA_COMPARE_VAL_VAL(a, b) DATA_COMPARE((a), (b))
  #define DATA_COMPARE_VAL_ARG(val, arg) DATA_COMPARE((val), (arg))
  #define DATA_COPY_VAL_FROM_VAL(dest_val, src_val) \
    DATA_COPY(&(dest_val), (src_val))
#elif defined(DATA_PASS_BY_PTR)
  #define DATA_ARG(arg_name) DATA_TYPENAME *arg_name
  #define DATA_ASSERT_ARG(arg_name) assert(arg_name)
  #ifdef DATA_MOVE
    #define DATA_MOVE_VAL_FROM_ARG(val, arg) \
      DATA_MOVE(&(val), (arg))
  #else
    #define DATA_MOVE_VAL_FROM_ARG(val, arg) \
      ((val) = *(arg))
  #endif
  #define DATA_COMPARE_VAL_VAL(a, b) DATA_COMPARE(&(a), &(b))
  #define DATA_COMPARE_VAL_ARG(val, arg) DATA_COMPARE(&(val), (arg))
  #define DATA_COPY_VAL_FROM_VAL(dest_val, src_val) \
    DATA_COPY(&(dest_val), &(src_val))
#endif

#ifdef DATA_MOVE
  #define DATA_MOVE_PTR_FROM_VAL(ptr, val) DATA_MOVE((ptr), &(val))
  #define DATA_MOVE_VAL_FROM_VAL(dest_val, src_val) \
    DATA_MOVE(&(dest_val), &(src_val))
#else
  #define DATA_MOVE_PTR_FROM_VAL(ptr, val) (*(ptr) = (val))
  #define DATA_MOVE_VAL_FROM_VAL(dest_val, src_val) \
    ((dest_val) = (src_val))
#endif

#ifdef DATA_CLEAR
  #if defined(DATA_PASS_BY_VAL)
    #define DATA_CLEAR_VAL(val_to_clear) DATA_CLEAR((val_to_clear))
  #elif defined(DATA_PASS_BY_PTR)
    #define DATA_CLEAR_VAL(val_to_clear) DATA_CLEAR(&(val_to_clear))
  #endif
#else
  #define DATA_CLEAR_VAL(val_to_clear)
#endif

#ifdef ALLOC_TYPENAME
  #define ALLOC_ARG(allocator) , ALLOC_TYPENAME *allocator
  #ifndef ALLOC_ALLOC
    #error "Collection macros require an allocation function if an allocator " \
      "type is defined."
  #endif
  #ifndef ALLOC_FREE
    #define ALLOC_FREE_SET
    #define ALLOC_FREE(allocator, ptr_to_free)
  #endif
#else
  #define ALLOC_ARG(allocator)
  #define ALLOC_ALLOC_SET
  #define ALLOC_ALLOC(allocator, size_to_alloc) malloc(size_to_alloc)
  #define ALLOC_FREE_SET
  #define ALLOC_FREE(allocator, ptr_to_free) free(ptr_to_free)
#endif

#ifndef RB_TREE_TYPENAME
  #define RB_TREE_TYPENAME_SET
  #define RB_TREE_TYPENAME COLN_CAT(DATA_TYPENAME, _rb_tree)
#endif

#define RB_NODE_TYPENAME COLN_CAT(RB_TREE_TYPENAME, _node)

#define RB_TREE_INIT_FNNAME COLN_CAT(RB_TREE_TYPENAME, _init)
#define RB_TREE_INIT_SIGN \
    void RB_TREE_INIT_FNNAME(RB_TREE_TYPENAME *to_init \
                             ALLOC_ARG(allocator))

#ifndef DATA_NO_COPY
  #define RB_TREE_COPY_FNNAME COLN_CAT(RB_TREE_TYPENAME, _copy)
  #define RB_TREE_COPY_SIGN \
    coln_result RB_TREE_COPY_FNNAME(RB_TREE_TYPENAME *dest, \
                                    RB_TREE_TYPENAME *src)
  #define RB_NODE_COPY_FNNAME COLN_CAT(RB_NODE_TYPENAME, _copy)
  #define RB_NODE_COPY_SIGN \
    static coln_result RB_NODE_COPY_FNNAME(RB_NODE_TYPENAME **new_dest, \
                                           RB_NODE_TYPENAME *src \
                                           ALLOC_ARG(allocator))
  #ifdef ALLOC_TYPENAME
    #define RB_NODE_COPY_CALL(new_dest_ptr_ptr, src_ptr, allocator) \
      RB_NODE_COPY_FNNAME((new_dest_ptr_ptr), (src_ptr), (allocator))
  #else
    #define RB_NODE_COPY_CALL(new_dest_ptr_ptr, src_ptr, allocator) \
      RB_NODE_COPY_FNNAME((new_dest_ptr_ptr), (src_ptr))
  #endif
#endif

#if defined(DATA_CLEAR) || !defined(ALLOC_TYPENAME) || !defined(ALLOC_FREE_SET)
  #define RB_TREE_CLEAR_FNNAME COLN_CAT(RB_TREE_TYPENAME, _clear)
  #define RB_TREE_CLEAR_SIGN void RB_TREE_CLEAR_FNNAME(RB_TREE_TYPENAME *to_clear)
  #define RB_NODE_DESTROY_FNNAME COLN_CAT(RB_NODE_TYPENAME, _destroy)
  #define RB_NODE_DESTROY_SIGN \
    static void RB_NODE_DESTROY_FNNAME(RB_NODE_TYPENAME *to_destroy \
                                       ALLOC_ARG(allocator))
  #ifdef ALLOC_TYPENAME
    #define RB_NODE_DESTROY_CALL(to_destroy, allocator) \
      RB_NODE_DESTROY_FNNAME((to_destroy), (allocator))
  #else
    #define RB_NODE_DESTROY_CALL(to_destroy, allocator) \
      RB_NODE_DESTROY_FNNAME((to_destroy))
  #endif
#endif

#define RB_TREE_INSERT_FNNAME COLN_CAT(RB_TREE_TYPENAME, _insert)
#define RB_TREE_INSERT_SIGN \
    coln_result RB_TREE_INSERT_FNNAME(RB_TREE_TYPENAME *tree, \
                                      DATA_ARG(to_insert))
#define RB_TREE_HANDLE_RED_VIOLATION_FNNAME \
  COLN_CAT(RB_TREE_TYPENAME, _handle_red_violation)
#define RB_TREE_HANDLE_RED_VIOLATION_SIGN \
  static void RB_TREE_HANDLE_RED_VIOLATION_FNNAME(RB_TREE_TYPENAME *tree, \
                                                  RB_NODE_TYPENAME *node) 

#define RB_TREE_SEARCH_FNNAME COLN_CAT(RB_TREE_TYPENAME, _search)
#define RB_TREE_SEARCH_SIGN \
  DATA_TYPENAME *RB_TREE_SEARCH_FNNAME(RB_TREE_TYPENAME *tree, \
                                            DATA_ARG(elem_to_search))

#define RB_TREE_REMOVE_FNNAME COLN_CAT(RB_TREE_TYPENAME, _remove)
#define RB_TREE_REMOVE_SIGN \
  coln_result RB_TREE_REMOVE_FNNAME(RB_TREE_TYPENAME *tree, \
                                    DATA_ARG(elem_to_remove), \
                                    DATA_TYPENAME *removed_elem)
#define RB_TREE_HANDLE_BLACK_VIOLATION_FNNAME COLN_CAT( \
  RB_TREE_TYPENAME, \
  _handle_black_violation)
#define RB_TREE_HANDLE_BLACK_VIOLATION_SIGN \
  static void RB_TREE_HANDLE_BLACK_VIOLATION_FNNAME( \
    RB_TREE_TYPENAME *tree, \
    RB_NODE_TYPENAME *parent, \
    int violator_pos)

#define RB_NODE_ROTATE_FNNAME COLN_CAT(RB_NODE_TYPENAME, _rotate)
#define RB_NODE_ROTATE_SIGN \
  static void RB_NODE_ROTATE_FNNAME(RB_NODE_TYPENAME **accessor, int from_dir)

#define RB_NODE_ROTATE_FROM_RR_FNNAME \
  COLN_CAT(RB_NODE_TYPENAME, _rotate_from_rr)
#define RB_NODE_ROTATE_FROM_RR_SIGN \
  static void RB_NODE_ROTATE_FROM_RR_FNNAME(RB_NODE_TYPENAME **accessor)

#define RB_NODE_ROTATE_FROM_LL_FNNAME \
  COLN_CAT(RB_NODE_TYPENAME, _rotate_from_ll)
#define RB_NODE_ROTATE_FROM_LL_SIGN \
  static void RB_NODE_ROTATE_FROM_LL_FNNAME(RB_NODE_TYPENAME **accessor)

#define RB_NODE_ROTATE_FROM_LR_FNNAME \
  COLN_CAT(RB_NODE_TYPENAME, _rotate_from_lr)
#define RB_NODE_ROTATE_FROM_LR_SIGN \
  static void RB_NODE_ROTATE_FROM_LR_FNNAME(RB_NODE_TYPENAME **accessor)

#define RB_NODE_ROTATE_FROM_RL_FNNAME COLN_CAT(RB_NODE_TYPENAME, _rotate_from_rl)
#define RB_NODE_ROTATE_FROM_RL_SIGN \
  static void RB_NODE_ROTATE_FROM_RL_FNNAME(RB_NODE_TYPENAME **accessor) 

#ifdef RB_TREE_HEADER

    typedef struct RB_NODE_TYPENAME
    {
      struct RB_NODE_TYPENAME *parent;
      struct RB_NODE_TYPENAME *children[2];
      int color;
      DATA_TYPENAME data;
    } RB_NODE_TYPENAME;

    typedef struct RB_TREE_TYPENAME
    {
  #ifdef ALLOC_TYPENAME
      ALLOC_TYPENAME *allocator;
  #endif
      RB_NODE_TYPENAME *root;
    } RB_TREE_TYPENAME;

    RB_TREE_INIT_SIGN;

  #ifndef DATA_NO_COPY
    RB_TREE_COPY_SIGN;
  #endif

  #if defined(DATA_CLEAR) || \
      !defined(ALLOC_TYPENAME) || \
      !defined(ALLOC_FREE_SET)
    RB_TREE_CLEAR_SIGN;
  #endif

    RB_TREE_INSERT_SIGN;
    RB_TREE_SEARCH_SIGN;
    RB_TREE_REMOVE_SIGN;

#endif

#ifdef RB_TREE_IMPL
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

  #ifndef DATA_NO_COPY
    RB_NODE_COPY_SIGN;
  #endif

  #if defined(DATA_CLEAR) || \
      !defined(ALLOC_TYPENAME) || \
      !defined(ALLOC_FREE_SET)
    RB_NODE_DESTROY_SIGN;
  #endif

    RB_TREE_HANDLE_RED_VIOLATION_SIGN;
    RB_TREE_HANDLE_BLACK_VIOLATION_SIGN;
    RB_NODE_ROTATE_SIGN;
    RB_NODE_ROTATE_FROM_RR_SIGN;
    RB_NODE_ROTATE_FROM_LL_SIGN;
    RB_NODE_ROTATE_FROM_LR_SIGN;
    RB_NODE_ROTATE_FROM_RL_SIGN;

    RB_TREE_INIT_SIGN
    {
      assert(to_init);
    #ifdef ALLOC_TYPENAME
      assert(allocator);
      to_init->allocator = allocator;
    #endif
      to_init->root = NULL;
    }

  #ifndef DATA_NO_COPY
    RB_TREE_COPY_SIGN
    {
      assert(dest);
      assert(src);
    #ifdef ALLOC_TYPENAME
      dest->allocator = src->allocator;
    #endif
      coln_result result;
      RB_NODE_TYPENAME *new_node;
      if((result = RB_NODE_COPY_CALL(&new_node, src->root, src->allocator)))
        return result;
      dest->root = new_node;
      return COLN_RESULT_SUCCESS;
    }

    RB_NODE_COPY_SIGN
    {
      COLN_INTERNAL_ASSERT(new_dest);
    #ifdef ALLOC_TYPENAME
      assert(allocator);
    #endif
      if(!src)
      {
        *new_dest = NULL;
        return COLN_RESULT_SUCCESS;
      }
      coln_result result;
      *new_dest = ALLOC_ALLOC(allocator, sizeof(RB_NODE_TYPENAME));
      if(!(*new_dest))
      {
        result = COLN_RESULT_ALLOC_FAILED;
        goto fail_on_alloc;
      }
    #ifdef DATA_COPY
      if(!DATA_COPY_VAL_FROM_VAL((*new_dest)->data, src->data))
      {
        result = COLN_RESULT_COPY_ELEM_FAILED;
        goto fail_on_elem_cp;
      }
    #else  
      (*new_dest)->data = src->data;
    #endif
      if((result = RB_NODE_COPY_CALL(&LEFT_CHILD(*new_dest),
                                     LEFT_CHILD(src),
                                     allocator)))
        goto fail_on_left;
      if((result = RB_NODE_COPY_CALL(&RIGHT_CHILD(*new_dest),
                                     RIGHT_CHILD(src),
                                     allocator)))
        goto fail_on_right;
      (*new_dest)->color = src->color;
      return COLN_RESULT_SUCCESS;
    fail_on_right:
      RB_NODE_DESTROY_CALL(LEFT_CHILD(*new_dest), allocator);
    fail_on_left:
      DATA_CLEAR_VAL((*new_dest)->data);
    /* The reason for using this label snippet is to shut a compiler */
    /* warning up. */
    #ifdef DATA_COPY
    // This will shut up a compiler warning about this label if it's unused
    fail_on_elem_cp:
    #endif
      ALLOC_FREE(allocator, *new_dest);
      *new_dest = NULL;
    fail_on_alloc:
      return result;
    }
  #endif

  #if defined(DATA_CLEAR) || \
      !defined(ALLOC_TYPENAME) || \
      !defined(ALLOC_FREE_SET)
    RB_TREE_CLEAR_SIGN
    {
      RB_NODE_DESTROY_CALL(to_clear->root, to_clear->allocator);
    }

    RB_NODE_DESTROY_SIGN
    {
      if(!to_destroy) return;
      RB_NODE_DESTROY_CALL(LEFT_CHILD(to_destroy), allocator);
      RB_NODE_DESTROY_CALL(RIGHT_CHILD(to_destroy), allocator);
      DATA_CLEAR_VAL(to_destroy->data);
      ALLOC_FREE(allocator, to_destroy);
    }
  #endif

    RB_TREE_INSERT_SIGN
    {
      assert(tree);
    #ifdef DATA_PASS_BY_PTR
      assert(to_insert);
    #endif
      RB_NODE_TYPENAME *node_to_insert =
        ALLOC_ALLOC(tree->allocator, sizeof(RB_NODE_TYPENAME));
      if(!node_to_insert) return COLN_RESULT_ALLOC_FAILED;
      memset(node_to_insert->children, 0, sizeof(node_to_insert->children));
      node_to_insert->color = RB_NODE_COLOR_RED;
      DATA_MOVE_VAL_FROM_ARG(node_to_insert->data, to_insert);
      RB_NODE_TYPENAME *parent = NULL;
      RB_NODE_TYPENAME **insertion_point = &(tree->root);
      while(*insertion_point)
      {
        parent = *insertion_point;
        if(DATA_COMPARE_VAL_VAL(
            (*insertion_point)->data,
            node_to_insert->data) > 0)
          insertion_point = &LEFT_CHILD(*insertion_point);
        else
          insertion_point = &RIGHT_CHILD(*insertion_point);
      }
      node_to_insert->parent = parent;
      *insertion_point = node_to_insert;
      RB_TREE_HANDLE_RED_VIOLATION_FNNAME(tree, node_to_insert);
      return COLN_RESULT_SUCCESS;
    }

    RB_TREE_HANDLE_RED_VIOLATION_SIGN
    {
      /* node is red and node->parent is red */
      while(true) \
      {
        if(!(node->parent))
        {
          node->color = RB_NODE_COLOR_BLACK;
          return;
        }
        else if(node->parent->color == RB_NODE_COLOR_BLACK) return;
        /* Note: Grandparent exists since parent is red and the root */
        /* cannot be red. */
        RB_NODE_TYPENAME *uncle =
          (node->parent == LEFT_CHILD(node->parent->parent)) ?
            RIGHT_CHILD(node->parent->parent) :
            LEFT_CHILD(node->parent->parent);
        if(COLOR(uncle) == RB_NODE_COLOR_RED)
        {
          node->parent->parent->color = RB_NODE_COLOR_RED;
          LEFT_CHILD(node->parent->parent)->color = RB_NODE_COLOR_BLACK;
          RIGHT_CHILD(node->parent->parent)->color = RB_NODE_COLOR_BLACK;
          node = node->parent->parent;
        }
        else
        {
          /*  gp is black */
          /*      parent is red */
          /*          cur is red */
          /*              c1 is black, since the red violations of */
          /*              the subtree would have been cleaned up in a */
          /*              previous iteration */
          /*              c2 is black, since the red violations of */
          /*              the subtree would have been cleaned up in a */
          /*              previous iteration */
          /*          sibling is black */
          /*      uncle is black */
          
          /*  Note that a rotation on gp, parent, cur will result in */
          /*  the lower level of the tree (grandchild level) being */
          /*  c1, c2, sibling, and uncle, which are all black. */
          /*  Therefore, setting the next level up (child level) to */
          /*  red will not cause a violation farther down the tree. */
          RB_NODE_TYPENAME **gp_accessor = ACCESSOR(tree, node->parent->parent);
          if(node == LEFT_CHILD(node->parent))
          {
            if(node->parent == LEFT_CHILD(node->parent->parent))
              RB_NODE_ROTATE_FROM_LL_FNNAME(gp_accessor);
            else RB_NODE_ROTATE_FROM_RL_FNNAME(gp_accessor);
          }
          else /* node == node->parent->right */
          {
            if(node->parent == LEFT_CHILD(node->parent->parent))
              RB_NODE_ROTATE_FROM_LR_FNNAME(gp_accessor);
            else RB_NODE_ROTATE_FROM_RR_FNNAME(gp_accessor);
          }
          node = *gp_accessor;
          node->color = RB_NODE_COLOR_BLACK;
          LEFT_CHILD(node)->color = RB_NODE_COLOR_RED;
          RIGHT_CHILD(node)->color = RB_NODE_COLOR_RED;
          return;
        }
      }
    }

    RB_TREE_SEARCH_SIGN
    {
      assert(tree);
    #ifdef DATA_PASS_BY_PTR
      assert(elem_to_search);
    #endif
      RB_NODE_TYPENAME *cur = tree->root;
      while(cur)
      {
        int cmp_res = DATA_COMPARE_VAL_ARG(cur->data, elem_to_search);
        if(cmp_res < 0) cur = RIGHT_CHILD(cur);
        else if(cmp_res == 0) return &(cur->data);
        else cur = LEFT_CHILD(cur);
      }
      return NULL;
    }

    RB_TREE_REMOVE_SIGN
    {
      assert(tree);
    #ifdef DATA_PASS_BY_PTR
      DATA_ASSERT_ARG(elem_to_remove);
    #endif
      int cur_pos;
      RB_NODE_TYPENAME **removal_point = &(tree->root);
      while(*removal_point)
      {
        int cmp_res = DATA_COMPARE_VAL_ARG((*removal_point)->data,
                                           elem_to_remove);
        if(cmp_res < 0)
        {
          cur_pos = RB_NODE_DIR_RIGHT;
          removal_point = &RIGHT_CHILD(*removal_point);
        }
        else if(cmp_res == 0)
        {
          break;
        }
        else
        {
          cur_pos = RB_NODE_DIR_LEFT;
          removal_point = &LEFT_CHILD(*removal_point);
        }
      }
      if(!(*removal_point)) return COLN_RESULT_ELEM_NOT_FOUND;
      if(removed_elem)
        DATA_MOVE_PTR_FROM_VAL(removed_elem, (*removal_point)->data);
      if(LEFT_CHILD(*removal_point) && RIGHT_CHILD(*removal_point))
      {
        RB_NODE_TYPENAME **succ_ptr = &RIGHT_CHILD(*removal_point);
        cur_pos = RB_NODE_DIR_RIGHT;
        while(LEFT_CHILD(*succ_ptr))
        {
          succ_ptr = &LEFT_CHILD(*succ_ptr);
          cur_pos = RB_NODE_DIR_LEFT;
        }
        DATA_MOVE_VAL_FROM_VAL((*removal_point)->data, (*succ_ptr)->data);
        removal_point = succ_ptr;
      }
      /*  We want the above case where we replace with successor to */
      /* fall-through so that a node is eventually removed. */
      RB_NODE_TYPENAME *to_remove = *removal_point;
      if(LEFT_CHILD(to_remove))
      {
        *removal_point = LEFT_CHILD(*removal_point);
        LEFT_CHILD(to_remove)->parent = to_remove->parent;
        LEFT_CHILD(to_remove)->color = RB_NODE_COLOR_BLACK;
      }
      else if(RIGHT_CHILD(to_remove))
      {
        *removal_point = RIGHT_CHILD(to_remove);
        RIGHT_CHILD(to_remove)->parent = to_remove->parent;
        RIGHT_CHILD(to_remove)->color = RB_NODE_COLOR_BLACK;
      }
      else
      {
        *removal_point = NULL;
        if(COLOR(to_remove) == RB_NODE_COLOR_BLACK)
          RB_TREE_HANDLE_BLACK_VIOLATION_FNNAME(tree,
                                                to_remove->parent,
                                                cur_pos);
      }
      ALLOC_FREE(tree->allocator, to_remove);
      return COLN_RESULT_SUCCESS;
    }

    RB_TREE_HANDLE_BLACK_VIOLATION_SIGN
    {
      static void(*BLACK_VIOLATION_ROT_LUT[2][2])(RB_NODE_TYPENAME **) =
      {
        {
          RB_NODE_ROTATE_FROM_RL_FNNAME,
          RB_NODE_ROTATE_FROM_RR_FNNAME
        },
        {
          RB_NODE_ROTATE_FROM_LL_FNNAME,
          RB_NODE_ROTATE_FROM_LR_FNNAME
        }
      };
      while(parent)
      {
        RB_NODE_TYPENAME *sibling = (violator_pos == RB_NODE_DIR_LEFT) ?
          RIGHT_CHILD(parent) :
          LEFT_CHILD(parent);
        if(COLOR(parent) == RB_NODE_COLOR_RED)
        {
          if(COLOR(LEFT_CHILD(sibling)) == RB_NODE_COLOR_BLACK &&
            COLOR(RIGHT_CHILD(sibling)) == RB_NODE_COLOR_BLACK)
          {
            parent->color = RB_NODE_COLOR_BLACK;
            sibling->color = RB_NODE_COLOR_RED;
            return;
          }
          else
          {
            RB_NODE_TYPENAME **parent_accessor = ACCESSOR(tree, parent);
            BLACK_VIOLATION_ROT_LUT[violator_pos][COLOR(LEFT_CHILD(sibling))](
              parent_accessor);
            parent = *parent_accessor;
            parent->color = RB_NODE_COLOR_RED;
            LEFT_CHILD(parent)->color = RB_NODE_COLOR_BLACK;
            RIGHT_CHILD(parent)->color = RB_NODE_COLOR_BLACK;
            return;
          }
        }
        else /* node_color(parent) == NODE_COLOR_BLACK */
        {
          if(COLOR(sibling) == RB_NODE_COLOR_RED)
          {
            parent->color = RB_NODE_COLOR_RED;
            sibling->color = RB_NODE_COLOR_BLACK;
            RB_NODE_TYPENAME **parent_accessor = ACCESSOR(tree, parent);
            switch(violator_pos)
            {
              case RB_NODE_DIR_LEFT:
                RB_NODE_ROTATE_FROM_RR_FNNAME(parent_accessor);
                break;
              case RB_NODE_DIR_RIGHT:
                RB_NODE_ROTATE_FROM_LL_FNNAME(parent_accessor);
                break;
            }
            /* This moves the redness in sibling to be the parent */
            /* of the violator so that case 1 above can run next */
            /* iteration and terminate. */
          }
          else
          {
            if(COLOR(LEFT_CHILD(sibling)) == RB_NODE_COLOR_BLACK &&
              COLOR(RIGHT_CHILD(sibling)) == RB_NODE_COLOR_BLACK)
            {
              sibling->color = RB_NODE_COLOR_RED;
              if(parent->parent && parent == LEFT_CHILD(parent->parent))
                violator_pos = RB_NODE_DIR_LEFT;
              else
                violator_pos = RB_NODE_DIR_RIGHT;
              parent = parent->parent;
            }
            else
            {
              RB_NODE_TYPENAME **parent_accessor = ACCESSOR(tree, parent);
              BLACK_VIOLATION_ROT_LUT[violator_pos][COLOR(LEFT_CHILD(sibling))](
                parent_accessor);
              parent = *parent_accessor;
              parent->color = RB_NODE_COLOR_BLACK;
              LEFT_CHILD(parent)->color = RB_NODE_COLOR_BLACK;
              RIGHT_CHILD(parent)->color = RB_NODE_COLOR_BLACK;
              return;
            }
          }
        }
      }
    }

    RB_NODE_ROTATE_SIGN
    {
      COLN_INTERNAL_ASSERT(accessor);
      COLN_INTERNAL_ASSERT(*accessor);
      COLN_INTERNAL_ASSERT((*accessor)->children[from_dir]);
      RB_NODE_TYPENAME *old_top = *accessor;
      RB_NODE_TYPENAME *new_top = (*accessor)->children[from_dir];
      *accessor = new_top;
      new_top->parent = old_top->parent;
      old_top->children[from_dir] =
        new_top->children[1 - from_dir];
      if(old_top->children[from_dir])
        old_top->children[from_dir]->parent = old_top;
      new_top->children[1 - from_dir] = old_top;
      old_top->parent = new_top;
    }

    RB_NODE_ROTATE_FROM_RR_SIGN
    {
      RB_NODE_ROTATE_FNNAME(accessor, RB_NODE_DIR_RIGHT);
    }

    RB_NODE_ROTATE_FROM_LL_SIGN
    {
      RB_NODE_ROTATE_FNNAME(accessor, RB_NODE_DIR_LEFT);
    }

    RB_NODE_ROTATE_FROM_LR_SIGN
    {
      COLN_INTERNAL_ASSERT(accessor);
      COLN_INTERNAL_ASSERT(*accessor);
      RB_NODE_ROTATE_FNNAME(&LEFT_CHILD(*accessor), RB_NODE_DIR_RIGHT);
      RB_NODE_ROTATE_FNNAME(accessor, RB_NODE_DIR_LEFT);
    }

    RB_NODE_ROTATE_FROM_RL_SIGN
    {
      COLN_INTERNAL_ASSERT(accessor);
      COLN_INTERNAL_ASSERT(*accessor);
      RB_NODE_ROTATE_FNNAME(&RIGHT_CHILD(*accessor), RB_NODE_DIR_LEFT);
      RB_NODE_ROTATE_FNNAME(accessor, RB_NODE_DIR_RIGHT);
    }

    #undef COLOR
    #undef ACCESSOR
    #undef RIGHT_CHILD
    #undef LEFT_CHILD
    #undef RB_NODE_DIR_RIGHT
    #undef RB_NODE_DIR_LEFT
    #undef RB_NODE_COLOR_BLACK
    #undef RB_NODE_COLOR_RED
#endif

#undef RB_NODE_ROTATE_FROM_RL_SIGN
#undef RB_NODE_ROTATE_FROM_RL_FNNAME
#undef RB_NODE_ROTATE_FROM_LR_SIGN
#undef RB_NODE_ROTATE_FROM_LR_FNNAME
#undef RB_NODE_ROTATE_FROM_LL_SIGN
#undef RB_NODE_ROTATE_FROM_LL_FNNAME
#undef RB_NODE_ROTATE_FROM_RR_SIGN
#undef RB_NODE_ROTATE_FROM_RR_FNNAME
#undef RB_NODE_ROTATE_SIGN
#undef RB_NODE_ROTATE_FNNAME

#undef RB_TREE_HANDLE_BLACK_VIOLATION_SIGN
#undef RB_TREE_HANDLE_BLACK_VIOLATION_FNNAME
#undef RB_TREE_REMOVE_SIGN
#undef RB_TREE_REMOVE_FNNAME

#undef RB_TREE_SEARCH_SIGN
#undef RB_TREE_SEARCH_FNNAME

#undef RB_TREE_HANDLE_RED_VIOLATION_SIGN
#undef RB_TREE_HANDLE_RED_VIOLATION_FNNAME
#undef RB_TREE_INSERT_SIGN
#undef RB_TREE_INSERT_FNNAME

#undef RB_NODE_DESTROY_CALL
#undef RB_NODE_DESTROY_SIGN
#undef RB_NODE_DESTROY_FNNAME
#undef RB_TREE_CLEAR_SIGN
#undef RB_TREE_CLEAR_FNNAME

#ifndef DATA_NO_COPY
  #undef RB_NODE_COPY_CALL
  #undef RB_NODE_COPY_SIGN
  #undef RB_NODE_COPY_FNNAME
  #undef RB_TREE_COPY_SIGN
  #undef RB_TREE_COPY_FNNAME
#endif

#undef RB_TREE_INIT_SIGN
#undef RB_TREE_INIT_FNNAME

#undef RB_TREE_STRUCT_DEFN
#undef RB_NODE_STRUCT_DEFN
#undef RB_NODE_TYPENAME

#ifdef RB_TREE_TYPENAME_SET
  #undef RB_TREE_TYPENAME_SET
  #undef RB_TREE_TYPENAME
#endif

#ifdef ALLOC_FREE_SET
  #undef ALLOC_FREE_SET
  #undef ALLOC_FREE
#endif

#ifdef ALLOC_ALLOC_SET
  #undef ALLOC_ALLOC_SET
  #undef ALLOC_ALLOC
#endif

#undef ALLOC_ARG
#undef DATA_CLEAR_VAL
#undef DATA_COPY_VAL_FROM_VAL
#undef DATA_MOVE_PTR_FROM_VAL
#undef DATA_MOVE_VAL_FROM_VAL
#undef DATA_COMPARE_VAL_ARG
#undef DATA_COMPARE_VAL_VAL
#undef DATA_MOVE_VAL_FROM_ARG
#undef DATA_ASSERT_ARG
#undef DATA_ARG
#undef COLN_INTERNAL_ASSERT
#undef COLN_CAT 
#undef COLN_CAT_
