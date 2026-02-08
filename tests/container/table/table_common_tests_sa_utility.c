#include ".\table_common_tests_sa.h"


// --- static helpers for utility tests ---

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
d_tests_sa_table_common_swap_rows
  Tests the d_table_common_swap_rows function.
  Tests the following:
  - NULL _data returns false
  - zero _struct_size returns false
  - zero _row_count returns false
  - out-of-bounds positive index returns false
  - out-of-bounds negative index returns false
  - swapping a row with itself is a no-op (returns true)
  - successful swap of two distinct rows
  - negative index swap
*/
bool
d_tests_sa_table_common_swap_rows
(
    struct d_test_counter* _counter
)
{
    bool result;

    struct d_test_table_row rows[] = D_TABLE_ROWS(
        { .id = 10, .name = "a", .value = 1.0 },
        { .id = 20, .name = "b", .value = 2.0 },
        { .id = 30, .name = "c", .value = 3.0 }
    );

    size_t ss;

    result = true;
    ss     = sizeof(struct d_test_table_row);

    // test 1: NULL _data
    result = d_assert_standalone(
        d_table_common_swap_rows(NULL, ss, 3, 0, 1) == false,
        "swap_rows_null_data",
        "NULL data should return false",
        _counter) && result;

    // test 2: zero struct_size
    result = d_assert_standalone(
        d_table_common_swap_rows(rows, 0, 3, 0, 1) == false,
        "swap_rows_zero_struct_size",
        "Zero struct_size should return false",
        _counter) && result;

    // test 3: zero row_count
    result = d_assert_standalone(
        d_table_common_swap_rows(rows, ss, 0, 0, 1) == false,
        "swap_rows_zero_row_count",
        "Zero row_count should return false",
        _counter) && result;

    // test 4: out-of-bounds positive index
    result = d_assert_standalone(
        d_table_common_swap_rows(rows, ss, 3, 0, 5) == false,
        "swap_rows_oob_positive",
        "Out-of-bounds positive index should return false",
        _counter) && result;

    // test 5: out-of-bounds negative index
    result = d_assert_standalone(
        d_table_common_swap_rows(rows, ss, 3, -4, 0) == false,
        "swap_rows_oob_negative",
        "Out-of-bounds negative index should return false",
        _counter) && result;

    // test 6: swap row with itself (no-op)
    result = d_assert_standalone(
        d_table_common_swap_rows(rows, ss, 3, 1, 1) == true,
        "swap_rows_self",
        "Swapping a row with itself should return true",
        _counter) && result;

    result = d_assert_standalone(
        rows[1].id == 20,
        "swap_rows_self_unchanged",
        "Row should be unchanged after self-swap",
        _counter) && result;

    // test 7: successful swap of rows 0 and 2
    // before: rows[0].id=10, rows[2].id=30
    result = d_assert_standalone(
        d_table_common_swap_rows(rows, ss, 3, 0, 2) == true,
        "swap_rows_success",
        "Swap of rows 0 and 2 should succeed",
        _counter) && result;

    result = d_assert_standalone(
        rows[0].id == 30 && rows[2].id == 10,
        "swap_rows_values",
        "After swap: rows[0].id=30, rows[2].id=10",
        _counter) && result;

    result = d_assert_standalone(
        rows[0].value == 3.0 && rows[2].value == 1.0,
        "swap_rows_values_double",
        "After swap: rows[0].value=3.0, rows[2].value=1.0",
        _counter) && result;

    // test 8: middle row unchanged
    result = d_assert_standalone(
        rows[1].id == 20 && rows[1].value == 2.0,
        "swap_rows_middle_unchanged",
        "Middle row should be unchanged after swap",
        _counter) && result;

    // test 9: negative index swap (-1 swaps with last row)
    // current state: [30, 20, 10]; swap(-1, 0) → swap(2, 0) → [10, 20, 30]
    result = d_assert_standalone(
        d_table_common_swap_rows(rows, ss, 3, -1, 0) == true,
        "swap_rows_negative",
        "Negative index swap should succeed",
        _counter) && result;

    result = d_assert_standalone(
        rows[0].id == 10 && rows[2].id == 30,
        "swap_rows_negative_values",
        "After negative swap: rows restored to [10, 20, 30]",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_sort_by_column
  Tests the d_table_common_sort_by_column function.
  Tests the following:
  - NULL _data returns false
  - zero _struct_size returns false
  - NULL _sort_column returns false
  - NULL _comparator returns false
  - zero rows is already sorted (returns true)
  - one row is already sorted (returns true)
  - sorts multiple rows in ascending order
  - preserves row integrity (all fields move together)
*/
bool
d_tests_sa_table_common_sort_by_column
(
    struct d_test_counter* _counter
)
{
    bool result;

    struct d_test_table_row rows[] = D_TABLE_ROWS(
        { .id = 30, .name = "c", .value = 3.0 },
        { .id = 10, .name = "a", .value = 1.0 },
        { .id = 50, .name = "e", .value = 5.0 },
        { .id = 20, .name = "b", .value = 2.0 },
        { .id = 40, .name = "d", .value = 4.0 }
    );

    static const struct d_table_column_desc id_col =
        D_TABLE_COLUMN_FIELD(struct d_test_table_row, id);

    struct d_test_table_row single[] = D_TABLE_ROWS(
        { .id = 42, .name = "x", .value = 9.9 }
    );

    size_t ss;

    result = true;
    ss     = sizeof(struct d_test_table_row);

    // test 1: NULL _data
    result = d_assert_standalone(
        d_table_common_sort_by_column(NULL, ss, 5,
            &id_col, d_test_compare_int) == false,
        "sort_null_data",
        "NULL data should return false",
        _counter) && result;

    // test 2: zero struct_size
    result = d_assert_standalone(
        d_table_common_sort_by_column(rows, 0, 5,
            &id_col, d_test_compare_int) == false,
        "sort_zero_struct_size",
        "Zero struct_size should return false",
        _counter) && result;

    // test 3: NULL sort_column
    result = d_assert_standalone(
        d_table_common_sort_by_column(rows, ss, 5,
            NULL, d_test_compare_int) == false,
        "sort_null_column",
        "NULL sort_column should return false",
        _counter) && result;

    // test 4: NULL comparator
    result = d_assert_standalone(
        d_table_common_sort_by_column(rows, ss, 5,
            &id_col, NULL) == false,
        "sort_null_comparator",
        "NULL comparator should return false",
        _counter) && result;

    // test 5: zero rows
    result = d_assert_standalone(
        d_table_common_sort_by_column(rows, ss, 0,
            &id_col, d_test_compare_int) == true,
        "sort_zero_rows",
        "Zero rows should return true (already sorted)",
        _counter) && result;

    // test 6: one row
    result = d_assert_standalone(
        d_table_common_sort_by_column(single, ss, 1,
            &id_col, d_test_compare_int) == true,
        "sort_one_row",
        "One row should return true (already sorted)",
        _counter) && result;

    result = d_assert_standalone(
        single[0].id == 42,
        "sort_one_row_unchanged",
        "Single row should be unchanged",
        _counter) && result;

    // test 7: sort 5 rows by id ascending
    result = d_assert_standalone(
        d_table_common_sort_by_column(rows, ss, 5,
            &id_col, d_test_compare_int) == true,
        "sort_success",
        "Sort should succeed",
        _counter) && result;

    result = d_assert_standalone(
        rows[0].id == 10 &&
        rows[1].id == 20 &&
        rows[2].id == 30 &&
        rows[3].id == 40 &&
        rows[4].id == 50,
        "sort_order",
        "Rows should be sorted ascending by id: 10,20,30,40,50",
        _counter) && result;

    // test 8: row integrity preserved (fields moved together)
    result = d_assert_standalone(
        strcmp(rows[0].name, "a") == 0 &&
        rows[0].value == 1.0,
        "sort_integrity_first",
        "First sorted row should be {10, \"a\", 1.0}",
        _counter) && result;

    result = d_assert_standalone(
        strcmp(rows[4].name, "e") == 0 &&
        rows[4].value == 5.0,
        "sort_integrity_last",
        "Last sorted row should be {50, \"e\", 5.0}",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_copy_to
  Tests the d_table_common_copy_to function.
  Tests the following:
  - NULL _source returns false
  - zero _struct_size returns false
  - NULL _destination returns false
  - zero _row_count is a valid no-op (returns true)
  - insufficient destination capacity returns false
  - successful copy with exact capacity
  - successful copy with excess capacity
  - data faithfully reproduced
*/
bool
d_tests_sa_table_common_copy_to
(
    struct d_test_counter* _counter
)
{
    bool result;

    struct d_test_table_row source[] = D_TABLE_ROWS(
        { .id = 1, .name = "x", .value = 0.1 },
        { .id = 2, .name = "y", .value = 0.2 },
        { .id = 3, .name = "z", .value = 0.3 }
    );

    struct d_test_table_row dest[4];
    size_t                  ss;
    size_t                  exact_cap;

    result    = true;
    ss        = sizeof(struct d_test_table_row);
    exact_cap = 3 * ss;

    // test 1: NULL source
    result = d_assert_standalone(
        d_table_common_copy_to(NULL, ss, 3, dest, exact_cap) == false,
        "copy_to_null_source",
        "NULL source should return false",
        _counter) && result;

    // test 2: zero struct_size
    result = d_assert_standalone(
        d_table_common_copy_to(source, 0, 3, dest, exact_cap) == false,
        "copy_to_zero_struct_size",
        "Zero struct_size should return false",
        _counter) && result;

    // test 3: NULL destination
    result = d_assert_standalone(
        d_table_common_copy_to(source, ss, 3, NULL, exact_cap) == false,
        "copy_to_null_dest",
        "NULL destination should return false",
        _counter) && result;

    // test 4: zero row_count (valid no-op)
    result = d_assert_standalone(
        d_table_common_copy_to(source, ss, 0, dest, exact_cap) == true,
        "copy_to_zero_rows",
        "Zero row_count should return true",
        _counter) && result;

    // test 5: insufficient capacity
    result = d_assert_standalone(
        d_table_common_copy_to(source, ss, 3, dest, ss) == false,
        "copy_to_insufficient",
        "Insufficient destination capacity should return false",
        _counter) && result;

    // test 6: successful copy with exact capacity
    result = d_assert_standalone(
        d_table_common_copy_to(source, ss, 3, dest, exact_cap) == true,
        "copy_to_exact",
        "Copy with exact capacity should succeed",
        _counter) && result;

    // test 7: data faithfully reproduced
    result = d_assert_standalone(
        dest[0].id == 1 && dest[1].id == 2 && dest[2].id == 3,
        "copy_to_ids",
        "Destination ids should be 1, 2, 3",
        _counter) && result;

    result = d_assert_standalone(
        dest[0].value == 0.1 &&
        dest[1].value == 0.2 &&
        dest[2].value == 0.3,
        "copy_to_values",
        "Destination values should be 0.1, 0.2, 0.3",
        _counter) && result;

    // test 8: successful copy with excess capacity
    result = d_assert_standalone(
        d_table_common_copy_to(source, ss, 3,
                               dest, sizeof(dest)) == true,
        "copy_to_excess",
        "Copy with excess capacity should succeed",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_utility_all
  Aggregation function that runs all utility function tests.
*/
bool
d_tests_sa_table_common_utility_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Utility Functions\n");
    printf("  ----------------------------\n");

    result = d_tests_sa_table_common_swap_rows(_counter)      && result;
    result = d_tests_sa_table_common_sort_by_column(_counter) && result;
    result = d_tests_sa_table_common_copy_to(_counter)        && result;

    return result;
}
