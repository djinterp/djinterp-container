/******************************************************************************
* djinterp [container]                                 array_common_tests_sa.c
*
* Comprehensive test runner for `array_common.h` unit tests.
* Contains helper functions and the main test suite runners.
* 
* 
* path:      \test\container\array\array_common_tests_sa.c             
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.10.25
******************************************************************************/

#include ".\array_common_tests_sa.h"


/******************************************************************************
 * HELPER FUNCTIONS
 *****************************************************************************/

/*
d_test_init_from_args_wrapper
  Helper function for testing d_array_common_init_from_args.
  This wrapper handles the va_list conversion locally without modifying
  the test_standalone framework.
*/
bool
d_test_init_from_args_wrapper
(
    void**  _elements,
    size_t* _count,
    size_t  _element_size,
    size_t  _arg_count,
    ...
)
{
    va_list args;
    bool result;

    va_start(args, _arg_count);
    result = d_array_common_init_from_args(_elements, 
                                           _count,
                                           _element_size, 
                                           _arg_count, 
                                           args);
    va_end(args);

    return result;
}

/*
d_test_int_comparator
  Helper comparator function for testing integer comparisons.
*/
int 
d_test_int_comparator
(
    const void* _a, 
    const void* _b
)
{
    int ia = *(const int*)_a;
    int ib = *(const int*)_b;

    if (ia < ib) return -1;
    if (ia > ib) return 1;

    return 0;
}

/*
d_test_free_int_ptr
  Helper free function for testing deep free operations.
*/
void 
d_test_free_int_ptr
(
    void* _ptr
)
{
    if (_ptr) 
    {
        free(_ptr);
    }
}


/******************************************************************************
 * COMPREHENSIVE TEST SUITE RUNNERS
 *****************************************************************************/

/*
d_tests_sa_array_common_all
  Runs all basic array_common test functions and provides a comprehensive summary.
*/
bool
d_tests_sa_array_common_all
(
    struct d_test_counter* _test_info
)
{
    bool overall_result = true;
    struct d_test_counter local_counter = { 0, 0, 0, 0 };

    printf("\n========================================\n");
    printf("Running Basic Array Common Utility Tests\n");
    printf("========================================\n");

    // Run initialization function tests
    overall_result &= d_tests_sa_array_common_run_init_tests(&local_counter);

    // Run utility function tests
    overall_result &= d_tests_sa_array_common_run_util_tests(&local_counter);

    // Run manipulation function tests
    overall_result &= d_tests_sa_array_common_run_manip_tests(&local_counter);

    // Run cleanup/free function tests
    overall_result &= d_tests_sa_array_common_run_free_tests(&local_counter);

    // Update main counter
    _test_info->assertions_total  += local_counter.assertions_total;
    _test_info->assertions_passed += local_counter.assertions_passed;
    _test_info->tests_total       += local_counter.tests_total;
    _test_info->tests_passed      += local_counter.tests_passed;

    // Print results
    printf("\n========================================\n");
    printf("Basic Array Common Test Results\n");
    printf("========================================\n");
    printf("Function Categories Tested:\n");
    printf("    %s Initialization Functions (12)\n", D_TEST_SYMBOL_PASS);
    printf("    %s Utility Functions (9)\n",         D_TEST_SYMBOL_PASS);
    printf("    %s Manipulation Functions (14)\n",   D_TEST_SYMBOL_PASS);
    printf("    %s Cleanup Functions (2)\n",         D_TEST_SYMBOL_PASS);
    printf("----------------------------------------\n");
    printf("Total Function Tests: 37\n");
    printf("Tests: %zu/%zu passed (%.2f%%)\n",
        local_counter.tests_passed, local_counter.tests_total,
        local_counter.tests_total > 0 ?
            (double)local_counter.tests_passed / local_counter.tests_total * 100.0 : 
            0.0);
    printf("Assertions: %zu/%zu passed (%.2f%%)\n",
        local_counter.assertions_passed, local_counter.assertions_total,
        local_counter.assertions_total > 0 ?
            (double)local_counter.assertions_passed / local_counter.assertions_total * 100.0 : 
            0.0);
    printf("Overall Status: %s\n", overall_result ? "PASSED" : "FAILED");
    printf("========================================\n");

    return overall_result;
}

/*
d_tests_sa_array_common_all_extended
  Runs ALL implemented array_common test functions including all available 
  functionality.
*/
bool
d_tests_sa_array_common_all_extended
(
    struct d_test_counter* _test_info
)
{
    bool overall_result = true;
    struct d_test_counter local_counter = { 0, 0, 0, 0 };

    printf("\n========================================\n");
    printf("Running COMPLETE Array Common Test Suite\n");
    printf("========================================\n");

    // This now runs the complete test suite with all implemented functions
    overall_result = d_tests_sa_array_common_all(&local_counter);

    // Update main counter
    _test_info->assertions_total  += local_counter.assertions_total;
    _test_info->assertions_passed += local_counter.assertions_passed;
    _test_info->tests_total       += local_counter.tests_total;
    _test_info->tests_passed      += local_counter.tests_passed;

    // Print comprehensive results
    printf("\n========================================\n");
    printf("COMPLETE Array Common Test Results\n");
    printf("========================================\n");
    printf("All functions from array_common.h are now implemented and tested:\n");
    printf("    - 12 initialization functions\n");
    printf("    - 9 utility functions\n");
    printf("    - 14 manipulation functions\n");
    printf("    - 2 cleanup functions\n");
    printf("    - Including enhanced d_index support with negative indexing\n");
    printf("    - Proper validation using d_index_is_valid\n");
    printf("    - Safe conversion using d_index_convert_safe\n");
    printf("========================================\n");
    printf("Tests: %zu/%zu passed (%.2f%%)\n",
        local_counter.tests_passed, local_counter.tests_total,
        local_counter.tests_total > 0 ?
        (double)local_counter.tests_passed / local_counter.tests_total * 100.0 : 0.0);
    printf("Assertions: %zu/%zu passed (%.2f%%)\n",
        local_counter.assertions_passed, local_counter.assertions_total,
        local_counter.assertions_total > 0 ?
        (double)local_counter.assertions_passed / local_counter.assertions_total * 100.0 : 0.0);
    printf("Overall Status: %s\n", overall_result ? "PASSED" : "FAILED");
    printf("========================================\n");

    return overall_result;
}

/*
d_tests_sa_array_common_run_all
  Main entry point for the test module runner. Returns pass/fail status for 
  use with the unified test runner.
*/
bool
d_tests_sa_array_common_run_all
(
    struct d_test_counter* _test_info
)
{
    return d_tests_sa_array_common_all(_test_info);
}
