/******************************************************************************
* djinterp [container]                                array_common_tests_manip.c
*
* Unit tests for `array_common.h` manipulation functions.
* Part of the array_common standalone test suite.
* 
* 
* path:      \test\container\array\array_common_tests_manip.c             
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.10.25
******************************************************************************/

#include ".\array_common_tests_sa.h"


//=============================================================================
//                       MANIPULATION FUNCTION TESTS
//=============================================================================


/*
d_tests_sa_array_common_insert_element
  Tests the `d_array_common_insert_element` function
*/
bool
d_tests_sa_array_common_insert_element
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int value = 99;
    bool result;

    printf("Testing `d_array_common_insert_element`...\n");

    // Test 1: Insert at beginning (index 0)
    int arr1[10] = { 1, 2, 3, 4, 5, 0, 0, 0, 0, 0 };
    result = d_array_common_insert_element(arr1,
                                           5,
                                           sizeof(int),
                                           &value,
                                           0);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr1[0] == 99)  &&
          (arr1[1] == 1)   &&
          (arr1[5] == 5) ),
        "`d_array_common_insert_element` correctly inserts at beginning",
        "`d_array_common_insert_element` failed to insert at beginning",
        _test_info
    );

    // Test 2: Insert in middle (index 2)
    int arr2[10] = { 1, 2, 3, 4, 5, 0, 0, 0, 0, 0 };
    result = d_array_common_insert_element(arr2,
                                           5,
                                           sizeof(int),
                                           &value,
                                           2);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr2[0] == 1)   &&
          (arr2[2] == 99)  &&
          (arr2[3] == 3) ),
        "`d_array_common_insert_element` correctly inserts in middle",
        "`d_array_common_insert_element` failed to insert in middle",
        _test_info
    );

    // Test 3: Insert at last valid index (index 4 in 5-element array)
    // Note: d_index_convert_safe treats index == count as invalid, so we test
    // inserting at the last valid position instead
    int arr3[10] = { 1, 2, 3, 4, 5, 0, 0, 0, 0, 0 };
    result = d_array_common_insert_element(arr3,
                                           5,
                                           sizeof(int),
                                           &value,
                                           4);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr3[3] == 4)   &&
          (arr3[4] == 99)  &&
          (arr3[5] == 5) ),
        "`d_array_common_insert_element` correctly inserts at last position",
        "`d_array_common_insert_element` failed to insert at last position",
        _test_info
    );

    // Test 4: Test with NULL parameters
    result = d_array_common_insert_element(NULL,
                                           5,
                                           sizeof(int),
                                           &value,
                                           0);

    test_result &= d_assert_standalone(
        result == false,
        "`d_array_common_insert_element` correctly handles NULL elements",
        "`d_array_common_insert_element` failed with NULL elements",
        _test_info
    );

    // Test 5: Test insert using negative index (-1 = last element)
    int arr5[10] = { 1, 2, 3, 4, 5, 0, 0, 0, 0, 0 };
    result = d_array_common_insert_element(arr5,
                                           5,
                                           sizeof(int),
                                           &value,
                                           -1);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr5[4] == 99)  &&
          (arr5[5] == 5) ),
        "`d_array_common_insert_element` correctly handles negative index",
        "`d_array_common_insert_element` failed with negative index",
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
d_tests_sa_array_common_insert_elements
  Tests the `d_array_common_insert_elements` function
