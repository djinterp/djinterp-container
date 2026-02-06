/******************************************************************************
* djinterp [container]                                               stack.h
*
* LIFO (last-in, first-out) stack container built on top of `d_linked_node`.
*
*
* path:      \inc\container\stack\stack.h
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.12.21
******************************************************************************/

#ifndef DJINTERP_C_CONTAINER_STACK_
#define DJINTERP_C_CONTAINER_STACK_ 1

#include <stdarg.h>
#include <stdlib.h>
#include "..\..\djinterp.h"
#include "..\..\dmemory.h"
#include "..\container.h"
#include "..\linked_node.h"
#include ".\stack_common.h"


// D_STACK_FOREACH
//   macro: iterate over every node in a stack (top to bottom).
#define D_STACK_FOREACH(_node, _stack_ptr)                                  \
    for (struct d_linked_node* (_node) = (_stack_ptr)->top;                  \
         (_node) != NULL;                                                   \
         (_node) = (_node)->next)

// D_STACK_FOREACH_VALUE
//   macro: iterate over every value pointer in a stack (top to bottom).
#define D_STACK_FOREACH_VALUE(_value, _node, _stack_ptr)                    \
    for (struct d_linked_node* (_node) = (_stack_ptr)->top;                  \
         (_node) != NULL && (((_value) = (_node)->value), true);            \
         (_node) = (_node)->next)


// d_stack
//   struct: a singly-linked stack.
struct d_stack
{
    size_t                count;
    struct d_linked_node* top;
};


// `d_stack` creation functions
struct d_stack* d_stack_new(void);
struct d_stack* d_stack_new_from_arr(const void* const* _values, size_t _value_count);
struct d_stack* d_stack_new_from_args(size_t _arg_count, ...);
struct d_stack* d_stack_new_copy(const struct d_stack* _other);
struct d_stack* d_stack_new_copy_range(const struct d_stack* _other, d_index _start, d_index _end);
struct d_stack* d_stack_new_slice(const struct d_stack* _other, d_index _start);
struct d_stack* d_stack_new_slice_range(const struct d_stack* _other, d_index _start, d_index _end);
struct d_stack* d_stack_new_fill(size_t _count, void* _value);
struct d_stack* d_stack_new_merge(size_t _stack_count, ...);


// `d_stack` manipulation functions
bool  d_stack_push(struct d_stack* _stack, void* _value);
bool  d_stack_push_values(struct d_stack* _stack, const void* const* _values, size_t _value_count);
bool  d_stack_push_args(struct d_stack* _stack, size_t _arg_count, ...);
bool  d_stack_push_stack(struct d_stack* _destination, const struct d_stack* _source);
bool  d_stack_push_stack_range(struct d_stack* _destination, const struct d_stack* _source, d_index _start, d_index _end);

bool  d_stack_insert(struct d_stack* _stack, void* _value, d_index _index);
bool  d_stack_insert_values(struct d_stack* _stack, const void* const* _values, size_t _value_count, d_index _index);
bool  d_stack_insert_stack(struct d_stack* _destination, const struct d_stack* _source, d_index _index);
bool  d_stack_insert_stack_range(struct d_stack* _destination, const struct d_stack* _source, d_index _start, d_index _end, d_index _index);

bool  d_stack_append(struct d_stack* _stack, void* _value);
bool  d_stack_append_values(struct d_stack* _stack, const void* const* _values, size_t _value_count);
bool  d_stack_append_stack(struct d_stack* _destination, const struct d_stack* _source);
bool  d_stack_append_stack_range(struct d_stack* _destination, const struct d_stack* _source, d_index _start, d_index _end);

void* d_stack_peek(const struct d_stack* _stack);
void* d_stack_peek_if_nonnull(const struct d_stack* _stack);
void* d_stack_pop(struct d_stack* _stack);

void* d_stack_get(const struct d_stack* _stack, d_index _index);
bool  d_stack_set(struct d_stack* _stack, d_index _index, void* _value);

struct d_linked_node* d_stack_get_node(const struct d_stack* _stack, d_index _index);

void* d_stack_remove_at(struct d_stack* _stack, d_index _index);
bool  d_stack_remove_first_match(struct d_stack* _stack, const void* _value, fn_comparator _comparator, void** _removed_value);

bool  d_stack_is_empty(const struct d_stack* _stack);

bool    d_stack_contains(const struct d_stack* _stack, const void* _value, fn_comparator _comparator);
ssize_t d_stack_find(const struct d_stack* _stack, const void* _value, fn_comparator _comparator);
struct d_linked_node* d_stack_find_node(const struct d_stack* _stack, const void* _value, fn_comparator _comparator);

bool  d_stack_reverse(struct d_stack* _stack);
void  d_stack_sort(struct d_stack* _stack, fn_comparator _comparator);

bool  d_stack_clear(struct d_stack* _stack);
bool  d_stack_clear_deep(struct d_stack* _stack, fn_free _free_fn);


// memory management
void d_stack_free(struct d_stack* _stack);
void d_stack_free_deep(struct d_stack* _stack, fn_free _free_fn);


#endif // DJINTERP_C_CONTAINER_STACK_
