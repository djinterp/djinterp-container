/******************************************************************************
* djinterp [test]                                   ptr_vector_tests_sa_capacity.c
*
*   Unit tests for ptr_vector capacity management functions.
*
*
* path:      \tests\container\vector\ptr_vector_tests_sa_capacity.c
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.01.29
******************************************************************************/

#include ".\ptr_vector_tests_sa.h"


/******************************************************************************
 * HELPER DATA FOR TESTS
 *****************************************************************************/

static int g_cap_test_values[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};


/*
d_tests_sa_ptr_vector_reserve
  Tests the d_ptr_vector_reserve function for reserving capacity.
  Tests the following:
  - NULL vector handling
  - Reserve smaller than current capacity (no-op)
  - Reserve larger than current capacity (grows)
  - Data preservation after reserve
*/
bool
d_tests_sa_ptr_vector_reserve
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;
    size_t               original_capacity;

    result = true;

    /* test 1: NULL vector should fail */
    result = d_assert_standalone(
        d_ptr_vector_reserve(NULL, 10) == D_FAILURE,
        "reserve_null_vector",
        "NULL vector should return D_FAILURE",
        _counter) && result;

    /* test 2: reserve smaller than current capacity (no-op) */
    vec = d_ptr_vector_new(20);
    if (vec)
    {
        original_capacity = vec->capacity;

        result = d_assert_standalone(
            d_ptr_vector_reserve(vec, 5) == D_SUCCESS,
            "reserve_smaller_success",
            "Reserve smaller should return D_SUCCESS",
            _counter) && result;

        result = d_assert_standalone(
            vec->capacity == original_capacity,
            "reserve_smaller_unchanged",
            "Capacity should remain unchanged",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 3: reserve larger than current capacity */
    vec = d_ptr_vector_new(5);
    if (vec)
    {
        /* Add some elements first */
        d_ptr_vector_push_back(vec, &g_cap_test_values[0]);
        d_ptr_vector_push_back(vec, &g_cap_test_values[1]);
        d_ptr_vector_push_back(vec, &g_cap_test_values[2]);

        result = d_assert_standalone(
            d_ptr_vector_reserve(vec, 50) == D_SUCCESS,
            "reserve_larger_success",
            "Reserve larger should return D_SUCCESS",
            _counter) && result;

        result = d_assert_standalone(
            vec->capacity >= 50,
            "reserve_larger_capacity",
            "Capacity should be at least 50",
            _counter) && result;

        /* Verify data preserved */
        result = d_assert_standalone(
            vec->count == 3,
            "reserve_larger_count_preserved",
            "Count should be preserved",
            _counter) && result;

        result = d_assert_standalone(
            vec->elements[0] == &g_cap_test_values[0] &&
            vec->elements[1] == &g_cap_test_values[1] &&
            vec->elements[2] == &g_cap_test_values[2],
            "reserve_larger_data_preserved",
            "Data should be preserved after reserve",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 4: reserve on empty vector */
    vec = d_ptr_vector_new(0);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_reserve(vec, 100) == D_SUCCESS,
            "reserve_empty_success",
            "Reserve on empty vector should succeed",
            _counter) && result;

        result = d_assert_standalone(
            vec->capacity >= 100,
            "reserve_empty_capacity",
            "Empty vector capacity should grow",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_shrink_to_fit
  Tests the d_ptr_vector_shrink_to_fit function for reducing capacity.
  Tests the following:
  - NULL vector handling
  - Shrink when count < capacity
  - Shrink empty vector
  - Data preservation after shrink
*/
bool
d_tests_sa_ptr_vector_shrink_to_fit
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;

    result = true;

    /* test 1: NULL vector should fail */
    result = d_assert_standalone(
        d_ptr_vector_shrink_to_fit(NULL) == D_FAILURE,
        "shrink_null_vector",
        "NULL vector should return D_FAILURE",
        _counter) && result;

    /* test 2: shrink when count < capacity */
    vec = d_ptr_vector_new(100);
    if (vec)
    {
        /* Add only 3 elements */
        d_ptr_vector_push_back(vec, &g_cap_test_values[0]);
        d_ptr_vector_push_back(vec, &g_cap_test_values[1]);
        d_ptr_vector_push_back(vec, &g_cap_test_values[2]);

        result = d_assert_standalone(
            vec->capacity == 100,
            "shrink_initial_capacity",
            "Initial capacity should be 100",
            _counter) && result;

        result = d_assert_standalone(
            d_ptr_vector_shrink_to_fit(vec) == D_SUCCESS,
            "shrink_success",
            "Shrink should return D_SUCCESS",
            _counter) && result;

        result = d_assert_standalone(
            vec->capacity == 3,
            "shrink_capacity_reduced",
            "Capacity should be reduced to count (3)",
            _counter) && result;

        result = d_assert_standalone(
            vec->count == 3,
            "shrink_count_preserved",
            "Count should be preserved",
            _counter) && result;

        /* Verify data preserved */
        result = d_assert_standalone(
            vec->elements[0] == &g_cap_test_values[0],
            "shrink_data_preserved",
            "Data should be preserved after shrink",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 3: shrink empty vector */
    vec = d_ptr_vector_new(50);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_shrink_to_fit(vec) == D_SUCCESS,
            "shrink_empty_success",
            "Shrink empty vector should succeed",
            _counter) && result;

        result = d_assert_standalone(
            vec->capacity == 0 && vec->count == 0,
            "shrink_empty_result",
            "Empty vector should have 0 capacity and count",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 4: shrink when already at minimum */
    vec = d_ptr_vector_new_from_args(3,
                                     &g_cap_test_values[0],
                                     &g_cap_test_values[1],
                                     &g_cap_test_values[2]);
    if (vec)
    {
        /* First shrink to fit */
        d_ptr_vector_shrink_to_fit(vec);
        size_t cap_before = vec->capacity;

        /* Shrink again should be no-op */
        result = d_assert_standalone(
            d_ptr_vector_shrink_to_fit(vec) == D_SUCCESS,
            "shrink_already_fit_success",
            "Shrink when already fit should succeed",
            _counter) && result;

        result = d_assert_standalone(
            vec->capacity == cap_before,
            "shrink_already_fit_unchanged",
            "Capacity should remain unchanged",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_ensure_capacity
  Tests the d_ptr_vector_ensure_capacity function.
  Tests the following:
  - NULL vector handling
  - Ensure when capacity already sufficient
  - Ensure when capacity needs to grow
*/
bool
d_tests_sa_ptr_vector_ensure_capacity
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;
    size_t               original_capacity;

    result = true;

    /* test 1: NULL vector should fail */
    result = d_assert_standalone(
        d_ptr_vector_ensure_capacity(NULL, 10) == D_FAILURE,
        "ensure_null_vector",
        "NULL vector should return D_FAILURE",
        _counter) && result;

    /* test 2: ensure when capacity already sufficient */
    vec = d_ptr_vector_new(50);
    if (vec)
    {
        original_capacity = vec->capacity;

        result = d_assert_standalone(
            d_ptr_vector_ensure_capacity(vec, 30) == D_SUCCESS,
            "ensure_sufficient_success",
            "Ensure with sufficient capacity should succeed",
            _counter) && result;

        result = d_assert_standalone(
            vec->capacity == original_capacity,
            "ensure_sufficient_unchanged",
            "Capacity should remain unchanged",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 3: ensure when capacity needs to grow */
    vec = d_ptr_vector_new(5);
    if (vec)
    {
        /* Add some elements */
        d_ptr_vector_push_back(vec, &g_cap_test_values[0]);
        d_ptr_vector_push_back(vec, &g_cap_test_values[1]);

        result = d_assert_standalone(
            d_ptr_vector_ensure_capacity(vec, 100) == D_SUCCESS,
            "ensure_grow_success",
            "Ensure with growth should succeed",
            _counter) && result;

        result = d_assert_standalone(
            vec->capacity >= 100,
            "ensure_grow_capacity",
            "Capacity should be at least 100",
            _counter) && result;

        /* Verify data preserved */
        result = d_assert_standalone(
            vec->count == 2 &&
            vec->elements[0] == &g_cap_test_values[0] &&
            vec->elements[1] == &g_cap_test_values[1],
            "ensure_grow_data_preserved",
            "Data should be preserved",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 4: ensure zero required */
    vec = d_ptr_vector_new(10);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_ensure_capacity(vec, 0) == D_SUCCESS,
            "ensure_zero_success",
            "Ensure with zero required should succeed",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_available
  Tests the d_ptr_vector_available function.
  Tests the following:
  - NULL vector handling
  - Available slots calculation
  - Available after operations
*/
bool
d_tests_sa_ptr_vector_available
(
    struct d_test_counter* _counter
)
{
    bool                 result;
    struct d_ptr_vector* vec;
    size_t               available;

    result = true;

    /* test 1: NULL vector should return 0 */
    available = d_ptr_vector_available(NULL);
    result = d_assert_standalone(
        available == 0,
        "available_null_vector",
        "NULL vector should return 0 available",
        _counter) && result;

    /* test 2: newly created vector */
    vec = d_ptr_vector_new(10);
    if (vec)
    {
        available = d_ptr_vector_available(vec);
        result = d_assert_standalone(
            available == 10,
            "available_new_vector",
            "New vector should have capacity available",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 3: available after adding elements */
    vec = d_ptr_vector_new(10);
    if (vec)
    {
        d_ptr_vector_push_back(vec, &g_cap_test_values[0]);
        d_ptr_vector_push_back(vec, &g_cap_test_values[1]);
        d_ptr_vector_push_back(vec, &g_cap_test_values[2]);

        available = d_ptr_vector_available(vec);
        result = d_assert_standalone(
            available == 7,
            "available_after_push",
            "Available should be capacity - count (7)",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 4: available when full */
    vec = d_ptr_vector_new(3);
    if (vec)
    {
        d_ptr_vector_push_back(vec, &g_cap_test_values[0]);
        d_ptr_vector_push_back(vec, &g_cap_test_values[1]);
        d_ptr_vector_push_back(vec, &g_cap_test_values[2]);

        available = d_ptr_vector_available(vec);
        result = d_assert_standalone(
            available == 0,
            "available_when_full",
            "Full vector should have 0 available",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* test 5: available on empty vector with zero capacity */
    vec = d_ptr_vector_new(0);
    if (vec)
    {
        available = d_ptr_vector_available(vec);
        result = d_assert_standalone(
            available == 0,
            "available_zero_capacity",
            "Zero capacity vector should have 0 available",
            _counter) && result;

        d_ptr_vector_free(vec);
    }

    return result;
}


/*
d_tests_sa_ptr_vector_capacity_all
  Aggregation function that runs all capacity management tests.
*/
bool
d_tests_sa_ptr_vector_capacity_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Capacity Management Functions\n");
    printf("  ----------------------------------------\n");

    result = d_tests_sa_ptr_vector_reserve(_counter) && result;
    result = d_tests_sa_ptr_vector_shrink_to_fit(_counter) && result;
    result = d_tests_sa_ptr_vector_ensure_capacity(_counter) && result;
    result = d_tests_sa_ptr_vector_available(_counter) && result;

    return result;
}