*/
bool
d_tests_sa_array_common_insert_elements
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int values[] = { 88, 99 };
    bool result;

    printf("Testing `d_array_common_insert_elements`...\n");

    // Test 1: Insert multiple at beginning
    int arr1[10] = { 1, 2, 3, 4, 5, 0, 0, 0, 0, 0 };
    result = d_array_common_insert_elements(arr1,
                                            5,
                                            sizeof(int),
                                            values,
                                            2,
                                            0);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr1[0] == 88)  &&
          (arr1[1] == 99)  &&
          (arr1[2] == 1) ),
        "`d_array_common_insert_elements` correctly inserts at beginning",
        "`d_array_common_insert_elements` failed to insert at beginning",
        _test_info
    );

    // Test 2: Insert multiple in middle
    int arr2[10] = { 1, 2, 3, 4, 5, 0, 0, 0, 0, 0 };
    result = d_array_common_insert_elements(arr2,
                                            5,
                                            sizeof(int),
                                            values,
                                            2,
                                            2);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr2[2] == 88)  &&
          (arr2[3] == 99)  &&
          (arr2[4] == 3) ),
        "`d_array_common_insert_elements` correctly inserts in middle",
        "`d_array_common_insert_elements` failed to insert in middle",
        _test_info
    );

    // Test 3: Insert zero elements
    int arr3[10] = { 1, 2, 3, 4, 5, 0, 0, 0, 0, 0 };
    result = d_array_common_insert_elements(arr3,
                                            5,
                                            sizeof(int),
                                            values,
                                            0,
                                            2);

    test_result &= d_assert_standalone(
        result == true,
        "`d_array_common_insert_elements` handles zero count correctly",
        "`d_array_common_insert_elements` failed with zero count",
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
d_tests_sa_array_common_is_valid_resize_amount
  Tests the `d_array_common_is_valid_resize_amount` function
*/
bool
d_tests_sa_array_common_is_valid_resize_amount
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    size_t result_size;
    bool result;

    printf("Testing `d_array_common_is_valid_resize_amount`...\n");

    // Test 1: Valid positive resize
    result = d_array_common_is_valid_resize_amount(10,
                                                   5,
                                                   &result_size);

    test_result &= d_assert_standalone(
        ( (result == true)    &&
          (result_size == 15) ),
        "`d_array_common_is_valid_resize_amount` handles positive resize",
        "`d_array_common_is_valid_resize_amount` failed with positive resize",
        _test_info
    );

    // Test 2: Valid negative resize
    result = d_array_common_is_valid_resize_amount(10,
                                                   -3,
                                                   &result_size);

    test_result &= d_assert_standalone(
        ( (result == true)   &&
          (result_size == 7) ),
        "`d_array_common_is_valid_resize_amount` handles negative resize",
        "`d_array_common_is_valid_resize_amount` failed with negative resize",
        _test_info
    );

    // Test 3: Underflow detection (negative amount > count)
    result = d_array_common_is_valid_resize_amount(5,
                                                   -10,
                                                   &result_size);

    test_result &= d_assert_standalone(
        result == false,
        "`d_array_common_is_valid_resize_amount` detects underflow",
        "`d_array_common_is_valid_resize_amount` failed to detect underflow",
        _test_info
    );

    // Test 4: NULL result pointer
    result = d_array_common_is_valid_resize_amount(10,
                                                   5,
                                                   NULL);

    test_result &= d_assert_standalone(
        result == false,
        "`d_array_common_is_valid_resize_amount` handles NULL result",
        "`d_array_common_is_valid_resize_amount` failed with NULL result",
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
d_tests_sa_array_common_is_valid_resize_factor
  Tests the `d_array_common_is_valid_resize_factor` function
*/
bool
d_tests_sa_array_common_is_valid_resize_factor
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    double result_size;
    bool result;

    printf("Testing `d_array_common_is_valid_resize_factor`...\n");

    // Test 1: Valid growth factor (2x)
    result = d_array_common_is_valid_resize_factor(10,
                                                   2.0,
                                                   &result_size,
                                                   false);

    test_result &= d_assert_standalone(
        ( (result == true)      &&
          (result_size == 20.0) ),
        "`d_array_common_is_valid_resize_factor` handles 2x factor",
        "`d_array_common_is_valid_resize_factor` failed with 2x factor",
        _test_info
    );

    // Test 2: Shrink factor (0.5x)
    result = d_array_common_is_valid_resize_factor(10,
                                                   0.5,
                                                   &result_size,
                                                   false);

    test_result &= d_assert_standalone(
        ( (result == true)     &&
          (result_size == 5.0) ),
        "`d_array_common_is_valid_resize_factor` handles shrink factor",
        "`d_array_common_is_valid_resize_factor` failed with shrink factor",
        _test_info
    );

    // Test 3: Zero factor
    result = d_array_common_is_valid_resize_factor(10,
                                                   0.0,
                                                   &result_size,
                                                   false);

    test_result &= d_assert_standalone(
        ( (result == true)     &&
          (result_size == 0.0) ),
        "`d_array_common_is_valid_resize_factor` handles zero factor",
        "`d_array_common_is_valid_resize_factor` failed with zero factor",
        _test_info
    );

    // Test 4: Negative factor (invalid)
    result = d_array_common_is_valid_resize_factor(10,
                                                   -1.0,
                                                   &result_size,
                                                   false);

    test_result &= d_assert_standalone(
        result == false,
        "`d_array_common_is_valid_resize_factor` rejects negative factor",
        "`d_array_common_is_valid_resize_factor` accepted negative factor",
        _test_info
    );

    // Test 5: Rounding behavior
    result = d_array_common_is_valid_resize_factor(10,
                                                   1.5,
                                                   &result_size,
                                                   true);

    test_result &= d_assert_standalone(
        ( (result == true)      &&
          (result_size == 15.0) ),
        "`d_array_common_is_valid_resize_factor` handles rounding",
        "`d_array_common_is_valid_resize_factor` failed with rounding",
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
d_tests_sa_array_common_prepend_element
  Tests the `d_array_common_prepend_element` function
*/
bool
d_tests_sa_array_common_prepend_element
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int elements[10] = { 1, 2, 3, 4, 5, 0, 0, 0, 0, 0 };
    int value = 99;
    bool result;

    printf("Testing `d_array_common_prepend_element`...\n");

    // Test 1: Prepend element
    result = d_array_common_prepend_element(elements,
                                            5,
                                            sizeof(int),
                                            &value);

    test_result &= d_assert_standalone(
        ( (result == true)    &&
          (elements[0] == 99) &&
          (elements[1] == 1)  &&
          (elements[5] == 5) ),
        "`d_array_common_prepend_element` correctly prepends element",
        "`d_array_common_prepend_element` failed to prepend element",
        _test_info
    );

    // Test 2: Test with NULL parameters
    result = d_array_common_prepend_element(NULL,
                                            5,
                                            sizeof(int),
                                            &value);

    test_result &= d_assert_standalone(
        result == false,
        "`d_array_common_prepend_element` handles NULL elements",
        "`d_array_common_prepend_element` failed with NULL elements",
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
d_tests_sa_array_common_prepend_elements
  Tests the `d_array_common_prepend_elements` function
*/
bool
d_tests_sa_array_common_prepend_elements
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int elements[10] = { 1, 2, 3, 4, 5, 0, 0, 0, 0, 0 };
    int values[] = { 88, 99 };
    bool result;

    printf("Testing `d_array_common_prepend_elements`...\n");

    // Test 1: Prepend multiple elements
    result = d_array_common_prepend_elements(elements,
                                             5,
                                             sizeof(int),
                                             values,
                                             2);

    test_result &= d_assert_standalone(
        ( (result == true)    &&
          (elements[0] == 88) &&
          (elements[1] == 99) &&
          (elements[2] == 1) ),
        "`d_array_common_prepend_elements` correctly prepends elements",
        "`d_array_common_prepend_elements` failed to prepend elements",
        _test_info
    );

    // Test 2: Prepend zero elements
    int arr2[10] = { 1, 2, 3, 4, 5, 0, 0, 0, 0, 0 };
    result = d_array_common_prepend_elements(arr2,
                                             5,
                                             sizeof(int),
                                             values,
                                             0);

    test_result &= d_assert_standalone(
        result == true,
        "`d_array_common_prepend_elements` handles zero count",
        "`d_array_common_prepend_elements` failed with zero count",
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
d_tests_sa_array_common_resize_amount
  Tests the `d_array_common_resize_amount` function
*/
bool
d_tests_sa_array_common_resize_amount
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int elements[20] = { 1, 2, 3, 4, 5 };
    ssize_t result;

    printf("Testing `d_array_common_resize_amount`...\n");

    // Test 1: Positive resize
    result = d_array_common_resize_amount(elements,
                                          10,
                                          sizeof(int),
                                          5);

    test_result &= d_assert_standalone(
        result == 15,
        "`d_array_common_resize_amount` handles positive resize",
        "`d_array_common_resize_amount` failed with positive resize",
        _test_info
    );

    // Test 2: Negative resize
    result = d_array_common_resize_amount(elements,
                                          10,
                                          sizeof(int),
                                          -3);

    test_result &= d_assert_standalone(
        result == 7,
        "`d_array_common_resize_amount` handles negative resize",
        "`d_array_common_resize_amount` failed with negative resize",
        _test_info
    );

    // Test 3: NULL elements
    result = d_array_common_resize_amount(NULL,
                                          10,
                                          sizeof(int),
                                          5);

    test_result &= d_assert_standalone(
        result == -1,
        "`d_array_common_resize_amount` returns -1 for NULL elements",
        "`d_array_common_resize_amount` failed with NULL elements",
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
d_tests_sa_array_common_resize_factor
  Tests the `d_array_common_resize_factor` function
*/
bool
d_tests_sa_array_common_resize_factor
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int elements[20] = { 1, 2, 3, 4, 5 };
    ssize_t result;

    printf("Testing `d_array_common_resize_factor`...\n");

    // Test 1: Growth factor (2x)
    result = d_array_common_resize_factor(elements,
                                          10,
                                          sizeof(int),
                                          2.0);

    test_result &= d_assert_standalone(
        result == 20,
        "`d_array_common_resize_factor` handles 2x growth",
        "`d_array_common_resize_factor` failed with 2x growth",
        _test_info
    );

    // Test 2: Shrink factor (0.5x)
    result = d_array_common_resize_factor(elements,
                                          10,
                                          sizeof(int),
                                          0.5);

    test_result &= d_assert_standalone(
        result == 5,
        "`d_array_common_resize_factor` handles 0.5x shrink",
        "`d_array_common_resize_factor` failed with 0.5x shrink",
        _test_info
    );

    // Test 3: NULL elements
    result = d_array_common_resize_factor(NULL,
                                          10,
                                          sizeof(int),
                                          2.0);

    test_result &= d_assert_standalone(
        result == -1,
        "`d_array_common_resize_factor` returns -1 for NULL elements",
        "`d_array_common_resize_factor` failed with NULL elements",
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
d_tests_sa_array_common_reverse
  Tests the `d_array_common_reverse` function
*/
bool
d_tests_sa_array_common_reverse
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    bool result;

    printf("Testing `d_array_common_reverse`...\n");

    // Test 1: Reverse odd-sized array
    int arr1[] = { 1, 2, 3, 4, 5 };
    result = d_array_common_reverse(arr1,
                                    5,
                                    sizeof(int));

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr1[0] == 5)   &&
          (arr1[2] == 3)   &&
          (arr1[4] == 1) ),
        "`d_array_common_reverse` correctly reverses odd-sized array",
        "`d_array_common_reverse` failed with odd-sized array",
        _test_info
    );

    // Test 2: Reverse even-sized array
    int arr2[] = { 10, 20, 30, 40 };
    result = d_array_common_reverse(arr2,
                                    4,
                                    sizeof(int));

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr2[0] == 40)  &&
          (arr2[1] == 30)  &&
          (arr2[3] == 10) ),
        "`d_array_common_reverse` correctly reverses even-sized array",
        "`d_array_common_reverse` failed with even-sized array",
        _test_info
    );

    // Test 3: Single element (trivial success)
    int arr3[] = { 42 };
    result = d_array_common_reverse(arr3,
                                    1,
                                    sizeof(int));

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr3[0] == 42) ),
        "`d_array_common_reverse` handles single element",
        "`d_array_common_reverse` failed with single element",
        _test_info
    );

    // Test 4: Empty array (trivial success)
    result = d_array_common_reverse(arr3,
                                    0,
                                    sizeof(int));

    test_result &= d_assert_standalone(
        result == true,
        "`d_array_common_reverse` handles empty array",
        "`d_array_common_reverse` failed with empty array",
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
d_tests_sa_array_common_shift_left
  Tests the `d_array_common_shift_left` function
*/
bool
d_tests_sa_array_common_shift_left
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    bool result;

    printf("Testing `d_array_common_shift_left`...\n");

    // Test 1: Shift left by 2
    int arr1[] = { 1, 2, 3, 4, 5 };
    result = d_array_common_shift_left(arr1,
                                       5,
                                       sizeof(int),
                                       2);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr1[0] == 3)   &&
          (arr1[1] == 4)   &&
          (arr1[2] == 5) ),
        "`d_array_common_shift_left` correctly shifts left by 2",
        "`d_array_common_shift_left` failed shifting left by 2",
        _test_info
    );

    // Test 2: Shift by amount >= count (should fail)
    int arr2[] = { 1, 2, 3 };
    result = d_array_common_shift_left(arr2,
                                       3,
                                       sizeof(int),
                                       5);

    test_result &= d_assert_standalone(
        result == false,
        "`d_array_common_shift_left` rejects shift >= count",
        "`d_array_common_shift_left` accepted invalid shift amount",
        _test_info
    );

    // Test 3: Zero shift (should succeed without changing)
    int arr3[] = { 1, 2, 3 };
    result = d_array_common_shift_left(arr3,
                                       3,
                                       sizeof(int),
                                       0);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr3[0] == 1) ),
        "`d_array_common_shift_left` handles zero shift",
        "`d_array_common_shift_left` failed with zero shift",
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
d_tests_sa_array_common_shift_left_circular
  Tests the `d_array_common_shift_left_circular` function
