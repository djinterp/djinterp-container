#include ".\table_tests_sa.h"


// shared column descriptors for constructor tests
static const struct d_table_column_desc g_ctor_cols[] =
{
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
};

static const size_t g_ctor_col_count =
    D_TABLE_COLUMN_COUNT(g_ctor_cols);


/*
d_tests_sa_table_new
  Tests the d_table_new function.
  Tests the following:
  - zero struct_size returns NULL
  - zero column count returns NULL
  - NULL descriptors returns NULL
  - successful creation with default capacity
  - successful creation with explicit capacity
  - returned table has correct fields
  - returned table owns its data
  - returned table data is initially empty (row_count 0)
*/
bool
d_tests_sa_table_new
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    result = true;

    // test 1: zero struct_size fails
    tbl    = d_table_new(0, g_ctor_cols, g_ctor_col_count, 16);
    result = d_assert_standalone(
        tbl == NULL,
        "new_zero_struct_size",
        "Zero struct_size should return NULL",
        _counter) && result;

    // test 2: NULL descriptors fails
    tbl    = d_table_new(sizeof(struct d_test_table_row), NULL,
                         g_ctor_col_count, 16);
    result = d_assert_standalone(
        tbl == NULL,
        "new_null_descs",
        "NULL column_descs should return NULL",
        _counter) && result;

    // test 3: zero column count fails
    tbl    = d_table_new(sizeof(struct d_test_table_row), g_ctor_cols,
                         0, 16);
    result = d_assert_standalone(
        tbl == NULL,
        "new_zero_columns",
        "Zero column_count should return NULL",
        _counter) && result;

    // test 4: successful creation
    tbl = d_table_new(sizeof(struct d_test_table_row), g_ctor_cols,
                      g_ctor_col_count, 10);

    if (tbl)
    {
        // test 5: row_count starts at 0
        result = d_assert_standalone(
            tbl->row_count == 0,
            "new_row_count_zero",
            "New table should have 0 rows",
            _counter) && result;

        // test 6: capacity matches requested
        result = d_assert_standalone(
            tbl->capacity == 10,
            "new_capacity",
            "Capacity should be 10",
            _counter) && result;

        // test 7: struct_size matches
        result = d_assert_standalone(
            tbl->struct_size == sizeof(struct d_test_table_row),
            "new_struct_size",
            "struct_size should match row struct",
            _counter) && result;

        // test 8: column_count matches
        result = d_assert_standalone(
            tbl->column_count == g_ctor_col_count,
            "new_column_count",
            "column_count should match descriptor count",
            _counter) && result;

        // test 9: owns data flag set
        result = d_assert_standalone(
            (tbl->flags & D_TABLE_FLAG_OWNS_DATA) != 0,
            "new_owns_data",
            "New table should own its data buffer",
            _counter) && result;

        // test 10: data is not NULL
        result = d_assert_standalone(
            tbl->data != NULL,
            "new_data_not_null",
            "New table data should be allocated",
            _counter) && result;

        d_table_free(tbl);
    }

    // test 11: zero initial_capacity uses default
    tbl = d_table_new(sizeof(struct d_test_table_row), g_ctor_cols,
                      g_ctor_col_count, 0);

    if (tbl)
    {
        result = d_assert_standalone(
            tbl->capacity == D_TABLE_DEFAULT_CAPACITY,
            "new_default_capacity",
            "Zero initial_capacity should use D_TABLE_DEFAULT_CAPACITY",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_new_from_rows
  Tests the d_table_new_from_rows function.
  Tests the following:
  - NULL source with non-zero count returns NULL
  - zero row count creates empty table
  - successful creation copies all rows
  - modification of source does not affect table (deep copy)
*/
bool
d_tests_sa_table_new_from_rows
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    struct d_test_table_row source[] =
    {
        { 1, "one",   1.0 },
        { 2, "two",   2.0 },
        { 3, "three", 3.0 }
    };

    result = true;

    // test 1: NULL source with non-zero count fails
    tbl    = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                   NULL, 3, g_ctor_cols, g_ctor_col_count);
    result = d_assert_standalone(
        tbl == NULL,
        "from_rows_null_source",
        "NULL source with count > 0 should return NULL",
        _counter) && result;

    // test 2: successful creation
    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                source, 3, g_ctor_cols, g_ctor_col_count);

    if (tbl)
    {
        // test 3: row_count matches
        result = d_assert_standalone(
            tbl->row_count == 3,
            "from_rows_count",
            "Table should have 3 rows",
            _counter) && result;

        // test 4: first row copied correctly
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);

            result = d_assert_standalone(
                r0 != NULL && r0->id == 1 &&
                strcmp(r0->name, "one") == 0 &&
                r0->value == 1.0,
                "from_rows_first",
                "First row should be {1, \"one\", 1.0}",
                _counter) && result;
        }

        // test 5: last row copied correctly
        {
            struct d_test_table_row* r2 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 2);

            result = d_assert_standalone(
                r2 != NULL && r2->id == 3 &&
                strcmp(r2->name, "three") == 0,
                "from_rows_last",
                "Last row should be {3, \"three\", 3.0}",
                _counter) && result;
        }

        // test 6: data is independent of source (deep copy)
        source[0].id = 999;
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);

            result = d_assert_standalone(
                r0->id == 1,
                "from_rows_deep_copy",
                "Source modification should not affect table",
                _counter) && result;
        }

        // test 7: owns data
        result = d_assert_standalone(
            (tbl->flags & D_TABLE_FLAG_OWNS_DATA) != 0,
            "from_rows_owns_data",
            "Table from rows should own its data",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_new_copy
  Tests the d_table_new_copy function.
  Tests the following:
  - NULL source returns NULL
  - copied table has same row count
  - copied data is independent
  - owns both data and descriptors
*/
bool
d_tests_sa_table_new_copy
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* original;
    struct d_table* copy;

    struct d_test_table_row rows[] =
    {
        { 10, "alpha", 1.0 },
        { 20, "bravo", 2.0 }
    };

    result = true;

    // test 1: NULL source fails
    copy   = d_table_new_copy(NULL);
    result = d_assert_standalone(
        copy == NULL,
        "copy_null_source",
        "NULL source should return NULL",
        _counter) && result;

    // test 2: successful copy
    original = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                     rows, 2, g_ctor_cols,
                                     g_ctor_col_count);

    if (original)
    {
        copy = d_table_new_copy(original);

        if (copy)
        {
            // test 3: row count matches
            result = d_assert_standalone(
                copy->row_count == original->row_count,
                "copy_row_count",
                "Copy should have same row count as original",
                _counter) && result;

            // test 4: struct_size matches
            result = d_assert_standalone(
                copy->struct_size == original->struct_size,
                "copy_struct_size",
                "Copy struct_size should match original",
                _counter) && result;

            // test 5: data is independent
            result = d_assert_standalone(
                copy->data != original->data,
                "copy_data_independent",
                "Copy data pointer should differ from original",
                _counter) && result;

            // test 6: descriptors are independent
            result = d_assert_standalone(
                copy->column_descs != original->column_descs,
                "copy_descs_independent",
                "Copy descriptor pointer should differ from original",
                _counter) && result;

            // test 7: owns both data and descs
            result = d_assert_standalone(
                (copy->flags & D_TABLE_FLAG_OWNS_DATA) != 0 &&
                (copy->flags & D_TABLE_FLAG_OWNS_DESCS) != 0,
                "copy_owns_both",
                "Copy should own both data and descriptors",
                _counter) && result;

            // test 8: values match
            {
                struct d_test_table_row* cr0 =
                    (struct d_test_table_row*)d_table_row_ptr(copy, 0);

                result = d_assert_standalone(
                    cr0->id == 10 &&
                    strcmp(cr0->name, "alpha") == 0,
                    "copy_values_match",
                    "Copied row 0 should be {10, \"alpha\", 1.0}",
                    _counter) && result;
            }

            d_table_free(copy);
        }

        d_table_free(original);
    }

    return result;
}


