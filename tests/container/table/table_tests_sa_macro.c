#include ".\table_tests_sa.h"


// shared column descriptors for macro tests
static const struct d_table_column_desc g_macro_cols[] =
{
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
};

static const size_t g_macro_col_count =
    D_TABLE_COLUMN_COUNT(g_macro_cols);


/*
d_tests_sa_table_view_macro
  Tests the D_TABLE_VIEW macro.
  Tests the following:
  - view wraps an existing array correctly
  - row_count matches the source array
  - capacity is 0 (non-owning)
  - flags are D_TABLE_FLAG_NONE
  - data pointer points to the original array
*/
bool
d_tests_sa_table_view_macro
(
    struct d_test_counter* _counter
)
{
    bool result;

    struct d_test_table_row rows[] =
    {
        { 10, "alpha",   1.0 },
        { 20, "bravo",   2.0 },
        { 30, "charlie", 3.0 }
    };

    struct d_table view = D_TABLE_VIEW(
        rows,
        sizeof(struct d_test_table_row),
        D_TABLE_ROW_COUNT(struct d_test_table_row, rows),
        g_macro_cols,
        g_macro_col_count);

    result = true;

    // test 1: data points to original array
    result = d_assert_standalone(
        view.data == (void*)rows,
        "view_data_ptr",
        "View data should point to the original array",
        _counter) && result;

    // test 2: row count matches source
    result = d_assert_standalone(
        view.row_count == 3,
        "view_row_count",
        "View should have 3 rows",
        _counter) && result;

    // test 3: struct_size matches
    result = d_assert_standalone(
        view.struct_size == sizeof(struct d_test_table_row),
        "view_struct_size",
        "View struct_size should match row struct",
        _counter) && result;

    // test 4: column_count matches
    result = d_assert_standalone(
        view.column_count == 3,
        "view_column_count",
        "View should have 3 columns",
        _counter) && result;

    // test 5: capacity is 0 (view, not owning)
    result = d_assert_standalone(
        view.capacity == 0,
        "view_capacity_zero",
        "View capacity should be 0",
        _counter) && result;

    // test 6: flags are NONE (no ownership)
    result = d_assert_standalone(
        view.flags == D_TABLE_FLAG_NONE,
        "view_flags_none",
        "View flags should be D_TABLE_FLAG_NONE",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_empty_macro
  Tests the D_TABLE_EMPTY macro.
  Tests the following:
  - empty table has NULL data
  - row_count is 0
  - column_count matches descriptor array
  - capacity is 0
  - flags are D_TABLE_FLAG_NONE
*/
bool
d_tests_sa_table_empty_macro
(
    struct d_test_counter* _counter
)
{
    bool result;

    struct d_table empty = D_TABLE_EMPTY(
        sizeof(struct d_test_table_row),
        g_macro_cols,
        g_macro_col_count);

    result = true;

    // test 1: data is NULL
    result = d_assert_standalone(
        empty.data == NULL,
        "empty_data_null",
        "Empty table data should be NULL",
        _counter) && result;

    // test 2: row_count is 0
    result = d_assert_standalone(
        empty.row_count == 0,
        "empty_row_count",
        "Empty table should have 0 rows",
        _counter) && result;

    // test 3: column_count matches
    result = d_assert_standalone(
        empty.column_count == 3,
        "empty_column_count",
        "Empty table should have 3 columns",
        _counter) && result;

    // test 4: struct_size matches
    result = d_assert_standalone(
        empty.struct_size == sizeof(struct d_test_table_row),
        "empty_struct_size",
        "Empty table struct_size should match row struct",
        _counter) && result;

    // test 5: capacity is 0
    result = d_assert_standalone(
        empty.capacity == 0,
        "empty_capacity",
        "Empty table capacity should be 0",
        _counter) && result;

    // test 6: flags are NONE
    result = d_assert_standalone(
        empty.flags == D_TABLE_FLAG_NONE,
        "empty_flags",
        "Empty table flags should be D_TABLE_FLAG_NONE",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_row_ptr_macro
  Tests the D_TABLE_ROW_PTR macro.
  Tests the following:
  - row 0 returns pointer to the first row
  - row 1 returns pointer to the second row
  - dereferenced row has correct field values
*/
bool
d_tests_sa_table_row_ptr_macro
(
    struct d_test_counter* _counter
)
{
    bool  result;
    void* ptr;

    struct d_test_table_row rows[] =
    {
        { 10, "alpha", 1.0 },
        { 20, "bravo", 2.0 }
    };

    struct d_table view = D_TABLE_VIEW(
        rows, sizeof(struct d_test_table_row), 2,
        g_macro_cols, g_macro_col_count);

    result = true;

    // test 1: row 0 points to start of array
    ptr    = D_TABLE_ROW_PTR(&view, 0);
    result = d_assert_standalone(
        ptr == (void*)&rows[0],
        "row_ptr_first",
        "D_TABLE_ROW_PTR(0) should point to rows[0]",
        _counter) && result;

    // test 2: row 1 points to second element
    ptr    = D_TABLE_ROW_PTR(&view, 1);
    result = d_assert_standalone(
        ptr == (void*)&rows[1],
        "row_ptr_second",
        "D_TABLE_ROW_PTR(1) should point to rows[1]",
        _counter) && result;

    // test 3: dereferenced row has correct id
    result = d_assert_standalone(
        ((struct d_test_table_row*)ptr)->id == 20,
        "row_ptr_deref_id",
        "Dereferenced row 1 id should be 20",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_row_as_macro
  Tests the D_TABLE_ROW_AS macro.
  Tests the following:
  - returns correctly typed pointer
  - typed access yields correct field values
*/
bool
d_tests_sa_table_row_as_macro
(
    struct d_test_counter* _counter
)
{
    bool                     result;
    struct d_test_table_row* typed;

    struct d_test_table_row rows[] =
    {
        { 42, "typed", 9.9 }
    };

    struct d_table view = D_TABLE_VIEW(
        rows, sizeof(struct d_test_table_row), 1,
        g_macro_cols, g_macro_col_count);

    result = true;

    typed = D_TABLE_ROW_AS(struct d_test_table_row*, &view, 0);

    // test 1: typed pointer is not NULL
    result = d_assert_standalone(
        typed != NULL,
        "row_as_not_null",
        "D_TABLE_ROW_AS should return non-NULL",
        _counter) && result;

    // test 2: id field matches
    result = d_assert_standalone(
        typed->id == 42,
        "row_as_id",
        "Typed row id should be 42",
        _counter) && result;

    // test 3: name field matches
    result = d_assert_standalone(
        strcmp(typed->name, "typed") == 0,
        "row_as_name",
        "Typed row name should be \"typed\"",
        _counter) && result;

    // test 4: value field matches
    result = d_assert_standalone(
        typed->value == 9.9,
        "row_as_value",
        "Typed row value should be 9.9",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_cell_ptr_macro
  Tests the D_TABLE_CELL_PTR macro.
  Tests the following:
  - cell (0,0) points to first row id
  - cell (1,1) points to second row name
  - cell (0,2) points to first row value
*/
bool
d_tests_sa_table_cell_ptr_macro
(
    struct d_test_counter* _counter
)
{
    bool  result;
    void* ptr;

    struct d_test_table_row rows[] =
    {
        { 10, "alpha", 1.5 },
        { 20, "bravo", 2.5 }
    };

    struct d_table view = D_TABLE_VIEW(
        rows, sizeof(struct d_test_table_row), 2,
        g_macro_cols, g_macro_col_count);

    result = true;

    // test 1: cell (0,0) — first row id
    ptr    = D_TABLE_CELL_PTR(&view, 0, 0);
    result = d_assert_standalone(
        *(int*)ptr == 10,
        "cell_ptr_0_0",
        "Cell (0,0) should dereference to id=10",
        _counter) && result;

    // test 2: cell (1,1) — second row name
    ptr    = D_TABLE_CELL_PTR(&view, 1, 1);
    result = d_assert_standalone(
        strcmp(*(const char**)ptr, "bravo") == 0,
        "cell_ptr_1_1",
        "Cell (1,1) should dereference to name=\"bravo\"",
        _counter) && result;

    // test 3: cell (0,2) — first row value
    ptr    = D_TABLE_CELL_PTR(&view, 0, 2);
    result = d_assert_standalone(
        *(double*)ptr == 1.5,
        "cell_ptr_0_2",
        "Cell (0,2) should dereference to value=1.5",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_cell_val_macro
  Tests the D_TABLE_CELL_VAL macro.
  Tests the following:
  - reads int cell correctly
  - reads double cell correctly
  - reads const char* cell correctly
*/
bool
d_tests_sa_table_cell_val_macro
(
    struct d_test_counter* _counter
)
{
    bool result;

    struct d_test_table_row rows[] =
    {
        { 77, "gamma", 7.7 }
    };

    struct d_table view = D_TABLE_VIEW(
        rows, sizeof(struct d_test_table_row), 1,
        g_macro_cols, g_macro_col_count);

    result = true;

    // test 1: read int cell
    result = d_assert_standalone(
        D_TABLE_CELL_VAL(int, &view, 0, 0) == 77,
        "cell_val_int",
        "CELL_VAL(int, 0, 0) should be 77",
        _counter) && result;

    // test 2: read const char* cell
    result = d_assert_standalone(
        strcmp(D_TABLE_CELL_VAL(const char*, &view, 0, 1), "gamma") == 0,
        "cell_val_str",
        "CELL_VAL(const char*, 0, 1) should be \"gamma\"",
        _counter) && result;

    // test 3: read double cell
    result = d_assert_standalone(
        D_TABLE_CELL_VAL(double, &view, 0, 2) == 7.7,
        "cell_val_double",
        "CELL_VAL(double, 0, 2) should be 7.7",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_macro_all
  Aggregation function that runs all macro tests.
*/
bool
d_tests_sa_table_macro_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Static Initialization & Accessor Macros\n");
    printf("  ---------------------------------------------------\n");

    result = d_tests_sa_table_view_macro(_counter) && result;
    result = d_tests_sa_table_empty_macro(_counter) && result;
    result = d_tests_sa_table_row_ptr_macro(_counter) && result;
    result = d_tests_sa_table_row_as_macro(_counter) && result;
    result = d_tests_sa_table_cell_ptr_macro(_counter) && result;
    result = d_tests_sa_table_cell_val_macro(_counter) && result;

    return result;
}
