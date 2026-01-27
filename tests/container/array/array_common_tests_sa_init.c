/******************************************************************************
* djinterp [container]                                 array_common_tests_init.c
*
* Unit tests for `array_common.h` initialization functions.
* Part of the array_common standalone test suite.
* 
* 
* path:      \test\container\array\array_common_tests_init.c             
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.10.25
******************************************************************************/

#include ".\array_common_tests_sa.h"


/*
===============================================================================
                        INITIALIZATION FUNCTION TESTS
===============================================================================
*/

/*
d_tests_sa_array_common_init_sized
  Tests the `d_array_common_init_sized` function which allocates and initializes
  an elements array with a given size.
  Tests the following:
  - zero size allocation handling
  - valid size memory allocation
  - different element sizes
  - proper count initialization
*/
bool
d_tests_sa_array_common_init_sized
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    void* elements = NULL;
    size_t count = SIZE_MAX;
    bool result;

    printf("Testing `d_array_common_init_sized`...\n");

    // Test 1: Initialize with zero size
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_sized(&elements, &count, sizeof(int), 0);

    test_result &= d_assert_standalone(
        result == true && elements == NULL && count == 0,
        "`d_array_common_init_sized` correctly handles zero size",
        "`d_array_common_init_sized` failed with zero size",
        _test_info
    );

    // Test 2: Initialize with valid size
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_sized(&elements, &count, sizeof(int), 10);

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 0,
        "`d_array_common_init_sized` correctly allocates memory for valid size",
        "`d_array_common_init_sized` failed with valid size",
        _test_info
    );

    if (elements)
    { 
        free(elements); 
        elements = NULL; 
    }

    // Test 3: Initialize with different element sizes
    elements = NULL;
    count    = SIZE_MAX;
    result   = d_array_common_init_sized(&elements, &count, sizeof(double), 5);

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 0,
        "`d_array_common_init_sized` works with different element sizes",
        "`d_array_common_init_sized` failed with different element sizes",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    // Test 4: Initialize with large size
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_sized(&elements, &count, sizeof(char), 1000);

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 0,
        "`d_array_common_init_sized` handles large allocations",
        "`d_array_common_init_sized` failed with large allocation",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_common_init_from_array
  Tests the `d_array_common_init_from_array` function
  Tests the following:
  - copying from valid source array
  - handling zero count
  - handling NULL source
  - proper memory allocation and copying
*/
bool
d_tests_sa_array_common_init_from_array
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int source[] = { 1, 2, 3, 4, 5 };
    void* elements = NULL;
    size_t count = SIZE_MAX;
    bool result;
    int* int_elements;

    printf("Testing `d_array_common_init_from_array`...\n");

    // Test 1: Initialize from valid array
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_from_array(&elements, &count, sizeof(int), source, 5);
    int_elements = (int*)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 5 && 
        int_elements[0] == 1 && int_elements[4] == 5,
        "`d_array_common_init_from_array` correctly copies from source array",
        "`d_array_common_init_from_array` failed to copy from source array",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    // Test 2: Initialize with zero count
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_from_array(&elements, &count, sizeof(int), source, 0);

    test_result &= d_assert_standalone(
        result == true && elements == NULL && count == 0,
        "`d_array_common_init_from_array` handles zero count correctly",
        "`d_array_common_init_from_array` failed with zero count",
        _test_info
    );

    // Test 3: Initialize with NULL source
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_from_array(&elements, &count, sizeof(int), NULL, 5);

    test_result &= d_assert_standalone(
        result == true && elements == NULL && count == 0,
        "`d_array_common_init_from_array` handles NULL source correctly",
        "`d_array_common_init_from_array` failed with NULL source",
        _test_info
    );

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_common_init_from_args
  Tests the `d_array_common_init_from_args` function
  Tests the following:
  - initialization from variadic arguments
  - handling zero argument count
  - proper memory allocation and element setting
*/
bool
d_tests_sa_array_common_init_from_args
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    void* elements = NULL;
    size_t count = SIZE_MAX;
    bool result;
    int* int_elements;

    printf("Testing `d_array_common_init_from_args`...\n");

    // Test 1: Initialize from valid arguments
    elements = NULL;
    count = SIZE_MAX;
    result = d_test_init_from_args_wrapper(&elements, &count, sizeof(int), 3, 10, 20, 30);
    int_elements = (int*)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 3 && 
        int_elements[0] == 10 && int_elements[1] == 20 && int_elements[2] == 30,
        "`d_array_common_init_from_args` correctly initializes from arguments",
        "`d_array_common_init_from_args` failed to initialize from arguments",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    // Test 2: Initialize with zero arguments
    elements = NULL;
    count = SIZE_MAX;
    result = d_test_init_from_args_wrapper(&elements, &count, sizeof(int), 0);

    test_result &= d_assert_standalone(
        result == true && elements == NULL && count == 0,
        "`d_array_common_init_from_args` handles zero arguments correctly",
        "`d_array_common_init_from_args` failed with zero arguments",
        _test_info
    );

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_common_init_copy
  Tests the `d_array_common_init_copy` function
  Tests the following:
  - copying initialization from source data
  - verification that it works as alias to init_from_array
