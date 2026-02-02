/******************************************************************************
* djinterp [test]                                    ptr_vector_tests_sa_utility.c
*
*   Unit tests for ptr_vector utility functions.
*
* path:      \tests\container\vector\ptr_vector_tests_sa_utility.c
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.01.29
******************************************************************************/

#include ".\ptr_vector_tests_sa.h"

static int g_util_test_values[] = {50, 10, 40, 20, 30};

static int
int_ptr_comparator(const void* _a, const void* _b)
{
    int val_a, val_b;
    if (!_a && !_b) return 0;
    if (!_a) return -1;
    if (!_b) return 1;
    val_a = *(const int*)_a;
    val_b = *(const int*)_b;
    return (val_a > val_b) - (val_a < val_b);
}

bool d_tests_sa_ptr_vector_swap(struct d_test_counter* _counter)
{
    bool result = true;
    struct d_ptr_vector* vec;

    result = d_assert_standalone(
        d_ptr_vector_swap(NULL, 0, 1) == D_FAILURE,
        "swap_null_vector", "NULL vector should return D_FAILURE", _counter) && result;

    vec = d_ptr_vector_new_from_args(3,
        &g_util_test_values[0], &g_util_test_values[1], &g_util_test_values[2]);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_swap(vec, 0, 2) == D_SUCCESS &&
            vec->elements[0] == &g_util_test_values[2] &&
            vec->elements[2] == &g_util_test_values[0],
            "swap_success", "Swap should exchange elements", _counter) && result;

        result = d_assert_standalone(
            d_ptr_vector_swap(vec, 1, 1) == D_SUCCESS,
            "swap_same_index", "Swap same index should succeed (no-op)", _counter) && result;

        result = d_assert_standalone(
            d_ptr_vector_swap(vec, -1, 0) == D_SUCCESS,
            "swap_negative_index", "Swap with negative index should work", _counter) && result;

        result = d_assert_standalone(
            d_ptr_vector_swap(vec, 0, 100) == D_FAILURE,
            "swap_oob", "Out-of-bounds swap should fail", _counter) && result;

        d_ptr_vector_free(vec);
    }
    return result;
}

bool d_tests_sa_ptr_vector_reverse(struct d_test_counter* _counter)
{
    bool result = true;
    struct d_ptr_vector* vec;

    result = d_assert_standalone(
        d_ptr_vector_reverse(NULL) == D_FAILURE,
        "reverse_null_vector", "NULL vector should return D_FAILURE", _counter) && result;

    vec = d_ptr_vector_new_from_args(5,
        &g_util_test_values[0], &g_util_test_values[1], &g_util_test_values[2],
        &g_util_test_values[3], &g_util_test_values[4]);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_reverse(vec) == D_SUCCESS &&
            vec->elements[0] == &g_util_test_values[4] &&
            vec->elements[1] == &g_util_test_values[3] &&
            vec->elements[2] == &g_util_test_values[2] &&
            vec->elements[3] == &g_util_test_values[1] &&
            vec->elements[4] == &g_util_test_values[0],
            "reverse_success", "Reverse should reverse element order", _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* Single element */
    vec = d_ptr_vector_new_from_args(1, &g_util_test_values[0]);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_reverse(vec) == D_SUCCESS &&
            vec->elements[0] == &g_util_test_values[0],
            "reverse_single", "Reverse single element should succeed", _counter) && result;

        d_ptr_vector_free(vec);
    }
    return result;
}

bool d_tests_sa_ptr_vector_sort(struct d_test_counter* _counter)
{
    bool result = true;
    struct d_ptr_vector* vec;

    /* NULL checks - sort returns void */
    d_ptr_vector_sort(NULL, int_ptr_comparator);
    result = d_assert_standalone(true,
        "sort_null_vector", "Sort NULL vector should not crash", _counter) && result;

    vec = d_ptr_vector_new_from_args(5,
        &g_util_test_values[0], &g_util_test_values[1], &g_util_test_values[2],
        &g_util_test_values[3], &g_util_test_values[4]);  /* 50, 10, 40, 20, 30 */
    if (vec)
    {
        d_ptr_vector_sort(vec, int_ptr_comparator);

        /* After sort: 10, 20, 30, 40, 50 */
        result = d_assert_standalone(
            *(int*)vec->elements[0] == 10 &&
            *(int*)vec->elements[1] == 20 &&
            *(int*)vec->elements[2] == 30 &&
            *(int*)vec->elements[3] == 40 &&
            *(int*)vec->elements[4] == 50,
            "sort_success", "Sort should order elements correctly", _counter) && result;

        d_ptr_vector_free(vec);
    }

    vec = d_ptr_vector_new(10);
    if (vec)
    {
        d_ptr_vector_sort(vec, int_ptr_comparator);
        result = d_assert_standalone(vec->count == 0,
            "sort_empty", "Sort empty vector should work", _counter) && result;

        d_ptr_vector_free(vec);
    }
    return result;
}

bool d_tests_sa_ptr_vector_copy_to(struct d_test_counter* _counter)
{
    bool result = true;
    struct d_ptr_vector* vec;
    void* dest[10];

    result = d_assert_standalone(
        d_ptr_vector_copy_to(NULL, dest, 10) == D_FAILURE,
        "copy_to_null_vector", "NULL vector should return D_FAILURE", _counter) && result;

    vec = d_ptr_vector_new_from_args(3,
        &g_util_test_values[0], &g_util_test_values[1], &g_util_test_values[2]);
    if (vec)
    {
        result = d_assert_standalone(
            d_ptr_vector_copy_to(vec, NULL, 10) == D_FAILURE,
            "copy_to_null_dest", "NULL destination should return D_FAILURE", _counter) && result;

        result = d_assert_standalone(
            d_ptr_vector_copy_to(vec, dest, 2) == D_FAILURE,
            "copy_to_small_dest", "Too small destination should fail", _counter) && result;

        result = d_assert_standalone(
            d_ptr_vector_copy_to(vec, dest, 10) == D_SUCCESS &&
            dest[0] == &g_util_test_values[0] &&
            dest[1] == &g_util_test_values[1] &&
            dest[2] == &g_util_test_values[2],
            "copy_to_success", "Copy to should copy all elements", _counter) && result;

        d_ptr_vector_free(vec);
    }
    return result;
}

bool d_tests_sa_ptr_vector_utility_all(struct d_test_counter* _counter)
{
    bool result = true;

    printf("\n  [SECTION] Utility Functions\n");
    printf("  ----------------------------\n");

    result = d_tests_sa_ptr_vector_swap(_counter) && result;
    result = d_tests_sa_ptr_vector_reverse(_counter) && result;
    result = d_tests_sa_ptr_vector_sort(_counter) && result;
    result = d_tests_sa_ptr_vector_copy_to(_counter) && result;

    return result;
}
