#include ".\table_tests_sa.h"


// shared column descriptors for capacity tests
static const struct d_table_column_desc g_cap_cols[] =
{
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, id),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, name),
    D_TABLE_COLUMN_FIELD(struct d_test_table_row, value)
};

static const size_t g_cap_col_count =
    D_TABLE_COLUMN_COUNT(g_cap_cols);


/*
d_tests_sa_table_reserve
  Tests the d_table_reserve function.
  Tests the following:
  - NULL table returns false
  - no-op when new capacity <= current capacity
  - successful reservation with larger capacity
  - elements preserved after reservation
  - view promoted to owned on reserve
*/
bool
d_tests_sa_table_reserve
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    struct d_test_table_row rows[] =
    {
        { 1, "a", 1.0 },
        { 2, "b", 2.0 }
    };

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_reserve(NULL, 100) == false,
        "reserve_null",
        "NULL table should return false",
        _counter) && result;

    // test 2: no-op when capacity sufficient
    tbl = d_table_new(sizeof(struct d_test_table_row), g_cap_cols,
                      g_cap_col_count, 50);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_reserve(tbl, 30) == true,
            "reserve_smaller_success",
            "Smaller capacity should succeed (no-op)",
            _counter) && result;

        result = d_assert_standalone(
            tbl->capacity == 50,
            "reserve_smaller_unchanged",
            "Capacity should remain unchanged",
            _counter) && result;

        d_table_free(tbl);
    }

    // test 3: successful larger reservation
    tbl = d_table_new_from_rows(sizeof(struct d_test_table_row),
                                rows, 2, g_cap_cols, g_cap_col_count);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_reserve(tbl, 100) == true,
            "reserve_larger_success",
            "Larger capacity reservation should succeed",
            _counter) && result;

        result = d_assert_standalone(
            tbl->capacity >= 100,
            "reserve_larger_capacity",
            "Capacity should be at least 100",
            _counter) && result;

        // test 4: elements preserved
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);
            struct d_test_table_row* r1 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 1);

            result = d_assert_standalone(
                r0->id == 1 && r1->id == 2 && tbl->row_count == 2,
                "reserve_preserved",
                "Elements should be preserved after reservation",
                _counter) && result;
        }

        d_table_free(tbl);
    }

    // test 5: view promoted on reserve
    {
        bool           reserve_ok;
        struct d_table view;

        // use d_memset + manual assignment to rule out
        // designated-initializer issues with D_TABLE_VIEW
        d_memset(&view, 0, sizeof(view));
        view.data         = (void*)rows;
        view.column_descs = (struct d_table_column_desc*)g_cap_cols;
        view.struct_size  = sizeof(struct d_test_table_row);
        view.row_count    = 2;
        view.column_count = g_cap_col_count;
        view.capacity     = 0;
        view.flags        = D_TABLE_FLAG_NONE;

        printf("      [DEBUG] sizeof(struct d_table) = %zu\n",
               sizeof(struct d_table));
        printf("      [DEBUG] BEFORE reserve:\n");
        printf("        data         = %p\n",   view.data);
        printf("        struct_size  = %zu\n",  view.struct_size);
        printf("        row_count    = %zu\n",  view.row_count);
        printf("        capacity     = %zu\n",  view.capacity);
        printf("        flags        = 0x%08x\n", (unsigned)view.flags);

        reserve_ok = d_table_reserve(&view, 20);

        printf("      [DEBUG] AFTER reserve (returned %s):\n",
               reserve_ok 
                   ? "true"
                   : "false");
        printf("        data         = %p\n",   view.data);
        printf("        struct_size  = %zu\n",  view.struct_size);
        printf("        row_count    = %zu\n",  view.row_count);
        printf("        capacity     = %zu\n",  view.capacity);
        printf("        flags        = 0x%08x\n", (unsigned)view.flags);

        result = d_assert_standalone(
            reserve_ok == true,
            "reserve_view_promote",
            "Reserve on view should promote to owned",
            _counter) && result;

        result = d_assert_standalone(
            (view.flags & D_TABLE_FLAG_OWNS_DATA) != 0,
            "reserve_view_owns_data",
            "Promoted view should own its data",
            _counter) && result;

        // clean up promoted view
        if (view.flags & D_TABLE_FLAG_OWNS_DATA)
        {
            free(view.data);
        }
    }

    return result;
}


/*
d_tests_sa_table_shrink_to_fit
  Tests the d_table_shrink_to_fit function.
  Tests the following:
  - NULL table returns false
  - successful shrink of over-allocated table
  - elements preserved after shrink
  - shrink of empty table
*/
bool
d_tests_sa_table_shrink_to_fit
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    struct d_test_table_row row = { 10, "shrink", 1.0 };

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_shrink_to_fit(NULL) == false,
        "shrink_null",
        "NULL table should return false",
        _counter) && result;

    // test 2: successful shrink
    tbl = d_table_new(sizeof(struct d_test_table_row), g_cap_cols,
                      g_cap_col_count, 100);

    if (tbl)
    {
        d_table_push_row(tbl, &row);
        row.id = 20;
        d_table_push_row(tbl, &row);
        row.id = 30;
        d_table_push_row(tbl, &row);

        result = d_assert_standalone(
            tbl->capacity == 100 && tbl->row_count == 3,
            "shrink_before",
            "Before shrink: capacity=100, row_count=3",
            _counter) && result;

        result = d_assert_standalone(
            d_table_shrink_to_fit(tbl) == true,
            "shrink_success",
            "Shrink should succeed",
            _counter) && result;

        result = d_assert_standalone(
            tbl->capacity == 3,
            "shrink_capacity",
            "Capacity should shrink to 3",
            _counter) && result;

        // test 3: elements preserved
        {
            struct d_test_table_row* r0 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 0);
            struct d_test_table_row* r2 =
                (struct d_test_table_row*)d_table_row_ptr(tbl, 2);

            result = d_assert_standalone(
                r0->id == 10 && r2->id == 30,
                "shrink_preserved",
                "Elements should be preserved after shrink",
                _counter) && result;
        }

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_ensure_capacity
  Tests the d_table_ensure_capacity function.
  Tests the following:
  - NULL table returns false
  - no-op when sufficient capacity exists
  - successful growth when needed
