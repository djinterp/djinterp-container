/******************************************************************************
* djinterp [container]                                        array_tests_sa.c
*
* Comprehensive test runner for `array.h` unit tests.
* Contains helper functions and the main test suite runners.
*
*
* path:      \test\container\array\array_tests_sa.c
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.01.30
******************************************************************************/

#include ".\array_tests_sa.h"


/******************************************************************************
 * HELPER FUNCTIONS
 *****************************************************************************/

 /*
 d_test_int_comparator
   Comparator function for integers used in sorting/search tests.

 Parameter(s):
   _a: pointer to first integer
   _b: pointer to second integer
 Return:
   negative if a < b, zero if a == b, positive if a > b
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

    if (ia < ib)
    {
        return -1;
    }

    if (ia > ib)
    {
        return 1;
    }

    return 0;
}

/*
d_test_arrays_equal_int
  Helper function to compare two integer arrays for equality.

Parameter(s):
  _arr1:  pointer to first array
  _arr2:  pointer to second array
  _count: number of elements to compare
Return:
  true if arrays are equal, false otherwise
*/
bool
d_test_arrays_equal_int
(
    const void* _arr1,
    const void* _arr2,
    size_t      _count
)
{
    const int* a1 = (const int*)_arr1;
    const int* a2 = (const int*)_arr2;
    size_t     i;

    if (!a1 || !a2)
    {
        return (a1 == a2);
    }

    for (i = 0; i < _count; i++)
    {
        if (a1[i] != a2[i])
        {
            return false;
        }
    }

    return true;
}

/*
d_test_create_test_data_int
  Creates test data array of integers [1, 2, 3, ..., _count].

Parameter(s):
  _data:  pointer to receive allocated data
  _count: number of elements to create
Return:
  none.
*/
void
d_test_create_test_data_int
(
    int** _data,
    size_t _count
)
{
    size_t i;

    *_data = malloc(_count * sizeof(int));

    if (*_data)
    {
        for (i = 0; i < _count; i++)
        {
            (*_data)[i] = (int)(i + 1);
        }
    }

    return;
}


/******************************************************************************
 * MEMORY MANAGEMENT TESTS
 *****************************************************************************/

 /*
 d_tests_sa_array_free
   Tests the `d_array_free` function.
   Tests the following:
   - freeing allocated array
   - handling NULL safely
 */
