#include ".\table_tests_sa.h"


// shared column descriptors for utility tests
static const struct d_table_column_desc g_util_cols[] =
{
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
};

static const size_t g_util_col_count =
    D_TABLE_COLUMN_COUNT(g_util_cols);


// d_test_int_cmp
//   function: comparator for int values used in sort tests.
static int
d_test_int_cmp
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
d_tests_sa_table_swap_rows
  Tests the d_table_swap_rows function.
  Tests the following:
  - NULL table returns false
  - out-of-bounds returns false
  - successful swap exchanges row contents
  - swap with self is no-op
  - negative index support
*/
bool
d_tests_sa_table_swap_rows
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    struct d_test_table_row rows[] =
    {
        { 10, "a", 1.0 },
        { 20, "b", 2.0 },
        { 30, "c", 3.0 }
    };

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_swap_rows(NULL, 0, 1) == false,
        "swap_null_table",
        "NULL table should return false",
        _counter) && result;

    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 3, g_util_cols, g_util_col_count);

    if (tbl)
    {
        // test 2: successful swap
        result = d_assert_standalone(
            d_table_swap_rows(tbl, 0, 2) == true,
            "swap_success",
            "Swap should succeed",
            _counter) && result;

        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);
            struct d_test_table_row* r2 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 2);

            result = d_assert_standalone(
                r0->id == 30 && r2->id == 10,
                "swap_values",
                "Rows 0 and 2 should be swapped",
                _counter) && result;
        }

        // test 3: swap with self
        result = d_assert_standalone(
            d_table_swap_rows(tbl, 1, 1) == true,
            "swap_self",
            "Swap with self should succeed (no-op)",
            _counter) && result;

        // test 4: negative index
        result = d_assert_standalone(
            d_table_swap_rows(tbl, 0, -1) == true,
            "swap_negative",
            "Swap with negative index should succeed",
            _counter) && result;

        // test 5: out-of-bounds
        result = d_assert_standalone(
            d_table_swap_rows(tbl, 0, 99) == false,
            "swap_oob",
            "Out-of-bounds swap should return false",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_reverse
  Tests the d_table_reverse function.
  Tests the following:
  - NULL table returns false
  - empty table returns true (no-op)
  - single row returns true (no-op)
  - reversal of multiple rows
  - field integrity preserved after reversal
*/
bool
d_tests_sa_table_reverse
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    struct d_test_table_row rows[] =
    {
        { 10, "a", 1.0 },
        { 20, "b", 2.0 },
        { 30, "c", 3.0 },
        { 40, "d", 4.0 }
    };

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_reverse(NULL) == false,
        "reverse_null",
        "NULL table should return false",
        _counter) && result;

    // test 2: reversal of 4 rows
    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 4, g_util_cols, g_util_col_count);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_reverse(tbl) == true,
            "reverse_success",
            "Reverse should succeed",
            _counter) && result;

        // verify order: 40, 30, 20, 10
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);
            struct d_test_table_row* r1 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 1);
            struct d_test_table_row* r2 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 2);
            struct d_test_table_row* r3 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 3);

            result = d_assert_standalone(
                r0->id == 40 && r1->id == 30 &&
                r2->id == 20 && r3->id == 10,
                "reverse_order",
                "Rows should be [40, 30, 20, 10]",
                _counter) && result;
        }

        // test 3: field integrity preserved (name still matches id)
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);

            result = d_assert_standalone(
                strcmp(r0->name, "d") == 0 && r0->value == 4.0,
                "reverse_integrity",
                "Reversed row 0 should be {40, \"d\", 4.0}",
                _counter) && result;
        }

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_sort_by_column
  Tests the d_table_sort_by_column function.
  Tests the following:
  - NULL table returns false
  - NULL comparator returns false
  - successful sort by int column
  - field integrity preserved (fields move together)
  - empty table sort is no-op
