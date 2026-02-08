#include ".\table_tests_sa.h"


// shared column descriptors for row access tests
static const struct d_table_column_desc g_racc_cols[] =
{
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
};

static const size_t g_racc_col_count =
    D_TABLE_COLUMN_COUNT(g_racc_cols);


/*
d_tests_sa_table_row_ptr
  Tests the d_table_row_ptr function.
  Tests the following:
  - NULL table returns NULL
  - empty table returns NULL
  - valid positive index
  - valid negative index
  - out-of-bounds returns NULL
*/
bool
d_tests_sa_table_row_ptr
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;
    void*           ptr;

    struct d_test_table_row rows[] =
    {
        { 10, "alpha", 1.0 },
        { 20, "bravo", 2.0 },
        { 30, "charlie", 3.0 }
    };

    result = true;

    // test 1: NULL table returns NULL
    ptr    = d_table_row_ptr(NULL, 0);
    result = d_assert_standalone(
        ptr == NULL,
        "row_ptr_null_table",
        "NULL table should return NULL",
        _counter) && result;

    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 3, g_racc_cols, g_racc_col_count);

    if (tbl)
    {
        // test 2: valid positive index
        ptr    = d_table_row_ptr(tbl, 1);
        result = d_assert_standalone(
            ptr != NULL &&
            ((struct d_test_table_row*)ptr)->id == 20,
            "row_ptr_positive",
            "Row 1 should have id=20",
            _counter) && result;

        // test 3: negative index (-1 = last)
        ptr    = d_table_row_ptr(tbl, -1);
        result = d_assert_standalone(
            ptr != NULL &&
            ((struct d_test_table_row*)ptr)->id == 30,
            "row_ptr_negative",
            "Row -1 should have id=30",
            _counter) && result;

        // test 4: out-of-bounds returns NULL
        ptr    = d_table_row_ptr(tbl, 10);
        result = d_assert_standalone(
            ptr == NULL,
            "row_ptr_oob",
            "Out-of-bounds index should return NULL",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_row_get
  Tests the d_table_row_get function.
  Tests the following:
  - NULL table returns false
  - NULL out returns false
  - successful get copies entire row
  - negative index support
*/
bool
d_tests_sa_table_row_get
(
    struct d_test_counter* _counter
)
{
    bool                    result;
    struct d_table*         tbl;
    struct d_test_table_row out;

    struct d_test_table_row rows[] =
    {
        { 10, "alpha", 1.0 },
        { 20, "bravo", 2.0 }
    };

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_row_get(NULL, 0, &out) == false,
        "row_get_null_table",
        "NULL table should return false",
        _counter) && result;

    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 2, g_racc_cols, g_racc_col_count);

    if (tbl)
    {
        // test 2: NULL out fails
        result = d_assert_standalone(
            d_table_row_get(tbl, 0, NULL) == false,
            "row_get_null_out",
            "NULL out should return false",
            _counter) && result;

        // test 3: successful get
        d_memset(&out, 0, sizeof(out));
        result = d_assert_standalone(
            d_table_row_get(tbl, 0, &out) == true,
            "row_get_success",
            "Row get should succeed",
            _counter) && result;

        result = d_assert_standalone(
            out.id == 10 &&
            strcmp(out.name, "alpha") == 0 &&
            out.value == 1.0,
            "row_get_values",
            "Got row should be {10, \"alpha\", 1.0}",
            _counter) && result;

        // test 4: negative index
        d_memset(&out, 0, sizeof(out));
        result = d_assert_standalone(
            d_table_row_get(tbl, -1, &out) == true &&
            out.id == 20,
            "row_get_negative",
            "Row get at -1 should yield id=20",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_row_set
  Tests the d_table_row_set function.
  Tests the following:
  - NULL table returns false
  - NULL source returns false
  - successful set overwrites entire row
  - other rows unmodified
*/
bool
d_tests_sa_table_row_set
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    struct d_test_table_row rows[] =
    {
        { 10, "alpha", 1.0 },
        { 20, "bravo", 2.0 },
        { 30, "charlie", 3.0 }
    };

    struct d_test_table_row replacement = { 99, "replaced", 9.9 };

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_row_set(NULL, 0, &replacement) == false,
        "row_set_null_table",
        "NULL table should return false",
        _counter) && result;

    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 3, g_racc_cols, g_racc_col_count);

    if (tbl)
    {
        // test 2: NULL source fails
        result = d_assert_standalone(
            d_table_row_set(tbl, 0, NULL) == false,
            "row_set_null_source",
            "NULL source should return false",
            _counter) && result;

        // test 3: successful set
        result = d_assert_standalone(
            d_table_row_set(tbl, 1, &replacement) == true,
            "row_set_success",
            "Row set should succeed",
            _counter) && result;

        {
            struct d_test_table_row* r1 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 1);

            result = d_assert_standalone(
                r1->id == 99 &&
                strcmp(r1->name, "replaced") == 0 &&
                r1->value == 9.9,
                "row_set_values",
                "Row 1 should be {99, \"replaced\", 9.9}",
                _counter) && result;
        }

        // test 4: other rows unmodified
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);
            struct d_test_table_row* r2 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 2);

            result = d_assert_standalone(
                r0->id == 10 && r2->id == 30,
                "row_set_others_unmodified",
                "Other rows should remain unchanged",
                _counter) && result;
        }

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_row_access_all
  Aggregation function that runs all row access tests.
*/
bool
d_tests_sa_table_row_access_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Row Access Functions\n");
    printf("  -------------------------------\n");

    result = d_tests_sa_table_row_ptr(_counter) && result;
    result = d_tests_sa_table_row_get(_counter) && result;
    result = d_tests_sa_table_row_set(_counter) && result;

    return result;
}
