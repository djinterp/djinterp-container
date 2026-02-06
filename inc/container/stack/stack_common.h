/******************************************************************************
* djinterp [container]                                         stack_common.h
*
* Common utilities and helper functions for stack implementations.
*
*
* path:      \inc\container\stack\stack_common.h
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.12.21
******************************************************************************/

#ifndef DJINTERP_C_CONTAINER_STACK_COMMON_
#define DJINTERP_C_CONTAINER_STACK_COMMON_ 1

#include <stdarg.h>
#include <stdlib.h>
#include "..\..\djinterp.h"
#include "..\..\dmemory.h"
#include "..\container.h"
#include "..\linked_node.h"


bool d_stack_common_init_empty(struct d_linked_node** _top, size_t* _count);
bool d_stack_common_init_from_arr(struct d_linked_node** _top, size_t* _count, const void* const* _values, size_t _value_count);
bool d_stack_common_init_from_args(struct d_linked_node** _top, size_t* _count, size_t _arg_count, va_list _args);
bool d_stack_common_init_copy(struct d_linked_node** _dst_top, size_t* _dst_count, const struct d_linked_node* _src_top, size_t _src_count);
bool d_stack_common_init_copy_range(struct d_linked_node** _dst_top, size_t* _dst_count, const struct d_linked_node* _src_top, size_t _src_count, d_index _start, d_index _end);

bool  d_stack_common_push_value(struct d_linked_node** _top, size_t* _count, void* _value);
void* d_stack_common_peek_value(const struct d_linked_node* _top);
void* d_stack_common_pop_value(struct d_linked_node** _top, size_t* _count);

bool  d_stack_common_insert_value(struct d_linked_node** _top, size_t* _count, void* _value, d_index _index);
bool  d_stack_common_append_value(struct d_linked_node** _top, size_t* _count, void* _value);

bool  d_stack_common_insert_nodes(struct d_linked_node** _top, size_t* _count, struct d_linked_node* _insert_top, struct d_linked_node* _insert_tail, size_t _insert_count, d_index _index);
bool  d_stack_common_append_nodes(struct d_linked_node** _top, size_t* _count, struct d_linked_node* _insert_top, struct d_linked_node* _insert_tail, size_t _insert_count);

struct d_linked_node* d_stack_common_node_at(struct d_linked_node* _top, d_index _index);
struct d_linked_node* d_stack_common_node_prev_at(struct d_linked_node* _top, d_index _index);

void* d_stack_common_get_value(const struct d_linked_node* _top, d_index _index);
bool  d_stack_common_set_value(struct d_linked_node* _top, d_index _index, void* _value);

struct d_linked_node* d_stack_common_detach_node_at(struct d_linked_node** _top, size_t* _count, d_index _index);
void* d_stack_common_remove_value_at(struct d_linked_node** _top, size_t* _count, d_index _index);
bool  d_stack_common_remove_first_match(struct d_linked_node** _top, size_t* _count, const void* _value, fn_comparator _comparator, void** _removed_value);

bool    d_stack_common_contains(const struct d_linked_node* _top, size_t _count, const void* _value, fn_comparator _comparator);
ssize_t d_stack_common_find(const struct d_linked_node* _top, size_t _count, const void* _value, fn_comparator _comparator);
struct d_linked_node* d_stack_common_find_node(const struct d_linked_node* _top, const void* _value, fn_comparator _comparator);

bool d_stack_common_reverse(struct d_linked_node** _top, size_t _count);
bool d_stack_common_validate_params(struct d_linked_node** _top, const size_t* _count);

void d_stack_common_free_nodes(struct d_linked_node* _top);
void d_stack_common_free_nodes_deep(struct d_linked_node* _top, fn_free _free_fn);


#endif // DJINTERP_C_CONTAINER_STACK_COMMON_
