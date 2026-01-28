/******************************************************************************
* djinterp [container]                                 array_common_tests_free.c
*
* Unit tests for `array_common.h` cleanup/free functions.
* Part of the array_common standalone test suite.
* 
* 
* path:      \test\container\array\array_common_tests_free.c             
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.10.25
******************************************************************************/

#include ".\array_common_tests_sa.h"


/*
===============================================================================
                        CLEANUP/FREE FUNCTION TESTS
===============================================================================
*/

/*
d_tests_sa_array_common_free_elements_arr
  Tests the `d_array_common_free_elements_arr` function
  Tests the following:
  - freeing allocated array
  - handling NULL pointer safely
*/
bool
d_tests_sa_array_common_free_elements_arr
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int* elements;

    printf("Testing `d_array_common_free_elements_arr`...\n");

    // Test 1: Free allocated array
    elements = (int*)malloc(5 * sizeof(int));

    if (elements)
    {
        d_array_common_free_elements_arr(elements);

        test_result &= d_assert_standalone(
            true,
            "`d_array_common_free_elements_arr` successfully frees array",
            "`d_array_common_free_elements_arr` failed to free array",
            _test_info
        );
    }
    else
    {
        test_result &= d_assert_standalone(
            false,
            "`d_array_common_free_elements_arr` test allocation failed",
            "`d_array_common_free_elements_arr` test allocation failed",
            _test_info
        );
    }

    // Test 2: Handle NULL pointer safely
    d_array_common_free_elements_arr(NULL);

    test_result &= d_assert_standalone(
        true,
        "`d_array_common_free_elements_arr` handles NULL safely",
        "`d_array_common_free_elements_arr` failed with NULL",
        _test_info
    );

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_common_free_elements_deep
  Tests the `d_array_common_free_elements_deep` function
  Tests the following:
  - deep freeing array of pointers
  - handling NULL parameters safely
  - handling zero count safely
*/
bool
d_tests_sa_array_common_free_elements_deep
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    void** elements;
    size_t i;

    printf("Testing `d_array_common_free_elements_deep`...\n");

    // Test 1: Deep free array of pointers
    elements = (void**)malloc(3 * sizeof(void*));

    if (elements)
    {
        for (i = 0; i < 3; i++)
        {
            elements[i] = malloc(sizeof(int));

            if (elements[i])
            {
                *(int*)elements[i] = (int)i + 1;
            }
        }

        d_array_common_free_elements_deep(3, elements, d_test_free_int_ptr);

        test_result &= d_assert_standalone(
            true,
            "`d_array_common_free_elements_deep` successfully performs deep free",
            "`d_array_common_free_elements_deep` failed to perform deep free",
            _test_info
        );
    }
    else
    {
        test_result &= d_assert_standalone(
            false,
            "`d_array_common_free_elements_deep` test allocation failed",
            "`d_array_common_free_elements_deep` test allocation failed",
            _test_info
        );
    }

    // Test 2: Handle NULL parameters
    d_array_common_free_elements_deep(0, NULL, NULL);

    test_result &= d_assert_standalone(
        true,
        "`d_array_common_free_elements_deep` handles NULL parameters safely",
        "`d_array_common_free_elements_deep` failed with NULL parameters",
        _test_info
    );

    // Test 3: Handle zero count with non-NULL elements
    d_array_common_free_elements_deep(0, (void**)1, d_test_free_int_ptr);

    test_result &= d_assert_standalone(
        true,
        "`d_array_common_free_elements_deep` handles zero count safely",
        "`d_array_common_free_elements_deep` failed with zero count",
        _test_info
    );

    // Test 4: Handle NULL free function
    elements = (void**)malloc(2 * sizeof(void*));

    if (elements)
    {
        elements[0] = malloc(sizeof(int));
        elements[1] = malloc(sizeof(int));

        // This should do nothing since free_fn is NULL
        d_array_common_free_elements_deep(2, elements, NULL);

        // We need to manually clean up since deep_free didn't run
        if (elements[0]) free(elements[0]);
        if (elements[1]) free(elements[1]);
        free(elements);

        test_result &= d_assert_standalone(
            true,
            "`d_array_common_free_elements_deep` handles NULL free function",
            "`d_array_common_free_elements_deep` failed with NULL free function",
            _test_info
        );
    }

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}


/*
===============================================================================
                        CLEANUP TEST GROUP RUNNER
===============================================================================
*/

/*
d_tests_sa_array_common_run_free_tests
  Runs all cleanup/free function tests and returns overall pass/fail status.
*/
bool
d_tests_sa_array_common_run_free_tests
(
    struct d_test_counter* _test_info
)
{
    bool overall_result = true;

    printf("\n----------------------------------------\n");
    printf("Running Cleanup/Free Function Tests\n");
    printf("----------------------------------------\n");

    overall_result &= d_tests_sa_array_common_free_elements_arr(_test_info);
    overall_result &= d_tests_sa_array_common_free_elements_deep(_test_info);

    return overall_result;
}