*/
bool
d_tests_sa_array_common_init_copy
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int source[] = { 100, 200, 300 };
    void* elements = NULL;
    size_t count = SIZE_MAX;
    bool result;
    int* int_elements;

    printf("Testing `d_array_common_init_copy`...\n");

    // Test 1: Copy initialization from valid source
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_copy(&elements, &count, sizeof(int), source, 3);
    int_elements = (int*)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 3 && 
        int_elements[0] == 100 && int_elements[2] == 300,
        "`d_array_common_init_copy` correctly performs copy initialization",
        "`d_array_common_init_copy` failed to perform copy initialization",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_common_init_copy_reverse
  Tests the `d_array_common_init_copy_reverse` function
  Tests the following:
  - copying elements in reverse order with positive indices
  - copying elements in reverse order with negative indices
  - proper d_index validation and conversion
  - invalid index handling
*/
bool
d_tests_sa_array_common_init_copy_reverse
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int source[] = { 1, 2, 3, 4, 5 };  // 5 elements, indices 0-4
    void* elements = NULL;
    size_t count = SIZE_MAX;
    bool result;
    int* int_elements;

    printf("Testing `d_array_common_init_copy_reverse`...\n");

    // Test 1: Copy reverse with positive indices (1 to 3) -> should copy [2,3,4] in reverse -> [4,3,2]
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_copy_reverse(&elements, &count, sizeof(int), source, 5, 1, 3);
    int_elements = (int*)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 3 && 
        int_elements[0] == 4 && int_elements[1] == 3 && int_elements[2] == 2,
        "`d_array_common_init_copy_reverse` correctly copies range in reverse with positive indices",
        "`d_array_common_init_copy_reverse` failed with positive indices",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    // Test 2: Copy reverse with negative indices (-3 to -1) -> indices 2 to 4 -> [3,4,5] in reverse -> [5,4,3]
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_copy_reverse(&elements, &count, sizeof(int), source, 5, -3, -1);
    int_elements = (int*)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 3 && 
        int_elements[0] == 5 && int_elements[1] == 4 && int_elements[2] == 3,
        "`d_array_common_init_copy_reverse` correctly handles negative indices",
        "`d_array_common_init_copy_reverse` failed with negative indices",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    // Test 3: Test invalid indices (start > end)
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_copy_reverse(&elements, &count, sizeof(int), source, 5, 3, 1);

    test_result &= d_assert_standalone(
        result == false && elements == NULL && count == 0,
        "`d_array_common_init_copy_reverse` correctly rejects invalid range (start > end)",
        "`d_array_common_init_copy_reverse` failed to reject invalid range",
        _test_info
    );

    // Test 4: Test out-of-bounds indices
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_copy_reverse(&elements, &count, sizeof(int), source, 5, 0, 10);

    test_result &= d_assert_standalone(
        result == false && elements == NULL && count == 0,
        "`d_array_common_init_copy_reverse` correctly rejects out-of-bounds indices",
        "`d_array_common_init_copy_reverse` failed to reject out-of-bounds indices",
        _test_info
    );

    // Test 5: Test with NULL parameters
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_copy_reverse(&elements, &count, sizeof(int), NULL, 5, 0, 2);

    test_result &= d_assert_standalone(
        result == false && elements == NULL && count == 0,
        "`d_array_common_init_copy_reverse` correctly handles NULL source",
        "`d_array_common_init_copy_reverse` failed with NULL source",
        _test_info
    );

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_common_init_copy_range
  Tests the `d_array_common_init_copy_range` function
  Tests the following:
  - copying a range with positive indices
  - copying a range with negative indices
  - proper d_index validation and conversion
  - invalid index handling
