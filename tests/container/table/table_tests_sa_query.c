#include ".\table_tests_sa.h"


// shared column descriptors for query tests
static const struct d_table_column_desc g_query_cols[] =
{
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
};

static const size_t g_query_col_count =
    D_TABLE_COLUMN_COUNT(g_query_cols);


/*
d_tests_sa_table_row_count
  Tests the d_table_row_count function.
  Tests the following:
  - NULL table returns 0
  - empty table returns 0
  - populated table returns correct count
*/
bool
d_tests_sa_table_row_count
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    struct d_test_table_row row = { 1, "a", 1.0 };

    result = true;

    // test 1: NULL table
    result = d_assert_standalone(
        d_table_row_count(NULL) == 0,
        "row_count_null",
        "NULL table should return 0",
        _counter) && result;

    // test 2: empty table
    tbl = d_table_new(sizeof(struct d_test_table_row), g_query_cols,
                      g_query_col_count, 10);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_row_count(tbl) == 0,
            "row_count_empty",
            "Empty table should return 0",
            _counter) && result;

        // test 3: after adding rows
        d_table_push_row(tbl, &row);
        d_table_push_row(tbl, &row);

        result = d_assert_standalone(
            d_table_row_count(tbl) == 2,
            "row_count_two",
            "Table with 2 rows should return 2",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_column_count
  Tests the d_table_column_count function.
  Tests the following:
  - NULL table returns 0
  - valid table returns correct column count
*/
bool
d_tests_sa_table_column_count
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    result = true;

    // test 1: NULL table
    result = d_assert_standalone(
        d_table_column_count(NULL) == 0,
        "col_count_null",
        "NULL table should return 0",
        _counter) && result;

    // test 2: valid table
    tbl = d_table_new(sizeof(struct d_test_table_row), g_query_cols,
                      g_query_col_count, 4);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_column_count(tbl) == 3,
            "col_count_three",
            "Table should have 3 columns",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_capacity_query
  Tests the d_table_capacity function.
  Tests the following:
  - NULL table returns 0
  - returns correct capacity value
*/
bool
d_tests_sa_table_capacity_query
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    result = true;

    // test 1: NULL table
    result = d_assert_standalone(
        d_table_capacity(NULL) == 0,
        "capacity_null",
        "NULL table should return 0",
        _counter) && result;

    // test 2: valid table
    tbl = d_table_new(sizeof(struct d_test_table_row), g_query_cols,
                      g_query_col_count, 25);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_capacity(tbl) == 25,
            "capacity_value",
            "Capacity should be 25",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_struct_size
  Tests the d_table_struct_size function.
  Tests the following:
  - NULL table returns 0
  - returns correct struct size
*/
bool
d_tests_sa_table_struct_size
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    result = true;

    // test 1: NULL table
    result = d_assert_standalone(
        d_table_struct_size(NULL) == 0,
        "struct_size_null",
        "NULL table should return 0",
        _counter) && result;

    // test 2: valid table
    tbl = d_table_new(sizeof(struct d_test_table_row), g_query_cols,
                      g_query_col_count, 4);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_struct_size(tbl) == sizeof(struct d_test_table_row),
            "struct_size_value",
            "struct_size should match d_test_table_row",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_is_empty
  Tests the d_table_is_empty function.
  Tests the following:
  - NULL table returns true
  - empty table returns true
  - non-empty table returns false
*/
bool
d_tests_sa_table_is_empty
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    struct d_test_table_row row = { 1, "x", 0.0 };

    result = true;

    // test 1: NULL table is empty
    result = d_assert_standalone(
        d_table_is_empty(NULL) == true,
        "is_empty_null",
        "NULL table should be empty",
        _counter) && result;

    // test 2: newly created table is empty
    tbl = d_table_new(sizeof(struct d_test_table_row), g_query_cols,
                      g_query_col_count, 10);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_is_empty(tbl) == true,
            "is_empty_new",
            "New table should be empty",
            _counter) && result;

        // test 3: non-empty after push
        d_table_push_row(tbl, &row);

        result = d_assert_standalone(
            d_table_is_empty(tbl) == false,
            "is_empty_after_push",
            "Table with rows should not be empty",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_is_full
  Tests the d_table_is_full function.
  Tests the following:
  - NULL table returns true (defensive)
  - not full when capacity > row_count
  - full when row_count == capacity
*/
bool
d_tests_sa_table_is_full
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    struct d_test_table_row row = { 1, "x", 0.0 };

    result = true;

    // test 1: NULL table
    result = d_assert_standalone(
        d_table_is_full(NULL) == true,
        "is_full_null",
        "NULL table should report full",
        _counter) && result;

    // test 2: not full
    tbl = d_table_new(sizeof(struct d_test_table_row), g_query_cols,
                      g_query_col_count, 3);

    if (tbl)
    {
        d_table_push_row(tbl, &row);

        result = d_assert_standalone(
            d_table_is_full(tbl) == false,
            "is_full_partial",
            "Partially filled table should not be full",
            _counter) && result;

        // test 3: fill to capacity
        d_table_push_row(tbl, &row);
        d_table_push_row(tbl, &row);

        result = d_assert_standalone(
            d_table_is_full(tbl) == true,
            "is_full_at_capacity",
            "Table at capacity should be full",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_data
  Tests the d_table_data function.
  Tests the following:
  - NULL table returns NULL
  - returns pointer to internal data buffer
*/
bool
d_tests_sa_table_data
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;
    void*           ptr;

    result = true;

    // test 1: NULL table
    ptr    = d_table_data(NULL);
    result = d_assert_standalone(
        ptr == NULL,
        "data_null_table",
        "NULL table should return NULL",
        _counter) && result;

    // test 2: valid data pointer
    tbl = d_table_new(sizeof(struct d_test_table_row), g_query_cols,
                      g_query_col_count, 10);

    if (tbl)
    {
        ptr    = d_table_data(tbl);
        result = d_assert_standalone(
            ptr == tbl->data,
            "data_pointer",
            "Data should return internal data pointer",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_query_all
  Aggregation function that runs all query tests.
*/
bool
d_tests_sa_table_query_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Query Functions\n");
    printf("  --------------------------\n");

    result = d_tests_sa_table_row_count(_counter) && result;
    result = d_tests_sa_table_column_count(_counter) && result;
    result = d_tests_sa_table_capacity_query(_counter) && result;
    result = d_tests_sa_table_struct_size(_counter) && result;
    result = d_tests_sa_table_is_empty(_counter) && result;
    result = d_tests_sa_table_is_full(_counter) && result;
    result = d_tests_sa_table_data(_counter) && result;

    return result;
}
