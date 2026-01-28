/******************************************************************************
* djinterp [container]                               array_common_tests_util.c
*
* Unit tests for `array_common.h` utility functions.
* Part of the array_common standalone test suite.
* 
* 
* path:      \test\container\array\array_common_tests_util.c             
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.10.25
******************************************************************************/

#include ".\array_common_tests_sa.h"


//=============================================================================
//                      UTILITY FUNCTION TESTS
//=============================================================================

/*
d_tests_sa_array_common_alloc
  Tests the `d_array_common_alloc` function
  Tests the following:
  - basic allocation with valid size
  - zero size handling
*/
bool
d_tests_sa_array_common_alloc
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    void* result;
    printf("Testing `d_array_common_alloc`...\n");
    // Test 1: Allocate with valid size
    result = d_array_common_alloc(sizeof(int));
    test_result &= d_assert_standalone(
        result != NULL,
        "`d_array_common_alloc` correctly allocates memory for valid size",
        "`d_array_common_alloc` failed with valid size",
        _test_info
    );

    if (result)
    {
        free(result);
        result = NULL;
    }

    // Test 2: Allocate with zero size
    result = d_array_common_alloc(0);
    test_result &= d_assert_standalone(
        result == NULL,
        "`d_array_common_alloc` correctly handles zero size",
        "`d_array_common_alloc` failed with zero size",
        _test_info
    );

    _test_info->tests_total++;

    if (test_result)
    {
        _test_info->tests_passed++;
    }

    return test_result;
}
/*
d_tests_sa_array_common_append_element
  Tests the `d_array_common_append_element` function
  Tests the following:
  - appending a single element to array
  - NULL parameter handling
*/
bool
d_tests_sa_array_common_append_element
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int elements[10] = { 1, 2, 3 };  // space for more
    int value = 42;
    bool result;

    printf("Testing `d_array_common_append_element`...\n");

    // Test 1: Append element to array
    result = d_array_common_append_element(elements,
                                           3,
                                           sizeof(int),
                                           &value);
    test_result &= d_assert_standalone(
        ( (result == true)    &&
          (elements[3] == 42) ),
        "`d_array_common_append_element` correctly appends element",
        "`d_array_common_append_element` failed to append element",
        _test_info
    );

    // Test 2: Test with NULL elements
    result = d_array_common_append_element(NULL,
                                           3,
                                           sizeof(int),
                                           &value);
    test_result &= d_assert_standalone(
        result == false,
        "`d_array_common_append_element` correctly handles NULL elements",
        "`d_array_common_append_element` failed with NULL elements",
        _test_info
    );

    // Test 3: Test with NULL value
    result = d_array_common_append_element(elements,
                                           3,
                                           sizeof(int),
                                           NULL);
    test_result &= d_assert_standalone(
        result == false,
        "`d_array_common_append_element` correctly handles NULL value",
        "`d_array_common_append_element` failed with NULL value",
        _test_info
    );

    _test_info->tests_total++;

    if (test_result)
    {
        _test_info->tests_passed++;
    }

    return test_result;
}
/*
d_tests_sa_array_common_append_elements
  Tests the `d_array_common_append_elements` function
  Tests the following:
  - appending multiple elements to array
  - appending zero elements
  - NULL parameter handling
*/
bool
d_tests_sa_array_common_append_elements
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int elements[10] = { 1, 2, 3 };  // space for more
    int values[]     = { 10, 20, 30 };
    bool result;

    printf("Testing `d_array_common_append_elements`...\n");

    // Test 1: Append multiple elements
    result = d_array_common_append_elements(elements,
                                            3,
                                            sizeof(int),
                                            values,
                                            3);
    test_result &= d_assert_standalone(
        ( (result == true)    &&
          (elements[3] == 10) &&
          (elements[4] == 20) &&
          (elements[5] == 30) ),
        "`d_array_common_append_elements` correctly appends multiple elements",
        "`d_array_common_append_elements` failed to append multiple elements",
        _test_info
    );

    // Test 2: Append zero elements
    result = d_array_common_append_elements(elements,
                                            3,
                                            sizeof(int),
                                            values,
                                            0);
    test_result &= d_assert_standalone(
        result == true,  // returns true for zero count
        "`d_array_common_append_elements` correctly handles zero count",
        "`d_array_common_append_elements` failed with zero count",
        _test_info
    );
    // Test 3: Test with NULL parameters
    result = d_array_common_append_elements(NULL,
                                            3,
                                            sizeof(int),
                                            values,
                                            3);
    test_result &= d_assert_standalone(
        result == false,
        "`d_array_common_append_elements` correctly handles NULL elements",
        "`d_array_common_append_elements` failed with NULL elements",
        _test_info
    );
    _test_info->tests_total++;
    if (test_result)
    {
        _test_info->tests_passed++;
    }

    return test_result;
}
/*
d_tests_sa_array_common_calc_capacity
  Tests the `d_array_common_calc_capacity` function
  Tests the following:
  - calculating capacity for various sizes
  - power-of-2 growth behavior
  - zero size handling
*/
bool
d_tests_sa_array_common_calc_capacity
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    size_t result;

    printf("Testing `d_array_common_calc_capacity`...\n");

    // Test 1: Calculate capacity for zero
    result = d_array_common_calc_capacity(0);
    test_result &= d_assert_standalone(
        result == 1,
        "`d_array_common_calc_capacity` returns 1 for zero request",
        "`d_array_common_calc_capacity` failed with zero request",
        _test_info
    );

    // Test 2: Calculate capacity for small size
    result = d_array_common_calc_capacity(5);
    test_result &= d_assert_standalone(
        ( (result >= 5)                  &&
          ((result & (result - 1)) == 0) ),  // is power of 2
        "`d_array_common_calc_capacity` returns power of 2 >= requested",
        "`d_array_common_calc_capacity` failed with small size",
        _test_info
    );

    // Test 3: Calculate capacity for exact power of 2
    result = d_array_common_calc_capacity(16);
    test_result &= d_assert_standalone(
        result == 16,
        "`d_array_common_calc_capacity` handles exact power of 2",
        "`d_array_common_calc_capacity` failed with power of 2",
        _test_info
    );

    // Test 4: Calculate capacity for larger size
    result = d_array_common_calc_capacity(100);
    test_result &= d_assert_standalone(
        ( (result >= 100)                &&
          ((result & (result - 1)) == 0) ),
        "`d_array_common_calc_capacity` handles larger sizes correctly",
        "`d_array_common_calc_capacity` failed with larger size",
        _test_info
    );

    _test_info->tests_total++;

    if (test_result)
    {
        _test_info->tests_passed++;
    }

    return test_result;
}
/*
d_tests_sa_array_common_contains
  Tests the `d_array_common_contains` function
  Tests the following:
  - finding existing element
  - not finding non-existent element
  - NULL parameter handling
*/
bool
d_tests_sa_array_common_contains
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int elements[]   = { 1, 2, 3, 4, 5 };
    int search_val   = 3;
    int not_found    = 99;
    bool result;

    printf("Testing `d_array_common_contains`...\n");

    // Test 1: Find existing element
    result = d_array_common_contains(elements,
                                     5,
                                     sizeof(int),
                                     &search_val,
                                     d_test_int_comparator);
    test_result &= d_assert_standalone(
        result == true,
        "`d_array_common_contains` finds existing element",
        "`d_array_common_contains` failed to find existing element",
        _test_info
    );
    // Test 2: Not find non-existent element
    result = d_array_common_contains(elements,
                                     5,
                                     sizeof(int),
                                     &not_found,
                                     d_test_int_comparator);
    test_result &= d_assert_standalone(
        result == false,
        "`d_array_common_contains` correctly returns false for missing element",
        "`d_array_common_contains` incorrectly found non-existent element",
        _test_info
    );
    // Test 3: Test with NULL comparator
    result = d_array_common_contains(elements,
                                     5,
                                     sizeof(int),
                                     &search_val,
                                     NULL);
    test_result &= d_assert_standalone(
        result == false,
        "`d_array_common_contains` correctly handles NULL comparator",
        "`d_array_common_contains` failed with NULL comparator",
        _test_info
    );
    _test_info->tests_total++;
    if (test_result)
    {
        _test_info->tests_passed++;
    }

    return test_result;
}
/*
d_tests_sa_array_common_fill
  Tests the `d_array_common_fill` function
  Tests the following:
  - filling array with value
  - return value validation
  - NULL parameter handling
*/
bool
d_tests_sa_array_common_fill
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int elements[5] = { 0 };
    int fill_value = 7;
    int result;
    printf("Testing `d_array_common_fill`...\n");
    // Test 1: Fill array with value
    result = d_array_common_fill(elements,
                                 5,
                                 sizeof(int),
                                 &fill_value);
    test_result &= d_assert_standalone(
        ( (result == 5)      &&
          (elements[0] == 7) &&
          (elements[4] == 7) ),
        "`d_array_common_fill` correctly fills array",
        "`d_array_common_fill` failed to fill array",
        _test_info
    );
    // Test 2: Fill with zero count
    result = d_array_common_fill(elements,
                                 0,
                                 sizeof(int),
                                 &fill_value);
    test_result &= d_assert_standalone(
        result == 0,
        "`d_array_common_fill` returns 0 for zero count",
        "`d_array_common_fill` failed with zero count",
        _test_info
    );
    // Test 3: Fill with NULL parameters
    result = d_array_common_fill(NULL,
                                 5,
                                 sizeof(int),
                                 &fill_value);
    test_result &= d_assert_standalone(
        result == -1,
        "`d_array_common_fill` returns -1 for NULL elements",
        "`d_array_common_fill` failed with NULL elements",
        _test_info
    );
    _test_info->tests_total++;
    if (test_result)
    {
        _test_info->tests_passed++;
    }

    return test_result;
}
/*
d_tests_sa_array_common_find
  Tests the `d_array_common_find` function
  Tests the following:
  - finding existing element (returns index)
  - not finding non-existent element (returns -1)
  - finding first occurrence
  - NULL parameter handling
*/
bool
d_tests_sa_array_common_find
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int elements[] = { 10, 20, 30, 40, 50 };
    int search_val = 30;
    int not_found = 99;
    ssize_t result;
    printf("Testing `d_array_common_find`...\n");
    // Test 1: Find existing element
    result = d_array_common_find(elements,
                                 5,
                                 sizeof(int),
                                 &search_val,
                                 d_test_int_comparator);
    test_result &= d_assert_standalone(
        result == 2,  // index of 30
        "`d_array_common_find` returns correct index for existing element",
        "`d_array_common_find` returned wrong index",
        _test_info
    );
    // Test 2: Not find non-existent element
    result = d_array_common_find(elements,
                                 5,
                                 sizeof(int),
                                 &not_found,
                                 d_test_int_comparator);
    test_result &= d_assert_standalone(
        result == -1,
        "`d_array_common_find` returns -1 for missing element",
        "`d_array_common_find` failed to return -1 for missing element",
        _test_info
    );
    // Test 3: Find first occurrence with duplicates
    int dup_elements[] = { 1, 2, 3, 2, 4 };
    int dup_val = 2;
    result = d_array_common_find(dup_elements,
                                 5,
                                 sizeof(int),
                                 &dup_val,
                                 d_test_int_comparator);
    test_result &= d_assert_standalone(
        result == 1,  // first occurrence at index 1
        "`d_array_common_find` returns first occurrence",
        "`d_array_common_find` failed to return first occurrence",
        _test_info
    );
    // Test 4: Test with NULL comparator
    result = d_array_common_find(elements,
                                 5,
                                 sizeof(int),
                                 &search_val,
                                 NULL);
    test_result &= d_assert_standalone(
        result == -1,
        "`d_array_common_find` returns -1 for NULL comparator",
        "`d_array_common_find` failed with NULL comparator",
        _test_info
    );
    _test_info->tests_total++;
    if (test_result)
    {
        _test_info->tests_passed++;
    }

    return test_result;
}
/*
d_tests_sa_array_common_find_closest
  Tests the `d_array_common_find_closest` function
  Tests the following:
  - finding exact match
  - finding closest element in sorted array
  - boundary cases
  - NULL parameter handling
*/
bool
d_tests_sa_array_common_find_closest
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int elements[] = { 10, 20, 30, 40, 50 };  // sorted
    int exact = 30;
    int between = 25;
    int below = 5;
    int above = 55;
    ssize_t result;
    printf("Testing `d_array_common_find_closest`...\n");
    // Test 1: Find exact match
    result = d_array_common_find_closest(elements,
                                         5,
                                         sizeof(int),
                                         &exact,
                                         d_test_int_comparator);
    test_result &= d_assert_standalone(
        result == 2,  // exact match at index 2
        "`d_array_common_find_closest` finds exact match",
        "`d_array_common_find_closest` failed to find exact match",
        _test_info
    );
    // Test 2: Find closest for value between elements
    result = d_array_common_find_closest(elements,
                                         5,
                                         sizeof(int),
                                         &between,
                                         d_test_int_comparator);
    test_result &= d_assert_standalone(
        ( (result == 2) ||
          (result == 3) ),  // should be close to 30 or 40
        "`d_array_common_find_closest` returns closest index",
        "`d_array_common_find_closest` failed for between value",
        _test_info
    );
    // Test 3: Find closest for value below all
    result = d_array_common_find_closest(elements,
                                         5,
                                         sizeof(int),
                                         &below,
                                         d_test_int_comparator);
    test_result &= d_assert_standalone(
        result == 0,  // should return first element
        "`d_array_common_find_closest` handles below-range value",
        "`d_array_common_find_closest` failed for below-range value",
        _test_info
    );
    // Test 4: Find closest for value above all
    result = d_array_common_find_closest(elements,
                                         5,
                                         sizeof(int),
                                         &above,
                                         d_test_int_comparator);
    test_result &= d_assert_standalone(
        result == 4,  // should return last element
        "`d_array_common_find_closest` handles above-range value",
        "`d_array_common_find_closest` failed for above-range value",
        _test_info
    );
    // Test 5: Test with empty array
    result = d_array_common_find_closest(elements,
                                         0,
                                         sizeof(int),
                                         &exact,
                                         d_test_int_comparator);
    test_result &= d_assert_standalone(
        result == -1,
        "`d_array_common_find_closest` returns -1 for empty array",
        "`d_array_common_find_closest` failed with empty array",
        _test_info
    );
    _test_info->tests_total++;
    if (test_result)
    {
        _test_info->tests_passed++;
    }

    return test_result;
}
/*
d_tests_sa_array_common_validate_params
  Tests the `d_array_common_validate_params` function
  Tests the following:
  - valid element size
  - zero element size
*/
bool
d_tests_sa_array_common_validate_params
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    bool result;

    // Test 1: Valid element size
    result = d_array_common_validate_params(sizeof(int));
    test_result &= d_assert_standalone(
        result == true,
        "`d_array_common_validate_params` accepts valid element size",
        "`d_array_common_validate_params` rejected valid element size",
        _test_info
    );
    // Test 2: Zero element size
    result = d_array_common_validate_params(0);
    test_result &= d_assert_standalone(
        result == false,
        "`d_array_common_validate_params` rejects zero element size",
        "`d_array_common_validate_params` accepted zero element size",
        _test_info
    );
    // Test 3: Large element size
    result = d_array_common_validate_params(1000);
    test_result &= d_assert_standalone(
        result == true,
        "`d_array_common_validate_params` accepts large element size",
        "`d_array_common_validate_params` rejected large element size",
        _test_info
    );
    _test_info->tests_total++;
    if (test_result)
    {
        _test_info->tests_passed++;
    }

    return test_result;
}


//=============================================================================
//                      UTILITY TEST GROUP RUNNER
//=============================================================================

/*
d_tests_sa_array_common_run_util_tests
  Runs all utility function tests and returns overall pass/fail status.
*/
bool
d_tests_sa_array_common_run_util_tests
(
    struct d_test_counter* _test_info
)
{
    bool overall_result = true;
    printf("\n----------------------------------------\n");
    printf("Running Utility Function Tests\n");
    printf("----------------------------------------\n");
    overall_result &= d_tests_sa_array_common_alloc(_test_info);
    overall_result &= d_tests_sa_array_common_append_element(_test_info);
    overall_result &= d_tests_sa_array_common_append_elements(_test_info);
    overall_result &= d_tests_sa_array_common_calc_capacity(_test_info);
    overall_result &= d_tests_sa_array_common_contains(_test_info);
    overall_result &= d_tests_sa_array_common_fill(_test_info);
    overall_result &= d_tests_sa_array_common_find(_test_info);
    overall_result &= d_tests_sa_array_common_find_closest(_test_info);
    overall_result &= d_tests_sa_array_common_validate_params(_test_info);
    return overall_result;
}
