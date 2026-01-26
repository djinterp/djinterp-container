/******************************************************************************
* djinterp [container]                                           linked_node.h
*
*   The module contains common building blocks for various graph data structures
* structures that have, at minimum, both a value and one or more edges to 
* adjacent vertices.
*   While this module can be certainly be used by itself, it is intentionally
* quite modest to accomodate the widest-possible range of data structures. As 
* such, users may find more utility in modules implementing this one, rather
* than this module itself.  This list includes: 
*
*
* path:      \inc\container\linked_node.h
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2023.04.26
******************************************************************************/

#ifndef DJINTERP_C_CONTAINER_LINKED_NODE_
#define DJINTERP_C_CONTAINER_LINKED_NODE_ 1

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "..\..\djinterp.h"
#include "..\container.h"


// d_linked_node
//   type: a struct containing only:
// 1. a value pointer, and 
// 2. a single pointer to another `d_linked_node`.
// `d_linked_node` is the foundation for many abstract data structures.
struct d_linked_node
{
	void*                 value;
	struct d_linked_node* next;
};

struct d_linked_node* d_linked_node_new(void*, struct d_linked_node*);

void d_linked_node_free(struct d_linked_node*);


#endif	// DJINTERP_C_CONTAINER_LINKED_NODE_