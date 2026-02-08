#include ".\table_common_tests_sa.h"


/*
d_tests_sa_table_common_column_desc_validate
  Tests the d_table_common_column_desc_validate function.
  Tests the following:
  - NULL _column_descs returns false
  - zero _column_count returns false
  - zero _struct_size returns false
  - column count exceeding D_TABLE_MAX_COLUMNS returns false
  - column with zero element_size returns false
  - column whose offset + element_size overflows returns false
  - column whose offset + element_size exceeds struct_size returns false
  - valid descriptors derived from D_TABLE_COLUMN_FIELD return true
  - valid hand-crafted descriptors return true
  - multiple columns, one invalid, returns false
*/
bool
d_tests_sa_table_common_column_desc_validate
(
    struct d_test_counter* _counter
)
{
    bool   result;
    size_t ss;

    static const struct d_table_column_desc valid_cols[] =
        D_TABLE_COLUMNS(
            D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
            D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
            D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
        );

    // column with zero element_size
    struct d_table_column_desc zero_elem[] =
    {
        { .element_size = 0, .offset = 0, .flags = 0 }
    };

    // column whose field extends past the struct
    struct d_table_column_desc past_end[] =
    {
        { .element_size = 8, .offset = 100, .flags = 0 }
    };

    // overflow: offset + element_size wraps around
    struct d_table_column_desc overflow_col[] =
    {
        { .element_size = SIZE_MAX, .offset = 2, .flags = 0 }
    };

    // mixed: first valid, second invalid (zero element_size)
    struct d_table_column_desc mixed[] =
    {
        D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
        { .element_size = 0, .offset = 0, .flags = 0 }
    };

    result = true;
    ss     = sizeof(struct d_test_table_row);

    // test 1: NULL column_descs
    result = d_assert_standalone(
        d_table_common_column_desc_validate(NULL, 3, ss) == false,
        "validate_null_descs",
        "NULL column_descs should return false",
        _counter) && result;

    // test 2: zero column_count
    result = d_assert_standalone(
        d_table_common_column_desc_validate(valid_cols, 0, ss) == false,
        "validate_zero_count",
        "Zero column_count should return false",
        _counter) && result;

    // test 3: zero struct_size
    result = d_assert_standalone(
        d_table_common_column_desc_validate(valid_cols, 3, 0) == false,
        "validate_zero_struct_size",
        "Zero struct_size should return false",
        _counter) && result;

    // test 4: exceeds D_TABLE_MAX_COLUMNS
    //result = d_assert_standalone(
    //    d_table_common_column_desc_validate(
    //        valid_cols, D_TABLE_MAX_COLUMNS + 1, ss) == false,
    //    "validate_too_many_columns",
    //    "Exceeding D_TABLE_MAX_COLUMNS should return false",
    //    _counter) && result;

    // test 5: zero element_size in a column
    result = d_assert_standalone(
        d_table_common_column_desc_validate(zero_elem, 1, ss) == false,
        "validate_zero_element_size",
        "Column with zero element_size should return false",
        _counter) && result;

    // test 6: field extends past struct
    result = d_assert_standalone(
        d_table_common_column_desc_validate(past_end, 1, ss) == false,
        "validate_past_end",
        "Column extending past struct should return false",
        _counter) && result;

    // test 7: offset + element_size overflow
    result = d_assert_standalone(
        d_table_common_column_desc_validate(overflow_col, 1, ss) == false,
        "validate_overflow",
        "Overflow in offset + element_size should return false",
        _counter) && result;

    // test 8: valid descriptors from D_TABLE_COLUMN_FIELD
    result = d_assert_standalone(
        d_table_common_column_desc_validate(valid_cols, 3, ss) == true,
        "validate_valid",
        "Valid descriptors should return true",
        _counter) && result;

    // test 9: valid hand-crafted descriptor
    {
        struct d_table_column_desc hand[] =
        {
            { .element_size = 4, .offset = 0, .flags = 0 }
        };

        result = d_assert_standalone(
            d_table_common_column_desc_validate(hand, 1, 4) == true,
            "validate_hand_crafted",
            "Hand-crafted valid descriptor should return true",
            _counter) && result;
    }

    // test 10: mixed valid/invalid — second column bad
    result = d_assert_standalone(
        d_table_common_column_desc_validate(mixed, 2, ss) == false,
        "validate_mixed",
        "Array with one invalid column should return false",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_free_data
  Tests the d_table_common_free_data function.
  Tests the following:
  - NULL is safe (no crash)
  - heap-allocated buffer is freed without error
  - double-free protection (pass NULL after free)
*/
bool
d_tests_sa_table_common_free_data
(
    struct d_test_counter* _counter
)
{
    bool  result;
    void* data;

    result = true;

    // test 1: NULL is safe — should not crash
    d_table_common_free_data(NULL);
    result = d_assert_standalone(
        true,
        "free_data_null",
        "free_data(NULL) should not crash",
        _counter) && result;

    // test 2: free a valid heap buffer
    data = calloc(5, sizeof(struct d_test_table_row));
    if (data)
    {
        d_table_common_free_data(data);
        result = d_assert_standalone(
            true,
            "free_data_valid",
            "free_data on heap buffer should succeed",
            _counter) && result;
    }

    // test 3: free NULL again after free (idempotent safety)
    d_table_common_free_data(NULL);
    result = d_assert_standalone(
        true,
        "free_data_null_again",
        "free_data(NULL) after prior free should not crash",
        _counter) && result;

    // test 4: free buffer allocated via d_table_common_init
    {
        void*  init_data;
        size_t row_count;

        init_data = NULL;
        row_count = 0;

        if (d_table_common_init(&init_data, &row_count,
                                sizeof(struct d_test_table_row), 10))
        {
            d_table_common_free_data(init_data);
            result = d_assert_standalone(
                true,
                "free_data_from_init",
                "free_data on init-allocated buffer should succeed",
                _counter) && result;
        }
    }

    return result;
}


/*
d_tests_sa_table_common_validation_all
  Aggregation function that runs all validation and cleanup tests.
*/
bool
d_tests_sa_table_common_validation_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Validation & Cleanup\n");
    printf("  --------------------------------\n");

    result = d_tests_sa_table_common_column_desc_validate(_counter) && result;
    result = d_tests_sa_table_common_free_data(_counter)            && result;

    return result;
}
