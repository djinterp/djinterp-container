#include ".\vector_common_tests_sa.h"


/*
d_tests_sa_vector_common_append
  Tests the d_vector_common_append function for appending multiple elements.
  Tests the following:
  - NULL parameter handling
  - zero element_size rejection
  - zero source_count (no-op success)
  - NULL source with non-zero count failure
  - successful append to empty vector
  - successful append to non-empty vector
  - automatic capacity growth
*/
bool
d_tests_sa_vector_common_append
(
    struct d_test_counter* _counter
)
{
    bool   result;
    void*  elements;
    size_t count;
    size_t capacity;
    int    source[3] = {100, 200, 300};

    result = true;

    // test 1: NULL elements pointer should fail
    count    = 0;
    capacity = 10;
    result   = d_assert_standalone(
        d_vector_common_append(NULL,
                               &count,
                               &capacity,
                               sizeof(int),
                               source,
                               3) == D_FAILURE,
        "append_null_elements",
        "NULL elements pointer should return D_FAILURE",
        _counter) && result;

    // test 2: NULL count pointer should fail
    elements = malloc(10 * sizeof(int));
    capacity = 10;
    result   = d_assert_standalone(
        d_vector_common_append(&elements,
                               NULL,
                               &capacity,
                               sizeof(int),
                               source,
                               3) == D_FAILURE,
        "append_null_count",
        "NULL count pointer should return D_FAILURE",
        _counter) && result;

    free(elements);

    // test 3: NULL capacity pointer should fail
    elements = malloc(10 * sizeof(int));
    count    = 0;
    result   = d_assert_standalone(
        d_vector_common_append(&elements,
                               &count,
                               NULL,
                               sizeof(int),
                               source,
                               3) == D_FAILURE,
        "append_null_capacity",
        "NULL capacity pointer should return D_FAILURE",
        _counter) && result;

    free(elements);

    // test 4: zero element_size should fail
    elements = malloc(10 * sizeof(int));
    count    = 0;
    capacity = 10;
    result   = d_assert_standalone(
        d_vector_common_append(&elements,
                               &count,
                               &capacity,
                               0,
                               source,
                               3) == D_FAILURE,
        "append_zero_element_size",
        "Zero element_size should return D_FAILURE",
        _counter) && result;

    free(elements);

    // test 5: zero source_count is no-op success
    elements = NULL;
    count    = 0;
    capacity = 0;
    d_vector_common_init(&elements, &count, &capacity, sizeof(int), 10);

    result = d_assert_standalone(
        d_vector_common_append(&elements,
                               &count,
                               &capacity,
                               sizeof(int),
                               source,
                               0) == D_SUCCESS,
        "append_zero_count",
        "Zero source_count should return D_SUCCESS (no-op)",
        _counter) && result;

    result = d_assert_standalone(
        count == 0,
        "append_zero_count_unchanged",
        "Count should remain unchanged",
        _counter) && result;

    // test 6: NULL source with non-zero count should fail
    result = d_assert_standalone(
        d_vector_common_append(&elements,
                               &count,
                               &capacity,
                               sizeof(int),
                               NULL,
                               3) == D_FAILURE,
        "append_null_source",
        "NULL source with non-zero count should return D_FAILURE",
        _counter) && result;

    // test 7: successful append to empty vector
    result = d_assert_standalone(
        d_vector_common_append(&elements,
                               &count,
                               &capacity,
                               sizeof(int),
                               source,
                               3) == D_SUCCESS,
        "append_empty_success",
        "Append to empty vector should return D_SUCCESS",
        _counter) && result;

    result = d_assert_standalone(
        count == 3,
        "append_empty_count",
        "Count should be 3",
        _counter) && result;

    {
        int* arr     = (int*)elements;
        bool correct = (arr[0] == 100) && (arr[1] == 200) && (arr[2] == 300);

        result = d_assert_standalone(
            correct,
            "append_empty_values",
            "Elements should be [100, 200, 300]",
            _counter) && result;
    }

    // test 8: successful append to non-empty vector
    int more_source[2] = {400, 500};
    result             = d_assert_standalone(
        d_vector_common_append(&elements,
                               &count,
                               &capacity,
                               sizeof(int),
                               more_source,
                               2) == D_SUCCESS,
        "append_nonempty_success",
        "Append to non-empty vector should return D_SUCCESS",
        _counter) && result;

    result = d_assert_standalone(
        count == 5,
        "append_nonempty_count",
        "Count should be 5",
        _counter) && result;

    {
        int* arr = (int*)elements;
        bool correct = (arr[0] == 100) && (arr[1] == 200) && (arr[2] == 300) &&
                       (arr[3] == 400) && (arr[4] == 500);

        result = d_assert_standalone(
            correct,
            "append_nonempty_values",
            "Elements should be [100, 200, 300, 400, 500]",
            _counter) && result;
    }

    if (elements)
    {
        free(elements);
    }

    // test 9: automatic capacity growth
    elements = NULL;
    count    = 0;
    capacity = 0;
    d_vector_common_init(&elements, &count, &capacity, sizeof(int), 2);

    int large_source[10];
    size_t i;
    for (i = 0; i < 10; i++)
    {
        large_source[i] = (int)(i * 10);
    }

    result = d_assert_standalone(
        d_vector_common_append(&elements,
                               &count,
                               &capacity,
                               sizeof(int),
                               large_source,
                               10) == D_SUCCESS,
        "append_grow_success",
        "Append beyond capacity should succeed (with growth)",
        _counter) && result;

    result = d_assert_standalone(
        count == 10,
        "append_grow_count",
        "Count should be 10",
        _counter) && result;

    result = d_assert_standalone(
        capacity >= 10,
        "append_grow_capacity",
        "Capacity should be at least 10",
        _counter) && result;

    if (elements)
    {
        free(elements);
    }

    return result;
}