*/
bool
d_tests_sa_table_ensure_capacity
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_ensure_capacity(NULL, 100) == false,
        "ensure_null",
        "NULL table should return false",
        _counter) && result;

    // test 2: no-op when sufficient
    tbl = d_table_new(sizeof(struct d_test_table_row), g_cap_cols,
                      g_cap_col_count, 50);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_ensure_capacity(tbl, 30) == true,
            "ensure_sufficient",
            "Sufficient capacity should succeed",
            _counter) && result;

        result = d_assert_standalone(
            tbl->capacity == 50,
            "ensure_sufficient_unchanged",
            "Capacity should remain 50",
            _counter) && result;

        d_table_free(tbl);
    }

    // test 3: growth when needed
    tbl = d_table_new(sizeof(struct d_test_table_row), g_cap_cols,
                      g_cap_col_count, 10);

    if (tbl)
    {
        result = d_assert_standalone(
            d_table_ensure_capacity(tbl, 100) == true,
            "ensure_growth",
            "Growth should succeed",
            _counter) && result;

        result = d_assert_standalone(
            tbl->capacity >= 100,
            "ensure_growth_capacity",
            "Capacity should be at least 100",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_grow
  Tests the d_table_grow function.
  Tests the following:
  - NULL table returns false
  - grow increases capacity by growth factor
  - elements preserved after grow
*/
bool
d_tests_sa_table_grow
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;
    size_t          old_cap;

    result = true;

    // test 1: NULL table fails
    result = d_assert_standalone(
        d_table_grow(NULL) == false,
        "grow_null",
        "NULL table should return false",
        _counter) && result;

    // test 2: grow increases capacity
    tbl = d_table_new(sizeof(struct d_test_table_row), g_cap_cols,
                      g_cap_col_count, 10);

    if (tbl)
    {
        old_cap = tbl->capacity;

        result = d_assert_standalone(
            d_table_grow(tbl) == true,
            "grow_success",
            "Grow should succeed",
            _counter) && result;

        // capacity should have increased (3/2 factor: 10 -> 15)
        result = d_assert_standalone(
            tbl->capacity > old_cap,
            "grow_increased",
            "Capacity should have increased after grow",
            _counter) && result;

        result = d_assert_standalone(
            tbl->capacity == (old_cap * D_TABLE_GROWTH_FACTOR_NUM)
                             / D_TABLE_GROWTH_FACTOR_DEN,
            "grow_factor",
            "Capacity should follow growth factor (3/2)",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_available
  Tests the d_table_available function.
  Tests the following:
  - NULL table returns 0
  - empty table returns full capacity
  - partially filled returns remaining
  - full table returns 0
*/
bool
d_tests_sa_table_available
(
    struct d_test_counter* _counter
)
{
    bool            result;
    struct d_table* tbl;
    size_t          avail;

    struct d_test_table_row row = { 1, "x", 0.0 };

    result = true;

    // test 1: NULL table returns 0
    avail  = d_table_available(NULL);
    result = d_assert_standalone(
        avail == 0,
        "available_null",
        "NULL table should return 0",
        _counter) && result;

    // test 2: empty table returns full capacity
    tbl = d_table_new(sizeof(struct d_test_table_row), g_cap_cols,
                      g_cap_col_count, 10);

    if (tbl)
    {
        avail  = d_table_available(tbl);
        result = d_assert_standalone(
            avail == 10,
            "available_empty",
            "Empty table should return full capacity (10)",
            _counter) && result;

        // test 3: partially filled
        d_table_push_row(tbl, &row);
        d_table_push_row(tbl, &row);
        d_table_push_row(tbl, &row);

        avail  = d_table_available(tbl);
        result = d_assert_standalone(
            avail == 7,
            "available_partial",
            "Partially filled should return 7",
            _counter) && result;

        d_table_free(tbl);
    }

    return result;
}


/*
d_tests_sa_table_capacity_all
  Aggregation function that runs all capacity tests.
*/
bool
d_tests_sa_table_capacity_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    printf("\n  [SECTION] Capacity Functions\n");
    printf("  -----------------------------\n");

    result = d_tests_sa_table_reserve(_counter) && result;
    result = d_tests_sa_table_shrink_to_fit(_counter) && result;
    result = d_tests_sa_table_ensure_capacity(_counter) && result;
    result = d_tests_sa_table_grow(_counter) && result;
    result = d_tests_sa_table_available(_counter) && result;

    return result;
}
