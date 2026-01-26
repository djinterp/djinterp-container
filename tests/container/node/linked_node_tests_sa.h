/******************************************************************************
* djinterp [container]                            linked_node_tests_standalone.h
*
*   Test suite for the linked_node module using the standalone test framework.
* This module is a dependency of DTest, so it uses test_standalone.h rather
* than DTest for unit testing.
*
*
* path:      \test\container\node\linked_node_tests_standalone.h 
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2026.01.24
******************************************************************************/

#ifndef DJINTERP_TESTING_CONTAINER_LINKED_NODE_STANDALONE_
#define DJINTERP_TESTING_CONTAINER_LINKED_NODE_STANDALONE_ 1

#include "..\..\..\inc\djinterp.h"
#include "..\..\..\inc\test\test_standalone.h"
#include "..\..\..\inc\container\node\linked_node.h"


/******************************************************************************
 * TEST SUITE FUNCTIONS
 *****************************************************************************/

// d_test_sa_linked_node_module
//   function: runs all linked_node tests and returns the test object tree.
// This is the main entry point for the linked_node test module.
struct d_test_object* d_test_sa_linked_node_module(void);


/******************************************************************************
 * INDIVIDUAL TEST GROUP FUNCTIONS
 *****************************************************************************/

// d_test_sa_linked_node_creation
//   function: tests for d_linked_node_new and related creation functions.
struct d_test_object* d_test_sa_linked_node_creation(void);

// d_test_sa_linked_node_destruction
//   function: tests for d_linked_node_free and memory management.
struct d_test_object* d_test_sa_linked_node_destruction(void);


#endif  // DJINTERP_TESTING_CONTAINER_LINKED_NODE_STANDALONE_
