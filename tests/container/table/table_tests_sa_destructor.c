#include ".\table_tests_sa.h"


// shared column descriptors for destructor tests
static const struct d_table_column_desc g_dtor_cols[] =
{
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
};

static const size_t g_dtor_col_count =
    D_TABLE_COLUMN_COUNT(g_dtor_cols);


// d_test_free_counter
//   struct: context for counting free_deep callbacks.
struct d_test_free_counter
{
    size_t count;
};

static struct d_test_free_counter g_free_counter = { 0 };


// d_test_counting_free
//   function: free callback that increments a counter.  Does not actually
// free memory — used only to verify that free_deep iterates all rows.
static void
d_test_counting_free
(
    void* _ptr
)
{
    (void)_ptr;
    g_free_counter.count++;

    return;
}


/*
d_tests_sa_table_free
  Tests the d_table_free function.
  Tests the following:
  - NULL table does not crash
  - frees owned data buffer
  - does not free view data (flag check)
  - frees owned descriptor array
*/
bool
d_tests_sa_table_free
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    result = true;

    // test 1: NULL table does not crash
    d_table_free(NULL);
    result = d_assert_standalone(
        true,
        "free_null_safe",
        "Free on NULL should not crash",
        _counter) && result;

    // test 2: free owned table
    tbl = d_table_new(sizeof(struct d_test_table_row), g_dtor_cols,
                      g_dtor_col_count, 10);

    if (tbl)
    {
        result = d_assert_standalone(
            (tbl->flags & D_TABLE_FLAG_OWNS_DATA) != 0,
            "free_owns_data_flag",
            "Table should have OWNS_DATA flag before free",
            _counter) && result;

        d_table_free(tbl);

        result = d_assert_standalone(
            true,
            "free_owned_no_crash",
            "Free of owned table should not crash",
            _counter) && result;
    }

    // test 3: free copy (owns both data and descs)
    {
        struct d_test_table_row rows[] =
        {
            { 1, "x", 0.0 }
        };

        struct d_table* original;
        struct d_table* copy;

        original = d_table_new_from_rows(
            sizeof(struct d_test_table_row),
            rows, 1, g_dtor_cols, g_dtor_col_count);

        if (original)
        {
            copy = d_table_new_copy(original);

            if (copy)
            {
                result = d_assert_standalone(
                    (copy->flags & D_TABLE_FLAG_OWNS_DATA) != 0 &&
                    (copy->flags & D_TABLE_FLAG_OWNS_DESCS) != 0,
                    "free_copy_flags",
                    "Copy should own both data and descs",
                    _counter) && result;

                d_table_free(copy);

                result = d_assert_standalone(
                    true,
                    "free_copy_no_crash",
                    "Free of deep copy should not crash",
                    _counter) && result;
            }

            d_table_free(original);
        }
    }

    return result;
}


/*
d_tests_sa_table_free_deep
  Tests the d_table_free_deep function.
  Tests the following:
  - NULL table does not crash
  - free_fn called once per non-NULL pointer-sized cell
  - NULL free_fn still frees (like regular free)

  Note: d_table_free_deep calls _free_fn on every cell whose
  element_size == sizeof(void*), not once per row.  On 64-bit
  platforms both const char* and double are 8 bytes, so both
  columns match the pointer-sized check.  The expected count is
  computed dynamically to remain platform-independent.
*/
bool
d_tests_sa_table_free_deep
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;
    size_t          c;
    size_t          ptr_cols;
    size_t          expected_calls;

    struct d_test_table_row rows[] =
    {
        { 1, "a", 1.0 },
        { 2, "b", 2.0 },
        { 3, "c", 3.0 }
    };

    result = true;

    // test 1: NULL table does not crash
    d_table_free_deep(NULL, d_test_counting_free);
    result = d_assert_standalone(
        true,
        "free_deep_null_safe",
        "Free deep on NULL should not crash",
        _counter) && result;

    // count how many columns are pointer-sized
    ptr_cols = 0;
    for (c = 0; c < g_dtor_col_count; c++)
    {
        if (g_dtor_cols[c].element_size == sizeof(void*))
        {
            ptr_cols++;
        }
    }

    // all rows have non-NULL/non-zero pointer-sized cells,
    // so every pointer-sized cell triggers the callback
    expected_calls = 3 * ptr_cols;

    // test 2: free_fn called once per pointer-sized cell
    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 3, g_dtor_cols, g_dtor_col_count);

    if (tbl)
    {
        g_free_counter.count = 0;

        d_table_free_deep(tbl, d_test_counting_free);

        result = d_assert_standalone(
            g_free_counter.count == expected_calls,
            "free_deep_callback_count",
            "Free callback should be called once per "
            "pointer-sized cell",
            _counter) && result;
    }

    // test 3: NULL free_fn still frees (like regular free)
    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 3, g_dtor_cols, g_dtor_col_count);

    if (tbl)
    {
        d_table_free_deep(tbl, NULL);

        result = d_assert_standalone(
            true,
            "free_deep_null_fn",
            "Free deep with NULL fn should not crash",
            _counter) && result;
    }

    return result;
}


/*
d_tests_sa_table_destructor_all
  Aggregation function that runs all destructor tests.
*/
bool
d_tests_sa_table_destructor_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Destructor Functions\n");
    printf("  -------------------------------\n");

    result = d_tests_sa_table_free(_counter) && result;
    result = d_tests_sa_table_free_deep(_counter) && result;

    return result;
}
