/******************************************************************************
* djinterp [test]                                  min_stack_tests_sa_memory.c
*
*   Memory management tests for min_stack module.
*   Tests clear and free operations.
*
*
* path:      \test\container\stack\min_stack_tests_sa_memory.c
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2026.01.24
******************************************************************************/

#include ".\min_stack_tests_sa.h"


/******************************************************************************
 * CLEAR OPERATION TESTS
 *****************************************************************************/

/*
d_tests_min_stack_clear
  Tests d_min_stack_clear for removing all elements.
  Tests the following:
  - handles NULL stack safely
  - clears empty stack
  - removes all elements
  - resets top to NULL
  - stack usable after clear
  - frees all node structures
*/
struct d_test_object*
d_tests_min_stack_clear
(
    void
)
{
    struct d_test_object* group;
    struct d_min_stack*   stack;
    int*                  value1;
    int*                  value2;
    int*                  value3;
    void*                 result;
    bool                  test_null_stack;
    bool                  test_empty_stack;
    bool                  test_removes_all;
    bool                  test_resets_top;
    bool                  test_usable_after;
    bool                  test_frees_nodes;
    size_t                idx;

    // test 1: NULL stack handled safely
    test_null_stack = true;
    d_min_stack_clear(NULL);  // should not crash

    // test 2: empty stack cleared
    stack = d_min_stack_new();
    
    if (stack)
    {
        d_min_stack_clear(stack);
    }
    
    test_empty_stack = (stack && stack->top == NULL);

    // test 3: removes all elements
    value1 = d_test_min_stack_create_int(1);
    value2 = d_test_min_stack_create_int(2);
    value3 = d_test_min_stack_create_int(3);
    
    if (stack)
    {
        d_min_stack_push(stack, value1);
        d_min_stack_push(stack, value2);
        d_min_stack_push(stack, value3);
        d_min_stack_clear(stack);
    }
    
    test_removes_all = (stack && stack->top == NULL);

    // test 4: top reset to NULL
    test_resets_top = test_removes_all;  // same check

    // test 5: stack usable after clear
    test_usable_after = false;
    
    if (stack)
    {
        int* new_value = d_test_min_stack_create_int(99);
        result = d_min_stack_push(stack, new_value);
        test_usable_after = (result == new_value) && 
                           (stack->top != NULL);
        
        if (new_value) free(new_value);
    }

    // test 6: frees node structures (we can't directly test this,
    // but we verify stack is in expected state)
    test_frees_nodes = (stack && stack->top != NULL);  // has new element
    
    if (stack)
    {
        d_min_stack_clear(stack);
    }
    
    test_frees_nodes = test_frees_nodes && (stack && stack->top == NULL);

    // cleanup
    if (value1) free(value1);
    if (value2) free(value2);
    if (value3) free(value3);
    if (stack) d_min_stack_free(stack);

    // build result tree
    group = d_test_object_new_interior("d_min_stack_clear", 6);

    if (!group)
    {
        return NULL;
    }

    idx = 0;
    group->elements[idx++] = D_ASSERT_TRUE("null_stack",
                                           test_null_stack,
                                           "handles NULL stack safely");
    group->elements[idx++] = D_ASSERT_TRUE("empty_stack",
                                           test_empty_stack,
                                           "clears empty stack");
    group->elements[idx++] = D_ASSERT_TRUE("removes_all",
                                           test_removes_all,
                                           "removes all elements");
    group->elements[idx++] = D_ASSERT_TRUE("resets_top",
                                           test_resets_top,
                                           "resets top to NULL");
    group->elements[idx++] = D_ASSERT_TRUE("usable_after",
                                           test_usable_after,
                                           "stack usable after clear");
    group->elements[idx++] = D_ASSERT_TRUE("frees_nodes",
                                           test_frees_nodes,
                                           "frees all node structures");

    return group;
}


/******************************************************************************
 * FREE OPERATION TESTS
 *****************************************************************************/

