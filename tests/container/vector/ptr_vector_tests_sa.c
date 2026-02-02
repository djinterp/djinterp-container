/******************************************************************************
* djinterp [test]                                          ptr_vector_tests_sa.c
*
*   Module-level aggregation for ptr_vector unit tests.
*
*
* path:      \tests\container\vector\ptr_vector_tests_sa.c
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.01.29
******************************************************************************/

#include ".\ptr_vector_tests_sa.h"


/*
d_tests_sa_ptr_vector_run_all
  Module-level aggregation function that runs all ptr_vector tests.
  Executes tests for all categories:
  - Constructor functions (new, new_default, from_array, from_args, copy, fill, merge)
  - Capacity management functions (reserve, shrink_to_fit, ensure_capacity, available)
  - Element manipulation functions (push_back, push_front, pop_back, pop_front, 
                                    insert, insert_range, erase, erase_range, 
                                    remove, clear)
  - Append/extend functions (append, append_vector, prepend, prepend_vector)
  - Resize functions (resize, resize_fill)
  - Access functions (at, front, back, data, get, set)
  - Query functions (is_empty, is_full, size, capacity)
  - Search functions (find, find_last, find_ptr, contains, contains_ptr, count_value)
  - Utility functions (swap, reverse, sort, copy_to)
  - Iteration helpers (foreach, foreach_with_context)
  - Destructor functions (free, free_deep, clear_deep)
*/
bool
d_tests_sa_ptr_vector_run_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    /* Run all test categories */
    result = d_tests_sa_ptr_vector_constructor_all(_counter) && result;
    result = d_tests_sa_ptr_vector_capacity_all(_counter) && result;
    result = d_tests_sa_ptr_vector_element_all(_counter) && result;
    result = d_tests_sa_ptr_vector_append_all(_counter) && result;
    result = d_tests_sa_ptr_vector_resize_all(_counter) && result;
    result = d_tests_sa_ptr_vector_access_all(_counter) && result;
    result = d_tests_sa_ptr_vector_query_all(_counter) && result;
    result = d_tests_sa_ptr_vector_search_all(_counter) && result;
    result = d_tests_sa_ptr_vector_utility_all(_counter) && result;
    result = d_tests_sa_ptr_vector_iteration_all(_counter) && result;
    result = d_tests_sa_ptr_vector_destructor_all(_counter) && result;

    return result;
}
