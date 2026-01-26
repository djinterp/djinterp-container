/******************************************************************************
* djinterp [test]                                    min_stack_tests_sa_core.c
*
*   Core operation tests for min_stack module.
*   Tests creation, push, peek, and pop operations.
*
*
* path:      \test\container\stack\min_stack_tests_sa_core.c
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2026.01.24
******************************************************************************/

#include ".\min_stack_tests_sa.h"


/******************************************************************************
 * HELPER FUNCTIONS
 *****************************************************************************/

/*
d_test_min_stack_create_int
  Helper function to create a heap-allocated integer for testing.

Parameter(s):
  _value: integer value to allocate
Return:
  Pointer to newly allocated integer, or NULL on failure.
*/
int*
d_test_min_stack_create_int
(
    int _value
)
{
    int* ptr = malloc(sizeof(int));
    
    if (ptr)
    {
        *ptr = _value;
    }
    
    return ptr;
}


/******************************************************************************
 * STACK CREATION TESTS
 *****************************************************************************/

/*
d_tests_min_stack_new
  Tests d_min_stack_new for stack creation.
  Tests the following:
  - allocates stack successfully
  - initializes top to NULL
  - stack is ready for use
  - multiple stacks can be created
*/
struct d_test_object*
d_tests_min_stack_new
(
    void
)
{
    struct d_test_object* group;
    struct d_min_stack*   stack1;
    struct d_min_stack*   stack2;
    bool                  test_allocation;
    bool                  test_initialization;
    bool                  test_ready_for_use;
    bool                  test_multiple_stacks;
    size_t                idx;

    // test 1: allocation succeeds
    stack1 = d_min_stack_new();
    test_allocation = (stack1 != NULL);

    // test 2: top initialized to NULL
    test_initialization = (stack1 != NULL) && (stack1->top == NULL);

    // test 3: ready for use (can push immediately)
    test_ready_for_use = false;
    
    if (stack1)
    {
        int* value = d_test_min_stack_create_int(42);
        void* result = d_min_stack_push(stack1, value);
        test_ready_for_use = (result == value);
        
        if (value) free(value);
    }

    // test 4: multiple stacks can be created
    stack2 = d_min_stack_new();
    test_multiple_stacks = (stack2 != NULL) && (stack2 != stack1);

    // cleanup
    if (stack1) d_min_stack_free(stack1);
    if (stack2) d_min_stack_free(stack2);

    // build result tree
    group = d_test_object_new_interior("d_min_stack_new", 4);

    if (!group)
    {
        return NULL;
    }

    idx = 0;
    group->elements[idx++] = D_ASSERT_TRUE("allocation",
                                           test_allocation,
                                           "allocates stack successfully");
    group->elements[idx++] = D_ASSERT_TRUE("initialization",
                                           test_initialization,
                                           "initializes top to NULL");
    group->elements[idx++] = D_ASSERT_TRUE("ready_for_use",
                                           test_ready_for_use,
                                           "stack is ready for use");
    group->elements[idx++] = D_ASSERT_TRUE("multiple_stacks",
                                           test_multiple_stacks,
                                           "multiple stacks can be created");

    return group;
}


/******************************************************************************
 * PUSH OPERATION TESTS
 *****************************************************************************/

/*
d_tests_min_stack_push
  Tests d_min_stack_push for adding elements.
  Tests the following:
  - returns NULL for NULL stack
  - pushes single element
  - returns value pointer on success
  - updates stack top
  - maintains LIFO order
  - handles multiple pushes
  - can push NULL values
*/
struct d_test_object*
d_tests_min_stack_push
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
    bool                  test_single_push;
    bool                  test_return_value;
    bool                  test_updates_top;
    bool                  test_lifo_order;
    bool                  test_multiple_pushes;
    bool                  test_null_value;
    size_t                idx;

    // test 1: NULL stack returns NULL
    value1 = d_test_min_stack_create_int(10);
    result = d_min_stack_push(NULL, value1);
    test_null_stack = (result == NULL);

    // test 2: single push succeeds
    stack = d_min_stack_new();
    result = stack ? d_min_stack_push(stack, value1) : NULL;
    test_single_push = (result != NULL);

    // test 3: returns correct value pointer
    test_return_value = (result == value1);

    // test 4: updates stack top
    test_updates_top = (stack && stack->top != NULL);

    // test 5: LIFO order maintained
    value2 = d_test_min_stack_create_int(20);
    value3 = d_test_min_stack_create_int(30);
    
    if (stack)
    {
        d_min_stack_push(stack, value2);
        d_min_stack_push(stack, value3);
    }
    
    test_lifo_order = (stack && stack->top && stack->top->value == value3);

    // test 6: multiple pushes work
    test_multiple_pushes = (stack && 
                           stack->top && 
                           stack->top->next && 
                           stack->top->next->value == value2);

    // test 7: can push NULL value
    if (stack)
    {
        result = d_min_stack_push(stack, NULL);
    }
    else
    {
        result = NULL;
    }
    
    test_null_value = (result == NULL);  // returns the NULL value

    // cleanup
    if (value1) free(value1);
    if (value2) free(value2);
    if (value3) free(value3);
    if (stack) d_min_stack_free(stack);

    // build result tree
    group = d_test_object_new_interior("d_min_stack_push", 7);

    if (!group)
    {
        return NULL;
    }

    idx = 0;
    group->elements[idx++] = D_ASSERT_TRUE("null_stack",
                                           test_null_stack,
                                           "returns NULL for NULL stack");
    group->elements[idx++] = D_ASSERT_TRUE("single_push",
                                           test_single_push,
                                           "pushes single element");
    group->elements[idx++] = D_ASSERT_TRUE("return_value",
                                           test_return_value,
                                           "returns value pointer on success");
    group->elements[idx++] = D_ASSERT_TRUE("updates_top",
                                           test_updates_top,
                                           "updates stack top");
    group->elements[idx++] = D_ASSERT_TRUE("lifo_order",
                                           test_lifo_order,
                                           "maintains LIFO order");
    group->elements[idx++] = D_ASSERT_TRUE("multiple_pushes",
                                           test_multiple_pushes,
                                           "handles multiple pushes");
    group->elements[idx++] = D_ASSERT_TRUE("null_value",
                                           test_null_value,
                                           "can push NULL values");

    return group;
}


