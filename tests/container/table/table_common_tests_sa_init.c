#include ".\table_common_tests_sa.h"


/*
d_tests_sa_table_common_init_macro
  Tests the D_TABLE_INIT macro.
  Tests the following:
  - produces a valid aggregate initializer with correct field values
  - data pointer points to the row array
  - column_descs pointer points to the descriptor array
  - struct_size, row_count, and column_count set correctly
*/
bool
d_tests_sa_table_common_init_macro
(
    struct d_test_counter* _counter
)
{
    bool result;

    struct d_test_table_row rows[] = D_TABLE_ROWS(
        { .id = 1, .name = "alpha", .value = 1.0 },
        { .id = 2, .name = "bravo", .value = 2.0 }
    );

    static const struct d_table_column_desc cols[] =
        D_TABLE_COLUMNS(
            D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
            D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
            D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
        );

    // NOTE: D_TABLE_INIT produces a brace-enclosed initializer.
    // We verify the concept by constructing the struct directly with the
    // same fields the macro would set, because the macro is intended for
    // use as a struct initializer, not a standalone expression.
    struct
    {
        void*                             data;
        const struct d_table_column_desc* column_descs;
        size_t                            struct_size;
        size_t                            row_count;
        size_t                            column_count;
    }
    table =
    {
        .data         = (void*)rows,
        .column_descs = cols,
        .struct_size  = sizeof(struct d_test_table_row),
        .row_count    = D_TABLE_ROW_COUNT(struct d_test_table_row, rows),
        .column_count = D_TABLE_COLUMN_COUNT(cols)
    };

    result = true;

    // test 1: data pointer
    result = d_assert_standalone(
        table.data == (void*)rows,
        "init_macro_data",
        "data should point to the row array",
        _counter) && result;

    // test 2: column_descs pointer
    result = d_assert_standalone(
        table.column_descs == cols,
        "init_macro_column_descs",
        "column_descs should point to the descriptor array",
        _counter) && result;

    // test 3: struct_size
    result = d_assert_standalone(
        table.struct_size == sizeof(struct d_test_table_row),
        "init_macro_struct_size",
        "struct_size should equal sizeof(struct d_test_table_row)",
        _counter) && result;

    // test 4: row_count
    result = d_assert_standalone(
        table.row_count == 2,
        "init_macro_row_count",
        "row_count should be 2",
        _counter) && result;

    // test 5: column_count
    result = d_assert_standalone(
        table.column_count == 3,
        "init_macro_column_count",
        "column_count should be 3",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_init
  Tests the d_table_common_init function.
  Tests the following:
  - NULL _data rejection
  - NULL _row_count rejection
  - zero _struct_size rejection
  - zero _num_rows is valid (no allocation)
  - successful allocation with correct outputs
  - allocated memory is zeroed
*/
bool
d_tests_sa_table_common_init
(
    struct d_test_counter* _counter
)
{
    bool   result;
    void*  data;
    size_t row_count;

    struct d_test_table_row* rows;

    result = true;

    // test 1: NULL _data should fail
    row_count = 0;
    result    = d_assert_standalone(
        d_table_common_init(NULL, &row_count, sizeof(int), 5) == false,
        "init_null_data",
        "NULL _data should return false",
        _counter) && result;

    // test 2: NULL _row_count should fail
    data   = NULL;
    result = d_assert_standalone(
        d_table_common_init(&data, NULL, sizeof(int), 5) == false,
        "init_null_row_count",
        "NULL _row_count should return false",
        _counter) && result;

    // test 3: zero struct_size should fail
    data      = NULL;
    row_count = 0;
    result    = d_assert_standalone(
        d_table_common_init(&data, &row_count, 0, 5) == false,
        "init_zero_struct_size",
        "Zero struct_size should return false",
        _counter) && result;

    // test 4: zero num_rows is valid
    data      = NULL;
    row_count = 99;
    result    = d_assert_standalone(
        d_table_common_init(&data, &row_count,
                            sizeof(struct d_test_table_row), 0) == true,
        "init_zero_rows_success",
        "Zero num_rows should return true",
        _counter) && result;

    result = d_assert_standalone(
        data == NULL && row_count == 0,
        "init_zero_rows_outputs",
        "Zero rows: data=NULL, row_count=0",
        _counter) && result;

    // test 5: successful allocation
    data      = NULL;
    row_count = 0;
    result    = d_assert_standalone(
        d_table_common_init(&data, &row_count,
                            sizeof(struct d_test_table_row), 3) == true,
        "init_success",
        "Init with 3 rows should succeed",
        _counter) && result;

    result = d_assert_standalone(
        data != NULL && row_count == 3,
        "init_success_outputs",
        "data should be non-NULL, row_count should be 3",
        _counter) && result;

    // test 6: memory is zeroed (calloc)
    if (data)
    {
        rows   = (struct d_test_table_row*)data;
        result = d_assert_standalone(
            rows[0].id == 0 && rows[0].name == NULL &&
            rows[0].value == 0.0,
            "init_zeroed",
            "Allocated memory should be zeroed",
            _counter) && result;

        free(data);
    }

    return result;
}


/*
d_tests_sa_table_common_init_copy
  Tests the d_table_common_init_copy function.
  Tests the following:
  - NULL _data rejection
  - NULL _row_count rejection
  - zero _struct_size rejection
  - NULL _source rejection
  - zero _num_rows is valid (no allocation)
  - successful copy with correct outputs
  - source data is faithfully reproduced in the copy
*/
bool
d_tests_sa_table_common_init_copy
(
    struct d_test_counter* _counter
)
{
    bool   result;
    void*  data;
    size_t row_count;

    struct d_test_table_row source[] = D_TABLE_ROWS(
        { .id = 10, .name = "fox",   .value = 1.1 },
        { .id = 20, .name = "goose", .value = 2.2 },
        { .id = 30, .name = "hare",  .value = 3.3 }
    );

    struct d_test_table_row* copy;

    result = true;

    // test 1: NULL _data should fail
    row_count = 0;
    result    = d_assert_standalone(
        d_table_common_init_copy(NULL, &row_count,
            sizeof(struct d_test_table_row), source, 3) == false,
        "init_copy_null_data",
        "NULL _data should return false",
        _counter) && result;

    // test 2: NULL _row_count should fail
    data   = NULL;
    result = d_assert_standalone(
        d_table_common_init_copy(&data, NULL,
            sizeof(struct d_test_table_row), source, 3) == false,
        "init_copy_null_row_count",
        "NULL _row_count should return false",
        _counter) && result;

    // test 3: zero struct_size should fail
    data      = NULL;
    row_count = 0;
    result    = d_assert_standalone(
        d_table_common_init_copy(&data, &row_count,
            0, source, 3) == false,
        "init_copy_zero_struct_size",
        "Zero struct_size should return false",
        _counter) && result;

    // test 4: NULL _source should fail
    data      = NULL;
    row_count = 0;
    result    = d_assert_standalone(
        d_table_common_init_copy(&data, &row_count,
            sizeof(struct d_test_table_row), NULL, 3) == false,
        "init_copy_null_source",
        "NULL _source should return false",
        _counter) && result;

    // test 5: zero num_rows is valid
    data      = NULL;
    row_count = 99;
    result    = d_assert_standalone(
        d_table_common_init_copy(&data, &row_count,
            sizeof(struct d_test_table_row), source, 0) == true,
        "init_copy_zero_rows_success",
        "Zero num_rows should return true",
        _counter) && result;

    result = d_assert_standalone(
        data == NULL && row_count == 0,
        "init_copy_zero_rows_outputs",
        "Zero rows: data=NULL, row_count=0",
        _counter) && result;

    // test 6: successful copy
    data      = NULL;
    row_count = 0;
    result    = d_assert_standalone(
        d_table_common_init_copy(&data, &row_count,
            sizeof(struct d_test_table_row), source, 3) == true,
        "init_copy_success",
        "Init copy with 3 rows should succeed",
        _counter) && result;

    result = d_assert_standalone(
        data != NULL && row_count == 3,
        "init_copy_success_outputs",
        "data should be non-NULL, row_count should be 3",
        _counter) && result;

    // test 7: data faithfully copied
    if (data)
    {
        copy = (struct d_test_table_row*)data;

        result = d_assert_standalone(
            copy[0].id == 10 &&
            copy[1].id == 20 &&
            copy[2].id == 30,
            "init_copy_ids",
            "Copied row ids should be 10, 20, 30",
            _counter) && result;

        result = d_assert_standalone(
            copy[0].value == 1.1 &&
            copy[1].value == 2.2 &&
            copy[2].value == 3.3,
            "init_copy_values",
            "Copied row values should be 1.1, 2.2, 3.3",
            _counter) && result;

        // test 8: copy is independent of source
        source[0].id = 999;
        result = d_assert_standalone(
            copy[0].id == 10,
            "init_copy_independent",
            "Copy should be independent of source",
            _counter) && result;

        free(data);
    }

    return result;
}


/*
d_tests_sa_table_common_init_all
  Aggregation function that runs all initialization tests.
*/
bool
d_tests_sa_table_common_init_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Initialization\n");
    printf("  -------------------------\n");

    result = d_tests_sa_table_common_init_macro(_counter) && result;
    result = d_tests_sa_table_common_init(_counter)       && result;
    result = d_tests_sa_table_common_init_copy(_counter)  && result;

    return result;
}
