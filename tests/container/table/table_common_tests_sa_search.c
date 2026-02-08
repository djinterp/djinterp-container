#include ".\table_common_tests_sa.h"


// --- static helpers for search tests ---

// d_test_compare_int
//   comparator for int values; suitable for fn_comparator.
static int
d_test_compare_int
(
    const void* _a,
    const void* _b
)
{
    int va;
    int vb;

    va = *(const int*)_a;
    vb = *(const int*)_b;

    if (va < vb) return -1;
    if (va > vb) return  1;

    return 0;
}


/*
d_tests_sa_table_common_find_row
  Tests the d_table_common_find_row function.
  Tests the following:
  - NULL _data returns -1
  - zero _struct_size returns -1
  - zero _row_count returns -1
  - NULL _search_column returns -1
  - NULL _value returns -1
  - NULL _comparator returns -1
  - finds first matching row
  - returns first occurrence when duplicates exist
  - returns -1 when value not found
  - finds match at last row
*/
bool
d_tests_sa_table_common_find_row
(
    struct d_test_counter* _counter
)
{
    bool    result;
    ssize_t idx;
    int     search_val;

    struct d_test_table_row rows[] = D_TABLE_ROWS(
        { .id = 10, .name = "a", .value = 1.0 },
        { .id = 20, .name = "b", .value = 2.0 },
        { .id = 30, .name = "c", .value = 3.0 },
        { .id = 20, .name = "d", .value = 4.0 },
        { .id = 50, .name = "e", .value = 5.0 }
    );

    static const struct d_table_column_desc id_col =
        D_TABLE_COLUMN_FIELD(struct d_test_table_row, id);

    result = true;

    // test 1: NULL _data
    search_val = 10;
    idx        = d_table_common_find_row(
                     NULL, sizeof(struct d_test_table_row),
                     5, &id_col, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == -1,
        "find_row_null_data",
        "NULL data should return -1",
        _counter) && result;

    // test 2: zero struct_size
    idx    = d_table_common_find_row(
                 rows, 0,
                 5, &id_col, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == -1,
        "find_row_zero_struct_size",
        "Zero struct_size should return -1",
        _counter) && result;

    // test 3: zero row_count
    idx    = d_table_common_find_row(
                 rows, sizeof(struct d_test_table_row),
                 0, &id_col, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == -1,
        "find_row_zero_row_count",
        "Zero row_count should return -1",
        _counter) && result;

    // test 4: NULL search_column
    idx    = d_table_common_find_row(
                 rows, sizeof(struct d_test_table_row),
                 5, NULL, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == -1,
        "find_row_null_column",
        "NULL search_column should return -1",
        _counter) && result;

    // test 5: NULL value
    idx    = d_table_common_find_row(
                 rows, sizeof(struct d_test_table_row),
                 5, &id_col, NULL, d_test_compare_int);
    result = d_assert_standalone(
        idx == -1,
        "find_row_null_value",
        "NULL value should return -1",
        _counter) && result;

    // test 6: NULL comparator
    idx    = d_table_common_find_row(
                 rows, sizeof(struct d_test_table_row),
                 5, &id_col, &search_val, NULL);
    result = d_assert_standalone(
        idx == -1,
        "find_row_null_comparator",
        "NULL comparator should return -1",
        _counter) && result;

    // test 7: find first matching row (id == 10, index 0)
    search_val = 10;
    idx        = d_table_common_find_row(
                     rows, sizeof(struct d_test_table_row),
                     5, &id_col, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == 0,
        "find_row_first",
        "id=10 should be found at index 0",
        _counter) && result;

    // test 8: first occurrence of duplicate (id == 20 at index 1, not 3)
    search_val = 20;
    idx        = d_table_common_find_row(
                     rows, sizeof(struct d_test_table_row),
                     5, &id_col, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == 1,
        "find_row_first_duplicate",
        "First id=20 should be at index 1",
        _counter) && result;

    // test 9: value not found
    search_val = 999;
    idx        = d_table_common_find_row(
                     rows, sizeof(struct d_test_table_row),
                     5, &id_col, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == -1,
        "find_row_not_found",
        "Non-existent value should return -1",
        _counter) && result;

    // test 10: match at last row
    search_val = 50;
    idx        = d_table_common_find_row(
                     rows, sizeof(struct d_test_table_row),
                     5, &id_col, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == 4,
        "find_row_last_row",
        "id=50 should be found at index 4",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_find_row_last
  Tests the d_table_common_find_row_last function.
  Tests the following:
  - NULL _data returns -1
  - zero _struct_size returns -1
  - zero _row_count returns -1
  - NULL _search_column returns -1
  - NULL _value returns -1
  - NULL _comparator returns -1
  - finds last matching row
  - returns last occurrence when duplicates exist
  - returns -1 when value not found
  - finds match at first row (single occurrence)
*/
bool
d_tests_sa_table_common_find_row_last
(
    struct d_test_counter* _counter
)
{
    bool    result;
    ssize_t idx;
    int     search_val;

    struct d_test_table_row rows[] = D_TABLE_ROWS(
        { .id = 10, .name = "a", .value = 1.0 },
        { .id = 20, .name = "b", .value = 2.0 },
        { .id = 30, .name = "c", .value = 3.0 },
        { .id = 20, .name = "d", .value = 4.0 },
        { .id = 50, .name = "e", .value = 5.0 }
    );

    static const struct d_table_column_desc id_col =
        D_TABLE_COLUMN_FIELD(struct d_test_table_row, id);

    result = true;

    // test 1: NULL _data
    search_val = 10;
    idx        = d_table_common_find_row_last(
                     NULL, sizeof(struct d_test_table_row),
                     5, &id_col, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == -1,
        "find_row_last_null_data",
        "NULL data should return -1",
        _counter) && result;

    // test 2: zero struct_size
    idx    = d_table_common_find_row_last(
                 rows, 0,
                 5, &id_col, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == -1,
        "find_row_last_zero_struct_size",
        "Zero struct_size should return -1",
        _counter) && result;

    // test 3: zero row_count
    idx    = d_table_common_find_row_last(
                 rows, sizeof(struct d_test_table_row),
                 0, &id_col, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == -1,
        "find_row_last_zero_row_count",
        "Zero row_count should return -1",
        _counter) && result;

    // test 4: NULL search_column
    idx    = d_table_common_find_row_last(
                 rows, sizeof(struct d_test_table_row),
                 5, NULL, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == -1,
        "find_row_last_null_column",
        "NULL search_column should return -1",
        _counter) && result;

    // test 5: NULL value
    idx    = d_table_common_find_row_last(
                 rows, sizeof(struct d_test_table_row),
                 5, &id_col, NULL, d_test_compare_int);
    result = d_assert_standalone(
        idx == -1,
        "find_row_last_null_value",
        "NULL value should return -1",
        _counter) && result;

    // test 6: NULL comparator
    idx    = d_table_common_find_row_last(
                 rows, sizeof(struct d_test_table_row),
                 5, &id_col, &search_val, NULL);
    result = d_assert_standalone(
        idx == -1,
        "find_row_last_null_comparator",
        "NULL comparator should return -1",
        _counter) && result;

    // test 7: last occurrence of duplicate (id == 20 at index 3, not 1)
    search_val = 20;
    idx        = d_table_common_find_row_last(
                     rows, sizeof(struct d_test_table_row),
                     5, &id_col, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == 3,
        "find_row_last_duplicate",
        "Last id=20 should be at index 3",
        _counter) && result;

    // test 8: value not found
    search_val = 999;
    idx        = d_table_common_find_row_last(
                     rows, sizeof(struct d_test_table_row),
                     5, &id_col, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == -1,
        "find_row_last_not_found",
        "Non-existent value should return -1",
        _counter) && result;

    // test 9: unique value at first row
    search_val = 10;
    idx        = d_table_common_find_row_last(
                     rows, sizeof(struct d_test_table_row),
                     5, &id_col, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == 0,
        "find_row_last_at_first",
        "Unique id=10 should be found at index 0",
        _counter) && result;

    // test 10: unique value at last row
    search_val = 50;
    idx        = d_table_common_find_row_last(
                     rows, sizeof(struct d_test_table_row),
                     5, &id_col, &search_val, d_test_compare_int);
    result = d_assert_standalone(
        idx == 4,
        "find_row_last_at_last",
        "Unique id=50 should be found at index 4",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_search_all
  Aggregation function that runs all search function tests.
*/
bool
d_tests_sa_table_common_search_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Search Functions\n");
    printf("  ---------------------------\n");

    result = d_tests_sa_table_common_find_row(_counter)      && result;
    result = d_tests_sa_table_common_find_row_last(_counter)  && result;

    return result;
}
