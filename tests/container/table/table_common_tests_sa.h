/******************************************************************************
* djinterp [test]                                      table_common_tests_sa.h
*
*   Unit test declarations for `table_common.h` module.
*   Provides comprehensive testing of all d_table_common functions and macros
* including column descriptor macros, row and cell value macros, static
* initialization, accessor macros, query macros, search functions, utility
* functions, validation, and cleanup.
*
*
* path:      \tests\container\table\table_common_tests_sa.h
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2026.02.06
******************************************************************************/

#ifndef DJINTERP_TESTS_TABLE_COMMON_SA_
#define DJINTERP_TESTS_TABLE_COMMON_SA_ 1

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "..\..\..\inc\test\test_standalone.h"
#include "..\..\..\inc\container\table\table_common.h"
#include "..\..\..\inc\dstring.h"


/******************************************************************************
 * TEST ROW STRUCT
 *
 *   Shared row type used across all table_common tests. Provides a mix of
 * field types (int, const char*, double) to exercise heterogeneous column
 * access, search, and sort operations.
 *****************************************************************************/

struct d_test_table_row
{
    int         id;
    const char* name;
    double      value;
};


/******************************************************************************
 * I. COLUMN DESCRIPTOR MACRO TESTS
 *
 *   Tests macros from table_common.h section III:
 *   D_TABLE_COLUMN_FIELD, D_TABLE_COLUMN_FIELD_FLAGS, D_TABLE_COLUMN_FULL,
 *   D_TABLE_COLUMNS, D_TABLE_COLUMN_COUNT
 *****************************************************************************/
bool d_tests_sa_table_common_column_field(struct d_test_counter* _counter);
bool d_tests_sa_table_common_column_field_flags(struct d_test_counter* _counter);
bool d_tests_sa_table_common_column_full(struct d_test_counter* _counter);
bool d_tests_sa_table_common_columns(struct d_test_counter* _counter);
bool d_tests_sa_table_common_column_count(struct d_test_counter* _counter);

// I.   aggregation function
bool d_tests_sa_table_common_column_desc_all(struct d_test_counter* _counter);


/******************************************************************************
 * II. ROW & CELL VALUE MACRO TESTS
 *
 *   Tests macros from table_common.h sections IV and V:
 *   D_TABLE_ROW, D_TABLE_ROW_PTR, D_TABLE_ROWS, D_TABLE_ROW_COUNT,
 *   D_TABLE_CELL_INT, D_TABLE_CELL_UINT, D_TABLE_CELL_FLOAT,
 *   D_TABLE_CELL_DOUBLE, D_TABLE_CELL_STR, D_TABLE_CELL_PTR,
 *   D_TABLE_CELL_BOOL, D_TABLE_CELL_NULL, D_TABLE_CELL_TYPED
 *****************************************************************************/
bool d_tests_sa_table_common_row_macro(struct d_test_counter* _counter);
bool d_tests_sa_table_common_row_ptr_macro(struct d_test_counter* _counter);
bool d_tests_sa_table_common_rows_macro(struct d_test_counter* _counter);
bool d_tests_sa_table_common_row_count_macro(struct d_test_counter* _counter);
bool d_tests_sa_table_common_cell_value_macros(struct d_test_counter* _counter);

// II.  aggregation function
bool d_tests_sa_table_common_row_cell_all(struct d_test_counter* _counter);


/******************************************************************************
 * III. INITIALIZATION TESTS
 *
 *   Tests the D_TABLE_INIT macro (table_common.h section VI) and the
 *   initialization functions d_table_common_init and
 *   d_table_common_init_copy (table_common.h section IX.1).
 *****************************************************************************/
bool d_tests_sa_table_common_init_macro(struct d_test_counter* _counter);
bool d_tests_sa_table_common_init(struct d_test_counter* _counter);
bool d_tests_sa_table_common_init_copy(struct d_test_counter* _counter);

// III. aggregation function
bool d_tests_sa_table_common_init_all(struct d_test_counter* _counter);


