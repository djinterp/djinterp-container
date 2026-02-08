#include ".\table_common_tests_sa.h"


/*
d_tests_sa_table_common_run_all
  Module-level aggregation function that runs all table_common tests.
  Executes tests for all categories:
  - Column descriptor macros
  - Row and cell value macros
  - Initialization macros and functions
  - Accessor macros
  - Query macros
  - Search functions
  - Utility functions
  - Validation and cleanup functions
*/
bool
d_tests_sa_table_common_run_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    // run all test categories
    result = d_tests_sa_table_common_column_desc_all(_counter) && result;
    result = d_tests_sa_table_common_row_cell_all(_counter)    && result;
    result = d_tests_sa_table_common_init_all(_counter)        && result;
    result = d_tests_sa_table_common_accessor_all(_counter)    && result;
    result = d_tests_sa_table_common_query_all(_counter)       && result;
    result = d_tests_sa_table_common_search_all(_counter)      && result;
    result = d_tests_sa_table_common_utility_all(_counter)     && result;
    result = d_tests_sa_table_common_validation_all(_counter)  && result;

    return result;
}
