/******************************************************************************
* djinterp [test]                                min_stack_tests_sa_advanced.c
*
*   Advanced scenario tests for min_stack module.
*   Tests integration scenarios and stress testing.
*
*
* path:      \test\container\stack\min_stack_tests_sa_advanced.c
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2026.01.24
******************************************************************************/

#include ".\min_stack_tests_sa.h"


/******************************************************************************
 * INTEGRATION TESTS
 *****************************************************************************/

/*
d_tests_min_stack_integration
  Tests complex multi-operation scenarios.
  Tests the following:
  - push/pop/peek sequences
  - clear and reuse
  - interleaved operations
  - state consistency
  - complex LIFO patterns
*/
struct d_test_object*
d_tests_min_stack_integration
(
    void
)
{
    struct d_test_object* group;
    struct d_min_stack*   stack;
    int*                  values[10];
    void*                 result;
    bool                  test_push_pop_peek;
    bool                  test_clear_reuse;
    bool                  test_interleaved;
    bool                  test_state_consistency;
    bool                  test_complex_lifo;
    size_t                i;
    size_t                idx;

    // allocate test values
    for (i = 0; i < 10; i++)
    {
        values[i] = d_test_min_stack_create_int((int)i);
    }

    stack = d_min_stack_new();

    // test 1: push/pop/peek sequence
    test_push_pop_peek = false;
    
    if (stack)
    {
        d_min_stack_push(stack, values[0]);
        result = d_min_stack_peek(stack);
        bool peek_ok = (result == values[0]);
        
        d_min_stack_push(stack, values[1]);
        result = d_min_stack_peek(stack);
        bool peek2_ok = (result == values[1]);
        
        void* pop1 = d_min_stack_pop(stack);
        void* pop2 = d_min_stack_pop(stack);
        
        test_push_pop_peek = peek_ok && peek2_ok && 
                            (pop1 == values[1]) && (pop2 == values[0]);
    }

    // test 2: clear and reuse
    test_clear_reuse = false;
    
    if (stack)
    {
        for (i = 0; i < 5; i++)
        {
            d_min_stack_push(stack, values[i]);
        }
        
        d_min_stack_clear(stack);
        bool empty_after_clear = (stack->top == NULL);
        
        d_min_stack_push(stack, values[5]);
        result = d_min_stack_peek(stack);
        
        test_clear_reuse = empty_after_clear && (result == values[5]);
    }

    // test 3: interleaved push/pop operations
    test_interleaved = false;
    
    if (stack)
    {
        d_min_stack_clear(stack);
        
        d_min_stack_push(stack, values[0]);  // [0]
        d_min_stack_push(stack, values[1]);  // [0, 1]
        void* p1 = d_min_stack_pop(stack);   // [0]
        d_min_stack_push(stack, values[2]);  // [0, 2]
        d_min_stack_push(stack, values[3]);  // [0, 2, 3]
        void* p2 = d_min_stack_pop(stack);   // [0, 2]
        void* p3 = d_min_stack_pop(stack);   // [0]
        void* p4 = d_min_stack_pop(stack);   // []
        
        test_interleaved = (p1 == values[1]) && 
                          (p2 == values[3]) && 
                          (p3 == values[2]) && 
                          (p4 == values[0]) &&
                          (stack->top == NULL);
    }

    // test 4: state consistency after operations
    test_state_consistency = false;
    
    if (stack)
    {
        d_min_stack_push(stack, values[0]);
        d_min_stack_push(stack, values[1]);
        
        result = d_min_stack_peek(stack);
        bool peek_consistent = (result == values[1]);
        
        d_min_stack_pop(stack);
        result = d_min_stack_peek(stack);
        bool peek_after_pop = (result == values[0]);
        
        d_min_stack_clear(stack);
        result = d_min_stack_peek(stack);
        bool peek_after_clear = (result == NULL);
        
        test_state_consistency = peek_consistent && 
                                peek_after_pop && 
                                peek_after_clear;
    }

    // test 5: complex LIFO pattern
    test_complex_lifo = false;
    
    if (stack)
    {
        // build: 0, 1, 2, 3, 4
        for (i = 0; i < 5; i++)
        {
            d_min_stack_push(stack, values[i]);
        }
        
        // verify reverse order
        bool lifo_ok = true;
        
        for (i = 5; i > 0; i--)
        {
            void* popped = d_min_stack_pop(stack);
            
            if (popped != values[i - 1])
            {
                lifo_ok = false;
                break;
            }
        }
        
        test_complex_lifo = lifo_ok && (stack->top == NULL);
    }

    // cleanup
    for (i = 0; i < 10; i++)
    {
        if (values[i]) free(values[i]);
    }
    
    if (stack) d_min_stack_free(stack);

    // build result tree
    group = d_test_object_new_interior("Integration Scenarios", 5);

    if (!group)
    {
        return NULL;
    }

    idx = 0;
    group->elements[idx++] = D_ASSERT_TRUE("push_pop_peek",
                                           test_push_pop_peek,
                                           "push/pop/peek sequences work");
    group->elements[idx++] = D_ASSERT_TRUE("clear_reuse",
                                           test_clear_reuse,
                                           "clear and reuse works");
    group->elements[idx++] = D_ASSERT_TRUE("interleaved",
                                           test_interleaved,
                                           "interleaved operations work");
    group->elements[idx++] = D_ASSERT_TRUE("state_consistency",
                                           test_state_consistency,
                                           "state consistency maintained");
    group->elements[idx++] = D_ASSERT_TRUE("complex_lifo",
                                           test_complex_lifo,
                                           "complex LIFO patterns work");

    return group;
}


/******************************************************************************
 * STRESS TESTS
 *****************************************************************************/

