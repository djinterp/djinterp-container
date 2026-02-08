#include ".\table_common_tests_sa.h"


/*
d_tests_sa_table_common_row_macro
  Tests the D_TABLE_ROW macro.
  Tests the following:
  - creates a compound literal with designated initializers
  - field values are accessible directly
  - uninitialized fields are zero
*/
bool
d_tests_sa_table_common_row_macro
(
    struct d_test_counter* _counter
)
{
    bool                    result;
    struct d_test_table_row row;

    result = true;

    // test 1: all fields initialized
    row = D_TABLE_ROW(struct d_test_table_row,
                      .id = 42, .name = "hello", .value = 3.14);

    result = d_assert_standalone(
        row.id == 42,
        "row_macro_id",
        "Row id should be 42",
        _counter) && result;

    result = d_assert_standalone(
        strcmp(row.name, "hello") == 0,
        "row_macro_name",
        "Row name should be \"hello\"",
        _counter) && result;

    result = d_assert_standalone(
        row.value == 3.14,
        "row_macro_value",
        "Row value should be 3.14",
        _counter) && result;

    // test 2: partial initialization (unset fields zero-initialized)
    row = D_TABLE_ROW(struct d_test_table_row, .id = 7);

    result = d_assert_standalone(
        row.id == 7 && row.name == NULL && row.value == 0.0,
        "row_macro_partial",
        "Partial init: id=7, name=NULL, value=0.0",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_row_ptr_macro
  Tests the D_TABLE_ROW_PTR macro.
  Tests the following:
  - yields a non-NULL pointer
  - pointed-to struct contains correct values
  - suitable for const void* consumption
*/
bool
d_tests_sa_table_common_row_ptr_macro
(
    struct d_test_counter* _counter
)
{
    bool                          result;
    const struct d_test_table_row* ptr;

    result = true;

    // test 1: pointer is non-NULL and values correct
    ptr = D_TABLE_ROW_PTR(struct d_test_table_row,
                          .id = 99, .name = "world", .value = 2.72);

    result = d_assert_standalone(
        ptr != NULL,
        "row_ptr_not_null",
        "D_TABLE_ROW_PTR should yield a non-NULL pointer",
        _counter) && result;

    result = d_assert_standalone(
        ptr->id == 99,
        "row_ptr_id",
        "Pointed-to row id should be 99",
        _counter) && result;

    result = d_assert_standalone(
        strcmp(ptr->name, "world") == 0,
        "row_ptr_name",
        "Pointed-to row name should be \"world\"",
        _counter) && result;

    result = d_assert_standalone(
        ptr->value == 2.72,
        "row_ptr_value",
        "Pointed-to row value should be 2.72",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_rows_macro
  Tests the D_TABLE_ROWS macro.
  Tests the following:
  - initializes a static array of row structs
  - individual rows accessible by index with correct values
*/
bool
d_tests_sa_table_common_rows_macro
(
    struct d_test_counter* _counter
)
{
    bool result;

    struct d_test_table_row rows[] = D_TABLE_ROWS(
        { .id = 1, .name = "alpha",   .value = 1.0 },
        { .id = 2, .name = "bravo",   .value = 2.0 },
        { .id = 3, .name = "charlie", .value = 3.0 }
    );

    result = true;

    // test 1: first row
    result = d_assert_standalone(
        rows[0].id == 1 &&
        strcmp(rows[0].name, "alpha") == 0 &&
        rows[0].value == 1.0,
        "rows_macro_first",
        "First row should be {1, \"alpha\", 1.0}",
        _counter) && result;

    // test 2: second row
    result = d_assert_standalone(
        rows[1].id == 2 &&
        strcmp(rows[1].name, "bravo") == 0,
        "rows_macro_second",
        "Second row should be {2, \"bravo\", ...}",
        _counter) && result;

    // test 3: third row
    result = d_assert_standalone(
        rows[2].id == 3 && rows[2].value == 3.0,
        "rows_macro_third",
        "Third row should be {3, ..., 3.0}",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_row_count_macro
  Tests the D_TABLE_ROW_COUNT macro.
  Tests the following:
  - correct count for a 3-row array
  - correct count for a 1-row array
*/
bool
d_tests_sa_table_common_row_count_macro
(
    struct d_test_counter* _counter
)
{
    bool   result;
    size_t count;

    struct d_test_table_row rows_3[] = D_TABLE_ROWS(
        { .id = 1 },
        { .id = 2 },
        { .id = 3 }
    );

    struct d_test_table_row rows_1[] = D_TABLE_ROWS(
        { .id = 1 }
    );

    result = true;

    // test 1: 3-row array
    count  = D_TABLE_ROW_COUNT(struct d_test_table_row, rows_3);
    result = d_assert_standalone(
        count == 3,
        "row_count_three",
        "D_TABLE_ROW_COUNT should return 3",
        _counter) && result;

    // test 2: 1-row array
    count  = D_TABLE_ROW_COUNT(struct d_test_table_row, rows_1);
    result = d_assert_standalone(
        count == 1,
        "row_count_one",
        "D_TABLE_ROW_COUNT should return 1",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_cell_value_macros
  Tests the D_TABLE_CELL_* compound-literal wrapper macros.
  Tests the following:
  - D_TABLE_CELL_INT yields correct int pointer
  - D_TABLE_CELL_UINT yields correct unsigned int pointer
  - D_TABLE_CELL_FLOAT yields correct float pointer
  - D_TABLE_CELL_DOUBLE yields correct double pointer
  - D_TABLE_CELL_STR yields correct const char* pointer
  - D_TABLE_CELL_PTR yields correct void* pointer
  - D_TABLE_CELL_BOOL yields correct bool pointer
  - D_TABLE_CELL_NULL is NULL
  - D_TABLE_CELL_TYPED works with size_t
*/
bool
d_tests_sa_table_common_cell_value_macros
(
    struct d_test_counter* _counter
)
{
    bool        result;
    int         dummy;
    const void* ptr;

    result = true;

    // test 1: D_TABLE_CELL_INT
    result = d_assert_standalone(
        *(const int*)D_TABLE_CELL_INT(42) == 42,
        "cell_int",
        "D_TABLE_CELL_INT(42) should dereference to 42",
        _counter) && result;

    // test 2: D_TABLE_CELL_UINT
    result = d_assert_standalone(
        *(const unsigned int*)D_TABLE_CELL_UINT(100u) == 100u,
        "cell_uint",
        "D_TABLE_CELL_UINT(100) should dereference to 100",
        _counter) && result;

    // test 3: D_TABLE_CELL_FLOAT
    result = d_assert_standalone(
        *(const float*)D_TABLE_CELL_FLOAT(1.5f) == 1.5f,
        "cell_float",
        "D_TABLE_CELL_FLOAT(1.5f) should dereference to 1.5f",
        _counter) && result;

    // test 4: D_TABLE_CELL_DOUBLE
    result = d_assert_standalone(
        *(const double*)D_TABLE_CELL_DOUBLE(2.718) == 2.718,
        "cell_double",
        "D_TABLE_CELL_DOUBLE(2.718) should dereference to 2.718",
        _counter) && result;

    // test 5: D_TABLE_CELL_STR
    result = d_assert_standalone(
        strcmp(
            *(const char* const*)D_TABLE_CELL_STR("test"), "test") == 0,
        "cell_str",
        "D_TABLE_CELL_STR(\"test\") should point to \"test\"",
        _counter) && result;

    // test 6: D_TABLE_CELL_PTR
    dummy = 0;
    ptr   = *(void* const*)D_TABLE_CELL_PTR(&dummy);
    result = d_assert_standalone(
        ptr == &dummy,
        "cell_ptr",
        "D_TABLE_CELL_PTR should wrap the given pointer",
        _counter) && result;

    // test 7: D_TABLE_CELL_BOOL
    result = d_assert_standalone(
        *(const bool*)D_TABLE_CELL_BOOL(true) == true,
        "cell_bool",
        "D_TABLE_CELL_BOOL(true) should dereference to true",
        _counter) && result;

    // test 8: D_TABLE_CELL_NULL
    result = d_assert_standalone(
        D_TABLE_CELL_NULL == NULL,
        "cell_null",
        "D_TABLE_CELL_NULL should be NULL",
        _counter) && result;

    // test 9: D_TABLE_CELL_TYPED with size_t
    result = d_assert_standalone(
        *(const size_t*)D_TABLE_CELL_TYPED(size_t, 999) == 999,
        "cell_typed",
        "D_TABLE_CELL_TYPED(size_t, 999) should dereference to 999",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_row_cell_all
  Aggregation function that runs all row and cell value macro tests.
*/
bool
d_tests_sa_table_common_row_cell_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Row & Cell Value Macros\n");
    printf("  -----------------------------------\n");

    result = d_tests_sa_table_common_row_macro(_counter)         && result;
    result = d_tests_sa_table_common_row_ptr_macro(_counter)     && result;
    result = d_tests_sa_table_common_rows_macro(_counter)        && result;
    result = d_tests_sa_table_common_row_count_macro(_counter)   && result;
    result = d_tests_sa_table_common_cell_value_macros(_counter) && result;

    return result;
}
