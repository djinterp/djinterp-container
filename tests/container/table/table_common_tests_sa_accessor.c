#include ".\table_common_tests_sa.h"


// --- shared test data for accessor tests ---

static struct d_test_table_row g_accessor_rows[] =
{
    { .id = 10, .name = "alpha",   .value = 1.5 },
    { .id = 20, .name = "bravo",   .value = 2.5 },
    { .id = 30, .name = "charlie", .value = 3.5 }
};

static const struct d_table_column_desc g_accessor_cols[] =
    D_TABLE_COLUMNS(
        D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
        D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
        D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
    );

static const size_t g_accessor_struct_size =
    sizeof(struct d_test_table_row);


/*
d_tests_sa_table_common_row_at
  Tests the D_TABLE_ROW_AT macro.
  Tests the following:
  - row 0 points to the start of the array
  - row 1 points exactly struct_size bytes ahead
  - row 2 points exactly 2 * struct_size bytes ahead
*/
bool
d_tests_sa_table_common_row_at
(
    struct d_test_counter* _counter
)
{
    bool  result;
    void* ptr;

    result = true;

    // test 1: row 0 points to start
    ptr    = D_TABLE_ROW_AT(g_accessor_rows, g_accessor_struct_size, 0);
    result = d_assert_standalone(
        ptr == (void*)&g_accessor_rows[0],
        "row_at_zero",
        "Row 0 should point to start of array",
        _counter) && result;

    // test 2: row 1
    ptr    = D_TABLE_ROW_AT(g_accessor_rows, g_accessor_struct_size, 1);
    result = d_assert_standalone(
        ptr == (void*)&g_accessor_rows[1],
        "row_at_one",
        "Row 1 should point to second element",
        _counter) && result;

    // test 3: row 2
    ptr    = D_TABLE_ROW_AT(g_accessor_rows, g_accessor_struct_size, 2);
    result = d_assert_standalone(
        ptr == (void*)&g_accessor_rows[2],
        "row_at_two",
        "Row 2 should point to third element",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_row_at_t
  Tests the D_TABLE_ROW_AT_T macro.
  Tests the following:
  - returns a typed pointer
  - fields accessible through the typed pointer
*/
bool
d_tests_sa_table_common_row_at_t
(
    struct d_test_counter* _counter
)
{
    bool                     result;
    struct d_test_table_row* typed_ptr;

    result = true;

    // test 1: typed access to row 0
    typed_ptr = D_TABLE_ROW_AT_T(struct d_test_table_row*,
                                 g_accessor_rows,
                                 g_accessor_struct_size,
                                 0);

    result = d_assert_standalone(
        typed_ptr->id == 10,
        "row_at_t_first_id",
        "Typed row 0 id should be 10",
        _counter) && result;

    // test 2: typed access to row 2
    typed_ptr = D_TABLE_ROW_AT_T(struct d_test_table_row*,
                                 g_accessor_rows,
                                 g_accessor_struct_size,
                                 2);

    result = d_assert_standalone(
        typed_ptr->id == 30 &&
        strcmp(typed_ptr->name, "charlie") == 0,
        "row_at_t_third",
        "Typed row 2 should be {30, \"charlie\", ...}",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_cell_at
  Tests the D_TABLE_CELL_AT macro.
  Tests the following:
  - cell (0, 0) points to first row's id field
  - cell (1, 1) points to second row's name field
  - cell (2, 2) points to third row's value field
  - returned pointer dereferences to correct value
*/
bool
d_tests_sa_table_common_cell_at
(
    struct d_test_counter* _counter
)
{
    bool  result;
    void* ptr;

    result = true;

    // test 1: cell (0, 0) — first row, id column
    ptr    = D_TABLE_CELL_AT(g_accessor_rows,
                             g_accessor_struct_size,
                             g_accessor_cols, 0, 0);
    result = d_assert_standalone(
        *(int*)ptr == 10,
        "cell_at_0_0",
        "Cell (0,0) should dereference to id=10",
        _counter) && result;

    // test 2: cell (1, 1) — second row, name column
    ptr    = D_TABLE_CELL_AT(g_accessor_rows,
                             g_accessor_struct_size,
                             g_accessor_cols, 1, 1);
    result = d_assert_standalone(
        strcmp(*(const char**)ptr, "bravo") == 0,
        "cell_at_1_1",
        "Cell (1,1) should dereference to name=\"bravo\"",
        _counter) && result;

    // test 3: cell (2, 2) — third row, value column
    ptr    = D_TABLE_CELL_AT(g_accessor_rows,
                             g_accessor_struct_size,
                             g_accessor_cols, 2, 2);
    result = d_assert_standalone(
        *(double*)ptr == 3.5,
        "cell_at_2_2",
        "Cell (2,2) should dereference to value=3.5",
        _counter) && result;

    // test 4: cell (0, 2) — first row, value column (cross check)
    ptr    = D_TABLE_CELL_AT(g_accessor_rows,
                             g_accessor_struct_size,
                             g_accessor_cols, 0, 2);
    result = d_assert_standalone(
        *(double*)ptr == 1.5,
        "cell_at_0_2",
        "Cell (0,2) should dereference to value=1.5",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_cell_as
  Tests the D_TABLE_CELL_AS macro.
  Tests the following:
  - reads an int cell correctly
  - reads a const char* cell correctly
  - reads a double cell correctly
*/
bool
d_tests_sa_table_common_cell_as
(
    struct d_test_counter* _counter
)
{
    bool        result;
    int         id_val;
    const char* name_val;
    double      dbl_val;

    result = true;

    // test 1: read int cell
    id_val = D_TABLE_CELL_AS(int,
                             g_accessor_rows,
                             g_accessor_struct_size,
                             g_accessor_cols, 1, 0);
    result = d_assert_standalone(
        id_val == 20,
        "cell_as_int",
        "CELL_AS(int, ..., 1, 0) should return 20",
        _counter) && result;

    // test 2: read const char* cell
    name_val = D_TABLE_CELL_AS(const char*,
                               g_accessor_rows,
                               g_accessor_struct_size,
                               g_accessor_cols, 0, 1);
    result = d_assert_standalone(
        strcmp(name_val, "alpha") == 0,
        "cell_as_str",
        "CELL_AS(const char*, ..., 0, 1) should return \"alpha\"",
        _counter) && result;

    // test 3: read double cell
    dbl_val = D_TABLE_CELL_AS(double,
                              g_accessor_rows,
                              g_accessor_struct_size,
                              g_accessor_cols, 2, 2);
    result = d_assert_standalone(
        dbl_val == 3.5,
        "cell_as_double",
        "CELL_AS(double, ..., 2, 2) should return 3.5",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_cell_size
  Tests the D_TABLE_CELL_SIZE macro.
  Tests the following:
  - returns sizeof(int) for id column
  - returns sizeof(const char*) for name column
  - returns sizeof(double) for value column
*/
bool
d_tests_sa_table_common_cell_size
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    // test 1: id column
    result = d_assert_standalone(
        D_TABLE_CELL_SIZE(g_accessor_cols, 0) == sizeof(int),
        "cell_size_int",
        "Column 0 size should be sizeof(int)",
        _counter) && result;

    // test 2: name column
    result = d_assert_standalone(
        D_TABLE_CELL_SIZE(g_accessor_cols, 1) == sizeof(const char*),
        "cell_size_ptr",
        "Column 1 size should be sizeof(const char*)",
        _counter) && result;

    // test 3: value column
    result = d_assert_standalone(
        D_TABLE_CELL_SIZE(g_accessor_cols, 2) == sizeof(double),
        "cell_size_double",
        "Column 2 size should be sizeof(double)",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_cell_offset
  Tests the D_TABLE_CELL_OFFSET macro.
  Tests the following:
  - returns offsetof for id field
  - returns offsetof for name field
  - returns offsetof for value field
*/
bool
d_tests_sa_table_common_cell_offset
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    // test 1: id column offset
    result = d_assert_standalone(
        D_TABLE_CELL_OFFSET(g_accessor_cols, 0) ==
            offsetof(struct d_test_table_row, id),
        "cell_offset_id",
        "Column 0 offset should match offsetof(id)",
        _counter) && result;

    // test 2: name column offset
    result = d_assert_standalone(
        D_TABLE_CELL_OFFSET(g_accessor_cols, 1) ==
            offsetof(struct d_test_table_row, name),
        "cell_offset_name",
        "Column 1 offset should match offsetof(name)",
        _counter) && result;

    // test 3: value column offset
    result = d_assert_standalone(
        D_TABLE_CELL_OFFSET(g_accessor_cols, 2) ==
            offsetof(struct d_test_table_row, value),
        "cell_offset_value",
        "Column 2 offset should match offsetof(value)",
        _counter) && result;

    return result;
}


/*
d_tests_sa_table_common_accessor_all
  Aggregation function that runs all accessor macro tests.
*/
bool
d_tests_sa_table_common_accessor_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Accessor Macros\n");
    printf("  --------------------------\n");

    result = d_tests_sa_table_common_row_at(_counter)      && result;
    result = d_tests_sa_table_common_row_at_t(_counter)    && result;
    result = d_tests_sa_table_common_cell_at(_counter)     && result;
    result = d_tests_sa_table_common_cell_as(_counter)     && result;
    result = d_tests_sa_table_common_cell_size(_counter)   && result;
    result = d_tests_sa_table_common_cell_offset(_counter) && result;

    return result;
}
