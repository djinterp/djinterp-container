#include ".\table_tests_sa.h"


/*
d_tests_sa_table_run_all
  Module-level aggregation function that runs all table tests.
  Executes tests for all categories:
  - Static initialization & accessor macros
  - Constructor functions
  - Capacity functions
  - Row mutation functions
  - Cell access functions
  - Row access functions
  - Query functions
  - Search functions
  - Utility functions
  - Destructor functions
*/
bool
d_tests_sa_table_run_all
(
    struct d_test_counter* _counter
)
{
    bool result;

    result = true;

    // run all test categories
    result = d_tests_sa_table_macro_all(_counter) && result;
    result = d_tests_sa_table_constructor_all(_counter) && result;
    result = d_tests_sa_table_capacity_all(_counter) && result;
    result = d_tests_sa_table_row_mutation_all(_counter) && result;
    result = d_tests_sa_table_cell_access_all(_counter) && result;
    result = d_tests_sa_table_row_access_all(_counter) && result;
    result = d_tests_sa_table_query_all(_counter) && result;
    result = d_tests_sa_table_search_all(_counter) && result;
    result = d_tests_sa_table_utility_all(_counter) && result;
    result = d_tests_sa_table_destructor_all(_counter) && result;

    return result;
}