/******************************************************************************
 * IV. ACCESSOR MACRO TESTS
 *
 *   Tests macros from table_common.h section VII:
 *   D_TABLE_ROW_AT, D_TABLE_ROW_AT_T, D_TABLE_CELL_AT,
 *   D_TABLE_CELL_AS, D_TABLE_CELL_SIZE, D_TABLE_CELL_OFFSET
 *****************************************************************************/
bool d_tests_sa_table_common_row_at(struct d_test_counter* _counter);
bool d_tests_sa_table_common_row_at_t(struct d_test_counter* _counter);
bool d_tests_sa_table_common_cell_at(struct d_test_counter* _counter);
bool d_tests_sa_table_common_cell_as(struct d_test_counter* _counter);
bool d_tests_sa_table_common_cell_size(struct d_test_counter* _counter);
bool d_tests_sa_table_common_cell_offset(struct d_test_counter* _counter);

// IV.  aggregation function
bool d_tests_sa_table_common_accessor_all(struct d_test_counter* _counter);


/******************************************************************************
 * V. QUERY MACRO TESTS
 *
 *   Tests macros from table_common.h section VIII:
 *   D_TABLE_IS_EMPTY, D_TABLE_TOTAL_SIZE, D_TABLE_IS_VALID_ROW,
 *   D_TABLE_IS_VALID_COLUMN, D_TABLE_IS_VALID_CELL
 *****************************************************************************/
bool d_tests_sa_table_common_is_empty(struct d_test_counter* _counter);
bool d_tests_sa_table_common_total_size(struct d_test_counter* _counter);
bool d_tests_sa_table_common_is_valid_row(struct d_test_counter* _counter);
bool d_tests_sa_table_common_is_valid_column(struct d_test_counter* _counter);
bool d_tests_sa_table_common_is_valid_cell(struct d_test_counter* _counter);

// V.   aggregation function
bool d_tests_sa_table_common_query_all(struct d_test_counter* _counter);


/******************************************************************************
 * VI. SEARCH FUNCTION TESTS
 *
 *   Tests functions from table_common.h section IX.2:
 *   d_table_common_find_row, d_table_common_find_row_last
 *****************************************************************************/
bool d_tests_sa_table_common_find_row(struct d_test_counter* _counter);
bool d_tests_sa_table_common_find_row_last(struct d_test_counter* _counter);

// VI.  aggregation function
bool d_tests_sa_table_common_search_all(struct d_test_counter* _counter);


/******************************************************************************
 * VII. UTILITY FUNCTION TESTS
 *
 *   Tests functions from table_common.h section IX.3:
 *   d_table_common_swap_rows, d_table_common_sort_by_column,
 *   d_table_common_copy_to
 *****************************************************************************/
bool d_tests_sa_table_common_swap_rows(struct d_test_counter* _counter);
bool d_tests_sa_table_common_sort_by_column(struct d_test_counter* _counter);
bool d_tests_sa_table_common_copy_to(struct d_test_counter* _counter);

// VII. aggregation function
bool d_tests_sa_table_common_utility_all(struct d_test_counter* _counter);


/******************************************************************************
 * VIII. VALIDATION & CLEANUP FUNCTION TESTS
 *
 *   Tests functions from table_common.h sections IX.4 and IX.5:
 *   d_table_common_column_desc_validate, d_table_common_free_data
 *****************************************************************************/
bool d_tests_sa_table_common_column_desc_validate(struct d_test_counter* _counter);
bool d_tests_sa_table_common_free_data(struct d_test_counter* _counter);

// VIII. aggregation function
bool d_tests_sa_table_common_validation_all(struct d_test_counter* _counter);


/******************************************************************************
 * MODULE-LEVEL AGGREGATION
 *****************************************************************************/
bool d_tests_sa_table_common_run_all(struct d_test_counter* _counter);


#endif  // DJINTERP_TESTS_TABLE_COMMON_SA_