*/
bool
d_tests_sa_array_common_init_copy_range
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int source[] = { 10, 20, 30, 40, 50 };  // 5 elements, indices 0-4
    void* elements = NULL;
    size_t count = SIZE_MAX;
    bool result;
    int* int_elements;

    printf("Testing `d_array_common_init_copy_range`...\n");

    // Test 1: Copy range with positive indices (1 to 3) -> should copy [20,30,40]
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_copy_range(&elements, &count, sizeof(int), source, 5, 1, 3);
    int_elements = (int*)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 3 && 
        int_elements[0] == 20 && int_elements[1] == 30 && int_elements[2] == 40,
        "`d_array_common_init_copy_range` correctly copies range with positive indices",
        "`d_array_common_init_copy_range` failed with positive indices",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    // Test 2: Copy range with negative indices (-2 to -1) -> indices 3 to 4 -> [40,50]
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_copy_range(&elements, &count, sizeof(int), source, 5, -2, -1);
    int_elements = (int*)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 2 && 
        int_elements[0] == 40 && int_elements[1] == 50,
        "`d_array_common_init_copy_range` correctly handles negative indices",
        "`d_array_common_init_copy_range` failed with negative indices",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    // Test 3: Test single element range (index 2 to 2) -> [30]
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_copy_range(&elements, &count, sizeof(int), source, 5, 2, 2);
    int_elements = (int*)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 1 && 
        int_elements[0] == 30,
        "`d_array_common_init_copy_range` correctly handles single element range",
        "`d_array_common_init_copy_range` failed with single element range",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    // Test 4: Test invalid range (start > end)
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_copy_range(&elements, &count, sizeof(int), source, 5, 3, 1);

    test_result &= d_assert_standalone(
        result == false && elements == NULL && count == 0,
        "`d_array_common_init_copy_range` correctly rejects invalid range",
        "`d_array_common_init_copy_range` failed to reject invalid range",
        _test_info
    );

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_common_init_copy_range_reverse
  Tests the `d_array_common_init_copy_range_reverse` function
  Tests the following:
  - copying a range in reverse order with positive indices
  - copying a range in reverse order with negative indices
  - proper d_index validation and conversion
*/
bool
d_tests_sa_array_common_init_copy_range_reverse
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    int source[] = { 100, 200, 300, 400, 500 };  // 5 elements, indices 0-4
    void* elements = NULL;
    size_t count = SIZE_MAX;
    bool result;
    int* int_elements;

    printf("Testing `d_array_common_init_copy_range_reverse`...\n");

    // Test 1: Copy range reverse with positive indices (0 to 2) -> [100,200,300] in reverse -> [300,200,100]
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_copy_range_reverse(&elements, &count, sizeof(int), source, 5, 0, 2);
    int_elements = (int*)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 3 && 
        int_elements[0] == 300 && int_elements[1] == 200 && int_elements[2] == 100,
        "`d_array_common_init_copy_range_reverse` correctly copies range in reverse",
        "`d_array_common_init_copy_range_reverse` failed with positive indices",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    // Test 2: Copy range reverse with negative indices (-3 to -2) -> indices 2 to 3 -> [300,400] in reverse -> [400,300]
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_copy_range_reverse(&elements, &count, sizeof(int), source, 5, -3, -2);
    int_elements = (int*)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 2 && 
        int_elements[0] == 400 && int_elements[1] == 300,
        "`d_array_common_init_copy_range_reverse` correctly handles negative indices",
        "`d_array_common_init_copy_range_reverse` failed with negative indices",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_common_init_fill
  Tests the `d_array_common_init_fill` function
  Tests the following:
  - filling array with a specific value
  - handling zero size
  - handling NULL value pointer
