/******************************************************************************
* djinterp [test]                                ptr_vector_tests_sa_constructor.c
*
*   Unit tests for ptr_vector constructor functions.
*
*
* path:      \tests\container\vector\ptr_vector_tests_sa_constructor.c
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.01.29
******************************************************************************/

#include ".\ptr_vector_tests_sa.h"


/******************************************************************************
 * HELPER DATA FOR TESTS
 *****************************************************************************/

/* Sample integers to use as pointed-to values */
static int g_test_values[] = {100, 200, 300, 400, 500};


/*
d_tests_sa_ptr_vector_new
  Tests the d_ptr_vector_new function for creating vectors with specified
  initial capacity.
  Tests the following:
  - Zero capacity handling (valid, creates empty vector)
  - Successful creation with valid capacity
  - Memory allocation verification
  - Correct count and capacity initialization
*/
bool
d_tests_sa_ptr_vector_new
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;

    result = true;

    /* test 1: zero capacity should succeed and create empty vector */
    vec = d_ptr_vector_new(0);
    result = d_assert_standalone(
        vec != NULL,
        "new_zero_capacity_not_null",
        "Zero capacity should return non-NULL vector",
        _counter) && result;

    if (vec)
    {
        result = d_assert_standalone(
            vec->count == 0,
            "new_zero_capacity_count",
            "Count should be 0 for zero-capacity vector",
            _counter) && result;

        result = d_assert_standalone(
            vec->capacity == 0,
            "new_zero_capacity_capacity",
            "Capacity should be 0 for zero-capacity vector",
            _counter) && result;

        result = d_assert_standalone(
            vec->elements == NULL,
            "new_zero_capacity_elements",
            "Elements should be NULL for zero-capacity vector",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 2: valid capacity should allocate correctly */
    vec = d_ptr_vector_new(10);
    result = d_assert_standalone(
        vec != NULL,
        "new_valid_not_null",
        "Valid capacity should return non-NULL vector",
        _counter) && result;

    if (vec)
    {
        result = d_assert_standalone(
            vec->count == 0,
            "new_valid_count_zero",
            "Count should be 0 (no elements added yet)",
            _counter) && result;

        result = d_assert_standalone(
            vec->capacity == 10,
            "new_valid_capacity",
            "Capacity should match requested initial capacity",
            _counter) && result;

        result = d_assert_standalone(
            vec->elements != NULL,
            "new_valid_elements_allocated",
            "Elements array should be allocated",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 3: large capacity should allocate correctly */
    vec = d_ptr_vector_new(1000);
    result = d_assert_standalone(
        vec != NULL,
        "new_large_capacity_not_null",
        "Large capacity should return non-NULL vector",
        _counter) && result;

    if (vec)
    {
        result = d_assert_standalone(
            vec->capacity == 1000,
            "new_large_capacity_value",
            "Large capacity should be set correctly",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_new_default
  Tests the d_ptr_vector_new_default function for creating vectors with
  default capacity.
  Tests the following:
  - Successful creation
  - Correct default capacity value
  - Count initialization to zero
*/
bool
d_tests_sa_ptr_vector_new_default
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;

    result = true;

    /* test 1: default creation should succeed */
    vec = d_ptr_vector_new_default();
    result = d_assert_standalone(
        vec != NULL,
        "new_default_not_null",
        "Default constructor should return non-NULL vector",
        _counter) && result;

    if (vec)
    {
        result = d_assert_standalone(
            vec->count == 0,
            "new_default_count_zero",
            "Count should be 0 for newly created vector",
            _counter) && result;

        result = d_assert_standalone(
            vec->capacity == D_VECTOR_DEFAULT_CAPACITY,
            "new_default_capacity",
            "Capacity should match D_VECTOR_DEFAULT_CAPACITY",
            _counter) && result;

        result = d_assert_standalone(
            vec->elements != NULL,
            "new_default_elements_allocated",
            "Elements array should be allocated",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_new_from_array
  Tests the d_ptr_vector_new_from_array function for creating vectors
  from an existing array of pointers.
  Tests the following:
  - NULL source with zero count (valid)
  - NULL source with non-zero count (invalid)
  - Zero count handling
  - Successful creation from valid array
  - Correct element copying verification
*/
bool
d_tests_sa_ptr_vector_new_from_array
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;
    void*                source[3];

    result = true;

    /* Setup source array with pointers to test values */
    source[0] = &g_test_values[0];
    source[1] = &g_test_values[1];
    source[2] = &g_test_values[2];

    /* test 1: NULL source with non-zero count should fail */
    vec = d_ptr_vector_new_from_array(NULL, 5);
    result = d_assert_standalone(
        vec == NULL,
        "new_from_array_null_source_nonzero",
        "NULL source with non-zero count should return NULL",
        _counter) && result;

    /* test 2: NULL source with zero count should succeed */
    vec = d_ptr_vector_new_from_array(NULL, 0);
    result = d_assert_standalone(
        vec != NULL,
        "new_from_array_null_source_zero",
        "NULL source with zero count should succeed",
        _counter) && result;

    if (vec)
    {
        result = d_assert_standalone(
            vec->count == 0,
            "new_from_array_null_zero_count",
            "Count should be 0",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 3: valid source with zero count */
    vec = d_ptr_vector_new_from_array((const void**)source, 0);
    result = d_assert_standalone(
        vec != NULL,
        "new_from_array_zero_count",
        "Zero count should return valid vector",
        _counter) && result;

    if (vec)
    {
        result = d_assert_standalone(
            vec->count == 0,
            "new_from_array_zero_count_value",
            "Count should be 0",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 4: successful creation from valid array */
    vec = d_ptr_vector_new_from_array((const void**)source, 3);
    result = d_assert_standalone(
        vec != NULL,
        "new_from_array_valid_not_null",
        "Valid array should create non-NULL vector",
        _counter) && result;

    if (vec)
    {
        result = d_assert_standalone(
            vec->count == 3,
            "new_from_array_valid_count",
            "Count should match source count",
            _counter) && result;

        result = d_assert_standalone(
            vec->capacity >= 3,
            "new_from_array_valid_capacity",
            "Capacity should be at least source count",
            _counter) && result;

        /* Verify pointers were copied correctly */
        result = d_assert_standalone(
            vec->elements[0] == source[0],
            "new_from_array_element_0",
            "Element 0 should match source",
            _counter) && result;

        result = d_assert_standalone(
            vec->elements[1] == source[1],
            "new_from_array_element_1",
            "Element 1 should match source",
            _counter) && result;

        result = d_assert_standalone(
            vec->elements[2] == source[2],
            "new_from_array_element_2",
            "Element 2 should match source",
            _counter) && result;

        /* Verify pointed-to values */
        result = d_assert_standalone(
            *(int*)vec->elements[0] == 100,
            "new_from_array_value_0",
            "Pointed-to value 0 should be 100",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_new_from_args
  Tests the d_ptr_vector_new_from_args function for creating vectors
  from variadic pointer arguments.
  Tests the following:
  - Zero arg count handling
  - Successful creation with variadic arguments
  - Correct element values
*/
bool
d_tests_sa_ptr_vector_new_from_args
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;

    result = true;

    /* test 1: zero arg count should create empty vector */
    vec = d_ptr_vector_new_from_args(0);
    result = d_assert_standalone(
        vec != NULL,
        "new_from_args_zero_not_null",
        "Zero args should return non-NULL vector",
        _counter) && result;

    if (vec)
    {
        result = d_assert_standalone(
            vec->count == 0,
            "new_from_args_zero_count",
            "Count should be 0 for zero args",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 2: successful creation with variadic arguments */
    vec = d_ptr_vector_new_from_args(3,
                                     &g_test_values[0],
                                     &g_test_values[1],
                                     &g_test_values[2]);
    result = d_assert_standalone(
        vec != NULL,
        "new_from_args_valid_not_null",
        "Variadic args should create non-NULL vector",
        _counter) && result;

    if (vec)
    {
        result = d_assert_standalone(
            vec->count == 3,
            "new_from_args_valid_count",
            "Count should match arg count",
            _counter) && result;

        result = d_assert_standalone(
            vec->elements[0] == &g_test_values[0],
            "new_from_args_element_0",
            "Element 0 should match first arg",
            _counter) && result;

        result = d_assert_standalone(
            vec->elements[1] == &g_test_values[1],
            "new_from_args_element_1",
            "Element 1 should match second arg",
            _counter) && result;

        result = d_assert_standalone(
            vec->elements[2] == &g_test_values[2],
            "new_from_args_element_2",
            "Element 2 should match third arg",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 3: single argument */
    vec = d_ptr_vector_new_from_args(1, &g_test_values[4]);
    result = d_assert_standalone(
        vec != NULL && vec->count == 1,
        "new_from_args_single",
        "Single arg should create vector with count 1",
        _counter) && result;

    if (vec)
    {
        result = d_assert_standalone(
            *(int*)vec->elements[0] == 500,
            "new_from_args_single_value",
            "Single element should point to value 500",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_new_copy
  Tests the d_ptr_vector_new_copy function for creating shallow copies.
  Tests the following:
  - NULL source handling
  - Empty vector copying
  - Successful shallow copy
  - Pointers are copied (not deep copied)
*/
bool
d_tests_sa_ptr_vector_new_copy
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* original;
    struct d_ptr_vector* copy;

    result = true;

    /* test 1: NULL source should fail */
    copy = d_ptr_vector_new_copy(NULL);
    result = d_assert_standalone(
        copy == NULL,
        "new_copy_null_source",
        "NULL source should return NULL",
        _counter) && result;

    /* test 2: copy empty vector */
    original = d_ptr_vector_new(5);
    if (original)
    {
        copy = d_ptr_vector_new_copy(original);
        result = d_assert_standalone(
            copy != NULL,
            "new_copy_empty_not_null",
            "Copy of empty vector should succeed",
            _counter) && result;

        if (copy)
        {
            result = d_assert_standalone(
                copy->count == 0,
                "new_copy_empty_count",
                "Copy count should be 0",
                _counter) && result;

            result = d_assert_standalone(
                copy->capacity >= original->capacity,
                "new_copy_empty_capacity",
                "Copy capacity should be at least original capacity",
                _counter) && result;

            d_ptr_vector_free(copy);
        }

        d_ptr_vector_free(original);
    }

    /* test 3: copy non-empty vector (shallow copy verification) */
    original = d_ptr_vector_new_from_args(3,
                                          &g_test_values[0],
                                          &g_test_values[1],
                                          &g_test_values[2]);
    if (original)
    {
        copy = d_ptr_vector_new_copy(original);
        result = d_assert_standalone(
            copy != NULL,
            "new_copy_valid_not_null",
            "Copy of valid vector should succeed",
            _counter) && result;

        if (copy)
        {
            result = d_assert_standalone(
                copy->count == original->count,
                "new_copy_valid_count",
                "Copy count should match original",
                _counter) && result;

            /* Verify shallow copy - pointers should be identical */
            result = d_assert_standalone(
                copy->elements[0] == original->elements[0],
                "new_copy_shallow_0",
                "Shallow copy: element 0 pointers should be same",
                _counter) && result;

            result = d_assert_standalone(
                copy->elements[1] == original->elements[1],
                "new_copy_shallow_1",
                "Shallow copy: element 1 pointers should be same",
                _counter) && result;

            result = d_assert_standalone(
                copy->elements[2] == original->elements[2],
                "new_copy_shallow_2",
                "Shallow copy: element 2 pointers should be same",
                _counter) && result;

            /* But the elements arrays should be different */
            result = d_assert_standalone(
                copy->elements != original->elements,
                "new_copy_different_arrays",
                "Copy should have different elements array",
                _counter) && result;

            d_ptr_vector_free(copy);
        }

        d_ptr_vector_free(original);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_new_fill
  Tests the d_ptr_vector_new_fill function for creating vectors filled
  with a repeated pointer value.
  Tests the following:
  - Zero count handling
  - NULL fill value handling
  - Successful fill creation
  - All elements match fill value
*/
bool
d_tests_sa_ptr_vector_new_fill
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;
    void*                fill_ptr;
    size_t               i;
    bool                 all_match;

    result   = true;
    fill_ptr = &g_test_values[3];  /* Points to value 400 */

    /* test 1: zero count should create empty vector */
    vec = d_ptr_vector_new_fill(0, fill_ptr);
    result = d_assert_standalone(
        vec != NULL,
        "new_fill_zero_count_not_null",
        "Zero count fill should return non-NULL vector",
        _counter) && result;

    if (vec)
    {
        result = d_assert_standalone(
            vec->count == 0,
            "new_fill_zero_count_value",
            "Count should be 0",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 2: NULL fill value should work (fill with NULL) */
    vec = d_ptr_vector_new_fill(5, NULL);
    result = d_assert_standalone(
        vec != NULL,
        "new_fill_null_value_not_null",
        "NULL fill value should succeed",
        _counter) && result;

    if (vec)
    {
        result = d_assert_standalone(
            vec->count == 5,
            "new_fill_null_value_count",
            "Count should be 5",
            _counter) && result;

        all_match = true;
        for (i = 0; i < vec->count; i++)
        {
            if (vec->elements[i] != NULL)
            {
                all_match = false;
                break;
            }
        }

        result = d_assert_standalone(
            all_match,
            "new_fill_null_all_null",
            "All elements should be NULL",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 3: successful fill with valid pointer */
    vec = d_ptr_vector_new_fill(5, fill_ptr);
    result = d_assert_standalone(
        vec != NULL,
        "new_fill_valid_not_null",
        "Valid fill should succeed",
        _counter) && result;

    if (vec)
    {
        result = d_assert_standalone(
            vec->count == 5,
            "new_fill_valid_count",
            "Count should be 5",
            _counter) && result;

        result = d_assert_standalone(
            vec->capacity >= 5,
            "new_fill_valid_capacity",
            "Capacity should be at least 5",
            _counter) && result;

        /* Verify all elements match fill value */
        all_match = true;
        for (i = 0; i < vec->count; i++)
        {
            if (vec->elements[i] != fill_ptr)
            {
                all_match = false;
                break;
            }
        }

        result = d_assert_standalone(
            all_match,
            "new_fill_all_match",
            "All elements should match fill pointer",
            _counter) && result;

        /* Verify pointed-to value */
        result = d_assert_standalone(
            *(int*)vec->elements[0] == 400,
            "new_fill_pointed_value",
            "Pointed-to value should be 400",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_new_merge
  Tests the d_ptr_vector_new_merge function for merging multiple ptr_vectors.
  Tests the following:
  - Zero vector count
  - NULL vectors in arguments
  - Successful merge of multiple vectors
  - Correct element ordering
*/
bool
d_tests_sa_ptr_vector_new_merge
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec1;
    struct d_ptr_vector* vec2;
    struct d_ptr_vector* vec3;
    struct d_ptr_vector* merged;

    result = true;

    /* test 1: zero count should return default vector */
    merged = d_ptr_vector_new_merge(0);
    result = d_assert_standalone(
        merged != NULL,
        "new_merge_zero_count_not_null",
        "Zero count merge should return non-NULL vector",
        _counter) && result;

    if (merged)
    {
        result = d_assert_standalone(
            merged->count == 0,
            "new_merge_zero_count_empty",
            "Zero count merge should be empty",
            _counter) && result;

        d_ptr_vector_free(merged);
    }

    /* Setup vectors for merge tests */
    vec1 = d_ptr_vector_new_from_args(2, &g_test_values[0], &g_test_values[1]);
    vec2 = d_ptr_vector_new_from_args(2, &g_test_values[2], &g_test_values[3]);
    vec3 = d_ptr_vector_new_from_args(1, &g_test_values[4]);

    if (vec1 && vec2 && vec3)
    {
        /* test 2: merge with NULL vector in arguments */
        merged = d_ptr_vector_new_merge(3, vec1, NULL, vec2);
        result = d_assert_standalone(
            merged != NULL,
            "new_merge_with_null_not_null",
            "Merge with NULL should succeed (skip NULL)",
            _counter) && result;

        if (merged)
        {
            /* NULL should be skipped, so only vec1 + vec2 = 4 elements */
            result = d_assert_standalone(
                merged->count == 4,
                "new_merge_with_null_count",
                "Merge count should be 4 (skipping NULL)",
                _counter) && result;

            d_ptr_vector_free(merged);
        }

        /* test 3: successful merge of all three vectors */
        merged = d_ptr_vector_new_merge(3, vec1, vec2, vec3);
        result = d_assert_standalone(
            merged != NULL,
            "new_merge_all_not_null",
            "Merge of all vectors should succeed",
            _counter) && result;

        if (merged)
        {
            result = d_assert_standalone(
                merged->count == 5,
                "new_merge_all_count",
                "Merged count should be 5",
                _counter) && result;

            /* Verify order: vec1 elements, then vec2, then vec3 */
            result = d_assert_standalone(
                merged->elements[0] == &g_test_values[0],
                "new_merge_order_0",
                "Element 0 should be from vec1",
                _counter) && result;

            result = d_assert_standalone(
                merged->elements[1] == &g_test_values[1],
                "new_merge_order_1",
                "Element 1 should be from vec1",
                _counter) && result;

            result = d_assert_standalone(
                merged->elements[2] == &g_test_values[2],
                "new_merge_order_2",
                "Element 2 should be from vec2",
                _counter) && result;

            result = d_assert_standalone(
                merged->elements[3] == &g_test_values[3],
                "new_merge_order_3",
                "Element 3 should be from vec2",
                _counter) && result;

            result = d_assert_standalone(
                merged->elements[4] == &g_test_values[4],
                "new_merge_order_4",
                "Element 4 should be from vec3",
                _counter) && result;

            d_ptr_vector_free(merged);
        }
    }

    /* Cleanup */
    if (vec1) d_ptr_vector_free(vec1);
    if (vec2) d_ptr_vector_free(vec2);
    if (vec3) d_ptr_vector_free(vec3);

    return result;
}


/*
d_tests_sa_ptr_vector_constructor_all
  Aggregation function that runs all constructor tests.
*/
bool
d_tests_sa_ptr_vector_constructor_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Constructor Functions\n");
    printf("  --------------------------------\n");

    result = d_tests_sa_ptr_vector_new(_counter) && result;
    result = d_tests_sa_ptr_vector_new_default(_counter) && result;
    result = d_tests_sa_ptr_vector_new_from_array(_counter) && result;
    result = d_tests_sa_ptr_vector_new_from_args(_counter) && result;
    result = d_tests_sa_ptr_vector_new_copy(_counter) && result;
    result = d_tests_sa_ptr_vector_new_fill(_counter) && result;
    result = d_tests_sa_ptr_vector_new_merge(_counter) && result;

    return result;
}
