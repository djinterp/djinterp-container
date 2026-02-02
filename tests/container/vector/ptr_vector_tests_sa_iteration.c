/******************************************************************************
* djinterp [test]                                  ptr_vector_tests_sa_iteration.c
*
*   Unit tests for ptr_vector iteration helper functions.
*
* path:      \tests\container\vector\ptr_vector_tests_sa_iteration.c
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.01.29
******************************************************************************/

#include ".\ptr_vector_tests_sa.h"

static int g_iter_test_values[] = {1, 2, 3, 4, 5};
static int g_apply_sum = 0;
static int g_apply_count = 0;

static void
apply_sum_fn(void* _element)
{
    if (_element)
    {
        g_apply_sum += *(int*)_element;
    }
    g_apply_count++;
}

static void
apply_ctx_fn(void* _element, void* _context)
{
    if (_element && _context)
    {
        *(int*)_context += *(int*)_element;
    }
}

bool d_tests_sa_ptr_vector_foreach(struct d_test_counter* _counter)
{
    bool result = true;
    struct d_ptr_vector* vec;

    /* Reset globals */
    g_apply_sum = 0;
    g_apply_count = 0;

    /* NULL checks - foreach returns void */
    d_ptr_vector_foreach(NULL, apply_sum_fn);
    result = d_assert_standalone(g_apply_count == 0,
        "foreach_null_vector", "Foreach NULL vector should not call function", _counter) && result;

    vec = d_ptr_vector_new_from_args(3,
        &g_iter_test_values[0], &g_iter_test_values[1], &g_iter_test_values[2]);
    if (vec)
    {
        d_ptr_vector_foreach(vec, NULL);
        result = d_assert_standalone(true,
            "foreach_null_fn", "Foreach with NULL function should not crash", _counter) && result;

        g_apply_sum = 0;
        g_apply_count = 0;
        d_ptr_vector_foreach(vec, apply_sum_fn);
        result = d_assert_standalone(
            g_apply_count == 3 && g_apply_sum == 6,
            "foreach_success", "Foreach should apply function to all elements", _counter) && result;

        d_ptr_vector_free(vec);
    }

    /* Empty vector */
    vec = d_ptr_vector_new(10);
    if (vec)
    {
        g_apply_count = 0;
        d_ptr_vector_foreach(vec, apply_sum_fn);
        result = d_assert_standalone(g_apply_count == 0,
            "foreach_empty", "Foreach on empty vector should not call function", _counter) && result;

        d_ptr_vector_free(vec);
    }
    return result;
}

bool d_tests_sa_ptr_vector_foreach_with_context(struct d_test_counter* _counter)
{
    bool result = true;
    struct d_ptr_vector* vec;
    int context_sum;

    /* NULL checks */
    context_sum = 0;
    d_ptr_vector_foreach_with_context(NULL, apply_ctx_fn, &context_sum);
    result = d_assert_standalone(context_sum == 0,
        "foreach_ctx_null_vector", "Foreach ctx NULL vector should not modify context", _counter) && result;

    vec = d_ptr_vector_new_from_args(5,
        &g_iter_test_values[0], &g_iter_test_values[1], &g_iter_test_values[2],
        &g_iter_test_values[3], &g_iter_test_values[4]);
    if (vec)
    {
        d_ptr_vector_foreach_with_context(vec, NULL, &context_sum);
        result = d_assert_standalone(true,
            "foreach_ctx_null_fn", "Foreach ctx with NULL function should not crash", _counter) && result;

        context_sum = 0;
        d_ptr_vector_foreach_with_context(vec, apply_ctx_fn, &context_sum);
        result = d_assert_standalone(context_sum == 15,
            "foreach_ctx_success", "Foreach ctx should sum to 15", _counter) && result;

        d_ptr_vector_free(vec);
    }
    return result;
}

bool d_tests_sa_ptr_vector_iteration_all(struct d_test_counter* _counter)
{
    bool result = true;

    printf("\n  [SECTION] Iteration Helper Functions\n");
    printf("  -------------------------------------\n");

    result = d_tests_sa_ptr_vector_foreach(_counter) && result;
    result = d_tests_sa_ptr_vector_foreach_with_context(_counter) && result;

    return result;
}