*/
bool
d_tests_sa_array_common_init_fill
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    void* elements = NULL;
    size_t count = SIZE_MAX;
    bool result;
    int* int_elements;
    int fill_value = 42;

    printf("Testing `d_array_common_init_fill`...\n");

    // Test 1: Fill array with specific value
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_fill(&elements, &count, sizeof(int), 5, &fill_value);
    int_elements = (int*)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 5 && 
        int_elements[0] == 42 && int_elements[4] == 42,
        "`d_array_common_init_fill` correctly fills array with value",
        "`d_array_common_init_fill` failed to fill array",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    // Test 2: Fill with zero size
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_fill(&elements, &count, sizeof(int), 0, &fill_value);

    test_result &= d_assert_standalone(
        result == true && elements == NULL && count == 0,
        "`d_array_common_init_fill` correctly handles zero size",
        "`d_array_common_init_fill` failed with zero size",
        _test_info
    );

    // Test 3: Fill with NULL value (should fail per implementation)
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_fill(&elements, &count, sizeof(int), 3, NULL);

    test_result &= d_assert_standalone(
        result == false && elements == NULL && count == 0,
        "`d_array_common_init_fill` correctly rejects NULL value pointer",
        "`d_array_common_init_fill` failed with NULL value",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_common_init_slice
  Tests the `d_array_common_init_slice` function
  Tests the following:
  - slicing from specific starting index with positive index
  - slicing from specific starting index with negative index
  - proper d_index validation and conversion
  Note: This function operates on arrays of pointers (void**), copying the
  pointer values themselves, not the data they point to.
*/
bool
d_tests_sa_array_common_init_slice
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    void* elements = NULL;
    size_t count = SIZE_MAX;
    bool result;

    // create array of pointers for testing
    int val1 = 10, val2 = 20, val3 = 30, val4 = 40, val5 = 50;
    void* ptr_arr[5] = { &val1, &val2, &val3, &val4, &val5 };
    void** result_ptrs;

    printf("Testing `d_array_common_init_slice`...\n");

    // Test 1: Slice from positive index 2 -> should copy pointers [&val3, &val4, &val5]
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_slice(&elements, &count, sizeof(void*), 
                                       (const void**)ptr_arr, 5, 2);
    result_ptrs = (void**)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 3 && 
        result_ptrs[0] == &val3 && result_ptrs[1] == &val4 && result_ptrs[2] == &val5,
        "`d_array_common_init_slice` correctly slices from positive index",
        "`d_array_common_init_slice` failed with positive index",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    // Test 2: Slice from negative index -2 -> index 3 -> should copy [&val4, &val5]
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_slice(&elements, &count, sizeof(void*), 
                                       (const void**)ptr_arr, 5, -2);
    result_ptrs = (void**)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 2 && 
        result_ptrs[0] == &val4 && result_ptrs[1] == &val5,
        "`d_array_common_init_slice` correctly handles negative index",
        "`d_array_common_init_slice` failed with negative index",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    // Test 3: Test invalid index
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_slice(&elements, &count, sizeof(void*), 
                                       (const void**)ptr_arr, 5, 10);

    test_result &= d_assert_standalone(
        result == false && elements == NULL && count == 0,
        "`d_array_common_init_slice` correctly rejects invalid index",
        "`d_array_common_init_slice` failed to reject invalid index",
        _test_info
    );

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_common_init_slice_reverse
  Tests the `d_array_common_init_slice_reverse` function
  Tests the following:
  - slicing entire array in reverse order
  - proper memory allocation and element copying
  Note: This function operates on arrays of pointers (void**), copying the
  pointer values themselves in reverse order.
*/
bool
d_tests_sa_array_common_init_slice_reverse
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    void* elements = NULL;
    size_t count = SIZE_MAX;
    bool result;

    // create array of pointers for testing
    int val1 = 100, val2 = 200, val3 = 300;
    void* ptr_arr[3] = { &val1, &val2, &val3 };
    void** result_ptrs;

    printf("Testing `d_array_common_init_slice_reverse`...\n");

    // Test 1: Slice reverse entire array -> [&val1,&val2,&val3] in reverse -> [&val3,&val2,&val1]
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_slice_reverse(&elements, &count, sizeof(void*), 
                                                (const void**)ptr_arr, 3);
    result_ptrs = (void**)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 3 && 
        result_ptrs[0] == &val3 && result_ptrs[1] == &val2 && result_ptrs[2] == &val1,
        "`d_array_common_init_slice_reverse` correctly reverses entire slice",
        "`d_array_common_init_slice_reverse` failed to reverse slice",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_common_init_slice_range
  Tests the `d_array_common_init_slice_range` function
  Tests the following:
  - slicing a range with positive indices
  - slicing a range with negative indices
  - proper d_index validation and conversion
  Note: This function operates on arrays of pointers (void**), copying the
  pointer values themselves.
*/
bool
d_tests_sa_array_common_init_slice_range
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    void* elements = NULL;
    size_t count = SIZE_MAX;
    bool result;

    // create array of pointers for testing
    int val1 = 11, val2 = 22, val3 = 33, val4 = 44, val5 = 55;
    void* ptr_arr[5] = { &val1, &val2, &val3, &val4, &val5 };
    void** result_ptrs;

    printf("Testing `d_array_common_init_slice_range`...\n");

    // Test 1: Slice range with positive indices (1 to 3) -> [&val2, &val3, &val4]
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_slice_range(&elements, &count, sizeof(void*), 
                                              (const void**)ptr_arr, 5, 1, 3);
    result_ptrs = (void**)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 3 && 
        result_ptrs[0] == &val2 && result_ptrs[1] == &val3 && result_ptrs[2] == &val4,
        "`d_array_common_init_slice_range` correctly slices range with positive indices",
        "`d_array_common_init_slice_range` failed with positive indices",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    // Test 2: Slice range with negative indices (-2 to -1) -> indices 3 to 4 -> [&val4, &val5]
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_slice_range(&elements, &count, sizeof(void*), 
                                              (const void**)ptr_arr, 5, -2, -1);
    result_ptrs = (void**)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 2 && 
        result_ptrs[0] == &val4 && result_ptrs[1] == &val5,
        "`d_array_common_init_slice_range` correctly handles negative indices",
        "`d_array_common_init_slice_range` failed with negative indices",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_common_init_slice_range_reverse
  Tests the `d_array_common_init_slice_range_reverse` function
  Tests the following:
  - slicing a range in reverse order with positive indices
  - slicing a range in reverse order with negative indices
  - proper d_index validation and conversion
  Note: This function operates on arrays of pointers (void**), copying the
  pointer values themselves in reverse order.
*/
bool
d_tests_sa_array_common_init_slice_range_reverse
(
    struct d_test_counter* _test_info
)
{
    bool test_result = true;
    void* elements = NULL;
    size_t count = SIZE_MAX;
    bool result;

    // create array of pointers for testing
    int val1 = 111, val2 = 222, val3 = 333, val4 = 444, val5 = 555;
    void* ptr_arr[5] = { &val1, &val2, &val3, &val4, &val5 };
    void** result_ptrs;

    printf("Testing `d_array_common_init_slice_range_reverse`...\n");

    // Test 1: Slice range reverse with positive indices (0 to 2) -> [&val1,&val2,&val3] reversed -> [&val3,&val2,&val1]
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_slice_range_reverse(&elements, &count, sizeof(void*), 
                                                      (const void**)ptr_arr, 5, 0, 2);
    result_ptrs = (void**)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 3 && 
        result_ptrs[0] == &val3 && result_ptrs[1] == &val2 && result_ptrs[2] == &val1,
        "`d_array_common_init_slice_range_reverse` correctly slices range in reverse",
        "`d_array_common_init_slice_range_reverse` failed with positive indices",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    // Test 2: Slice range reverse with negative indices (-3 to -2) -> indices 2 to 3 -> [&val3,&val4] reversed -> [&val4,&val3]
    elements = NULL;
    count = SIZE_MAX;
    result = d_array_common_init_slice_range_reverse(&elements, &count, sizeof(void*), 
                                                      (const void**)ptr_arr, 5, -3, -2);
    result_ptrs = (void**)elements;

    test_result &= d_assert_standalone(
        result == true && elements != NULL && count == 2 && 
        result_ptrs[0] == &val4 && result_ptrs[1] == &val3,
        "`d_array_common_init_slice_range_reverse` correctly handles negative indices",
        "`d_array_common_init_slice_range_reverse` failed with negative indices",
        _test_info
    );

    if (elements) { free(elements); elements = NULL; }

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}


/*
===============================================================================
                        INITIALIZATION TEST GROUP RUNNER
===============================================================================
*/

/*
d_tests_sa_array_common_run_init_tests
  Runs all initialization function tests and returns overall pass/fail status.
*/
bool
d_tests_sa_array_common_run_init_tests
(
    struct d_test_counter* _test_info
)
{
    bool overall_result = true;

    printf("\n----------------------------------------\n");
    printf("Running Initialization Function Tests\n");
    printf("----------------------------------------\n");

    overall_result &= d_tests_sa_array_common_init_sized(_test_info);
    overall_result &= d_tests_sa_array_common_init_from_array(_test_info);
    overall_result &= d_tests_sa_array_common_init_from_args(_test_info);
    overall_result &= d_tests_sa_array_common_init_copy(_test_info);
    overall_result &= d_tests_sa_array_common_init_copy_reverse(_test_info);
    overall_result &= d_tests_sa_array_common_init_copy_range(_test_info);
    overall_result &= d_tests_sa_array_common_init_copy_range_reverse(_test_info);
    overall_result &= d_tests_sa_array_common_init_fill(_test_info);
    overall_result &= d_tests_sa_array_common_init_slice(_test_info);
    overall_result &= d_tests_sa_array_common_init_slice_reverse(_test_info);
    overall_result &= d_tests_sa_array_common_init_slice_range(_test_info);
    overall_result &= d_tests_sa_array_common_init_slice_range_reverse(_test_info);

    return overall_result;
}
