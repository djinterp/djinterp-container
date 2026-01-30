/******************************************************************************
* djinterp [container]                                             min_stack.h
*
*   A (minimal stack) is a LIFO (last-in, first-out) data structure optimized 
* to consume minimal space.
*   This operation only supports peek (check), pop (add), and push (remove) 
* operations, and is in essence implemented as a singly linked-link without 
* compare, search or traversal operations.
*
*
* path:      \inc\container\stack\min_stack.h
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2023.04.26
******************************************************************************/

#ifndef DJINTERP_C_CONTAINER_STACK_MIN_
#define DJINTERP_C_CONTAINER_STACK_MIN_ 1

#include <stdlib.h>
#include "..\..\djinterp.h"
#include "..\container.h"
#include "..\node\linked_node.h"


// d_min_stack
//   struct: a bare-bones stack LIFO (last-in, first-out) data structure
// optimized to consume minimal space in memory.
struct d_min_stack
{
	struct d_linked_node* top;
};

// creation function
struct d_min_stack* d_min_stack_new(void);

// modification functions
void   d_min_stack_clear(struct d_min_stack* _min_stack);
void*  d_min_stack_peek(struct d_min_stack* _min_stack);
void*  d_min_stack_pop(struct d_min_stack* _min_stack);
void*  d_min_stack_push(struct d_min_stack* _min_stack, void* _value);

// destruction function
void   d_min_stack_free(struct d_min_stack* _min_stack);


#endif	// DJINTERP_C_CONTAINER_STACK_MIN_