/******************************************************************************
 * PEEK OPERATION TESTS
 *****************************************************************************/

/*
d_tests_min_stack_peek
  Tests d_min_stack_peek for viewing top element.
  Tests the following:
  - returns NULL for NULL stack
  - returns NULL for empty stack
  - returns top value without removal
  - does not modify stack
  - returns correct value after pushes
  - works with NULL values
*/
struct d_test_object*
d_tests_min_stack_peek
(
    void
)
{
    struct d_test_object* group;
    struct d_min_stack*   stack;
    int*                  value1;
    int*                  value2;
    void*                 result;
    void*                 top_before;
    void*                 top_after;
    bool                  test_null_stack;
    bool                  test_empty_stack;
    bool                  test_no_removal;
    bool                  test_no_modification;
    bool                  test_correct_value;
    bool                  test_null_value;
    size_t                idx;

    // test 1: NULL stack returns NULL
    result = d_min_stack_peek(NULL);
    test_null_stack = (result == NULL);

    // test 2: empty stack returns NULL
    stack = d_min_stack_new();
    result = stack ? d_min_stack_peek(stack) : NULL;
    test_empty_stack = (result == NULL);

    // test 3: returns value without removal
    value1 = d_test_min_stack_create_int(42);
    
    if (stack)
    {
        d_min_stack_push(stack, value1);
        result = d_min_stack_peek(stack);
    }
    else
    {
        result = NULL;
    }
    
    test_no_removal = (result == value1);

    // test 4: does not modify stack
    top_before = stack ? stack->top : NULL;
    result = stack ? d_min_stack_peek(stack) : NULL;
    top_after = stack ? stack->top : NULL;
    test_no_modification = (top_before == top_after);

    // test 5: returns correct value after multiple pushes
    value2 = d_test_min_stack_create_int(99);
    
    if (stack)
    {
        d_min_stack_push(stack, value2);
        result = d_min_stack_peek(stack);
    }
    else
    {
        result = NULL;
    }
    
    test_correct_value = (result == value2);

    // test 6: works with NULL value on top
    if (stack)
    {
        d_min_stack_push(stack, NULL);
        result = d_min_stack_peek(stack);
    }
    else
    {
        result = (void*)1;  // non-NULL for test failure
    }
    
    test_null_value = (result == NULL);

    // cleanup
    if (value1) free(value1);
    if (value2) free(value2);
    if (stack) d_min_stack_free(stack);

    // build result tree
    group = d_test_object_new_interior("d_min_stack_peek", 6);

    if (!group)
    {
        return NULL;
    }

    idx = 0;
    group->elements[idx++] = D_ASSERT_TRUE("null_stack",
                                           test_null_stack,
                                           "returns NULL for NULL stack");
    group->elements[idx++] = D_ASSERT_TRUE("empty_stack",
                                           test_empty_stack,
                                           "returns NULL for empty stack");
    group->elements[idx++] = D_ASSERT_TRUE("no_removal",
                                           test_no_removal,
                                           "returns top value without removal");
    group->elements[idx++] = D_ASSERT_TRUE("no_modification",
                                           test_no_modification,
                                           "does not modify stack");
    group->elements[idx++] = D_ASSERT_TRUE("correct_value",
                                           test_correct_value,
                                           "returns correct value after pushes");
    group->elements[idx++] = D_ASSERT_TRUE("null_value",
                                           test_null_value,
                                           "works with NULL values");

    return group;
}


/******************************************************************************
 * POP OPERATION TESTS
 *****************************************************************************/

