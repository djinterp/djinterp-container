/******************************************************************************
* djinterp [test]                                         min_stack_tests_sa.h
*
*   Unit tests for the min_stack module (minimal LIFO stack).
*   Tests cover stack creation, push/pop operations, peek operations,
* memory management, and stress testing.
*
*
* path:      \test\container\stack\min_stack_tests_sa.h
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2026.01.24
******************************************************************************/

#ifndef DJINTERP_MIN_STACK_TESTS_STANDALONE_
#define DJINTERP_MIN_STACK_TESTS_STANDALONE_ 1

#include "..\..\..\inc\test\test_standalone.h"
#include "..\..\..\inc\container\stack\min_stack.h"


/******************************************************************************
 * TEST CONFIGURATION
 *****************************************************************************/

// D_TEST_MIN_STACK_SMALL_SIZE
//   constant: small number of elements for basic tests.
#define D_TEST_MIN_STACK_SMALL_SIZE     5

// D_TEST_MIN_STACK_MEDIUM_SIZE
//   constant: medium number of elements for standard tests.
#define D_TEST_MIN_STACK_MEDIUM_SIZE    20

// D_TEST_MIN_STACK_LARGE_SIZE
//   constant: large number of elements for stress tests.
#define D_TEST_MIN_STACK_LARGE_SIZE     100

// D_TEST_MIN_STACK_STRESS_SIZE
//   constant: very large number for stress testing.
#define D_TEST_MIN_STACK_STRESS_SIZE    1000


/******************************************************************************
 * TEST UTILITY FUNCTIONS
 *****************************************************************************/

// d_test_create_int
//   function: helper to create heap-allocated integer for testing.
int* d_test_min_stack_create_int(int _value);


/******************************************************************************
 * CORE OPERATION TESTS
 *****************************************************************************/

// creation tests
struct d_test_object* d_tests_min_stack_new(void);

// push operation tests
struct d_test_object* d_tests_min_stack_push(void);

// peek operation tests
struct d_test_object* d_tests_min_stack_peek(void);
struct d_test_object* d_tests_min_stack_peek_if_nonnull(void);

// pop operation tests
struct d_test_object* d_tests_min_stack_pop(void);

// core operations aggregator
struct d_test_object* d_tests_min_stack_core_all(void);


/******************************************************************************
 * MEMORY MANAGEMENT TESTS
 *****************************************************************************/

// clear operation tests
struct d_test_object* d_tests_min_stack_clear(void);

// destruction tests
struct d_test_object* d_tests_min_stack_free(void);

// memory management aggregator
struct d_test_object* d_tests_min_stack_memory_all(void);


/******************************************************************************
 * ADVANCED SCENARIO TESTS
 *****************************************************************************/

// integration tests (complex multi-operation scenarios)
struct d_test_object* d_tests_min_stack_integration(void);

// stress tests (high-volume operations)
struct d_test_object* d_tests_min_stack_stress(void);

// advanced tests aggregator
struct d_test_object* d_tests_min_stack_advanced_all(void);


/******************************************************************************
 * MASTER TEST RUNNER
 *****************************************************************************/

// master test runner for all min_stack tests
struct d_test_object* d_tests_min_stack_run_all(void);


#endif  // DJINTERP_MIN_STACK_TESTS_STANDALONE_
