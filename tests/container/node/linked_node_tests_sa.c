/******************************************************************************
* djinterp [container]                             linked_node_tests_standalone.c
*
*   Test suite implementation for the linked_node module.
*
*
* path:      \test\container\node\linked_node_tests_standalone.c 
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2026.01.24
******************************************************************************/

#include ".\linked_node_tests_sa.h"


/******************************************************************************
 * HELPER FUNCTIONS FOR TEST DATA
 *****************************************************************************/

/*
d_test_create_int
  Helper function to create a heap-allocated integer for testing.

Parameter(s):
  _value: integer value to allocate
Return:
  Pointer to newly allocated integer, or NULL on failure.
*/
static int*
d_test_create_int
(
    int _value
)
{
    int* ptr = malloc(sizeof(int));
    
    if (ptr)
    {
        *ptr = _value;
    }
    
    return ptr;
}


/******************************************************************************
 * TEST GROUP: LINKED NODE CREATION
 *****************************************************************************/

/*
d_test_sa_linked_node_creation
  Tests for d_linked_node_new function.

Parameter(s):
  none
Return:
  Test object tree containing all creation-related tests.
*/
struct d_test_object*
d_test_sa_linked_node_creation
(
    void
)
{
    struct d_test_object* group;
    struct d_test_object* test_basic;
    struct d_test_object* test_with_next;
    struct d_test_object* test_null_value;
    int*                  value1;
    int*                  value2;
    int*                  value3;
    struct d_linked_node* node;
    struct d_linked_node* next_node;
    struct d_linked_node* head_node;
    struct d_linked_node* null_value_node;
    size_t                child_idx;

    // create test group
    group = d_test_object_new_interior("d_linked_node_new", 3);
    
    if (!group)
    {
        return NULL;
    }

    child_idx = 0;

    // ===========================================================================
    // Test 1: Basic node creation with NULL next
    // ===========================================================================
    value1 = d_test_create_int(42);
    node   = d_linked_node_new(value1, NULL);

    test_basic = d_test_object_new_interior("basic node creation", 3);
    
    if (test_basic)
    {
        d_test_object_add_child(test_basic,
            D_ASSERT_NOT_NULL("node should be allocated", node,
                             "d_linked_node_new returned NULL"),
            0);
        
        d_test_object_add_child(test_basic,
            D_ASSERT_EQUAL("node value should match", 
                          node ? node->value : NULL, value1,
                          "node value pointer mismatch"),
            1);
        
        d_test_object_add_child(test_basic,
            D_ASSERT_NULL("next pointer should be NULL",
                         node ? node->next : (void*)1,
                         "node next should be NULL"),
            2);
    }
    
    d_test_object_add_child(group, test_basic, child_idx++);

    // ===========================================================================
    // Test 2: Node creation with next pointer
    // ===========================================================================
    value2    = d_test_create_int(99);
    value3    = d_test_create_int(1);
    next_node = d_linked_node_new(value2, NULL);
    head_node = d_linked_node_new(value3, next_node);

    test_with_next = d_test_object_new_interior("node with next pointer", 3);
    
    if (test_with_next)
    {
        d_test_object_add_child(test_with_next,
            D_ASSERT_TRUE("head node links to next",
                         head_node && head_node->next == next_node,
                         "node linking failed"),
            0);
        
        d_test_object_add_child(test_with_next,
            D_ASSERT_EQUAL("head value is correct",
                          (head_node && head_node->value) ? 
                              *(int*)head_node->value : -1,
                          1,
                          "head node value mismatch"),
            1);
        
        d_test_object_add_child(test_with_next,
            D_ASSERT_EQUAL("next value is correct",
                          (next_node && next_node->value) ? 
                              *(int*)next_node->value : -1,
                          99,
                          "next node value mismatch"),
            2);
    }
    
    d_test_object_add_child(group, test_with_next, child_idx++);

    // ===========================================================================
    // Test 3: Node creation with NULL value
    // ===========================================================================
    null_value_node = d_linked_node_new(NULL, NULL);

    test_null_value = d_test_object_new_interior("NULL value handling", 2);
    
    if (test_null_value)
    {
        d_test_object_add_child(test_null_value,
            D_ASSERT_NOT_NULL("node with NULL value should succeed",
                             null_value_node,
                             "d_linked_node_new(NULL, NULL) failed"),
            0);
        
        d_test_object_add_child(test_null_value,
            D_ASSERT_NULL("NULL value should be stored",
                         null_value_node ? null_value_node->value : (void*)1,
                         "NULL value not stored correctly"),
            1);
    }
    
    d_test_object_add_child(group, test_null_value, child_idx++);

    // ===========================================================================
    // Cleanup
    // ===========================================================================
    if (value1) free(value1);
    if (value2) free(value2);
    if (value3) free(value3);
    if (node) d_linked_node_free(node);
    if (next_node) d_linked_node_free(next_node);
    if (head_node) d_linked_node_free(head_node);
    if (null_value_node) d_linked_node_free(null_value_node);

    return group;
}