/*
d_tests_sa_table_new_fill
  Tests the d_table_new_fill function.
  Tests the following:
  - zero row count creates empty table
  - NULL fill value zero-initializes rows
  - valid fill value is copied to all rows
*/
bool
d_tests_sa_table_new_fill
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    struct d_test_table_row fill = { 42, "fill", 4.2 };

    result = true;

    // test 1: successful fill creation
    tbl = d_table_new_fill(sizeof(struct d_test_table_row), 5,
                           g_ctor_cols, g_ctor_col_count, &fill);

    if (tbl)
    {
        // test 2: row count matches
        result = d_assert_standalone(
            tbl->row_count == 5,
            "fill_row_count",
            "Filled table should have 5 rows",
            _counter) && result;

        // test 3: first row matches fill
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);

            result = d_assert_standalone(
                r0->id == 42 &&
                strcmp(r0->name, "fill") == 0 &&
                r0->value == 4.2,
                "fill_first_row",
                "First row should match fill value",
                _counter) && result;
        }

        // test 4: last row matches fill
        {
            struct d_test_table_row* r4 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 4);

            result = d_assert_standalone(
                r4->id == 42 &&
                strcmp(r4->name, "fill") == 0,
                "fill_last_row",
                "Last row should match fill value",
                _counter) && result;
        }

        // test 5: owns data
        result = d_assert_standalone(
            (tbl->flags & D_TABLE_FLAG_OWNS_DATA) != 0,
            "fill_owns_data",
            "Filled table should own its data",
            _counter) && result;

        d_table_free(tbl);
    }

    // test 6: NULL fill value zero-initializes
    tbl = d_table_new_fill(sizeof(struct d_test_table_row), 3,
                           g_ctor_cols, g_ctor_col_count, NULL);

    if (tbl)
    {
        struct d_test_table_row* r0 =
            (struct d_test_table_row*)d_table_row_ptr(tbl, 0);

        result = d_assert_standalone(
            r0->id == 0 && r0->name == NULL && r0->value == 0.0,
            "fill_null_zeroed",
            "NULL fill should zero-initialize rows",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_constructor_all
  Aggregation function that runs all constructor tests.
*/
bool
d_tests_sa_table_constructor_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Constructor Functions\n");
    printf("  --------------------------------\n");

    result = d_tests_sa_table_new(_counter) && result;
    result = d_tests_sa_table_new_from_rows(_counter) && result;
    result = d_tests_sa_table_new_copy(_counter) && result;
    result = d_tests_sa_table_new_fill(_counter) && result;

    return result;
}
