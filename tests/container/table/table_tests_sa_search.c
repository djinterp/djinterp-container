#include ".\table_tests_sa.h"


// shared column descriptors for search tests
static const struct d_table_column_desc g_search_cols[] =
{
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
};

static const size_t g_search_col_count =
    D_TABLE_COLUMN_COUNT(g_search_cols);


// d_test_int_comparator
//   function: comparator for int values used in search tests.
static int
d_test_int_comparator
(
    const void* _a,
    const void* _b
)
{
    int a;
    int b;

    a = *(const int*)_a;
    b = *(const int*)_b;

    if (a < b)
    {
        return -1;
    }

    if (a > b)
    {
        return 1;
    }

    return 0;
}


/*
d_tests_sa_table_find_row
  Tests the d_table_find_row function.
  Tests the following:
  - NULL table returns -1
  - NULL comparator returns -1
  - value not found returns -1
  - finds first matching row
  - finds correct index among duplicates
*/
bool
d_tests_sa_table_find_row
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;
    int             search_val;
    ssize_t         found;

    struct d_test_table_row rows[] =
    {
        { 10, "a", 1.0 },
        { 20, "b", 2.0 },
        { 30, "c", 3.0 },
        { 20, "d", 4.0 }
    };

    result = true;

    // test 1: NULL table
    search_val = 10;
    found      = d_table_find_row(NULL, 0, &search_val,
                                  d_test_int_comparator);
    result     = d_assert_standalone(
        found == -1,
        "find_null_table",
        "NULL table should return -1",
        _counter) && result;

    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 4, g_search_cols,
                                g_search_col_count);

    if (tbl)
    {
        // test 2: NULL comparator
        found  = d_table_find_row(tbl, 0, &search_val, NULL);
        result = d_assert_standalone(
            found == -1,
            "find_null_comparator",
            "NULL comparator should return -1",
            _counter) && result;

        // test 3: find existing value (first match)
        search_val = 20;
        found      = d_table_find_row(tbl, 0, &search_val,
                                      d_test_int_comparator);
        result     = d_assert_standalone(
            found == 1,
            "find_first_match",
            "First occurrence of 20 should be at index 1",
            _counter) && result;

        // test 4: value not found
        search_val = 99;
        found      = d_table_find_row(tbl, 0, &search_val,
                                      d_test_int_comparator);
        result     = d_assert_standalone(
            found == -1,
            "find_not_found",
            "Value 99 should not be found",
            _counter) && result;

        // test 5: find in first row
        search_val = 10;
        found      = d_table_find_row(tbl, 0, &search_val,
                                      d_test_int_comparator);
        result     = d_assert_standalone(
            found == 0,
            "find_first_row",
            "Value 10 should be at index 0",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_find_row_last
  Tests the d_table_find_row_last function.
  Tests the following:
  - NULL table returns -1
  - finds last matching row among duplicates
  - value not found returns -1
*/
bool
d_tests_sa_table_find_row_last
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;
    int             search_val;
    ssize_t         found;

    struct d_test_table_row rows[] =
    {
        { 10, "a", 1.0 },
        { 20, "b", 2.0 },
        { 30, "c", 3.0 },
        { 20, "d", 4.0 }
    };

    result = true;

    // test 1: NULL table
    search_val = 20;
    found      = d_table_find_row_last(NULL, 0, &search_val,
                                       d_test_int_comparator);
    result     = d_assert_standalone(
        found == -1,
        "find_last_null_table",
        "NULL table should return -1",
        _counter) && result;

    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 4, g_search_cols,
                                g_search_col_count);

    if (tbl)
    {
        // test 2: finds last occurrence of duplicate
        search_val = 20;
        found      = d_table_find_row_last(tbl, 0, &search_val,
                                           d_test_int_comparator);
        result     = d_assert_standalone(
            found == 3,
            "find_last_duplicate",
            "Last occurrence of 20 should be at index 3",
            _counter) && result;

        // test 3: unique value
        search_val = 30;
        found      = d_table_find_row_last(tbl, 0, &search_val,
                                           d_test_int_comparator);
        result     = d_assert_standalone(
            found == 2,
            "find_last_unique",
            "Unique value 30 should be at index 2",
            _counter) && result;

        // test 4: not found
        search_val = 99;
        found      = d_table_find_row_last(tbl, 0, &search_val,
                                           d_test_int_comparator);
        result     = d_assert_standalone(
            found == -1,
            "find_last_not_found",
            "Value 99 should not be found",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_contains
  Tests the d_table_contains function.
  Tests the following:
  - NULL table returns false
  - existing value returns true
  - missing value returns false
*/
bool
d_tests_sa_table_contains
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;
    int             search_val;

    struct d_test_table_row rows[] =
    {
        { 10, "a", 1.0 },
        { 20, "b", 2.0 },
        { 30, "c", 3.0 }
    };

    result = true;

    // test 1: NULL table
    search_val = 10;
    result     = d_assert_standalone(
        d_table_contains(NULL, 0, &search_val,
                         d_test_int_comparator) == false,
        "contains_null_table",
        "NULL table should return false",
        _counter) && result;

    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 3, g_search_cols,
                                g_search_col_count);

    if (tbl)
    {
        // test 2: existing value
        search_val = 20;
        result     = d_assert_standalone(
            d_table_contains(tbl, 0, &search_val,
                             d_test_int_comparator) == true,
            "contains_found",
            "Contains should return true for 20",
            _counter) && result;

        // test 3: missing value
        search_val = 99;
        result     = d_assert_standalone(
            d_table_contains(tbl, 0, &search_val,
                             d_test_int_comparator) == false,
            "contains_not_found",
            "Contains should return false for 99",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_count_value
  Tests the d_table_count_value function.
  Tests the following:
  - NULL table returns 0
  - counts correct number of matches
  - returns 0 for missing value
*/
bool
d_tests_sa_table_count_value
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;
    int             search_val;

    struct d_test_table_row rows[] =
    {
        { 10, "a", 1.0 },
        { 20, "b", 2.0 },
        { 20, "c", 3.0 },
        { 30, "d", 4.0 },
        { 20, "e", 5.0 }
    };

    result = true;

    // test 1: NULL table returns 0
    search_val = 20;
    result     = d_assert_standalone(
        d_table_count_value(NULL, 0, &search_val,
                            d_test_int_comparator) == 0,
        "count_null_table",
        "NULL table should return 0",
        _counter) && result;

    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 5, g_search_cols,
                                g_search_col_count);

    if (tbl)
    {
        // test 2: count duplicates
        search_val = 20;
        result     = d_assert_standalone(
            d_table_count_value(tbl, 0, &search_val,
                                d_test_int_comparator) == 3,
            "count_three",
            "Count of 20 should be 3",
            _counter) && result;

        // test 3: count unique
        search_val = 10;
        result     = d_assert_standalone(
            d_table_count_value(tbl, 0, &search_val,
                                d_test_int_comparator) == 1,
            "count_one",
            "Count of 10 should be 1",
            _counter) && result;

        // test 4: count missing
        search_val = 99;
        result     = d_assert_standalone(
            d_table_count_value(tbl, 0, &search_val,
                                d_test_int_comparator) == 0,
            "count_zero",
            "Count of 99 should be 0",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_search_all
  Aggregation function that runs all search tests.
*/
bool
d_tests_sa_table_search_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Search Functions\n");
    printf("  ---------------------------\n");

    result = d_tests_sa_table_find_row(_counter) && result;
    result = d_tests_sa_table_find_row_last(_counter) && result;
    result = d_tests_sa_table_contains(_counter) && result;
    result = d_tests_sa_table_count_value(_counter) && result;

    return result;
}
