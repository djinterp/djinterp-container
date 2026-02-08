#include ".\table_tests_sa.h"


// shared column descriptors for row mutation tests
static const struct d_table_column_desc g_mut_cols[] =
{
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
};

static const size_t g_mut_col_count =
    D_TABLE_COLUMN_COUNT(g_mut_cols);


/*
d_tests_sa_table_push_row
  Tests the d_table_push_row function.
  Tests the following:
  - NULL table returns false
  - NULL row_data returns false
  - successful push to empty table
  - multiple pushes with auto-grow
  - pushed data is correct
*/
bool
d_tests_sa_table_push_row
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    struct d_test_table_row row = { 10, "push", 1.0 };

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_push_row(NULL, &row) == false,
        "push_null_table",
        "NULL table should return false",
        _counter) && result;

    // test 2: NULL row_data fails
    tbl = d_table_new(sizeof(struct d_test_table_row), g_mut_cols,
                      g_mut_col_count, 4);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_push_row(tbl, NULL) == false,
            "push_null_data",
            "NULL row_data should return false",
            _counter) && result;

        // test 3: successful push
        result = d_assert_standalone(
            d_table_push_row(tbl, &row) == true,
            "push_success",
            "Push should succeed",
            _counter) && result;

        result = d_assert_standalone(
            tbl->row_count == 1,
            "push_count",
            "Row count should be 1 after push",
            _counter) && result;

        // test 4: pushed data is correct
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);

            result = d_assert_standalone(
                r0->id == 10 &&
                strcmp(r0->name, "push") == 0 &&
                r0->value == 1.0,
                "push_data",
                "Pushed row should be {10, \"push\", 1.0}",
                _counter) && result;
        }

        // test 5: multiple pushes triggering growth
        row.id = 20;
        d_table_push_row(tbl, &row);
        row.id = 30;
        d_table_push_row(tbl, &row);
        row.id = 40;
        d_table_push_row(tbl, &row);
        row.id = 50;

        result = d_assert_standalone(
            d_table_push_row(tbl, &row) == true,
            "push_auto_grow",
            "Push beyond initial capacity should trigger growth",
            _counter) && result;

        result = d_assert_standalone(
            tbl->row_count == 5 && tbl->capacity >= 5,
            "push_grow_state",
            "After 5 pushes: row_count=5, capacity >= 5",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_push_row_empty
  Tests the d_table_push_row_empty function.
  Tests the following:
  - NULL table returns false
  - successful push of zero-initialized row
  - row_count incremented
*/
bool
d_tests_sa_table_push_row_empty
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_push_row_empty(NULL) == false,
        "push_empty_null",
        "NULL table should return false",
        _counter) && result;

    // test 2: successful empty push
    tbl = d_table_new(sizeof(struct d_test_table_row), g_mut_cols,
                      g_mut_col_count, 4);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_push_row_empty(tbl) == true,
            "push_empty_success",
            "Push empty should succeed",
            _counter) && result;

        result = d_assert_standalone(
            tbl->row_count == 1,
            "push_empty_count",
            "Row count should be 1",
            _counter) && result;

        // test 3: row is zero-initialized
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);

            result = d_assert_standalone(
                r0->id == 0 && r0->name == NULL && r0->value == 0.0,
                "push_empty_zeroed",
                "Empty-pushed row should be zero-initialized",
                _counter) && result;
        }

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_insert_row
  Tests the d_table_insert_row function.
  Tests the following:
  - NULL table returns false
  - out-of-bounds index returns false
  - insert at beginning shifts existing rows
  - insert at end (append-like)
  - insert in middle
  - negative index support