*/
bool
d_tests_sa_array_common_shift_left_circular
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    bool result;

    printf("Testing `d_array_common_shift_left_circular`...\n");

    // Test 1: Circular shift left by 2
    int arr1[] = { 1, 2, 3, 4, 5 };
    result = d_array_common_shift_left_circular(arr1,
                                                5,
                                                sizeof(int),
                                                2);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr1[0] == 3)   &&
          (arr1[3] == 1)   &&
          (arr1[4] == 2) ),
        "`d_array_common_shift_left_circular` correctly circular shifts left",
        "`d_array_common_shift_left_circular` failed circular shift",
        _test_info
    );

    // Test 2: Shift by exact count (should return to original)
    int arr2[] = { 10, 20, 30 };
    result = d_array_common_shift_left_circular(arr2,
                                                3,
                                                sizeof(int),
                                                3);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr2[0] == 10)  &&
          (arr2[1] == 20)  &&
          (arr2[2] == 30) ),
        "`d_array_common_shift_left_circular` handles shift by count",
        "`d_array_common_shift_left_circular` failed with shift by count",
        _test_info
    );

    // Test 3: Zero shift
    int arr3[] = { 1, 2, 3 };
    result = d_array_common_shift_left_circular(arr3,
                                                3,
                                                sizeof(int),
                                                0);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr3[0] == 1) ),
        "`d_array_common_shift_left_circular` handles zero shift",
        "`d_array_common_shift_left_circular` failed with zero shift",
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
d_tests_sa_array_common_shift_right
  Tests the `d_array_common_shift_right` function