/*
d_tests_min_stack_pop
  Tests d_min_stack_pop for removing elements.
  Tests the following:
  - returns NULL for NULL stack
  - returns NULL for empty stack
  - returns and removes top element
  - updates stack top correctly
  - maintains LIFO order
  - empties stack after popping all
  - handles NULL values
*/
struct d_test_object*
d_tests_min_stack_pop
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
    void*                 pop1;
    void*                 pop2;
    void*                 pop3;
    bool                  test_null_stack;
    bool                  test_empty_stack;
    bool                  test_returns_removes;
    bool                  test_updates_top;
    bool                  test_lifo_order;
    bool                  test_empties_stack;
    bool                  test_null_value;
    size_t                idx;

    // test 1: NULL stack returns NULL
    result = d_min_stack_pop(NULL);
    test_null_stack = (result == NULL);

    // test 2: empty stack returns NULL
    stack = d_min_stack_new();
    result = stack ? d_min_stack_pop(stack) : NULL;
    test_empty_stack = (result == NULL);

    // test 3: returns and removes top element
    value1 = d_test_min_stack_create_int(42);
    
    if (stack)
    {
        d_min_stack_push(stack, value1);
        result = d_min_stack_pop(stack);
    }
    else
    {
        result = NULL;
    }
    
    test_returns_removes = (result == value1) && 
                          (stack && stack->top == NULL);

    // test 4: updates top correctly
    value2 = d_test_min_stack_create_int(10);
    value3 = d_test_min_stack_create_int(20);
    
    if (stack)
    {
        d_min_stack_push(stack, value2);
        d_min_stack_push(stack, value3);
        d_min_stack_pop(stack);
    }
    
    test_updates_top = (stack && stack->top && stack->top->value == value2);

    // test 5: LIFO order maintained
    if (stack)
    {
        d_min_stack_clear(stack);
        d_min_stack_push(stack, value1);
        d_min_stack_push(stack, value2);
        d_min_stack_push(stack, value3);
        pop1 = d_min_stack_pop(stack);
        pop2 = d_min_stack_pop(stack);
        pop3 = d_min_stack_pop(stack);
    }
    else
    {
        pop1 = pop2 = pop3 = NULL;
    }
    
    test_lifo_order = (pop1 == value3) && 
                     (pop2 == value2) && 
                     (pop3 == value1);

    // test 6: stack empty after popping all
    test_empties_stack = (stack && stack->top == NULL);

    // test 7: handles NULL value
    if (stack)
    {
        d_min_stack_push(stack, NULL);
        result = d_min_stack_pop(stack);
    }
    else
    {
        result = (void*)1;  // non-NULL for test failure
    }
    
    test_null_value = (result == NULL) && (stack && stack->top == NULL);

    // cleanup
    if (value1) free(value1);
    if (value2) free(value2);
    if (value3) free(value3);
    if (stack) d_min_stack_free(stack);

    // build result tree
    group = d_test_object_new_interior("d_min_stack_pop", 7);

    if (!group)
    {
        return NULL;
    }

    idx = 0;
    group->elements[idx++] = D_ASSERT_TRUE("null_stack",
                                           test_null_stack,
                                           "returns NULL for NULL stack");
    group->elements[idx++] = D_ASSERT_TRUE("empty_stack",
                                           test_empty_stack,
                                           "returns NULL for empty stack");
    group->elements[idx++] = D_ASSERT_TRUE("returns_removes",
                                           test_returns_removes,
                                           "returns and removes top element");
    group->elements[idx++] = D_ASSERT_TRUE("updates_top",
                                           test_updates_top,
                                           "updates stack top correctly");
    group->elements[idx++] = D_ASSERT_TRUE("lifo_order",
                                           test_lifo_order,
                                           "maintains LIFO order");
    group->elements[idx++] = D_ASSERT_TRUE("empties_stack",
                                           test_empties_stack,
                                           "empties stack after popping all");
    group->elements[idx++] = D_ASSERT_TRUE("null_value",
                                           test_null_value,
                                           "handles NULL values");

    return group;
}


/******************************************************************************
 * CORE OPERATIONS AGGREGATOR
 *****************************************************************************/

/*
d_tests_min_stack_core_all
  Runs all core operation tests.
  Tests the following:
  - d_min_stack_new
  - d_min_stack_push
  - d_min_stack_peek
  - d_min_stack_pop
*/
struct d_test_object*
d_tests_min_stack_core_all
(
    void
)
{
    struct d_test_object* group;
    size_t                idx;

    group = d_test_object_new_interior("Core Stack Operations", 4);

    if (!group)
    {
        return NULL;
    }

    idx = 0;
    group->elements[idx++] = d_tests_min_stack_new();
    group->elements[idx++] = d_tests_min_stack_push();
    group->elements[idx++] = d_tests_min_stack_peek();
    group->elements[idx++] = d_tests_min_stack_pop();

    return group;
}
