#include ".\table_common_tests_sa.h"


/*
d_tests_sa_table_common_column_field
  Tests the D_TABLE_COLUMN_FIELD macro.
  Tests the following:
  - element_size derived from sizeof(field)
  - offset derived from offsetof(struct, field)
  - flags default to D_TABLE_COLUMN_FLAG_NONE
  - works with int, pointer, and double fields
*/
bool
d_tests_sa_table_common_column_field
(
    struct d_test_counter* _counter
)
{
    bool result;

    struct d_table_column_desc id_col;
    struct d_table_column_desc name_col;
    struct d_table_column_desc value_col;

    result = true;

    // test 1: int field — element_size and offset
    id_col = (struct d_table_column_desc)
        D_TABLE_COLUMN_FIELD(struct d_test_table_row, id);

    result = d_assert_standalone(
        id_col.element_size == sizeof(int),
        "column_field_int_size",
        "int field element_size should equal sizeof(int)",
        _counter) && result;

    result = d_assert_standalone(
        id_col.offset == offsetof(struct d_test_table_row, id),
        "column_field_int_offset",
        "int field offset should match offsetof",
        _counter) && result;

    result = d_assert_standalone(
        id_col.flags == D_TABLE_COLUMN_FLAG_NONE,
        "column_field_int_flags",
        "Default flags should be D_TABLE_COLUMN_FLAG_NONE",
        _counter) && result;

    // test 2: pointer field
    name_col = (struct d_table_column_desc)
        D_TABLE_COLUMN_FIELD(struct d_test_table_row, name);

    result = d_assert_standalone(
        name_col.element_size == sizeof(const char*),
        "column_field_ptr_size",
        "Pointer field element_size should equal sizeof(const char*)",
        _counter) && result;

    result = d_assert_standalone(
        name_col.offset == offsetof(struct d_test_table_row, name),
        "column_field_ptr_offset",
        "Pointer field offset should match offsetof",
        _counter) && result;

    // test 3: double field
    value_col = (struct d_table_column_desc)
        D_TABLE_COLUMN_FIELD(struct d_test_table_row, value);

    result = d_assert_standalone(
        value_col.element_size == sizeof(double),
        "column_field_double_size",
        "double field element_size should equal sizeof(double)",
        _counter) && result;

    result = d_assert_standalone(
        value_col.offset == offsetof(struct d_test_table_row, value),
        "column_field_double_offset",
        "double field offset should match offsetof",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_column_field_flags
  Tests the D_TABLE_COLUMN_FIELD_FLAGS macro.
  Tests the following:
  - single flag applied correctly
  - multiple flags combined with bitwise OR
  - element_size and offset still derived correctly
*/
bool
d_tests_sa_table_common_column_field_flags
(
    struct d_test_counter* _counter
)
{
    bool result;

    struct d_table_column_desc col_ro;
    struct d_table_column_desc col_multi;

    result = true;

    // test 1: single flag (READ_ONLY)
    col_ro = (struct d_table_column_desc)
        D_TABLE_COLUMN_FIELD_FLAGS(struct d_test_table_row,
                                   id,
                                   D_TABLE_COLUMN_FLAG_READ_ONLY);

    result = d_assert_standalone(
        col_ro.flags == D_TABLE_COLUMN_FLAG_READ_ONLY,
        "column_field_flags_single",
        "Single flag should be D_TABLE_COLUMN_FLAG_READ_ONLY",
        _counter) && result;

    result = d_assert_standalone(
        col_ro.element_size == sizeof(int) &&
        col_ro.offset == offsetof(struct d_test_table_row, id),
        "column_field_flags_layout",
        "Size and offset should still be correct with flags",
        _counter) && result;

    // test 2: multiple flags combined
    col_multi = (struct d_table_column_desc)
        D_TABLE_COLUMN_FIELD_FLAGS(
            struct d_test_table_row,
            name,
            D_TABLE_COLUMN_FLAG_KEY | D_TABLE_COLUMN_FLAG_NULLABLE);

    result = d_assert_standalone(
        (col_multi.flags & D_TABLE_COLUMN_FLAG_KEY) != 0,
        "column_field_flags_multi_key",
        "KEY flag should be set",
        _counter) && result;

    result = d_assert_standalone(
        (col_multi.flags & D_TABLE_COLUMN_FLAG_NULLABLE) != 0,
        "column_field_flags_multi_nullable",
        "NULLABLE flag should be set",
        _counter) && result;

    result = d_assert_standalone(
        (col_multi.flags & D_TABLE_COLUMN_FLAG_READ_ONLY) == 0,
        "column_field_flags_multi_no_ro",
        "READ_ONLY flag should NOT be set",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_column_full
  Tests the D_TABLE_COLUMN_FULL macro.
  Tests the following:
  - explicit byte_size, byte_offset, and flags set correctly
  - values are not derived from any struct
*/
bool
d_tests_sa_table_common_column_full
(
    struct d_test_counter* _counter
)
{
    bool result;

    struct d_table_column_desc col;

    result = true;

    // test 1: fully explicit descriptor
    col = (struct d_table_column_desc)
        D_TABLE_COLUMN_FULL(8, 16, D_TABLE_COLUMN_FLAG_HIDDEN);

    result = d_assert_standalone(
        col.element_size == 8,
        "column_full_size",
        "element_size should be 8",
        _counter) && result;

    result = d_assert_standalone(
        col.offset == 16,
        "column_full_offset",
        "offset should be 16",
        _counter) && result;

    result = d_assert_standalone(
        col.flags == D_TABLE_COLUMN_FLAG_HIDDEN,
        "column_full_flags",
        "flags should be D_TABLE_COLUMN_FLAG_HIDDEN",
        _counter) && result;

    // test 2: zero offset, no flags
    col = (struct d_table_column_desc)
        D_TABLE_COLUMN_FULL(4, 0, D_TABLE_COLUMN_FLAG_NONE);

    result = d_assert_standalone(
        col.element_size == 4 && col.offset == 0 &&
        col.flags == D_TABLE_COLUMN_FLAG_NONE,
        "column_full_zero_offset",
        "Zero offset and no flags should work correctly",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_columns
  Tests the D_TABLE_COLUMNS macro.
  Tests the following:
  - creates a valid array of column descriptors
  - individual descriptors accessible by index
*/
bool
d_tests_sa_table_common_columns
(
    struct d_test_counter* _counter
)
{
    bool result;

    static const struct d_table_column_desc cols[] =
        D_TABLE_COLUMNS(
            D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
            D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
            D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
        );

    result = true;

    // test 1: first column is the id field
    result = d_assert_standalone(
        cols[0].element_size == sizeof(int),
        "columns_first_size",
        "First column element_size should be sizeof(int)",
        _counter) && result;

    // test 2: second column is the name field
    result = d_assert_standalone(
        cols[1].offset == offsetof(struct d_test_table_row, name),
        "columns_second_offset",
        "Second column offset should match name field",
        _counter) && result;

    // test 3: third column is the value field
    result = d_assert_standalone(
        cols[2].element_size == sizeof(double),
        "columns_third_size",
        "Third column element_size should be sizeof(double)",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_column_count
  Tests the D_TABLE_COLUMN_COUNT macro.
  Tests the following:
  - correct count for a 3-column array
  - correct count for a 1-column array
*/
bool
d_tests_sa_table_common_column_count
(
    struct d_test_counter* _counter
)
{
    bool result;

    static const struct d_table_column_desc cols_3[] =
        D_TABLE_COLUMNS(
            D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
            D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
            D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
        );

    static const struct d_table_column_desc cols_1[] =
        D_TABLE_COLUMNS(
            D_TABLE_COLUMN_FIELD(struct d_test_table_row, id)
        );

    result = true;

    // test 1: 3-column array
    result = d_assert_standalone(
        D_TABLE_COLUMN_COUNT(cols_3) == 3,
        "column_count_three",
        "D_TABLE_COLUMN_COUNT should return 3",
        _counter) && result;

    // test 2: 1-column array
    result = d_assert_standalone(
        D_TABLE_COLUMN_COUNT(cols_1) == 1,
        "column_count_one",
        "D_TABLE_COLUMN_COUNT should return 1",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_column_desc_all
  Aggregation function that runs all column descriptor macro tests.
*/
bool
d_tests_sa_table_common_column_desc_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Column Descriptor Macros\n");
    printf("  ------------------------------------\n");

    result = d_tests_sa_table_common_column_field(_counter)       && result;
    result = d_tests_sa_table_common_column_field_flags(_counter) && result;
    result = d_tests_sa_table_common_column_full(_counter)        && result;
    result = d_tests_sa_table_common_columns(_counter)            && result;
    result = d_tests_sa_table_common_column_count(_counter)       && result;

    return result;
}