/******************************************************************************
 * TEST GROUP: LINKED NODE DESTRUCTION
 *****************************************************************************/

/*
d_test_sa_linked_node_destruction
  Tests for d_linked_node_free function.

Parameter(s):
  none
Return:
  Test object tree containing all destruction-related tests.
*/
struct d_test_object*
d_test_sa_linked_node_destruction
(
    void
)
{
    struct d_test_object* group;
    struct d_test_object* test_null_free;
    struct d_test_object* test_valid_free;
    int*                  value;
    struct d_linked_node* node;
    bool                  null_free_completed;
    bool                  valid_free_completed;
    size_t                child_idx;

    // create test group
    group = d_test_object_new_interior("d_linked_node_free", 2);
    
    if (!group)
    {
        return NULL;
    }

    child_idx = 0;

    // ===========================================================================
    // Test 1: Freeing NULL node should not crash
    // ===========================================================================
    null_free_completed = true;
    d_linked_node_free(NULL);
    
    test_null_free = d_test_object_new_interior("NULL node free", 1);
    
    if (test_null_free)
    {
        d_test_object_add_child(test_null_free,
            D_ASSERT_TRUE("freeing NULL should not crash",
                         null_free_completed,
                         "d_linked_node_free(NULL) crashed"),
            0);
    }
    
    d_test_object_add_child(group, test_null_free, child_idx++);

    // ===========================================================================
    // Test 2: Freeing valid node should complete without crash
    // ===========================================================================
    value = d_test_create_int(123);
    node  = d_linked_node_new(value, NULL);
    
    valid_free_completed = true;
    
    if (node)
    {
        d_linked_node_free(node);
    }
    
    test_valid_free = d_test_object_new_interior("valid node free", 1);
    
    if (test_valid_free)
    {
        d_test_object_add_child(test_valid_free,
            D_ASSERT_TRUE("freeing valid node should not crash",
                         valid_free_completed,
                         "d_linked_node_free crashed on valid node"),
            0);
    }
    
    d_test_object_add_child(group, test_valid_free, child_idx++);

    // ===========================================================================
    // Cleanup
    // ===========================================================================
    // Note: node already freed above
    // Note: value is NOT freed here because d_linked_node_free does NOT free
    // the value pointer - only the node structure itself. In real usage, the
    // caller is responsible for managing the value's memory.
    if (value) free(value);

    return group;
}


/******************************************************************************
 * MODULE ENTRY POINT
 *****************************************************************************/

/*
d_test_sa_linked_node_module
  Main test module for linked_node. Creates the complete test tree.

Parameter(s):
  none
Return:
  Test object tree containing all linked_node tests, or NULL on failure.
*/
struct d_test_object*
d_test_sa_linked_node_module
(
    void
)
{
    struct d_test_object* module;
    struct d_test_object* creation_tests;
    struct d_test_object* destruction_tests;
    size_t                child_idx;

    // create module root
    module = d_test_object_new_interior("linked_node", 2);
    
    if (!module)
    {
        return NULL;
    }

    child_idx = 0;

    // add test groups
    creation_tests = d_test_sa_linked_node_creation();
    d_test_object_add_child(module, creation_tests, child_idx++);

    destruction_tests = d_test_sa_linked_node_destruction();
    d_test_object_add_child(module, destruction_tests, child_idx++);

    return module;
}