/*
d_tests_sa_vector_common_prepend
  Tests the d_vector_common_prepend function for prepending multiple elements.
  Tests the following:
  - NULL parameter handling
  - zero source_count (no-op success)
  - NULL source with non-zero count failure
  - successful prepend to empty vector
  - successful prepend to non-empty vector
  - elements correctly shifted
*/
bool
d_tests_sa_vector_common_prepend
(
    struct d_test_counter* _counter
)
{
    bool   result;
    void*  elements;
    size_t count;
    size_t capacity;
    int    source[3] = {10, 20, 30};

    result = true;

    // test 1: NULL elements pointer should fail
    count    = 0;
    capacity = 10;
    result   = d_assert_standalone(
        d_vector_common_prepend(NULL,
                                &count,
                                &capacity,
                                sizeof(int),
                                source,
                                3) == D_FAILURE,
        "prepend_null_elements",
        "NULL elements pointer should return D_FAILURE",
        _counter) && result;

    // test 2: NULL count pointer should fail
    elements = malloc(10 * sizeof(int));
    capacity = 10;
    result   = d_assert_standalone(
        d_vector_common_prepend(&elements,
                                NULL,
                                &capacity,
                                sizeof(int),
                                source,
                                3) == D_FAILURE,
        "prepend_null_count",
        "NULL count pointer should return D_FAILURE",
        _counter) && result;

    free(elements);

    // test 3: zero source_count is no-op success
    elements = NULL;
    count    = 0;
    capacity = 0;
    d_vector_common_init(&elements, &count, &capacity, sizeof(int), 10);

    result = d_assert_standalone(
        d_vector_common_prepend(&elements,
                                &count,
                                &capacity,
                                sizeof(int),
                                source,
                                0) == D_SUCCESS,
        "prepend_zero_count",
        "Zero source_count should return D_SUCCESS (no-op)",
        _counter) && result;

    // test 4: NULL source with non-zero count should fail
    result = d_assert_standalone(
        d_vector_common_prepend(&elements,
                                &count,
                                &capacity,
                                sizeof(int),
                                NULL,
                                3) == D_FAILURE,
        "prepend_null_source",
        "NULL source with non-zero count should fail",
        _counter) && result;

    // test 5: successful prepend to empty vector
    result = d_assert_standalone(
        d_vector_common_prepend(&elements,
                                &count,
                                &capacity,
                                sizeof(int),
                                source,
                                3) == D_SUCCESS,
        "prepend_empty_success",
        "Prepend to empty vector should return D_SUCCESS",
        _counter) && result;

    result = d_assert_standalone(
        count == 3,
        "prepend_empty_count",
        "Count should be 3",
        _counter) && result;

    {
        int* arr     = (int*)elements;
        bool correct = (arr[0] == 10) && (arr[1] == 20) && (arr[2] == 30);

        result = d_assert_standalone(
            correct,
            "prepend_empty_values",
            "Elements should be [10, 20, 30]",
            _counter) && result;
    }

    // test 6: successful prepend to non-empty vector (shifts existing)
    int prepend_source[2] = {1, 2};
    result                = d_assert_standalone(
        d_vector_common_prepend(&elements,
                                &count,
                                &capacity,
                                sizeof(int),
                                prepend_source,
                                2) == D_SUCCESS,
        "prepend_nonempty_success",
        "Prepend to non-empty vector should return D_SUCCESS",
        _counter) && result;

    result = d_assert_standalone(
        count == 5,
        "prepend_nonempty_count",
        "Count should be 5",
        _counter) && result;

    {
        int* arr = (int*)elements;
        bool correct = (arr[0] == 1) && (arr[1] == 2) && (arr[2] == 10) &&
                       (arr[3] == 20) && (arr[4] == 30);

        result = d_assert_standalone(
            correct,
            "prepend_nonempty_values",
            "Elements should be [1, 2, 10, 20, 30]",
            _counter) && result;
    }

    if (elements)
    {
        free(elements);
    }

    return result;
}


/*
d_tests_sa_vector_common_append_all
  Aggregation function that runs all append/extend tests.
*/
bool
d_tests_sa_vector_common_append_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Append/Extend Functions\n");
    printf("  ----------------------------------\n");

    result = d_tests_sa_vector_common_append(_counter) && result;
    result = d_tests_sa_vector_common_prepend(_counter) && result;

    return result;
}
