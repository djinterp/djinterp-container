#include ".\table_tests_sa.h"


// shared column descriptors for cell access tests
static const struct d_table_column_desc g_cell_cols[] =
{
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
};

static const size_t g_cell_col_count =
    D_TABLE_COLUMN_COUNT(g_cell_cols);


/*
d_tests_sa_table_cell_ptr
  Tests the d_table_cell_ptr function.
  Tests the following:
  - NULL table returns NULL
  - empty table returns NULL
  - valid (row, col) returns correct pointer
  - negative row index support
  - out-of-bounds returns NULL
*/
bool
d_tests_sa_table_cell_ptr
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;
    void*           ptr;

    struct d_test_table_row rows[] =
    {
        { 10, "alpha", 1.5 },
        { 20, "bravo", 2.5 }
    };

    result = true;

    // test 1: NULL table returns NULL
    ptr    = d_table_cell_ptr(NULL, 0, 0);
    result = d_assert_standalone(
        ptr == NULL,
        "cell_ptr_null_table",
        "NULL table should return NULL",
        _counter) && result;

    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 2, g_cell_cols, g_cell_col_count);

    if (tbl)
    {
        // test 2: valid cell (0, 0) — id of first row
        ptr    = d_table_cell_ptr(tbl, 0, 0);
        result = d_assert_standalone(
            ptr != NULL && *(int*)ptr == 10,
            "cell_ptr_0_0",
            "Cell (0,0) should point to id=10",
            _counter) && result;

        // test 3: valid cell (1, 2) — value of second row
        ptr    = d_table_cell_ptr(tbl, 1, 2);
        result = d_assert_standalone(
            ptr != NULL && *(double*)ptr == 2.5,
            "cell_ptr_1_2",
            "Cell (1,2) should point to value=2.5",
            _counter) && result;

        // test 4: negative row index (-1 = last row)
        ptr    = d_table_cell_ptr(tbl, -1, 0);
        result = d_assert_standalone(
            ptr != NULL && *(int*)ptr == 20,
            "cell_ptr_negative_row",
            "Cell (-1,0) should point to id=20",
            _counter) && result;

        // test 5: out-of-bounds row returns NULL
        ptr    = d_table_cell_ptr(tbl, 10, 0);
        result = d_assert_standalone(
            ptr == NULL,
            "cell_ptr_oob_row",
            "Out-of-bounds row should return NULL",
            _counter) && result;

        // test 6: out-of-bounds col returns NULL
        ptr    = d_table_cell_ptr(tbl, 0, 10);
        result = d_assert_standalone(
            ptr == NULL,
            "cell_ptr_oob_col",
            "Out-of-bounds col should return NULL",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_cell_get
  Tests the d_table_cell_get function.
  Tests the following:
  - NULL table returns false
  - NULL out returns false
  - successful get copies cell value
  - negative index support
*/
bool
d_tests_sa_table_cell_get
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;
    int             out_int;
    double          out_dbl;

    struct d_test_table_row rows[] =
    {
        { 42, "test", 3.14 },
        { 99, "last", 2.72 }
    };

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_cell_get(NULL, 0, 0, &out_int) == false,
        "cell_get_null_table",
        "NULL table should return false",
        _counter) && result;

    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 2, g_cell_cols, g_cell_col_count);

    if (tbl)
    {
        // test 2: NULL out fails
        result = d_assert_standalone(
            d_table_cell_get(tbl, 0, 0, NULL) == false,
            "cell_get_null_out",
            "NULL out should return false",
            _counter) && result;

        // test 3: successful get of int cell
        out_int = 0;
        result  = d_assert_standalone(
            d_table_cell_get(tbl, 0, 0, &out_int) == true &&
            out_int == 42,
            "cell_get_int",
            "Get cell (0,0) should yield 42",
            _counter) && result;

        // test 4: successful get of double cell
        out_dbl = 0.0;
        result  = d_assert_standalone(
            d_table_cell_get(tbl, 0, 2, &out_dbl) == true &&
            out_dbl == 3.14,
            "cell_get_double",
            "Get cell (0,2) should yield 3.14",
            _counter) && result;

        // test 5: negative index
        out_int = 0;
        result  = d_assert_standalone(
            d_table_cell_get(tbl, -1, 0, &out_int) == true &&
            out_int == 99,
            "cell_get_negative",
            "Get cell (-1,0) should yield 99",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_cell_set
  Tests the d_table_cell_set function.
  Tests the following:
  - NULL table returns false
  - NULL value returns false
  - successful set modifies cell value
  - negative index support
*/
bool
d_tests_sa_table_cell_set
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;
    int             new_id;
    double          new_val;

    struct d_test_table_row rows[] =
    {
        { 10, "orig", 1.0 },
        { 20, "keep", 2.0 }
    };

    result = true;

    // test 1: NULL table fails
    new_id = 99;
    result = d_assert_standalone(
        d_table_cell_set(NULL, 0, 0, &new_id) == false,
        "cell_set_null_table",
        "NULL table should return false",
        _counter) && result;

    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 2, g_cell_cols, g_cell_col_count);

    if (tbl)
    {
        // test 2: NULL value fails
        result = d_assert_standalone(
            d_table_cell_set(tbl, 0, 0, NULL) == false,
            "cell_set_null_value",
            "NULL value should return false",
            _counter) && result;

        // test 3: successful set of int cell
        new_id = 999;
        result = d_assert_standalone(
            d_table_cell_set(tbl, 0, 0, &new_id) == true,
            "cell_set_success",
            "Set cell (0,0) should succeed",
            _counter) && result;

        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);

            result = d_assert_standalone(
                r0->id == 999,
                "cell_set_value",
                "Cell (0,0) should now be 999",
                _counter) && result;
        }

        // test 4: set with negative index
        new_val = 7.77;
        result  = d_assert_standalone(
            d_table_cell_set(tbl, -1, 2, &new_val) == true,
            "cell_set_negative",
            "Set cell (-1,2) should succeed",
            _counter) && result;

        {
            struct d_test_table_row* r1 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 1);

            result = d_assert_standalone(
                r1->value == 7.77,
                "cell_set_negative_value",
                "Cell (-1,2) should now be 7.77",
                _counter) && result;
        }

        // test 5: other cells unmodified
        {
            struct d_test_table_row* r1 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 1);

            result = d_assert_standalone(
                r1->id == 20 &&
                strcmp(r1->name, "keep") == 0,
                "cell_set_others_unmodified",
                "Other cells should remain unchanged",
                _counter) && result;
        }

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_cell_access_all
  Aggregation function that runs all cell access tests.
*/
bool
d_tests_sa_table_cell_access_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Cell Access Functions\n");
    printf("  --------------------------------\n");

    result = d_tests_sa_table_cell_ptr(_counter) && result;
    result = d_tests_sa_table_cell_get(_counter) && result;
    result = d_tests_sa_table_cell_set(_counter) && result;

    return result;
}
