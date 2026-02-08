/******************************************************************************
* djinterp [test]                                             table_tests_sa.h
*
*   Unit test declarations for `table.h` module.
*   Provides comprehensive testing of all d_table functions including
* static initialization macros, convenience accessor macros, constructors,
* capacity management, row mutation, cell access, row access, query
* functions, search functions, utility functions, and destructors.
*
*
* path:      \tests\container\table\table_tests_sa.h
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2026.02.07
******************************************************************************/

#ifndef DJINTERP_TESTS_TABLE_SA_
#define DJINTERP_TESTS_TABLE_SA_ 1

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "..\..\..\inc\test\test_standalone.h"
#include "..\..\..\inc\container\table\table.h"
#include "..\..\..\inc\string_fn.h"


// ---------------------------------------------------------------------------
//  shared test row struct
// ---------------------------------------------------------------------------

// d_test_table_row
//   struct: simple three-column row used throughout the table test suite.
// Fields are intentionally varied types (int, const char*, double) to
// exercise byte-offset arithmetic and alignment.
struct d_test_table_row
{
    int         id;
    const char* name;
    double      value;
};


/******************************************************************************
 * I. STATIC INITIALIZATION & ACCESSOR MACRO TESTS (sections III + IV)
 *****************************************************************************/
bool d_tests_sa_table_view_macro(struct d_test_counter* _counter);
bool d_tests_sa_table_empty_macro(struct d_test_counter* _counter);
bool d_tests_sa_table_row_ptr_macro(struct d_test_counter* _counter);
bool d_tests_sa_table_row_as_macro(struct d_test_counter* _counter);
bool d_tests_sa_table_cell_ptr_macro(struct d_test_counter* _counter);
bool d_tests_sa_table_cell_val_macro(struct d_test_counter* _counter);

// I.   aggregation function
bool d_tests_sa_table_macro_all(struct d_test_counter* _counter);


/******************************************************************************
 * II. CONSTRUCTOR FUNCTION TESTS (section V)
 *****************************************************************************/
bool d_tests_sa_table_new(struct d_test_counter* _counter);
bool d_tests_sa_table_new_from_rows(struct d_test_counter* _counter);
bool d_tests_sa_table_new_copy(struct d_test_counter* _counter);
bool d_tests_sa_table_new_fill(struct d_test_counter* _counter);

// II.  aggregation function
bool d_tests_sa_table_constructor_all(struct d_test_counter* _counter);


/******************************************************************************
 * III. CAPACITY FUNCTION TESTS (section VI)
 *****************************************************************************/
bool d_tests_sa_table_reserve(struct d_test_counter* _counter);
bool d_tests_sa_table_shrink_to_fit(struct d_test_counter* _counter);
bool d_tests_sa_table_ensure_capacity(struct d_test_counter* _counter);
bool d_tests_sa_table_grow(struct d_test_counter* _counter);
bool d_tests_sa_table_available(struct d_test_counter* _counter);

// III. aggregation function
bool d_tests_sa_table_capacity_all(struct d_test_counter* _counter);


/******************************************************************************
 * IV. ROW MUTATION FUNCTION TESTS (section VII)
 *****************************************************************************/
bool d_tests_sa_table_push_row(struct d_test_counter* _counter);
bool d_tests_sa_table_push_row_empty(struct d_test_counter* _counter);
bool d_tests_sa_table_insert_row(struct d_test_counter* _counter);
bool d_tests_sa_table_insert_rows(struct d_test_counter* _counter);
bool d_tests_sa_table_remove_row(struct d_test_counter* _counter);
bool d_tests_sa_table_remove_range(struct d_test_counter* _counter);
bool d_tests_sa_table_pop_row(struct d_test_counter* _counter);
bool d_tests_sa_table_clear(struct d_test_counter* _counter);

// IV.  aggregation function
bool d_tests_sa_table_row_mutation_all(struct d_test_counter* _counter);


/******************************************************************************
 * V. CELL ACCESS FUNCTION TESTS (section VIII)
 *****************************************************************************/
bool d_tests_sa_table_cell_ptr(struct d_test_counter* _counter);
bool d_tests_sa_table_cell_get(struct d_test_counter* _counter);
bool d_tests_sa_table_cell_set(struct d_test_counter* _counter);

// V.   aggregation function
bool d_tests_sa_table_cell_access_all(struct d_test_counter* _counter);


/******************************************************************************
 * VI. ROW ACCESS FUNCTION TESTS (section IX)
 *****************************************************************************/
bool d_tests_sa_table_row_ptr(struct d_test_counter* _counter);
bool d_tests_sa_table_row_get(struct d_test_counter* _counter);
bool d_tests_sa_table_row_set(struct d_test_counter* _counter);

// VI.  aggregation function
bool d_tests_sa_table_row_access_all(struct d_test_counter* _counter);


/******************************************************************************
 * VII. QUERY FUNCTION TESTS (section X)
 *****************************************************************************/
bool d_tests_sa_table_row_count(struct d_test_counter* _counter);
bool d_tests_sa_table_column_count(struct d_test_counter* _counter);
bool d_tests_sa_table_capacity_query(struct d_test_counter* _counter);
bool d_tests_sa_table_struct_size(struct d_test_counter* _counter);
bool d_tests_sa_table_is_empty(struct d_test_counter* _counter);
bool d_tests_sa_table_is_full(struct d_test_counter* _counter);
bool d_tests_sa_table_data(struct d_test_counter* _counter);

// VII. aggregation function
bool d_tests_sa_table_query_all(struct d_test_counter* _counter);


/******************************************************************************
 * VIII. SEARCH FUNCTION TESTS (section XI)
 *****************************************************************************/
bool d_tests_sa_table_find_row(struct d_test_counter* _counter);
bool d_tests_sa_table_find_row_last(struct d_test_counter* _counter);
bool d_tests_sa_table_contains(struct d_test_counter* _counter);
bool d_tests_sa_table_count_value(struct d_test_counter* _counter);

// VIII. aggregation function
bool d_tests_sa_table_search_all(struct d_test_counter* _counter);


/******************************************************************************
 * IX. UTILITY FUNCTION TESTS (section XII)
 *****************************************************************************/
bool d_tests_sa_table_swap_rows(struct d_test_counter* _counter);
bool d_tests_sa_table_reverse(struct d_test_counter* _counter);
bool d_tests_sa_table_sort_by_column(struct d_test_counter* _counter);
bool d_tests_sa_table_copy_to(struct d_test_counter* _counter);
bool d_tests_sa_table_validate(struct d_test_counter* _counter);

// IX.  aggregation function
bool d_tests_sa_table_utility_all(struct d_test_counter* _counter);


/******************************************************************************
 * X. DESTRUCTOR FUNCTION TESTS (section XIII)
 *****************************************************************************/
bool d_tests_sa_table_free(struct d_test_counter* _counter);
bool d_tests_sa_table_free_deep(struct d_test_counter* _counter);

// X.   aggregation function
bool d_tests_sa_table_destructor_all(struct d_test_counter* _counter);


/******************************************************************************
 * MODULE-LEVEL AGGREGATION
 *****************************************************************************/
bool d_tests_sa_table_run_all(struct d_test_counter* _counter);


#endif  // DJINTERP_TESTS_TABLE_SA_