bool
d_tests_sa_array_free
(
    struct d_test_counter* _test_info
)
{
    bool            test_result = true;
    struct d_array* arr;

    printf("Testing `d_array_free`...\n");

    // test freeing allocated array
    arr = d_array_new(sizeof(int), 10);

    if (arr)
    {
        d_array_free(arr);

        test_result &= d_assert_standalone(
            true,
            "`d_array_free` successfully frees array",
            "`d_array_free` failed",
            _test_info
        );
    }

    // test handling NULL safely
    d_array_free(NULL);

    test_result &= d_assert_standalone(
        true,
        "`d_array_free` handles NULL safely",
        "`d_array_free` crashed on NULL",
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
d_tests_sa_array_s_free
  Tests the `d_array_s_free` function.
  Tests the following:
  - freeing allocated array
  - handling NULL safely
*/
bool
d_tests_sa_array_s_free
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array_s* arr;

    printf("Testing `d_array_s_free`...\n");

    // test freeing allocated array
    arr = d_array_s_new(sizeof(int), 10);

    if (arr)
    {
        d_array_s_free(arr);

        test_result &= d_assert_standalone(
            true,
            "`d_array_s_free` successfully frees array",
            "`d_array_s_free` failed",
            _test_info
        );
    }

    // test handling NULL safely
    d_array_s_free(NULL);

    test_result &= d_assert_standalone(
        true,
        "`d_array_s_free` handles NULL safely",
        "`d_array_s_free` crashed on NULL",
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
d_tests_sa_array_run_memory_tests
  Runs all memory management tests.
*/
bool
d_tests_sa_array_run_memory_tests
(
    struct d_test_counter* _test_info
)
{
    bool overall_result = true;

    printf("\n----------------------------------------\n");
    printf("Running Memory Management Tests\n");
    printf("----------------------------------------\n");

    overall_result &= d_tests_sa_array_free(_test_info);
    overall_result &= d_tests_sa_array_s_free(_test_info);

    return overall_result;
}


/******************************************************************************
 * INTEGRATION AND EDGE CASE TESTS
 *****************************************************************************/

 /*
 d_tests_sa_array_null_parameter_safety
   Tests NULL parameter handling across multiple functions.
   Tests the following:
   - all creation functions reject NULL where appropriate
   - all manipulation functions reject NULL arrays
 */
bool
d_tests_sa_array_null_parameter_safety
(
    struct d_test_counter* _test_info
)
{
    bool            test_result = true;
    struct d_array* arr;
    int             value = 42;

    printf("Testing NULL parameter safety...\n");

    // test d_array_new_copy with NULL
    arr = d_array_new_copy(NULL, sizeof(int));

    test_result &= d_assert_standalone(
        arr == NULL,
        "d_array_new_copy rejects NULL source",
        "d_array_new_copy accepted NULL source",
        _test_info
    );

    // test d_array_append_element with NULL array
    test_result &= d_assert_standalone(
        d_array_append_element(NULL, sizeof(int), &value) == false,
        "d_array_append_element rejects NULL array",
        "d_array_append_element accepted NULL array",
        _test_info
    );

    // test d_array_contains with NULL array
    test_result &= d_assert_standalone(
        d_array_contains(NULL, sizeof(int), &value) == false,
        "d_array_contains rejects NULL array",
        "d_array_contains accepted NULL array",
        _test_info
    );

    // test d_array_find with NULL array
    test_result &= d_assert_standalone(
        d_array_find(NULL, sizeof(int), &value) == -1,
        "d_array_find returns -1 for NULL array",
        "d_array_find did not return -1 for NULL array",
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
d_tests_sa_array_zero_element_size_safety
  Tests zero element size handling across multiple functions.
  Tests the following:
  - all creation functions reject zero element size
  - all manipulation functions reject zero element size
*/
bool
d_tests_sa_array_zero_element_size_safety
(
    struct d_test_counter* _test_info
)
{
    bool            test_result = true;
    struct d_array* arr;
    struct d_array* test_arr;
    int             data[] = { 1, 2, 3, 4, 5 };
    int             value = 42;

    printf("Testing zero element size safety...\n");

    test_arr = d_array_new_from_arr(sizeof(int), data, 5);

    if (!test_arr)
    {
        _test_info->tests_total++;

        return false;
    }

    // test d_array_new with zero element size
    arr = d_array_new(0, 10);

    test_result &= d_assert_standalone(
        arr == NULL,
        "d_array_new rejects zero element size",
        "d_array_new accepted zero element size",
        _test_info
    );

    // test d_array_new_from_arr with zero element size
    arr = d_array_new_from_arr(0, data, 5);

    test_result &= d_assert_standalone(
        arr == NULL,
        "d_array_new_from_arr rejects zero element size",
        "d_array_new_from_arr accepted zero element size",
        _test_info
    );

    // test d_array_append_element with zero element size
    test_result &= d_assert_standalone(
        d_array_append_element(test_arr, 0, &value) == false,
        "d_array_append_element rejects zero element size",
        "d_array_append_element accepted zero element size",
        _test_info
    );

    // test d_array_contains with zero element size
    test_result &= d_assert_standalone(
        d_array_contains(test_arr, 0, &value) == false,
        "d_array_contains rejects zero element size",
        "d_array_contains accepted zero element size",
        _test_info
    );

    d_array_free(test_arr);

    _test_info->tests_total++;
    if (test_result)
    {
        _test_info->tests_passed++;
    }

    return test_result;
}

/*
d_tests_sa_array_negative_indexing
  Tests negative index support in various functions.
  Tests the following:
  - copy_range with negative indices
  - slice with negative indices
*/
bool
d_tests_sa_array_negative_indexing
(
    struct d_test_counter* _test_info
)
{
    bool            test_result = true;
    struct d_array* arr;
    struct d_array* source;
    int* int_elements;

    printf("Testing negative indexing...\n");

    source = d_array_new_from_arr(sizeof(int), (int[]) { 1, 2, 3, 4, 5 }, 5);

    if (!source)
    {
        _test_info->tests_total++;

        return false;
    }

    // test d_array_new_copy_range with negative indices
    // -3 to -1 -> indices 2 to 4 -> [3, 4, 5]
    arr = d_array_new_copy_range(source, sizeof(int), -3, -1);
    int_elements = arr ? (int*)arr->elements : NULL;

    test_result &= d_assert_standalone(
        arr != NULL && arr->count == 3 &&
        int_elements[0] == 3 && int_elements[2] == 5,
        "d_array_new_copy_range handles negative indices correctly",
        "d_array_new_copy_range failed with negative indices",
        _test_info
    );

    if (arr)
    {
        d_array_free(arr);
    }

    // test d_array_new_copy_reverse with negative indices
    arr = d_array_new_copy_reverse(source, sizeof(int), -3, -1);
    int_elements = arr ? (int*)arr->elements : NULL;

    test_result &= d_assert_standalone(
        arr != NULL && arr->count == 3 &&
        int_elements[0] == 5 && int_elements[2] == 3,
        "d_array_new_copy_reverse handles negative indices correctly",
        "d_array_new_copy_reverse failed with negative indices",
        _test_info
    );

    if (arr)
    {
        d_array_free(arr);
    }

    d_array_free(source);

    _test_info->tests_total++;
    if (test_result)
    {
        _test_info->tests_passed++;
    }

    return test_result;
}

/*
d_tests_sa_array_large_data_sets
  Tests handling of large data sets.
  Tests the following:
  - creating large arrays
  - operations on large arrays
*/
bool
d_tests_sa_array_large_data_sets
(
    struct d_test_counter* _test_info
)
{
    bool            test_result = true;
    struct d_array* arr;
    const size_t    large_size = 10000;

    printf("Testing large data sets...\n");

    // test creating large array
    arr = d_array_new(sizeof(int), large_size);

    test_result &= d_assert_standalone(
        arr != NULL && arr->elements != NULL,
        "d_array_new handles large size",
        "d_array_new failed with large size",
        _test_info
    );

    if (arr)
    {
        // test filling large array
        int fill_value = 42;
        bool fill_result = d_array_fill(arr, sizeof(int), &fill_value);

        test_result &= d_assert_standalone(
            fill_result == true,
            "d_array_fill handles large array",
            "d_array_fill failed with large array",
            _test_info
        );

        d_array_free(arr);
    }

    _test_info->tests_total++;
    if (test_result)
    {
        _test_info->tests_passed++;
    }

    return test_result;
}

/*
d_tests_sa_array_run_integration_tests
  Runs all integration and edge case tests.
*/
bool
d_tests_sa_array_run_integration_tests
(
    struct d_test_counter* _test_info
)
{
    bool overall_result = true;

    printf("\n----------------------------------------\n");
    printf("Running Integration and Edge Case Tests\n");
    printf("----------------------------------------\n");

    overall_result &= d_tests_sa_array_null_parameter_safety(_test_info);
    overall_result &= d_tests_sa_array_zero_element_size_safety(_test_info);
    overall_result &= d_tests_sa_array_negative_indexing(_test_info);
    overall_result &= d_tests_sa_array_large_data_sets(_test_info);

    return overall_result;
}


/******************************************************************************
 * COMPREHENSIVE TEST SUITE RUNNERS
 *****************************************************************************/

 /*
 d_tests_sa_array_s_new
   Tests the `d_array_s_new` function.
 */
bool
d_tests_sa_array_s_new
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array_s* arr;

    printf("Testing `d_array_s_new`...\n");

    arr = d_array_s_new(sizeof(int), 10);

    test_result &= d_assert_standalone(
        arr != NULL && arr->count == 0 && arr->element_size == sizeof(int),
        "`d_array_s_new` creates array with correct fields",
        "`d_array_s_new` failed",
        _test_info
    );

    if (arr) d_array_s_free(arr);

    arr = d_array_s_new(0, 10);

    test_result &= d_assert_standalone(
        arr == NULL,
        "`d_array_s_new` rejects zero element size",
        "`d_array_s_new` accepted zero element size",
        _test_info
    );

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_s_new_default_size
  Tests the `d_array_s_new_default_size` function.
*/
bool
d_tests_sa_array_s_new_default_size
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array_s* arr;

    printf("Testing `d_array_s_new_default_size`...\n");

    arr = d_array_s_new_default_size(sizeof(int));

    test_result &= d_assert_standalone(
        arr != NULL && arr->element_size == sizeof(int),
        "`d_array_s_new_default_size` creates array correctly",
        "`d_array_s_new_default_size` failed",
        _test_info
    );

    if (arr) d_array_s_free(arr);

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_s_new_from_arr
  Tests the `d_array_s_new_from_arr` function.
*/
bool
d_tests_sa_array_s_new_from_arr
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array_s* arr;
    int               data[] = { 1, 2, 3, 4, 5 };

    printf("Testing `d_array_s_new_from_arr`...\n");

    arr = d_array_s_new_from_arr(sizeof(int), data, 5);

    test_result &= d_assert_standalone(
        arr != NULL && arr->count == 5 && arr->element_size == sizeof(int),
        "`d_array_s_new_from_arr` creates array correctly",
        "`d_array_s_new_from_arr` failed",
        _test_info
    );

    if (arr) d_array_s_free(arr);

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_s_new_from_args
  Tests the `d_array_s_new_from_args` function.
*/
bool
d_tests_sa_array_s_new_from_args
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array_s* arr;

    printf("Testing `d_array_s_new_from_args`...\n");

    arr = d_array_s_new_from_args(sizeof(int), 5, 1, 2, 3, 4, 5);

    test_result &= d_assert_standalone(
        arr != NULL && arr->count == 5 && arr->element_size == sizeof(int),
        "`d_array_s_new_from_args` creates array correctly",
        "`d_array_s_new_from_args` failed",
        _test_info
    );

    if (arr) d_array_s_free(arr);

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_s_new_copy
  Tests the `d_array_s_new_copy` function.
*/
bool
d_tests_sa_array_s_new_copy
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array    original = { 5, NULL };
    struct d_array_s* copy;
    int               data[] = { 1, 2, 3, 4, 5 };

    printf("Testing `d_array_s_new_copy`...\n");

    original.elements = data;

    copy = d_array_s_new_copy(&original, sizeof(int));

    test_result &= d_assert_standalone(
        copy != NULL && copy->count == 5 && copy->element_size == sizeof(int),
        "`d_array_s_new_copy` creates copy correctly",
        "`d_array_s_new_copy` failed",
        _test_info
    );

    if (copy) d_array_s_free(copy);

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_s_new_copy_reverse
  Tests the `d_array_s_new_copy_reverse` function.
*/
bool
d_tests_sa_array_s_new_copy_reverse
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array    original = { 5, NULL };
    struct d_array_s* copy;
    int               data[] = { 1, 2, 3, 4, 5 };
    int* int_elements;

    printf("Testing `d_array_s_new_copy_reverse`...\n");

    original.elements = data;

    copy = d_array_s_new_copy_reverse(&original, sizeof(int), 0, 4);
    int_elements = copy ? (int*)copy->elements : NULL;

    test_result &= d_assert_standalone(
        copy != NULL && int_elements[0] == 5 && int_elements[4] == 1,
        "`d_array_s_new_copy_reverse` creates reversed copy",
        "`d_array_s_new_copy_reverse` failed",
        _test_info
    );

    if (copy) d_array_s_free(copy);

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_s_new_copy_range
  Tests the `d_array_s_new_copy_range` function.
*/
bool
d_tests_sa_array_s_new_copy_range
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array    original = { 5, NULL };
    struct d_array_s* copy;
    int               data[] = { 1, 2, 3, 4, 5 };
    int* int_elements;

    printf("Testing `d_array_s_new_copy_range`...\n");

    original.elements = data;

    copy = d_array_s_new_copy_range(&original, sizeof(int), 1, 3);
    int_elements = copy ? (int*)copy->elements : NULL;

    test_result &= d_assert_standalone(
        copy != NULL && copy->count == 3 && int_elements[0] == 2,
        "`d_array_s_new_copy_range` creates range copy",
        "`d_array_s_new_copy_range` failed",
        _test_info
    );

    if (copy) d_array_s_free(copy);

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_s_new_copy_range_reverse
  Tests the `d_array_s_new_copy_range_reverse` function.
*/
bool
d_tests_sa_array_s_new_copy_range_reverse
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array    original = { 5, NULL };
    struct d_array_s* copy;
    int               data[] = { 1, 2, 3, 4, 5 };
    int* int_elements;

    printf("Testing `d_array_s_new_copy_range_reverse`...\n");

    original.elements = data;

    copy = d_array_s_new_copy_range_reverse(&original, sizeof(int), 1, 3);
    int_elements = copy ? (int*)copy->elements : NULL;

    test_result &= d_assert_standalone(
        copy != NULL && copy->count == 3 && int_elements[0] == 4,
        "`d_array_s_new_copy_range_reverse` creates reversed range",
        "`d_array_s_new_copy_range_reverse` failed",
        _test_info
    );

    if (copy) d_array_s_free(copy);

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_s_new_fill
  Tests the `d_array_s_new_fill` function.
*/
bool
d_tests_sa_array_s_new_fill
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array_s* arr;
    int               fill_value = 42;
    int* int_elements;

    printf("Testing `d_array_s_new_fill`...\n");

    arr = d_array_s_new_fill(sizeof(int), 5, &fill_value);
    int_elements = arr ? (int*)arr->elements : NULL;

    test_result &= d_assert_standalone(
        arr != NULL && arr->count == 5 && int_elements[0] == 42,
        "`d_array_s_new_fill` creates filled array",
        "`d_array_s_new_fill` failed",
        _test_info
    );

    if (arr) d_array_s_free(arr);

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_s_new_merge
  Tests the `d_array_s_new_merge` function.
*/
bool
d_tests_sa_array_s_new_merge
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array_s* merged;
    struct d_array    arr1 = { 3, NULL };
    struct d_array    arr2 = { 2, NULL };
    int               data1[] = { 1, 2, 3 };
    int               data2[] = { 4, 5 };

    printf("Testing `d_array_s_new_merge`...\n");

    arr1.elements = data1;
    arr2.elements = data2;

    merged = d_array_s_new_merge(sizeof(int), 2, &arr1, &arr2);

    test_result &= d_assert_standalone(
        merged != NULL && merged->count == 5,
        "`d_array_s_new_merge` creates merged array",
        "`d_array_s_new_merge` failed",
        _test_info
    );

    if (merged) d_array_s_free(merged);

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_s_new_slice
  Tests the `d_array_s_new_slice` function.
*/
bool
d_tests_sa_array_s_new_slice
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array    original = { 5, NULL };
    struct d_array_s* slice;
    int               data[] = { 1, 2, 3, 4, 5 };

    printf("Testing `d_array_s_new_slice`...\n");

    original.elements = data;

    slice = d_array_s_new_slice(&original, sizeof(int), 2);

    test_result &= d_assert_standalone(
        slice != NULL && slice->element_size == sizeof(int),
        "`d_array_s_new_slice` creates slice",
        "`d_array_s_new_slice` failed",
        _test_info
    );

    if (slice) d_array_s_free(slice);

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_s_new_slice_reverse
  Tests the `d_array_s_new_slice_reverse` function.
*/
bool
d_tests_sa_array_s_new_slice_reverse
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array    original = { 5, NULL };
    struct d_array_s* slice;
    int               data[] = { 1, 2, 3, 4, 5 };

    printf("Testing `d_array_s_new_slice_reverse`...\n");

    original.elements = data;

    slice = d_array_s_new_slice_reverse(&original, sizeof(int), 0);

    test_result &= d_assert_standalone(
        slice != NULL,
        "`d_array_s_new_slice_reverse` creates reversed slice",
        "`d_array_s_new_slice_reverse` failed",
        _test_info
    );

    if (slice) d_array_s_free(slice);

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_s_new_slice_range
  Tests the `d_array_s_new_slice_range` function.
*/
bool
d_tests_sa_array_s_new_slice_range
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array    original = { 5, NULL };
    struct d_array_s* slice;
    int               data[] = { 1, 2, 3, 4, 5 };

    printf("Testing `d_array_s_new_slice_range`...\n");

    original.elements = data;

    slice = d_array_s_new_slice_range(&original, sizeof(int), 1, 3);

    test_result &= d_assert_standalone(
        slice != NULL,
        "`d_array_s_new_slice_range` creates range slice",
        "`d_array_s_new_slice_range` failed",
        _test_info
    );

    if (slice) d_array_s_free(slice);

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_s_new_slice_range_reverse
  Tests the `d_array_s_new_slice_range_reverse` function.
*/
bool
d_tests_sa_array_s_new_slice_range_reverse
(
    struct d_test_counter* _test_info
)
{
    bool              test_result = true;
    struct d_array    original = { 5, NULL };
    struct d_array_s* slice;
    int               data[] = { 1, 2, 3, 4, 5 };

    printf("Testing `d_array_s_new_slice_range_reverse`...\n");

    original.elements = data;

    slice = d_array_s_new_slice_range_reverse(&original, sizeof(int), 1, 3);

    test_result &= d_assert_standalone(
        slice != NULL,
        "`d_array_s_new_slice_range_reverse` creates reversed range slice",
        "`d_array_s_new_slice_range_reverse` failed",
        _test_info
    );

    if (slice) d_array_s_free(slice);

    _test_info->tests_total++;
    if (test_result) _test_info->tests_passed++;

    return test_result;
}

/*
d_tests_sa_array_run_creation_s_tests
  Runs all d_array_s creation function tests.
*/
bool
d_tests_sa_array_run_creation_s_tests
(
    struct d_test_counter* _test_info
)
{
    bool overall_result = true;

    printf("\n----------------------------------------\n");
    printf("Running d_array_s Creation Function Tests\n");
    printf("----------------------------------------\n");

    overall_result &= d_tests_sa_array_s_new(_test_info);
    overall_result &= d_tests_sa_array_s_new_default_size(_test_info);
    overall_result &= d_tests_sa_array_s_new_from_arr(_test_info);
    overall_result &= d_tests_sa_array_s_new_from_args(_test_info);
    overall_result &= d_tests_sa_array_s_new_copy(_test_info);
    overall_result &= d_tests_sa_array_s_new_copy_reverse(_test_info);
    overall_result &= d_tests_sa_array_s_new_copy_range(_test_info);
    overall_result &= d_tests_sa_array_s_new_copy_range_reverse(_test_info);
    overall_result &= d_tests_sa_array_s_new_fill(_test_info);
    overall_result &= d_tests_sa_array_s_new_merge(_test_info);
    overall_result &= d_tests_sa_array_s_new_slice(_test_info);
    overall_result &= d_tests_sa_array_s_new_slice_reverse(_test_info);
    overall_result &= d_tests_sa_array_s_new_slice_range(_test_info);
    overall_result &= d_tests_sa_array_s_new_slice_range_reverse(_test_info);

    return overall_result;
}

/*
d_tests_sa_array_all
  Runs all core array tests.
*/
bool
d_tests_sa_array_all
(
    struct d_test_counter* _test_info
)
{
    bool                   overall_result = true;
    struct d_test_counter  local_counter = { 0, 0, 0, 0 };

    printf("\n========================================\n");
    printf("Running Core Array Module Tests\n");
    printf("========================================\n");

    // run d_array creation function tests
    overall_result &= d_tests_sa_array_run_creation_tests(&local_counter);

    // run d_array_s creation function tests
    overall_result &= d_tests_sa_array_run_creation_s_tests(&local_counter);

    // run manipulation function tests
    overall_result &= d_tests_sa_array_run_manipulation_tests(&local_counter);

    // run memory management tests
    overall_result &= d_tests_sa_array_run_memory_tests(&local_counter);

    // update main counter
    _test_info->assertions_total += local_counter.assertions_total;
    _test_info->assertions_passed += local_counter.assertions_passed;
    _test_info->tests_total += local_counter.tests_total;
    _test_info->tests_passed += local_counter.tests_passed;

    // print results
    printf("\n========================================\n");
    printf("Core Array Test Results\n");
    printf("========================================\n");
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
d_tests_sa_array_all_extended
  Runs all array tests including integration and edge cases.
*/
bool
d_tests_sa_array_all_extended
(
    struct d_test_counter* _test_info
)
{
    bool                   overall_result = true;
    struct d_test_counter  local_counter = { 0, 0, 0, 0 };

    printf("\n========================================\n");
    printf("Running COMPLETE Array Module Test Suite\n");
    printf("========================================\n");

    // run all core tests
    overall_result &= d_tests_sa_array_run_creation_tests(&local_counter);
    overall_result &= d_tests_sa_array_run_creation_s_tests(&local_counter);
    overall_result &= d_tests_sa_array_run_manipulation_tests(&local_counter);
    overall_result &= d_tests_sa_array_run_memory_tests(&local_counter);

    // run integration tests
    overall_result &= d_tests_sa_array_run_integration_tests(&local_counter);

    // update main counter
    _test_info->assertions_total += local_counter.assertions_total;
    _test_info->assertions_passed += local_counter.assertions_passed;
    _test_info->tests_total += local_counter.tests_total;
    _test_info->tests_passed += local_counter.tests_passed;

    // print comprehensive results
    printf("\n========================================\n");
    printf("COMPLETE Array Test Results\n");
    printf("========================================\n");
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
d_tests_sa_array_run_all
  Main entry point for the test module runner.
*/
bool
d_tests_sa_array_run_all
(
    struct d_test_counter* _test_info
)
{
    return d_tests_sa_array_all_extended(_test_info);
}