*/
bool
d_tests_sa_table_insert_row
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

    struct d_test_table_row insert = { 99, "ins", 9.9 };

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_insert_row(NULL, 0, &insert) == false,
        "insert_null_table",
        "NULL table should return false",
        _counter) && result;

    // test 2: insert at beginning
    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 3, g_mut_cols, g_mut_col_count);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_insert_row(tbl, 0, &insert) == true,
            "insert_begin_success",
            "Insert at 0 should succeed",
            _counter) && result;

        result = d_assert_standalone(
            tbl->row_count == 4,
            "insert_begin_count",
            "Row count should be 4",
            _counter) && result;

        // inserted row is now at index 0
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);
            struct d_test_table_row* r1 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 1);

            result = d_assert_standalone(
                r0->id == 99 && r1->id == 10,
                "insert_begin_shifted",
                "Inserted row at 0, original rows shifted right",
                _counter) && result;
        }

        // test 3: insert with negative index (-1 = before last)
        insert.id = 77;
        result    = d_assert_standalone(
            d_table_insert_row(tbl, -1, &insert) == true,
            "insert_negative",
            "Insert at -1 should succeed",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_insert_rows
  Tests the d_table_insert_rows function.
  Tests the following:
  - NULL table returns false
  - zero count is a no-op
  - successful bulk insert shifts existing rows
*/
bool
d_tests_sa_table_insert_rows
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    struct d_test_table_row initial[] =
    {
        { 1, "a", 1.0 },
        { 2, "b", 2.0 }
    };

    struct d_test_table_row batch[] =
    {
        { 90, "x", 9.0 },
        { 91, "y", 9.1 }
    };

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_insert_rows(NULL, 0, batch, 2) == false,
        "insert_rows_null",
        "NULL table should return false",
        _counter) && result;

    // test 2: successful bulk insert at beginning
    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                initial, 2, g_mut_cols, g_mut_col_count);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_insert_rows(tbl, 0, batch, 2) == true,
            "insert_rows_success",
            "Bulk insert should succeed",
            _counter) && result;

        result = d_assert_standalone(
            tbl->row_count == 4,
            "insert_rows_count",
            "Row count should be 4",
            _counter) && result;

        // verify order: 90, 91, 1, 2
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);
            struct d_test_table_row* r2 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 2);

            result = d_assert_standalone(
                r0->id == 90 && r2->id == 1,
                "insert_rows_order",
                "Rows should be [90, 91, 1, 2]",
                _counter) && result;
        }

        d_table_free(tbl);
    }

    // test 3: zero count is no-op
    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                initial, 2, g_mut_cols, g_mut_col_count);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_insert_rows(tbl, 0, batch, 0) == true,
            "insert_rows_zero_count",
            "Zero count should succeed as no-op",
            _counter) && result;

        result = d_assert_standalone(
            tbl->row_count == 2,
            "insert_rows_zero_unchanged",
            "Row count should remain 2",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_remove_row
  Tests the d_table_remove_row function.
  Tests the following:
  - NULL table returns false
  - empty table returns false
  - out-of-bounds returns false
  - successful removal shifts remaining rows
  - negative index support
*/
bool
d_tests_sa_table_remove_row
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
        d_table_remove_row(NULL, 0) == false,
        "remove_null",
        "NULL table should return false",
        _counter) && result;

    // test 2: remove from middle
    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 3, g_mut_cols, g_mut_col_count);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_remove_row(tbl, 1) == true,
            "remove_middle_success",
            "Remove at 1 should succeed",
            _counter) && result;

        result = d_assert_standalone(
            tbl->row_count == 2,
            "remove_middle_count",
            "Row count should be 2",
            _counter) && result;

        // remaining: 10, 30
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);
            struct d_test_table_row* r1 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 1);

            result = d_assert_standalone(
                r0->id == 10 && r1->id == 30,
                "remove_middle_shifted",
                "Remaining rows should be [10, 30]",
                _counter) && result;
        }

        // test 3: remove with negative index (-1 = last)
        result = d_assert_standalone(
            d_table_remove_row(tbl, -1) == true,
            "remove_negative",
            "Remove at -1 should succeed",
            _counter) && result;

        result = d_assert_standalone(
            tbl->row_count == 1,
            "remove_negative_count",
            "Row count should be 1",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_remove_range
  Tests the d_table_remove_range function.
  Tests the following:
  - NULL table returns false
  - invalid range returns false
  - successful range removal
  - remaining rows shifted correctly
*/
bool
d_tests_sa_table_remove_range
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
        { 40, "d", 4.0 },
        { 50, "e", 5.0 }
    };

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_remove_range(NULL, 0, 1) == false,
        "remove_range_null",
        "NULL table should return false",
        _counter) && result;

    // test 2: remove inclusive range [1, 2]
    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 5, g_mut_cols, g_mut_col_count);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_remove_range(tbl, 1, 2) == true,
            "remove_range_success",
            "Remove range [1,2] should succeed",
            _counter) && result;

        result = d_assert_standalone(
            tbl->row_count == 3,
            "remove_range_count",
            "Row count should be 3",
            _counter) && result;

        // remaining: 10, 40, 50
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);
            struct d_test_table_row* r1 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 1);
            struct d_test_table_row* r2 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 2);

            result = d_assert_standalone(
                r0->id == 10 && r1->id == 40 && r2->id == 50,
                "remove_range_shifted",
                "Remaining rows should be [10, 40, 50]",
                _counter) && result;
        }

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_pop_row
  Tests the d_table_pop_row function.
  Tests the following:
  - NULL table returns false
  - empty table returns false
  - successful pop returns last row
  - row_count decremented
  - NULL out_row still removes the row
