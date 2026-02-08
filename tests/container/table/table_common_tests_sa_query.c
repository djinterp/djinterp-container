#include ".\table_common_tests_sa.h"


/*
d_tests_sa_table_common_is_empty
  Tests the D_TABLE_IS_EMPTY macro.
  Tests the following:
  - returns true for row_count == 0
  - returns false for row_count == 1
  - returns false for row_count == 100
*/
bool
d_tests_sa_table_common_is_empty
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    // test 1: zero rows
    result = d_assert_standalone(
        D_TABLE_IS_EMPTY(0) == true,
        "is_empty_zero",
        "D_TABLE_IS_EMPTY(0) should be true",
        _counter) && result;

    // test 2: one row
    result = d_assert_standalone(
        D_TABLE_IS_EMPTY(1) == false,
        "is_empty_one",
        "D_TABLE_IS_EMPTY(1) should be false",
        _counter) && result;

    // test 3: many rows
    result = d_assert_standalone(
        D_TABLE_IS_EMPTY(100) == false,
        "is_empty_many",
        "D_TABLE_IS_EMPTY(100) should be false",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_total_size
  Tests the D_TABLE_TOTAL_SIZE macro.
  Tests the following:
  - 0 rows yields 0 bytes
  - 1 row yields struct_size bytes
  - multiple rows yields row_count * struct_size bytes
*/
bool
d_tests_sa_table_common_total_size
(
    struct d_test_counter* _counter
)
{
    bool   result;
    size_t ss;

    result = true;
    ss     = sizeof(struct d_test_table_row);

    // test 1: zero rows
    result = d_assert_standalone(
        D_TABLE_TOTAL_SIZE(0, ss) == 0,
        "total_size_zero",
        "0 rows should yield 0 bytes",
        _counter) && result;

    // test 2: one row
    result = d_assert_standalone(
        D_TABLE_TOTAL_SIZE(1, ss) == ss,
        "total_size_one",
        "1 row should yield struct_size bytes",
        _counter) && result;

    // test 3: five rows
    result = d_assert_standalone(
        D_TABLE_TOTAL_SIZE(5, ss) == 5 * ss,
        "total_size_five",
        "5 rows should yield 5 * struct_size bytes",
        _counter) && result;

    // test 4: with a simple size (no struct padding concerns)
    result = d_assert_standalone(
        D_TABLE_TOTAL_SIZE(10, 4) == 40,
        "total_size_simple",
        "10 rows of 4 bytes should yield 40 bytes",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_is_valid_row
  Tests the D_TABLE_IS_VALID_ROW macro.
  Tests the following:
  - index 0 is valid in a non-empty table
  - last valid index is row_count - 1
  - index == row_count is invalid (out-of-bounds)
  - negative index -1 is valid (maps to last row)
  - negative index exceeding count is invalid
*/
bool
d_tests_sa_table_common_is_valid_row
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    // test 1: index 0 in a 5-row table
    result = d_assert_standalone(
        D_TABLE_IS_VALID_ROW(0, 5) != 0,
        "is_valid_row_zero",
        "Index 0 should be valid in a 5-row table",
        _counter) && result;

    // test 2: last valid index
    result = d_assert_standalone(
        D_TABLE_IS_VALID_ROW(4, 5) != 0,
        "is_valid_row_last",
        "Index 4 should be valid in a 5-row table",
        _counter) && result;

    // test 3: out-of-bounds (index == count)
    result = d_assert_standalone(
        D_TABLE_IS_VALID_ROW(5, 5) == 0,
        "is_valid_row_oob",
        "Index 5 should be invalid in a 5-row table",
        _counter) && result;

    // test 4: negative index -1 (last row)
    result = d_assert_standalone(
        D_TABLE_IS_VALID_ROW(-1, 5) != 0,
        "is_valid_row_neg",
        "Index -1 should be valid in a 5-row table",
        _counter) && result;

    // test 5: negative index exceeding count
    result = d_assert_standalone(
        D_TABLE_IS_VALID_ROW(-6, 5) == 0,
        "is_valid_row_neg_oob",
        "Index -6 should be invalid in a 5-row table",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_is_valid_column
  Tests the D_TABLE_IS_VALID_COLUMN macro.
  Tests the following:
  - index 0 is valid
  - last column is valid
  - index == column_count is invalid
  - negative index -1 is valid
*/
bool
d_tests_sa_table_common_is_valid_column
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    // test 1: column 0 in a 3-column table
    result = d_assert_standalone(
        D_TABLE_IS_VALID_COLUMN(0, 3) != 0,
        "is_valid_col_zero",
        "Column 0 should be valid in a 3-column table",
        _counter) && result;

    // test 2: last column
    result = d_assert_standalone(
        D_TABLE_IS_VALID_COLUMN(2, 3) != 0,
        "is_valid_col_last",
        "Column 2 should be valid in a 3-column table",
        _counter) && result;

    // test 3: out-of-bounds
    result = d_assert_standalone(
        D_TABLE_IS_VALID_COLUMN(3, 3) == 0,
        "is_valid_col_oob",
        "Column 3 should be invalid in a 3-column table",
        _counter) && result;

    // test 4: negative index
    result = d_assert_standalone(
        D_TABLE_IS_VALID_COLUMN(-1, 3) != 0,
        "is_valid_col_neg",
        "Column -1 should be valid in a 3-column table",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_is_valid_cell
  Tests the D_TABLE_IS_VALID_CELL macro.
  Tests the following:
  - both indices valid → true
  - valid row but invalid column → false
  - invalid row but valid column → false
  - both indices invalid → false
*/
bool
d_tests_sa_table_common_is_valid_cell
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    // test 1: both valid
    result = d_assert_standalone(
        D_TABLE_IS_VALID_CELL(0, 0, 5, 3) != 0,
        "is_valid_cell_both_valid",
        "(0,0) should be valid in a 5x3 table",
        _counter) && result;

    // test 2: valid row, invalid column
    result = d_assert_standalone(
        D_TABLE_IS_VALID_CELL(0, 3, 5, 3) == 0,
        "is_valid_cell_bad_col",
        "(0,3) should be invalid in a 5x3 table",
        _counter) && result;

    // test 3: invalid row, valid column
    result = d_assert_standalone(
        D_TABLE_IS_VALID_CELL(5, 0, 5, 3) == 0,
        "is_valid_cell_bad_row",
        "(5,0) should be invalid in a 5x3 table",
        _counter) && result;

    // test 4: both invalid
    result = d_assert_standalone(
        D_TABLE_IS_VALID_CELL(10, 10, 5, 3) == 0,
        "is_valid_cell_both_bad",
        "(10,10) should be invalid in a 5x3 table",
        _counter) && result;

    // test 5: negative indices both valid
    result = d_assert_standalone(
        D_TABLE_IS_VALID_CELL(-1, -1, 5, 3) != 0,
        "is_valid_cell_neg",
        "(-1,-1) should be valid in a 5x3 table",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_query_all
  Aggregation function that runs all query macro tests.
*/
bool
d_tests_sa_table_common_query_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Query Macros\n");
    printf("  -----------------------\n");

    result = d_tests_sa_table_common_is_empty(_counter)      && result;
    result = d_tests_sa_table_common_total_size(_counter)     && result;
    result = d_tests_sa_table_common_is_valid_row(_counter)   && result;
    result = d_tests_sa_table_common_is_valid_column(_counter) && result;
    result = d_tests_sa_table_common_is_valid_cell(_counter)  && result;

    return result;
}
