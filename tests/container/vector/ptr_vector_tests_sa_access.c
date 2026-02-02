/******************************************************************************
* djinterp [test]                                     ptr_vector_tests_sa_access.c
*
*   Unit tests for ptr_vector access functions.
*
*
* path:      \tests\container\vector\ptr_vector_tests_sa_access.c
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.01.29
******************************************************************************/

#include ".\ptr_vector_tests_sa.h"


/******************************************************************************
 * HELPER DATA FOR TESTS
 *****************************************************************************/

static int g_access_test_values[] = {100, 200, 300, 400, 500};


/*
d_tests_sa_ptr_vector_at
  Tests the d_ptr_vector_at function for element access.
*/
bool
d_tests_sa_ptr_vector_at
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;
    void*                elem;

    result = true;

    /* test 1: NULL vector should return NULL */
    result = d_assert_standalone(
        d_ptr_vector_at(NULL, 0) == NULL,
        "at_null_vector",
        "NULL vector should return NULL",
        _counter) && result;

    /* test 2: empty vector should return NULL */
    vec = d_ptr_vector_new(10);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_at(vec, 0) == NULL,
            "at_empty_vector",
            "Empty vector should return NULL",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 3: valid positive index access */
    vec = d_ptr_vector_new_from_args(3,
                                     &g_access_test_values[0],
                                     &g_access_test_values[1],
                                     &g_access_test_values[2]);
    if (vec)
    {
        elem = d_ptr_vector_at(vec, 0);
        result = d_assert_standalone(
            elem == &g_access_test_values[0],
            "at_index_0",
            "Index 0 should return first element",
            _counter) && result;

        elem = d_ptr_vector_at(vec, 1);
        result = d_assert_standalone(
            elem == &g_access_test_values[1],
            "at_index_1",
            "Index 1 should return second element",
            _counter) && result;

        elem = d_ptr_vector_at(vec, 2);
        result = d_assert_standalone(
            elem == &g_access_test_values[2],
            "at_index_2",
            "Index 2 should return third element",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 4: valid negative index access */
    vec = d_ptr_vector_new_from_args(3,
                                     &g_access_test_values[0],
                                     &g_access_test_values[1],
                                     &g_access_test_values[2]);
    if (vec)
    {
        elem = d_ptr_vector_at(vec, -1);
        result = d_assert_standalone(
            elem == &g_access_test_values[2],
            "at_neg_1",
            "Index -1 should return last element",
            _counter) && result;

        elem = d_ptr_vector_at(vec, -2);
        result = d_assert_standalone(
            elem == &g_access_test_values[1],
            "at_neg_2",
            "Index -2 should return second-to-last",
            _counter) && result;

        elem = d_ptr_vector_at(vec, -3);
        result = d_assert_standalone(
            elem == &g_access_test_values[0],
            "at_neg_3",
            "Index -3 should return first element",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 5: out-of-bounds should return NULL */
    vec = d_ptr_vector_new_from_args(3,
                                     &g_access_test_values[0],
                                     &g_access_test_values[1],
                                     &g_access_test_values[2]);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_at(vec, 10) == NULL,
            "at_oob_positive",
            "Out-of-bounds positive should return NULL",
            _counter) && result;

        result = d_assert_standalone(
            d_ptr_vector_at(vec, -10) == NULL,
            "at_oob_negative",
            "Out-of-bounds negative should return NULL",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_front
  Tests the d_ptr_vector_front function for accessing first element.
*/
bool
d_tests_sa_ptr_vector_front
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;

    result = true;

    /* test 1: NULL vector should return NULL */
    result = d_assert_standalone(
        d_ptr_vector_front(NULL) == NULL,
        "front_null_vector",
        "NULL vector should return NULL",
        _counter) && result;

    /* test 2: empty vector should return NULL */
    vec = d_ptr_vector_new(10);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_front(vec) == NULL,
            "front_empty_vector",
            "Empty vector should return NULL",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 3: valid front access */
    vec = d_ptr_vector_new_from_args(3,
                                     &g_access_test_values[0],
                                     &g_access_test_values[1],
                                     &g_access_test_values[2]);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_front(vec) == &g_access_test_values[0],
            "front_valid",
            "Front should return first element",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 4: single element vector */
    vec = d_ptr_vector_new_from_args(1, &g_access_test_values[4]);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_front(vec) == &g_access_test_values[4],
            "front_single",
            "Front of single-element vector should work",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_back
  Tests the d_ptr_vector_back function for accessing last element.
*/
bool
d_tests_sa_ptr_vector_back
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;

    result = true;

    /* test 1: NULL vector should return NULL */
    result = d_assert_standalone(
        d_ptr_vector_back(NULL) == NULL,
        "back_null_vector",
        "NULL vector should return NULL",
        _counter) && result;

    /* test 2: empty vector should return NULL */
    vec = d_ptr_vector_new(10);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_back(vec) == NULL,
            "back_empty_vector",
            "Empty vector should return NULL",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 3: valid back access */
    vec = d_ptr_vector_new_from_args(3,
                                     &g_access_test_values[0],
                                     &g_access_test_values[1],
                                     &g_access_test_values[2]);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_back(vec) == &g_access_test_values[2],
            "back_valid",
            "Back should return last element",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 4: single element vector */
    vec = d_ptr_vector_new_from_args(1, &g_access_test_values[4]);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_back(vec) == &g_access_test_values[4],
            "back_single",
            "Back of single-element vector should work",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_data
  Tests the d_ptr_vector_data function for getting raw array pointer.
*/
bool
d_tests_sa_ptr_vector_data
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;
    void**               data;

    result = true;

    /* test 1: NULL vector should return NULL */
    result = d_assert_standalone(
        d_ptr_vector_data(NULL) == NULL,
        "data_null_vector",
        "NULL vector should return NULL",
        _counter) && result;

    /* test 2: valid data access */
    vec = d_ptr_vector_new_from_args(3,
                                     &g_access_test_values[0],
                                     &g_access_test_values[1],
                                     &g_access_test_values[2]);
    if (vec)
    {
        data = d_ptr_vector_data(vec);
        result = d_assert_standalone(
            data == vec->elements,
            "data_same_as_elements",
            "Data should return elements array",
            _counter) && result;

        /* Can access via returned pointer */
        result = d_assert_standalone(
            data[0] == &g_access_test_values[0] &&
            data[1] == &g_access_test_values[1] &&
            data[2] == &g_access_test_values[2],
            "data_access",
            "Data pointer can access elements",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_get
  Tests the d_ptr_vector_get function (alias for at).
*/
bool
d_tests_sa_ptr_vector_get
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;

    result = true;

    /* test 1: NULL vector should return NULL */
    result = d_assert_standalone(
        d_ptr_vector_get(NULL, 0) == NULL,
        "get_null_vector",
        "NULL vector should return NULL",
        _counter) && result;

    /* test 2: valid get */
    vec = d_ptr_vector_new_from_args(3,
                                     &g_access_test_values[0],
                                     &g_access_test_values[1],
                                     &g_access_test_values[2]);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_get(vec, 1) == &g_access_test_values[1],
            "get_valid",
            "Get should return correct element",
            _counter) && result;

        /* Negative index */
        result = d_assert_standalone(
            d_ptr_vector_get(vec, -1) == &g_access_test_values[2],
            "get_negative",
            "Get with negative index should work",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_set
  Tests the d_ptr_vector_set function for setting element at index.
*/
bool
d_tests_sa_ptr_vector_set
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;

    result = true;

    /* test 1: NULL vector should fail */
    result = d_assert_standalone(
        d_ptr_vector_set(NULL, 0, &g_access_test_values[0]) == D_FAILURE,
        "set_null_vector",
        "NULL vector should return D_FAILURE",
        _counter) && result;

    /* test 2: empty vector should fail */
    vec = d_ptr_vector_new(10);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_set(vec, 0, &g_access_test_values[0]) == D_FAILURE,
            "set_empty_vector",
            "Empty vector should return D_FAILURE",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 3: valid set with positive index */
    vec = d_ptr_vector_new_from_args(3,
                                     &g_access_test_values[0],
                                     &g_access_test_values[1],
                                     &g_access_test_values[2]);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_set(vec, 1, &g_access_test_values[4]) == D_SUCCESS,
            "set_positive_success",
            "Set with positive index should succeed",
            _counter) && result;

        result = d_assert_standalone(
            vec->elements[1] == &g_access_test_values[4],
            "set_positive_value",
            "Element should be updated",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 4: valid set with negative index */
    vec = d_ptr_vector_new_from_args(3,
                                     &g_access_test_values[0],
                                     &g_access_test_values[1],
                                     &g_access_test_values[2]);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_set(vec, -1, &g_access_test_values[4]) == D_SUCCESS &&
            vec->elements[2] == &g_access_test_values[4],
            "set_negative",
            "Set with negative index should work",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 5: set to NULL */
    vec = d_ptr_vector_new_from_args(3,
                                     &g_access_test_values[0],
                                     &g_access_test_values[1],
                                     &g_access_test_values[2]);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_set(vec, 0, NULL) == D_SUCCESS &&
            vec->elements[0] == NULL,
            "set_null_value",
            "Set to NULL should work",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 6: out-of-bounds should fail */
    vec = d_ptr_vector_new_from_args(3,
                                     &g_access_test_values[0],
                                     &g_access_test_values[1],
                                     &g_access_test_values[2]);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_set(vec, 100, &g_access_test_values[4]) == D_FAILURE,
            "set_oob",
            "Out-of-bounds set should fail",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_access_all
  Aggregation function that runs all access tests.
*/
bool
d_tests_sa_ptr_vector_access_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Access Functions\n");
    printf("  ---------------------------\n");

    result = d_tests_sa_ptr_vector_at(_counter) && result;
    result = d_tests_sa_ptr_vector_front(_counter) && result;
    result = d_tests_sa_ptr_vector_back(_counter) && result;
    result = d_tests_sa_ptr_vector_data(_counter) && result;
    result = d_tests_sa_ptr_vector_get(_counter) && result;
    result = d_tests_sa_ptr_vector_set(_counter) && result;

    return result;
}
