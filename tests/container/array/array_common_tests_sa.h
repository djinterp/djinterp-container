/******************************************************************************
* djinterp [container]                                 array_common_tests_sa.h
*
* This is the main header for `array_common.h` unit tests.
* For the file itself, go to `\inc\container\array\array_common.h`.
* Note: this module is required to build DTest, so it uses `test_standalone.h`,
* rather than DTest for unit testing. Any modules that are not dependencies of
* DTest should use DTest for unit tests.
*
* Test organization:
*   - array_common_tests_init.c     : Initialization function tests
*   - array_common_tests_util.c     : Utility function tests
*   - array_common_tests_manip.c    : Manipulation function tests  
*   - array_common_tests_free.c     : Cleanup/free function tests
*   - array_common_tests_sa.c       : Comprehensive test runner
* 
* 
* path:      \test\container\array\array_common_tests_sa.h             
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.10.25
******************************************************************************/

#ifndef DJINTERP_TESTING_CONTAINER_ARRAY_COMMON_
#define DJINTERP_TESTING_CONTAINER_ARRAY_COMMON_ 1

#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include "..\..\..\inc\djinterp.h"
#include "..\..\..\inc\dmemory.h"
#include "..\..\..\inc\test\test_standalone.h"
#include "..\..\..\inc\container\array\array_common.h"


/******************************************************************************
 * HELPER FUNCTION DECLARATIONS
 *****************************************************************************/

// d_test_init_from_args_wrapper
//   helper: wrapper for d_array_common_init_from_args that handles va_list.
bool d_test_init_from_args_wrapper(void**  _elements,
                                   size_t* _count,
                                   size_t  _element_size,
                                   size_t  _arg_count,
                                   ...);

// d_test_int_comparator
//   helper: comparator function for integer values.
int d_test_int_comparator(const void* _a, const void* _b);

// d_test_free_int_ptr
//   helper: free function for integer pointers.
void d_test_free_int_ptr(void* _ptr);


/******************************************************************************
 * INITIALIZATION FUNCTION TESTS
 * (defined in array_common_tests_init.c)
 *****************************************************************************/

// d_array_common_init_* function tests (in header order)
bool d_tests_sa_array_common_init_sized(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_init_from_array(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_init_from_args(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_init_copy(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_init_copy_reverse(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_init_copy_range(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_init_copy_range_reverse(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_init_fill(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_init_slice(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_init_slice_reverse(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_init_slice_range(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_init_slice_range_reverse(struct d_test_counter* _test_info);

// initialization test group runner
bool d_tests_sa_array_common_run_init_tests(struct d_test_counter* _test_info);


/******************************************************************************
 * UTILITY FUNCTION TESTS
 * (defined in array_common_tests_util.c)
 *****************************************************************************/

// d_array_common utility function tests (in header order)
bool d_tests_sa_array_common_alloc(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_append_element(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_append_elements(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_calc_capacity(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_contains(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_fill(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_find(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_find_closest(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_validate_params(struct d_test_counter* _test_info);

// utility test group runner
bool d_tests_sa_array_common_run_util_tests(struct d_test_counter* _test_info);


/******************************************************************************
 * MANIPULATION FUNCTION TESTS
 * (defined in array_common_tests_manip.c)
 *****************************************************************************/

// d_array_common manipulation function tests (in header order)
bool d_tests_sa_array_common_insert_element(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_insert_elements(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_is_valid_resize_amount(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_is_valid_resize_factor(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_prepend_element(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_prepend_elements(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_resize_amount(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_resize_factor(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_reverse(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_shift_left(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_shift_left_circular(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_shift_right(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_shift_right_circular(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_sort(struct d_test_counter* _test_info);

// manipulation test group runner
bool d_tests_sa_array_common_run_manip_tests(struct d_test_counter* _test_info);


/******************************************************************************
 * CLEANUP/FREE FUNCTION TESTS
 * (defined in array_common_tests_free.c)
 *****************************************************************************/

// d_array_common cleanup function tests (in header order)
bool d_tests_sa_array_common_free_elements_arr(struct d_test_counter* _test_info);
bool d_tests_sa_array_common_free_elements_deep(struct d_test_counter* _test_info);

// cleanup test group runner
bool d_tests_sa_array_common_run_free_tests(struct d_test_counter* _test_info);


/******************************************************************************
 * COMPREHENSIVE TEST SUITE RUNNERS
 * (defined in array_common_tests_sa.c)
 *****************************************************************************/

// d_tests_sa_array_common_all
//   function: runs all basic array_common test functions.
bool d_tests_sa_array_common_all(struct d_test_counter* _test_info);

// d_tests_sa_array_common_all_extended  
//   function: runs ALL array_common test functions including all available 
// functionality.
bool d_tests_sa_array_common_all_extended(struct d_test_counter* _test_info);

// d_tests_sa_array_common_run_all
//   function: main entry point for the test module runner. Returns pass/fail
// status for use with the unified test runner.
bool d_tests_sa_array_common_run_all(struct d_test_counter* _test_info);


#endif	// DJINTERP_TESTING_CONTAINER_ARRAY_COMMON_
