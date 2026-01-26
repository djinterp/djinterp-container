/******************************************************************************
* djinterp [test]                                       min_stack_tests_sa.c
*
*   Master test runner for min_stack module.
*   Coordinates execution of all test categories.
*
*
* path:      \test\container\stack\min_stack_tests_sa.c
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2026.01.24
******************************************************************************/

#include ".\min_stack_tests_sa.h"


/******************************************************************************
 * MASTER TEST RUNNER
 *****************************************************************************/

/*
d_tests_min_stack_run_all
  Master test runner for all min_stack tests.
  Tests the following:
  - Core stack operations (new, push, peek, pop)
  - Memory management (clear, free)
  - Advanced scenarios (integration, stress)
  
  This function aggregates all test categories and returns a complete
  test tree for execution by the test framework.
*/
struct d_test_object*
d_tests_min_stack_run_all
(
    void
)
{
    struct d_test_object* group;
    size_t                idx;

    // create master test group
    group = d_test_object_new_interior("min_stack Module Tests", 3);

    if (!group)
    {
        return NULL;
    }

    // add all test categories
    idx = 0;
    group->elements[idx++] = d_tests_min_stack_core_all();
    group->elements[idx++] = d_tests_min_stack_memory_all();
    group->elements[idx++] = d_tests_min_stack_advanced_all();

    return group;
}