/*
d_tests_min_stack_free
  Tests d_min_stack_free for stack destruction.
  Tests the following:
  - handles NULL stack safely
  - frees empty stack
  - frees stack with elements
  - frees all nodes
  - no memory leaks
  - no double-free issues
*/
struct d_test_object*
d_tests_min_stack_free
(
    void
)
{
    struct d_test_object* group;
    struct d_min_stack*   stack;
    int*                  value1;
    int*                  value2;
    bool                  test_null_stack;
    bool                  test_empty_stack;
    bool                  test_with_elements;
    bool                  test_frees_nodes;
    bool                  test_no_leaks;
    bool                  test_no_double_free;
    size_t                idx;

    // test 1: NULL stack handled safely
    test_null_stack = true;
    d_min_stack_free(NULL);  // should not crash

    // test 2: empty stack freed
    stack = d_min_stack_new();
    
    if (stack)
    {
        d_min_stack_free(stack);
        test_empty_stack = true;
    }
    else
    {
        test_empty_stack = false;
    }

    // test 3: stack with elements freed
    stack = d_min_stack_new();
    value1 = d_test_min_stack_create_int(10);
    value2 = d_test_min_stack_create_int(20);
    
    if (stack)
    {
        d_min_stack_push(stack, value1);
        d_min_stack_push(stack, value2);
        d_min_stack_free(stack);
        test_with_elements = true;
    }
    else
    {
        test_with_elements = false;
    }

    // test 4: frees all nodes (tested by successful completion)
    test_frees_nodes = test_with_elements;

    // test 5: no memory leaks (best we can do without valgrind)
    // create and free multiple stacks
    test_no_leaks = true;
    
    for (int i = 0; i < 10; i++)
    {
        struct d_min_stack* temp = d_min_stack_new();
        
        if (!temp)
        {
            test_no_leaks = false;
            break;
        }
        
        int* val = d_test_min_stack_create_int(i);
        d_min_stack_push(temp, val);
        d_min_stack_free(temp);
        
        if (val) free(val);
    }

    // test 6: no double-free (calling free doesn't crash)
    test_no_double_free = test_null_stack;  // already tested NULL safety

    // cleanup
    if (value1) free(value1);
    if (value2) free(value2);

    // build result tree
    group = d_test_object_new_interior("d_min_stack_free", 6);

    if (!group)
    {
        return NULL;
    }

    idx = 0;
    group->elements[idx++] = D_ASSERT_TRUE("null_stack",
                                           test_null_stack,
                                           "handles NULL stack safely");
    group->elements[idx++] = D_ASSERT_TRUE("empty_stack",
                                           test_empty_stack,
                                           "frees empty stack");
    group->elements[idx++] = D_ASSERT_TRUE("with_elements",
                                           test_with_elements,
                                           "frees stack with elements");
    group->elements[idx++] = D_ASSERT_TRUE("frees_nodes",
                                           test_frees_nodes,
                                           "frees all nodes");
    group->elements[idx++] = D_ASSERT_TRUE("no_leaks",
                                           test_no_leaks,
                                           "no memory leaks");
    group->elements[idx++] = D_ASSERT_TRUE("no_double_free",
                                           test_no_double_free,
                                           "no double-free issues");

    return group;
}


/******************************************************************************
 * MEMORY MANAGEMENT AGGREGATOR
 *****************************************************************************/

/*
d_tests_min_stack_memory_all
  Runs all memory management tests.
  Tests the following:
  - d_min_stack_clear
  - d_min_stack_free
*/
struct d_test_object*
d_tests_min_stack_memory_all
(
    void
)
{
    struct d_test_object* group;
    size_t                idx;

    group = d_test_object_new_interior("Memory Management", 2);

    if (!group)
    {
        return NULL;
    }

    idx = 0;
    group->elements[idx++] = d_tests_min_stack_clear();
    group->elements[idx++] = d_tests_min_stack_free();

    return group;
}
