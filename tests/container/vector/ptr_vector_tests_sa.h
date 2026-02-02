/******************************************************************************
* djinterp [test]                                          ptr_vector_tests_sa.h
*
*   Unit test declarations for `ptr_vector.h` module.
*   Provides comprehensive testing of all d_ptr_vector struct wrapper functions
* including constructors, capacity management, element manipulation, 
* append/prepend operations, resize operations, access functions, query 
* functions, search functions, utility functions, iteration helpers, and 
* destructor operations.
*
*
* path:      \tests\container\vector\ptr_vector_tests_sa.h
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.01.29
******************************************************************************/

#ifndef DJINTERP_TESTS_PTR_VECTOR_SA_
#define DJINTERP_TESTS_PTR_VECTOR_SA_ 1

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "..\..\..\inc\test\test_standalone.h"
#include "..\..\..\inc\container\vector\vector.h"
#include "..\..\..\inc\container\vector\ptr_vector.h"
#include "..\..\..\inc\string_fn.h"


/******************************************************************************
 * I. CONSTRUCTOR FUNCTION TESTS
 *****************************************************************************/

/*
 * d_tests_sa_ptr_vector_new
 *   Tests basic constructor with specified initial capacity.
 */
bool d_tests_sa_ptr_vector_new(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_new_default
 *   Tests default constructor with D_VECTOR_DEFAULT_CAPACITY.
 */
bool d_tests_sa_ptr_vector_new_default(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_new_from_array
 *   Tests constructor that initializes from an array of pointers.
 */
bool d_tests_sa_ptr_vector_new_from_array(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_new_from_args
 *   Tests constructor that initializes from variadic pointer arguments.
 */
bool d_tests_sa_ptr_vector_new_from_args(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_new_copy
 *   Tests copy constructor for shallow copying.
 */
bool d_tests_sa_ptr_vector_new_copy(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_new_fill
 *   Tests constructor that fills with a specified pointer value.
 */
bool d_tests_sa_ptr_vector_new_fill(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_new_merge
 *   Tests constructor that merges multiple ptr_vectors.
 */
bool d_tests_sa_ptr_vector_new_merge(struct d_test_counter* _counter);

/* I.   aggregation function */
bool d_tests_sa_ptr_vector_constructor_all(struct d_test_counter* _counter);


/******************************************************************************
 * II. CAPACITY MANAGEMENT FUNCTION TESTS
 *****************************************************************************/

/*
 * d_tests_sa_ptr_vector_reserve
 *   Tests reserving capacity for a minimum number of pointers.
 */
bool d_tests_sa_ptr_vector_reserve(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_shrink_to_fit
 *   Tests shrinking capacity to match current count.
 */
bool d_tests_sa_ptr_vector_shrink_to_fit(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_ensure_capacity
 *   Tests ensuring minimum capacity is available.
 */
bool d_tests_sa_ptr_vector_ensure_capacity(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_available
 *   Tests querying available slots without reallocation.
 */
bool d_tests_sa_ptr_vector_available(struct d_test_counter* _counter);

/* II.  aggregation function */
bool d_tests_sa_ptr_vector_capacity_all(struct d_test_counter* _counter);


/******************************************************************************
 * III. ELEMENT MANIPULATION FUNCTION TESTS
 *****************************************************************************/

/*
 * d_tests_sa_ptr_vector_push_back
 *   Tests appending a pointer to the end.
 */
bool d_tests_sa_ptr_vector_push_back(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_push_front
 *   Tests prepending a pointer to the beginning.
 */
bool d_tests_sa_ptr_vector_push_front(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_pop_back
 *   Tests removing and returning the last pointer.
 */
bool d_tests_sa_ptr_vector_pop_back(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_pop_front
 *   Tests removing and returning the first pointer.
 */
bool d_tests_sa_ptr_vector_pop_front(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_insert
 *   Tests inserting a pointer at a specified index.
 */
bool d_tests_sa_ptr_vector_insert(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_insert_range
 *   Tests inserting multiple pointers at a specified index.
 */
bool d_tests_sa_ptr_vector_insert_range(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_erase
 *   Tests removing a pointer at a specified index without returning it.
 */
bool d_tests_sa_ptr_vector_erase(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_erase_range
 *   Tests removing a range of pointers.
 */
bool d_tests_sa_ptr_vector_erase_range(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_remove
 *   Tests removing and returning a pointer at a specified index.
 */
bool d_tests_sa_ptr_vector_remove(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_clear
 *   Tests clearing all pointers without deallocating storage.
 */
bool d_tests_sa_ptr_vector_clear(struct d_test_counter* _counter);

/* III. aggregation function */
bool d_tests_sa_ptr_vector_element_all(struct d_test_counter* _counter);


/******************************************************************************
 * IV. APPEND/EXTEND FUNCTION TESTS
 *****************************************************************************/

/*
 * d_tests_sa_ptr_vector_append
 *   Tests appending multiple pointers from an array.
 */
bool d_tests_sa_ptr_vector_append(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_append_vector
 *   Tests appending all pointers from another ptr_vector.
 */
bool d_tests_sa_ptr_vector_append_vector(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_prepend
 *   Tests prepending multiple pointers from an array.
 */
bool d_tests_sa_ptr_vector_prepend(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_prepend_vector
 *   Tests prepending all pointers from another ptr_vector.
 */
bool d_tests_sa_ptr_vector_prepend_vector(struct d_test_counter* _counter);

/* IV.  aggregation function */
bool d_tests_sa_ptr_vector_append_all(struct d_test_counter* _counter);


/******************************************************************************
 * V. RESIZE FUNCTION TESTS
 *****************************************************************************/

/*
 * d_tests_sa_ptr_vector_resize
 *   Tests resizing to a new count (new slots initialized to NULL).
 */
bool d_tests_sa_ptr_vector_resize(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_resize_fill
 *   Tests resizing with a fill value for new slots.
 */
bool d_tests_sa_ptr_vector_resize_fill(struct d_test_counter* _counter);

/* V.   aggregation function */
bool d_tests_sa_ptr_vector_resize_all(struct d_test_counter* _counter);


/******************************************************************************
 * VI. ACCESS FUNCTION TESTS
 *****************************************************************************/

/*
 * d_tests_sa_ptr_vector_at
 *   Tests element access at a specified index (supports negative indexing).
 */
bool d_tests_sa_ptr_vector_at(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_front
 *   Tests accessing the first pointer.
 */
bool d_tests_sa_ptr_vector_front(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_back
 *   Tests accessing the last pointer.
 */
bool d_tests_sa_ptr_vector_back(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_data
 *   Tests getting the underlying array pointer.
 */
bool d_tests_sa_ptr_vector_data(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_get
 *   Tests get (alias for at).
 */
bool d_tests_sa_ptr_vector_get(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_set
 *   Tests setting a pointer at a specified index.
 */
bool d_tests_sa_ptr_vector_set(struct d_test_counter* _counter);

/* VI.  aggregation function */
bool d_tests_sa_ptr_vector_access_all(struct d_test_counter* _counter);


/******************************************************************************
 * VII. QUERY FUNCTION TESTS
 *****************************************************************************/

/*
 * d_tests_sa_ptr_vector_is_empty
 *   Tests checking if vector contains no pointers.
 */
bool d_tests_sa_ptr_vector_is_empty(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_is_full
 *   Tests checking if count equals capacity.
 */
bool d_tests_sa_ptr_vector_is_full(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_size
 *   Tests getting the number of pointers.
 */
bool d_tests_sa_ptr_vector_size(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_capacity_query
 *   Tests getting the current capacity.
 */
bool d_tests_sa_ptr_vector_capacity_query(struct d_test_counter* _counter);

/* VII. aggregation function */
bool d_tests_sa_ptr_vector_query_all(struct d_test_counter* _counter);


/******************************************************************************
 * VIII. SEARCH FUNCTION TESTS
 *****************************************************************************/

/*
 * d_tests_sa_ptr_vector_find
 *   Tests finding first occurrence using a comparator.
 */
bool d_tests_sa_ptr_vector_find(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_find_last
 *   Tests finding last occurrence using a comparator.
 */
bool d_tests_sa_ptr_vector_find_last(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_find_ptr
 *   Tests finding a pointer by identity (address comparison).
 */
bool d_tests_sa_ptr_vector_find_ptr(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_contains
 *   Tests checking if a value exists using a comparator.
 */
bool d_tests_sa_ptr_vector_contains(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_contains_ptr
 *   Tests checking if a pointer exists by identity.
 */
bool d_tests_sa_ptr_vector_contains_ptr(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_count_value
 *   Tests counting occurrences of a value using a comparator.
 */
bool d_tests_sa_ptr_vector_count_value(struct d_test_counter* _counter);

/* VIII. aggregation function */
bool d_tests_sa_ptr_vector_search_all(struct d_test_counter* _counter);


/******************************************************************************
 * IX. UTILITY FUNCTION TESTS
 *****************************************************************************/

/*
 * d_tests_sa_ptr_vector_swap
 *   Tests swapping two pointers by index.
 */
bool d_tests_sa_ptr_vector_swap(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_reverse
 *   Tests reversing the order of pointers.
 */
bool d_tests_sa_ptr_vector_reverse(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_sort
 *   Tests sorting pointers using a comparator.
 */
bool d_tests_sa_ptr_vector_sort(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_copy_to
 *   Tests copying pointers to a destination buffer.
 */
bool d_tests_sa_ptr_vector_copy_to(struct d_test_counter* _counter);

/* IX.  aggregation function */
bool d_tests_sa_ptr_vector_utility_all(struct d_test_counter* _counter);


/******************************************************************************
 * X. ITERATION HELPER FUNCTION TESTS
 *****************************************************************************/

/*
 * d_tests_sa_ptr_vector_foreach
 *   Tests applying a function to each pointer.
 */
bool d_tests_sa_ptr_vector_foreach(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_foreach_with_context
 *   Tests applying a function with context to each pointer.
 */
bool d_tests_sa_ptr_vector_foreach_with_context(struct d_test_counter* _counter);

/* X.   aggregation function */
bool d_tests_sa_ptr_vector_iteration_all(struct d_test_counter* _counter);


/******************************************************************************
 * XI. DESTRUCTOR FUNCTION TESTS
 *****************************************************************************/

/*
 * d_tests_sa_ptr_vector_free
 *   Tests freeing the vector without freeing pointed-to objects.
 */
bool d_tests_sa_ptr_vector_free(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_free_deep
 *   Tests freeing the vector and all pointed-to objects.
 */
bool d_tests_sa_ptr_vector_free_deep(struct d_test_counter* _counter);

/*
 * d_tests_sa_ptr_vector_clear_deep
 *   Tests clearing and freeing pointed-to objects while keeping vector valid.
 */
bool d_tests_sa_ptr_vector_clear_deep(struct d_test_counter* _counter);

/* XI.  aggregation function */
bool d_tests_sa_ptr_vector_destructor_all(struct d_test_counter* _counter);


/******************************************************************************
 * MODULE-LEVEL AGGREGATION
 *****************************************************************************/

/*
 * d_tests_sa_ptr_vector_run_all
 *   Runs all ptr_vector tests across all categories.
 */
bool d_tests_sa_ptr_vector_run_all(struct d_test_counter* _counter);


#endif  /* DJINTERP_TESTS_PTR_VECTOR_SA_ */