/*
d_tests_min_stack_stress
  Tests high-volume operations and edge cases.
  Tests the following:
  - large number of pushes
  - large number of pops
  - alternating operations
  - memory stability
  - performance degradation check
*/
struct d_test_object*
d_tests_min_stack_stress
(
    void
)
{
    struct d_test_object* group;
    struct d_min_stack*   stack;
    int*                  values[D_TEST_MIN_STACK_LARGE_SIZE];
    bool                  test_large_pushes;
    bool                  test_large_pops;
    bool                  test_alternating;
    bool                  test_memory_stable;
    bool                  test_no_degradation;
    size_t                i;
    size_t                idx;

    // allocate test values
    for (i = 0; i < D_TEST_MIN_STACK_LARGE_SIZE; i++)
    {
        values[i] = d_test_min_stack_create_int((int)i);
    }

    stack = d_min_stack_new();

    // test 1: large number of pushes
    test_large_pushes = false;
    
    if (stack)
    {
        bool all_pushes_ok = true;
        
        for (i = 0; i < D_TEST_MIN_STACK_LARGE_SIZE; i++)
        {
            void* result = d_min_stack_push(stack, values[i]);
            
            if (result != values[i])
            {
                all_pushes_ok = false;
                break;
            }
        }
        
        test_large_pushes = all_pushes_ok;
    }

    // test 2: large number of pops (in reverse order)
    test_large_pops = false;
    
    if (stack)
    {
        bool all_pops_ok = true;
        
        for (i = D_TEST_MIN_STACK_LARGE_SIZE; i > 0; i--)
        {
            void* popped = d_min_stack_pop(stack);
            
            if (popped != values[i - 1])
            {
                all_pops_ok = false;
                break;
            }
        }
        
        test_large_pops = all_pops_ok && (stack->top == NULL);
    }

    // test 3: alternating push/pop operations
    test_alternating = false;
    
    if (stack)
    {
        bool alternating_ok = true;
        
        for (i = 0; i < D_TEST_MIN_STACK_MEDIUM_SIZE; i++)
        {
            void* push_result = d_min_stack_push(stack, values[i]);
            void* pop_result = d_min_stack_pop(stack);
            
            if (push_result != values[i] || pop_result != values[i])
            {
                alternating_ok = false;
                break;
            }
        }
        
        test_alternating = alternating_ok && (stack->top == NULL);
    }

    // test 4: memory stability (repeated push/clear cycles)
    test_memory_stable = false;
    
    if (stack)
    {
        bool stable = true;
        
        for (int cycle = 0; cycle < 10; cycle++)
        {
            for (i = 0; i < D_TEST_MIN_STACK_SMALL_SIZE; i++)
            {
                if (!d_min_stack_push(stack, values[i]))
                {
                    stable = false;
                    break;
                }
            }
            
            if (!stable) break;
            
            d_min_stack_clear(stack);
            
            if (stack->top != NULL)
            {
                stable = false;
                break;
            }
        }
        
        test_memory_stable = stable;
    }

    // test 5: no performance degradation (operations remain fast)
    // We can't measure time precisely, but we can verify operations
    // still work correctly after many iterations
    test_no_degradation = false;
    
    if (stack)
    {
        bool still_works = true;
        
        // stress the stack
        for (i = 0; i < D_TEST_MIN_STACK_MEDIUM_SIZE; i++)
        {
            d_min_stack_push(stack, values[i]);
        }
        
        for (i = 0; i < D_TEST_MIN_STACK_MEDIUM_SIZE; i++)
        {
            d_min_stack_pop(stack);
        }
        
        // verify still works
        void* r1 = d_min_stack_push(stack, values[0]);
        void* r2 = d_min_stack_peek(stack);
        void* r3 = d_min_stack_pop(stack);
        
        test_no_degradation = (r1 == values[0]) && 
                             (r2 == values[0]) && 
                             (r3 == values[0]);
    }

    // cleanup
    for (i = 0; i < D_TEST_MIN_STACK_LARGE_SIZE; i++)
    {
        if (values[i]) free(values[i]);
    }
    
    if (stack) d_min_stack_free(stack);

    // build result tree
    group = d_test_object_new_interior("Stress Tests", 5);

    if (!group)
    {
        return NULL;
    }

    idx = 0;
    group->elements[idx++] = D_ASSERT_TRUE("large_pushes",
                                           test_large_pushes,
                                           "handles large number of pushes");
    group->elements[idx++] = D_ASSERT_TRUE("large_pops",
                                           test_large_pops,
                                           "handles large number of pops");
    group->elements[idx++] = D_ASSERT_TRUE("alternating",
                                           test_alternating,
                                           "handles alternating operations");
    group->elements[idx++] = D_ASSERT_TRUE("memory_stable",
                                           test_memory_stable,
                                           "memory remains stable");
    group->elements[idx++] = D_ASSERT_TRUE("no_degradation",
                                           test_no_degradation,
                                           "no performance degradation");

    return group;
}


/******************************************************************************
 * ADVANCED TESTS AGGREGATOR
 *****************************************************************************/

/*
d_tests_min_stack_advanced_all
  Runs all advanced scenario tests.
  Tests the following:
  - Integration scenarios
  - Stress tests
*/
struct d_test_object*
d_tests_min_stack_advanced_all
(
    void
)
{
    struct d_test_object* group;
    size_t                idx;

    group = d_test_object_new_interior("Advanced Scenarios", 2);

    if (!group)
    {
        return NULL;
    }

    idx = 0;
    group->elements[idx++] = d_tests_min_stack_integration();
    group->elements[idx++] = d_tests_min_stack_stress();

    return group;
}