*/
bool
d_tests_sa_array_common_shift_right
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    bool result;

    printf("Testing `d_array_common_shift_right`...\n");

    // Test 1: Shift right by 2
    int arr1[] = { 1, 2, 3, 4, 5 };
    result = d_array_common_shift_right(arr1,
                                        5,
                                        sizeof(int),
                                        2);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr1[2] == 1)   &&
          (arr1[3] == 2)   &&
          (arr1[4] == 3) ),
        "`d_array_common_shift_right` correctly shifts right by 2",
        "`d_array_common_shift_right` failed shifting right by 2",
        _test_info
    );

    // Test 2: Shift by amount >= count (should fail)
    int arr2[] = { 1, 2, 3 };
    result = d_array_common_shift_right(arr2,
                                        3,
                                        sizeof(int),
                                        5);

    test_result &= d_assert_standalone(
        result == false,
        "`d_array_common_shift_right` rejects shift >= count",
        "`d_array_common_shift_right` accepted invalid shift amount",
        _test_info
    );

    // Test 3: Zero shift
    int arr3[] = { 1, 2, 3 };
    result = d_array_common_shift_right(arr3,
                                        3,
                                        sizeof(int),
                                        0);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr3[0] == 1) ),
        "`d_array_common_shift_right` handles zero shift",
        "`d_array_common_shift_right` failed with zero shift",
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
d_tests_sa_array_common_shift_right_circular
  Tests the `d_array_common_shift_right_circular` function