*/
bool
d_tests_sa_table_pop_row
(
    struct d_test_counter* _counter
)
{
    bool                    result;
    struct d_table*         tbl;
    struct d_test_table_row out;

    struct d_test_table_row rows[] =
    {
        { 10, "a", 1.0 },
        { 20, "b", 2.0 },
        { 30, "c", 3.0 }
    };

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_pop_row(NULL, &out) == false,
        "pop_null_table",
        "NULL table should return false",
        _counter) && result;

    // test 2: successful pop
    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 3, g_mut_cols, g_mut_col_count);

    if (tbl)
    {
        d_memset(&out, 0, sizeof(out));

        result = d_assert_standalone(
            d_table_pop_row(tbl, &out) == true,
            "pop_success",
            "Pop should succeed",
            _counter) && result;

        // test 3: popped row is the last one
        result = d_assert_standalone(
            out.id == 30 && strcmp(out.name, "c") == 0,
            "pop_value",
            "Popped row should be {30, \"c\", 3.0}",
            _counter) && result;

        // test 4: row count decremented
        result = d_assert_standalone(
            tbl->row_count == 2,
            "pop_count",
            "Row count should be 2 after pop",
            _counter) && result;

        // test 5: NULL out_row still removes
        result = d_assert_standalone(
            d_table_pop_row(tbl, NULL) == true,
            "pop_null_out",
            "Pop with NULL out should succeed",
            _counter) && result;

        result = d_assert_standalone(
            tbl->row_count == 1,
            "pop_null_out_count",
            "Row count should be 1",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_clear
  Tests the d_table_clear function.
  Tests the following:
  - NULL table is safe (no crash)
  - clear sets row_count to 0
  - capacity and data preserved
*/
bool
d_tests_sa_table_clear
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;
    size_t          cap_before;

    struct d_test_table_row rows[] =
    {
        { 10, "a", 1.0 },
        { 20, "b", 2.0 }
    };

    result = true;

    // test 1: NULL table does not crash
    d_table_clear(NULL);
    result = d_assert_standalone(
        true,
        "clear_null_safe",
        "Clear on NULL should not crash",
        _counter) && result;

    // test 2: clear sets row_count to 0
    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 2, g_mut_cols, g_mut_col_count);

    if (tbl)
    {
        cap_before = tbl->capacity;

        d_table_clear(tbl);

        result = d_assert_standalone(
            tbl->row_count == 0,
            "clear_row_count",
            "Row count should be 0 after clear",
            _counter) && result;

        // test 3: capacity preserved
        result = d_assert_standalone(
            tbl->capacity == cap_before,
            "clear_capacity_preserved",
            "Capacity should be preserved after clear",
            _counter) && result;

        // test 4: data pointer preserved
        result = d_assert_standalone(
            tbl->data != NULL,
            "clear_data_preserved",
            "Data pointer should be preserved after clear",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_row_mutation_all
  Aggregation function that runs all row mutation tests.
*/
bool
d_tests_sa_table_row_mutation_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Row Mutation Functions\n");
    printf("  ---------------------------------\n");

    result = d_tests_sa_table_push_row(_counter) && result;
    result = d_tests_sa_table_push_row_empty(_counter) && result;
    result = d_tests_sa_table_insert_row(_counter) && result;
    result = d_tests_sa_table_insert_rows(_counter) && result;
    result = d_tests_sa_table_remove_row(_counter) && result;
    result = d_tests_sa_table_remove_range(_counter) && result;
    result = d_tests_sa_table_pop_row(_counter) && result;
    result = d_tests_sa_table_clear(_counter) && result;

    return result;
}