*/
bool
d_tests_sa_table_sort_by_column
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    struct d_test_table_row rows[] =
    {
        { 30, "c", 3.0 },
        { 10, "a", 1.0 },
        { 50, "e", 5.0 },
        { 20, "b", 2.0 },
        { 40, "d", 4.0 }
    };

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_sort_by_column(NULL, 0, d_test_int_cmp) == false,
        "sort_null_table",
        "NULL table should return false",
        _counter) && result;

    // test 2: NULL comparator fails
    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 5, g_util_cols, g_util_col_count);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_sort_by_column(tbl, 0, NULL) == false,
            "sort_null_comparator",
            "NULL comparator should return false",
            _counter) && result;

        // test 3: successful sort by id (column 0)
        result = d_assert_standalone(
            d_table_sort_by_column(tbl, 0, d_test_int_cmp) == true,
            "sort_success",
            "Sort should succeed",
            _counter) && result;

        // verify ascending order
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);
            struct d_test_table_row* r1 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 1);
            struct d_test_table_row* r4 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 4);

            result = d_assert_standalone(
                r0->id == 10 && r1->id == 20 && r4->id == 50,
                "sort_order",
                "Rows should be sorted ascending by id",
                _counter) && result;
        }

        // test 4: field integrity — name and value travel with id
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);
            struct d_test_table_row* r4 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 4);

            result = d_assert_standalone(
                strcmp(r0->name, "a") == 0 && r0->value == 1.0,
                "sort_integrity_first",
                "First sorted row should be {10, \"a\", 1.0}",
                _counter) && result;

            result = d_assert_standalone(
                strcmp(r4->name, "e") == 0 && r4->value == 5.0,
                "sort_integrity_last",
                "Last sorted row should be {50, \"e\", 5.0}",
                _counter) && result;
        }

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_copy_to
  Tests the d_table_copy_to function.
  Tests the following:
  - NULL table returns false
  - NULL destination returns false
  - insufficient capacity returns false
  - successful copy writes all rows
*/
bool
d_tests_sa_table_copy_to
(
    struct d_test_counter* _counter
)
{
    bool                    result;
    struct d_table*         tbl;
    struct d_test_table_row dest[5];

    struct d_test_table_row rows[] =
    {
        { 10, "a", 1.0 },
        { 20, "b", 2.0 },
        { 30, "c", 3.0 }
    };

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_copy_to(NULL, dest, 5) == false,
        "copy_to_null_table",
        "NULL table should return false",
        _counter) && result;

    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 3, g_util_cols, g_util_col_count);

    if (tbl)
    {
        // test 2: NULL destination fails
        result = d_assert_standalone(
            d_table_copy_to(tbl, NULL, sizeof(dest)) == false,
            "copy_to_null_dest",
            "NULL destination should return false",
            _counter) && result;

        // test 3: insufficient capacity (1 byte) fails
        result = d_assert_standalone(
            d_table_copy_to(tbl, dest, 1) == false,
            "copy_to_insufficient",
            "Insufficient byte capacity should return false",
            _counter) && result;

        // test 4: successful copy
        d_memset(dest, 0, sizeof(dest));

        result = d_assert_standalone(
            d_table_copy_to(tbl, dest, sizeof(dest)) == true,
            "copy_to_success",
            "Copy should succeed",
            _counter) && result;

        result = d_assert_standalone(
            dest[0].id == 10 && dest[1].id == 20 && dest[2].id == 30,
            "copy_to_values",
            "Copied rows should have ids [10, 20, 30]",
            _counter) && result;

        result = d_assert_standalone(
            dest[0].name != NULL && strcmp(dest[0].name, "a") == 0 &&
            dest[2].name != NULL && strcmp(dest[2].name, "c") == 0,
            "copy_to_strings",
            "Copied names should match source",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_validate
  Tests the d_table_validate function.
  Tests the following:
  - NULL table returns false
  - valid table returns true
  - table with invalid struct_size returns false
*/
bool
d_tests_sa_table_validate
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;
    struct d_table  bad;

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_validate(NULL) == false,
        "validate_null",
        "NULL table should return false",
        _counter) && result;

    // test 2: valid table passes
    tbl = d_table_new(sizeof(struct d_test_table_row), g_util_cols,
                      g_util_col_count, 10);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_validate(tbl) == true,
            "validate_valid",
            "Valid table should pass validation",
            _counter) && result;

        d_table_free(tbl);
    }

    // test 3: zero struct_size fails
    d_memset(&bad, 0, sizeof(bad));
    bad.struct_size  = 0;
    bad.column_descs = (struct d_table_column_desc*)g_util_cols;
    bad.column_count = g_util_col_count;

    result = d_assert_standalone(
        d_table_validate(&bad) == false,
        "validate_zero_struct",
        "Zero struct_size should fail validation",
        _counter) && result;

    // test 4: NULL descs with column_count > 0 fails
    d_memset(&bad, 0, sizeof(bad));
    bad.struct_size  = sizeof(struct d_test_table_row);
    bad.column_descs = NULL;
    bad.column_count = 3;

    result = d_assert_standalone(
        d_table_validate(&bad) == false,
        "validate_null_descs",
        "NULL descs with columns > 0 should fail",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_utility_all
  Aggregation function that runs all utility tests.
*/
bool
d_tests_sa_table_utility_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Utility Functions\n");
    printf("  ----------------------------\n");

    result = d_tests_sa_table_swap_rows(_counter) && result;
    result = d_tests_sa_table_reverse(_counter) && result;
    result = d_tests_sa_table_sort_by_column(_counter) && result;
    result = d_tests_sa_table_copy_to(_counter) && result;
    result = d_tests_sa_table_validate(_counter) && result;

    return result;
}