*/
bool
d_tests_sa_array_common_shift_right_circular
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    bool result;

    printf("Testing `d_array_common_shift_right_circular`...\n");

    // Test 1: Circular shift right by 2
    int arr1[] = { 1, 2, 3, 4, 5 };
    result = d_array_common_shift_right_circular(arr1,
                                                 5,
                                                 sizeof(int),
                                                 2);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr1[0] == 4)   &&
          (arr1[1] == 5)   &&
          (arr1[2] == 1) ),
        "`d_array_common_shift_right_circular` correctly circular shifts right",
        "`d_array_common_shift_right_circular` failed circular shift",
        _test_info
    );

    // Test 2: Shift by exact count (should return to original)
    int arr2[] = { 10, 20, 30 };
    result = d_array_common_shift_right_circular(arr2,
                                                 3,
                                                 sizeof(int),
                                                 3);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr2[0] == 10)  &&
          (arr2[1] == 20)  &&
          (arr2[2] == 30) ),
        "`d_array_common_shift_right_circular` handles shift by count",
        "`d_array_common_shift_right_circular` failed with shift by count",
        _test_info
    );

    // Test 3: Zero shift
    int arr3[] = { 1, 2, 3 };
    result = d_array_common_shift_right_circular(arr3,
                                                 3,
                                                 sizeof(int),
                                                 0);

    test_result &= d_assert_standalone(
        ( (result == true) &&
          (arr3[0] == 1) ),
        "`d_array_common_shift_right_circular` handles zero shift",
        "`d_array_common_shift_right_circular` failed with zero shift",
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
d_tests_sa_array_common_sort
  Tests the `d_array_common_sort` function
*/
bool
d_tests_sa_array_common_sort
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;

    printf("Testing `d_array_common_sort`...\n");

    // Test 1: Sort unsorted array
    int arr1[] = { 3, 1, 4, 1, 5, 9, 2, 6 };
    d_array_common_sort(arr1,
                        8,
                        sizeof(int),
                        d_test_int_comparator);

    test_result &= d_assert_standalone(
        ( (arr1[0] == 1) &&
          (arr1[1] == 1) &&
          (arr1[2] == 2) &&
          (arr1[7] == 9) ),
        "`d_array_common_sort` correctly sorts unsorted array",
        "`d_array_common_sort` failed to sort unsorted array",
        _test_info
    );

    // Test 2: Sort already sorted array
    int arr2[] = { 1, 2, 3, 4, 5 };
    d_array_common_sort(arr2,
                        5,
                        sizeof(int),
                        d_test_int_comparator);

    test_result &= d_assert_standalone(
        ( (arr2[0] == 1) &&
          (arr2[4] == 5) ),
        "`d_array_common_sort` handles already sorted array",
        "`d_array_common_sort` failed with sorted array",
        _test_info
    );

    // Test 3: Sort reverse-sorted array
    int arr3[] = { 5, 4, 3, 2, 1 };
    d_array_common_sort(arr3,
                        5,
                        sizeof(int),
                        d_test_int_comparator);

    test_result &= d_assert_standalone(
        ( (arr3[0] == 1) &&
          (arr3[4] == 5) ),
        "`d_array_common_sort` sorts reverse-sorted array",
        "`d_array_common_sort` failed with reverse-sorted array",
        _test_info
    );

    // Test 4: Single element (no-op)
    int arr4[] = { 42 };
    d_array_common_sort(arr4,
                        1,
                        sizeof(int),
                        d_test_int_comparator);

    test_result &= d_assert_standalone(
        arr4[0] == 42,
        "`d_array_common_sort` handles single element",
        "`d_array_common_sort` failed with single element",
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
//                        MANIPULATION TEST GROUP RUNNER
//=============================================================================

/*
d_tests_sa_array_common_run_manip_tests
  Runs all manipulation function tests and returns overall pass/fail status.
*/
bool
d_tests_sa_array_common_run_manip_tests
(
    struct d_test_counter* _test_info
)
{
    bool overall_result = true;

    printf("\n----------------------------------------\n");
    printf("Running Manipulation Function Tests\n");
    printf("----------------------------------------\n");

    overall_result &= d_tests_sa_array_common_insert_element(_test_info);
    overall_result &= d_tests_sa_array_common_insert_elements(_test_info);
    overall_result &= d_tests_sa_array_common_is_valid_resize_amount(
        _test_info
    );
    overall_result &= d_tests_sa_array_common_is_valid_resize_factor(
        _test_info
    );
    overall_result &= d_tests_sa_array_common_prepend_element(_test_info);
    overall_result &= d_tests_sa_array_common_prepend_elements(_test_info);
    overall_result &= d_tests_sa_array_common_resize_amount(_test_info);
    overall_result &= d_tests_sa_array_common_resize_factor(_test_info);
    overall_result &= d_tests_sa_array_common_reverse(_test_info);
    overall_result &= d_tests_sa_array_common_shift_left(_test_info);
    overall_result &= d_tests_sa_array_common_shift_left_circular(_test_info);
    overall_result &= d_tests_sa_array_common_shift_right(_test_info);
    overall_result &= d_tests_sa_array_common_shift_right_circular(_test_info);
    overall_result &= d_tests_sa_array_common_sort(_test_info);

    return overall_result;
}
