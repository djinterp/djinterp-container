/******************************************************************************
* djinterp [container]                                               stack.c
*
* LIFO (last-in, first-out) stack container built on top of `d_linked_node`.
*
*
* path:      \src\container\stack\stack.c
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.12.21
******************************************************************************/

#include ".\stack.h"





/******************************************************************************
 * INTERNAL HELPERS
 *****************************************************************************/

static bool
d_internal_stack_normalize_index
(
    size_t   _count,
    d_index  _index,
    size_t*  _out_index
)
{
    d_index normalized;

    if (!_out_index)
    {
        return false;
    }

    normalized = _index;

    if (normalized < 0)
    {
        normalized = (d_index)_count + normalized;
    }

    if ( (normalized < 0) || (normalized >= (d_index)_count) )
    {
        return false;
    }

    *_out_index = (size_t)normalized;

    return true;
}

/******************************************************************************
 * CREATION
 *****************************************************************************/

/*
d_stack_new
  Creates a new empty stack.

Return:
  Pointer to new stack, or NULL if allocation failed.
*/
struct d_stack*
d_stack_new
(
    void
)
{
    struct d_stack* result;

    result = NULL;

    result = (struct d_stack*)malloc(sizeof(struct d_stack));
    if (!result)
    {
        return NULL;
    }

    result->count = 0;
    result->top   = NULL;

    return result;
}

/*
d_stack_new_from_arr
  Creates a new stack from an array of value pointers.

Note:
  Values are treated as top-first order: `_values[0]` becomes the top element.
*/
struct d_stack*
d_stack_new_from_arr
(
    const void* const* _values,
    size_t             _value_count
)
{
    struct d_stack* result;
    bool            ok;

    if ( (_value_count > 0) && (!_values) )
    {
        return NULL;
    }

    result = NULL;
    ok     = false;

    result = d_stack_new();
    if (!result)
    {
        return NULL;
    }

    ok = d_stack_common_init_from_arr(&result->top,
                                      &result->count,
                                      _values,
                                      _value_count);
    if (!ok)
    {
        free(result);

        return NULL;
    }

    return result;
}

/*
d_stack_new_from_args
  Creates a new stack from varargs containing value pointers.

Note:
  Values are treated as top-first order.
*/
struct d_stack*
d_stack_new_from_args
(
    size_t _arg_count,
    ...
)
{
    struct d_stack* result;
    va_list         args;
    bool            ok;

    result = NULL;
    ok     = false;

    result = d_stack_new();
    if (!result)
    {
        return NULL;
    }

    va_start(args, _arg_count);

    ok = d_stack_common_init_from_args(&result->top,
                                       &result->count,
                                       _arg_count,
                                       args);

    va_end(args);

    if (!ok)
    {
        free(result);

        return NULL;
    }

    return result;
}

/*
d_stack_new_copy
  Creates a new stack that is a shallow copy of `_other`.
*/
struct d_stack*
d_stack_new_copy
(
    const struct d_stack* _other
)
{
    struct d_stack* result;
    bool            ok;

    if (!_other)
    {
        return NULL;
    }

    result = NULL;
    ok     = false;

    result = d_stack_new();
    if (!result)
    {
        return NULL;
    }

    ok = d_stack_common_init_copy(&result->top,
                                  &result->count,
                                  _other->top,
                                  _other->count);
    if (!ok)
    {
        free(result);

        return NULL;
    }

    return result;
}

/*
d_stack_new_copy_range
  Creates a new stack that is a shallow copy of a range of `_other`.

Note:
  `_start` and `_end` are inclusive indices. Negative indices count backward
  from the end (e.g. -1 is the last element).
*/
struct d_stack*
d_stack_new_copy_range
(
    const struct d_stack* _other,
    d_index               _start,
    d_index               _end
)
{
    struct d_stack* result;
    bool            ok;

    if (!_other)
    {
        return NULL;
    }

    result = NULL;
    ok     = false;

    result = d_stack_new();
    if (!result)
    {
        return NULL;
    }

    ok = d_stack_common_init_copy_range(&result->top,
                                        &result->count,
                                        _other->top,
                                        _other->count,
                                        _start,
                                        _end);
    if (!ok)
    {
        free(result);

        return NULL;
    }

    return result;
}

/*
d_stack_new_slice
  Creates a shallow copy slice from `_start` to the end of `_other`.
*/
struct d_stack*
d_stack_new_slice
(
    const struct d_stack* _other,
    d_index               _start
)
{
    if (!_other)
    {
        return NULL;
    }

    if (_other->count == 0)
    {
        return d_stack_new();
    }

    return d_stack_new_copy_range(_other, _start, (d_index)(_other->count - 1));
}

/*
d_stack_new_slice_range
  Creates a shallow copy slice from `_start` to `_end` within `_other`.
*/
struct d_stack*
d_stack_new_slice_range
(
    const struct d_stack* _other,
    d_index               _start,
    d_index               _end
)
{
    return d_stack_new_copy_range(_other, _start, _end);
}

/*
d_stack_new_fill
  Creates a stack containing `_count` nodes, each with value pointer `_value`.

Note:
  This is a shallow fill (the same pointer is stored in each node).
*/
struct d_stack*
d_stack_new_fill
(
    size_t _count,
    void*  _value
)
{
    struct d_stack*        result;
    struct d_linked_node*  top;
    struct d_linked_node*  tail;
    struct d_linked_node*  node;
    size_t                 i;

    result = NULL;
    top    = NULL;
    tail   = NULL;
    node   = NULL;
    i      = 0;

    result = d_stack_new();
    if (!result)
    {
        return NULL;
    }

    // build top-first order: the first created node becomes the top
    for (i = 0; i < _count; ++i)
    {
        node = d_linked_node_new(_value, NULL);
        if (!node)
        {
            d_stack_common_free_nodes(top);
            free(result);

            return NULL;
        }

        if (!top)
        {
            top  = node;
            tail = node;
        }
        else
        {
            tail->next = node;
            tail       = node;
        }
    }

    result->top   = top;
    result->count = _count;

    return result;
}


/******************************************************************************
 * MANIPULATION
 *****************************************************************************/

/*
d_stack_push
  Pushes a value onto the stack.
*/
bool
d_stack_push
(
    struct d_stack* _stack,
    void*           _value
)
{
    if (!_stack)
    {
        return false;
    }

    return d_stack_common_push_value(&_stack->top, &_stack->count, _value);
}

/*
d_stack_peek
  Returns the value at the top of the stack, or NULL if empty.
*/
void*
d_stack_peek
(
    const struct d_stack* _stack
)
{
    if (!_stack)
    {
        return NULL;
    }

    return d_stack_common_peek_value(_stack->top);
}

/*
d_stack_peek_if_nonnull
  Returns the value at the top of the stack, or NULL if stack is NULL or empty.
*/
void*
d_stack_peek_if_nonnull
(
    const struct d_stack* _stack
)
{
    return d_stack_peek(_stack);
}

/*
d_stack_pop
  Pops the top value from the stack.
*/
void*
d_stack_pop
(
    struct d_stack* _stack
)
{
    if (!_stack)
    {
        return NULL;
    }

    return d_stack_common_pop_value(&_stack->top, &_stack->count);
}

/*
d_stack_get
  Returns the value pointer at an index, or NULL if out of range.

Note:
  Negative indices count backward from the end (e.g. -1 is the last element).
*/
void*
d_stack_get
(
    const struct d_stack* _stack,
    d_index               _index
)
{
    size_t index;

    if (!_stack)
    {
        return NULL;
    }

    index = 0;

    if (!d_internal_stack_normalize_index(_stack->count, _index, &index))
    {
        return NULL;
    }

    return d_stack_common_get_value(_stack->top, (d_index)index);
}


/*
d_stack_set
  Sets the value pointer at an index.

Note:
  Negative indices count backward from the end (e.g. -1 is the last element).

Return:
  true if the index exists, otherwise false.
*/
bool
d_stack_set
(
    struct d_stack* _stack,
    d_index         _index,
    void*           _value
)
{
    size_t index;

    if (!_stack)
    {
        return false;
    }

    index = 0;

    if (!d_internal_stack_normalize_index(_stack->count, _index, &index))
    {
        return false;
    }

    return d_stack_common_set_value(_stack->top, (d_index)index, _value);
}


/*
d_stack_get_node
  Returns the node at an index, or NULL if out of range.

Note:
  Negative indices count backward from the end (e.g. -1 is the last element).
*/
struct d_linked_node*
d_stack_get_node
(
    const struct d_stack* _stack,
    d_index               _index
)
{
    size_t index;

    if (!_stack)
    {
        return NULL;
    }

    index = 0;

    if (!d_internal_stack_normalize_index(_stack->count, _index, &index))
    {
        return NULL;
    }

    return d_stack_common_node_at(_stack->top, (d_index)index);
}


/*
d_stack_remove_at
  Removes a node at an index, frees it, and returns its stored value.

Note:
  Negative indices count backward from the end (e.g. -1 is the last element).
*/
void*
d_stack_remove_at
(
    struct d_stack* _stack,
    d_index         _index
)
{
    size_t index;

    if (!_stack)
    {
        return NULL;
    }

    index = 0;

    if (!d_internal_stack_normalize_index(_stack->count, _index, &index))
    {
        return NULL;
    }

    return d_stack_common_remove_value_at(&_stack->top,
                                          &_stack->count,
                                          (d_index)index);
}


/*
d_stack_remove_first_match
  Removes the first matching value and returns it through `_removed_value`.

Return:
  true if a value was removed, otherwise false.
*/
bool
d_stack_remove_first_match
(
    struct d_stack* _stack,
    const void*     _value,
    fn_comparator   _comparator,
    void**          _removed_value
)
{
    if (!_stack)
    {
        return false;
    }

    return d_stack_common_remove_first_match(&_stack->top,
                                             &_stack->count,
                                             _value,
                                             _comparator,
                                             _removed_value);
}

/*
d_stack_is_empty
  Returns true if the stack is empty (or NULL), otherwise false.
*/
bool
d_stack_is_empty
(
    const struct d_stack* _stack
)
{
    if (!_stack)
    {
        return true;
    }

    return (_stack->count == 0);
}

/*
d_stack_contains
  Returns true if the stack contains the value.
*/
bool
d_stack_contains
(
    const struct d_stack* _stack,
    const void*           _value,
    fn_comparator         _comparator
)
{
    if (!_stack)
    {
        return false;
    }

    return d_stack_common_contains(_stack->top,
                                   _stack->count,
                                   _value,
                                   _comparator);
}

/*
d_stack_find
  Returns the index of the first match, or -1 if not found.
*/
ssize_t
d_stack_find
(
    const struct d_stack* _stack,
    const void*           _value,
    fn_comparator         _comparator
)
{
    if (!_stack)
    {
        return (ssize_t)-1;
    }

    return d_stack_common_find(_stack->top,
                               _stack->count,
                               _value,
                               _comparator);
}

/*
d_stack_find_node
  Returns the first matching node, or NULL if not found.
*/
struct d_linked_node*
d_stack_find_node
(
    const struct d_stack* _stack,
    const void*           _value,
    fn_comparator         _comparator
)
{
    if (!_stack)
    {
        return NULL;
    }

    return d_stack_common_find_node(_stack->top, _value, _comparator);
}

/*
d_stack_reverse
  Reverses the stack order in place.
*/
bool
d_stack_reverse
(
    struct d_stack* _stack
)
{
    if (!_stack)
    {
        return false;
    }

    return d_stack_common_reverse(&_stack->top, _stack->count);
}


/******************************************************************************
 * SORTING
 *****************************************************************************/

static bool
d_internal_stack_sort_before
(
    const void*   _a,
    const void*   _b,
    fn_comparator _comparator
)
{
    if (!_comparator)
    {
        // no comparator: do not reorder
        return false;
    }

    return (_comparator(_a, _b) < 0);
}

/*
d_stack_sort
  Sorts the stack in-place by relinking nodes.

Ordering:
  If `_comparator(a, b) < 0`, `a` will appear before `b` (closer to the top).
*/
void
d_stack_sort
(
    struct d_stack* _stack,
    fn_comparator   _comparator
)
{
    struct d_linked_node* sorted;
    struct d_linked_node* cur;
    struct d_linked_node* next;
    struct d_linked_node* insert_prev;

    if ( (!_stack) || (!_comparator) )
    {
        return;
    }

    sorted = NULL;
    cur    = _stack->top;
    next   = NULL;

    while (cur)
    {
        next = cur->next;
        cur->next = NULL;

        if (!sorted)
        {
            sorted = cur;
            cur    = next;
            continue;
        }

        // insert at head?
        if (d_internal_stack_sort_before(cur->value,
                                         sorted->value,
                                         _comparator))
        {
            cur->next = sorted;
            sorted    = cur;
            cur       = next;
            continue;
        }

        // find insertion point
        insert_prev = sorted;

        while (insert_prev->next &&
               !d_internal_stack_sort_before(cur->value,
                                             insert_prev->next->value,
                                             _comparator))
        {
            insert_prev = insert_prev->next;
        }

        cur->next = insert_prev->next;
        insert_prev->next = cur;

        cur = next;
    }

    _stack->top = sorted;

    return;
}


/******************************************************************************
 * MEMORY MANAGEMENT
 *****************************************************************************/

/*
d_stack_clear
  Clears a stack (shallow; does not free stored values).

Return:
  true if the stack pointer was valid, otherwise false.
*/
bool
d_stack_clear
(
    struct d_stack* _stack
)
{
    if (!_stack)
    {
        return false;
    }

    d_stack_common_free_nodes(_stack->top);

    _stack->count = 0;
    _stack->top   = NULL;

    return true;
}

/*
d_stack_clear_deep
  Clears a stack and frees stored values using `_free_fn`.

Return:
  true if the stack pointer was valid, otherwise false.
*/
bool
d_stack_clear_deep
(
    struct d_stack* _stack,
    fn_free         _free_fn
)
{
    if (!_stack)
    {
        return false;
    }

    d_stack_common_free_nodes_deep(_stack->top, _free_fn);

    _stack->count = 0;
    _stack->top   = NULL;

    return true;
}

/*
d_stack_free
  Frees a stack (shallow; does not free stored values).
*/
void
d_stack_free
(
    struct d_stack* _stack
)
{
    if (_stack)
    {
        d_stack_clear(_stack);

        free(_stack);
    }

    return;
}

/*
d_stack_free_deep
  Frees a stack and also frees stored values using `_free_fn`.
*/
void
d_stack_free_deep
(
    struct d_stack* _stack,
    fn_free         _free_fn
)
{
    if (_stack)
    {
        d_stack_clear_deep(_stack, _free_fn);

        free(_stack);
    }

    return;
}


/******************************************************************************
 * INTERNAL HELPERS (INSERT INDICES)
 *****************************************************************************/

static bool
d_internal_stack_normalize_insert_index
(
    size_t   _count,
    d_index  _index,
    size_t*  _out_index
)
{
    d_index normalized;

    if (!_out_index)
    {
        return false;
    }

    normalized = _index;

    if (normalized < 0)
    {
        normalized = (d_index)_count + normalized;
    }

    if ( (normalized < 0) || (normalized > (d_index)_count) )
    {
        return false;
    }

    *_out_index = (size_t)normalized;

    return true;
}


/******************************************************************************
 * EXTENDED CREATION
 *****************************************************************************/

/*
d_stack_new_merge
  Creates a new stack by concatenating multiple stacks (shallow copy).

Note:
  Stacks are concatenated in the given order, from first to last.
  Within each source stack, values retain their top-to-bottom order.
*/
struct d_stack*
d_stack_new_merge
(
    size_t _stack_count,
    ...
)
{
    struct d_stack*        result;
    va_list                args;
    size_t                 i;
    const struct d_stack*  src;
    bool                   ok;

    result = NULL;
    i      = 0;
    src    = NULL;
    ok     = false;

    result = d_stack_new();
    if (!result)
    {
        return NULL;
    }

    if (_stack_count == 0)
    {
        return result;
    }

    va_start(args, _stack_count);

    for (i = 0; i < _stack_count; ++i)
    {
        src = va_arg(args, const struct d_stack*);
        if (!src)
        {
            va_end(args);

            d_stack_free(result);

            return NULL;
        }

        ok = d_stack_append_stack(result, src);
        if (!ok)
        {
            va_end(args);

            d_stack_free(result);

            return NULL;
        }
    }

    va_end(args);

    return result;
}


/******************************************************************************
 * BULK / LIST-LIKE MANIPULATION
 *****************************************************************************/

/*
d_stack_push_values
  Pushes multiple values onto the stack (top-first order).

Note:
  `_values[0]` becomes the new top element, `_values[1]` becomes the next, etc.
*/
bool
d_stack_push_values
(
    struct d_stack*     _stack,
    const void* const*  _values,
    size_t              _value_count
)
{
    struct d_linked_node* chain_top;
    size_t                chain_count;
    bool                  ok;

    if (!_stack)
    {
        return false;
    }

    if ( (_value_count > 0) && (!_values) )
    {
        return false;
    }

    if (_value_count == 0)
    {
        return true;
    }

    chain_top   = NULL;
    chain_count = 0;
    ok          = false;

    ok = d_stack_common_init_from_arr(&chain_top,
                                      &chain_count,
                                      _values,
                                      _value_count);
    if (!ok)
    {
        return false;
    }

    ok = d_stack_common_insert_nodes(&_stack->top,
                                     &_stack->count,
                                     chain_top,
                                     NULL,
                                     chain_count,
                                     0);
    if (!ok)
    {
        d_stack_common_free_nodes(chain_top);

        return false;
    }

    return true;
}

/*
d_stack_push_args
  Pushes multiple values onto the stack from varargs (top-first order).
*/
bool
d_stack_push_args
(
    struct d_stack* _stack,
    size_t          _arg_count,
    ...
)
{
    struct d_linked_node* chain_top;
    size_t                chain_count;
    va_list               args;
    bool                  ok;

    if (!_stack)
    {
        return false;
    }

    if (_arg_count == 0)
    {
        return true;
    }

    chain_top   = NULL;
    chain_count = 0;
    ok          = false;

    va_start(args, _arg_count);

    ok = d_stack_common_init_from_args(&chain_top,
                                       &chain_count,
                                       _arg_count,
                                       args);

    va_end(args);

    if (!ok)
    {
        return false;
    }

    ok = d_stack_common_insert_nodes(&_stack->top,
                                     &_stack->count,
                                     chain_top,
                                     NULL,
                                     chain_count,
                                     0);
    if (!ok)
    {
        d_stack_common_free_nodes(chain_top);

        return false;
    }

    return true;
}

/*
d_stack_push_stack
  Pushes a shallow copy of `_source` onto `_destination` (top-first order).
*/
bool
d_stack_push_stack
(
    struct d_stack*        _destination,
    const struct d_stack*  _source
)
{
    struct d_linked_node* chain_top;
    size_t                chain_count;
    bool                  ok;

    if ( (!_destination) || (!_source) )
    {
        return false;
    }

    if (_source->count == 0)
    {
        return true;
    }

    chain_top   = NULL;
    chain_count = 0;
    ok          = false;

    ok = d_stack_common_init_copy(&chain_top,
                                  &chain_count,
                                  _source->top,
                                  _source->count);
    if (!ok)
    {
        return false;
    }

    ok = d_stack_common_insert_nodes(&_destination->top,
                                     &_destination->count,
                                     chain_top,
                                     NULL,
                                     chain_count,
                                     0);
    if (!ok)
    {
        d_stack_common_free_nodes(chain_top);

        return false;
    }

    return true;
}

/*
d_stack_push_stack_range
  Pushes a shallow copy of a range of `_source` onto `_destination`.

Note:
  `_start` and `_end` are inclusive indices and support negative indexing.
*/
bool
d_stack_push_stack_range
(
    struct d_stack*        _destination,
    const struct d_stack*  _source,
    d_index                _start,
    d_index                _end
)
{
    struct d_linked_node* chain_top;
    size_t                chain_count;
    bool                  ok;

    if ( (!_destination) || (!_source) )
    {
        return false;
    }

    if (_source->count == 0)
    {
        return true;
    }

    chain_top   = NULL;
    chain_count = 0;
    ok          = false;

    ok = d_stack_common_init_copy_range(&chain_top,
                                        &chain_count,
                                        _source->top,
                                        _source->count,
                                        _start,
                                        _end);
    if (!ok)
    {
        return false;
    }

    ok = d_stack_common_insert_nodes(&_destination->top,
                                     &_destination->count,
                                     chain_top,
                                     NULL,
                                     chain_count,
                                     0);
    if (!ok)
    {
        d_stack_common_free_nodes(chain_top);

        return false;
    }

    return true;
}

/*
d_stack_insert
  Inserts a value pointer at an index (0 inserts at the top).

Note:
  Negative indices count backward from the end (e.g. -1 inserts before the last
  element). Use `d_stack_append()` to insert at the bottom.
*/
bool
d_stack_insert
(
    struct d_stack* _stack,
    void*           _value,
    d_index         _index
)
{
    size_t index;

    if (!_stack)
    {
        return false;
    }

    index = 0;

    if (!d_internal_stack_normalize_insert_index(_stack->count, _index, &index))
    {
        return false;
    }

    return d_stack_common_insert_value(&_stack->top,
                                       &_stack->count,
                                       _value,
                                       (d_index)index);
}

/*
d_stack_insert_values
  Inserts multiple values at an index (top-first within the inserted block).
*/
bool
d_stack_insert_values
(
    struct d_stack*     _stack,
    const void* const*  _values,
    size_t              _value_count,
    d_index             _index
)
{
    struct d_linked_node* chain_top;
    size_t                chain_count;
    size_t                index;
    bool                  ok;

    if (!_stack)
    {
        return false;
    }

    if ( (_value_count > 0) && (!_values) )
    {
        return false;
    }

    if (_value_count == 0)
    {
        return true;
    }

    index = 0;

    if (!d_internal_stack_normalize_insert_index(_stack->count, _index, &index))
    {
        return false;
    }

    chain_top   = NULL;
    chain_count = 0;
    ok          = false;

    ok = d_stack_common_init_from_arr(&chain_top,
                                      &chain_count,
                                      _values,
                                      _value_count);
    if (!ok)
    {
        return false;
    }

    ok = d_stack_common_insert_nodes(&_stack->top,
                                     &_stack->count,
                                     chain_top,
                                     NULL,
                                     chain_count,
                                     (d_index)index);
    if (!ok)
    {
        d_stack_common_free_nodes(chain_top);

        return false;
    }

    return true;
}

/*
d_stack_insert_stack
  Inserts a shallow copy of `_source` into `_destination` at an index.
*/
bool
d_stack_insert_stack
(
    struct d_stack*        _destination,
    const struct d_stack*  _source,
    d_index                _index
)
{
    if ( (!_destination) || (!_source) )
    {
        return false;
    }

    if (_source->count == 0)
    {
        return true;
    }

    return d_stack_insert_stack_range(_destination,
                                      _source,
                                      0,
                                      (d_index)(_source->count - 1),
                                      _index);
}

/*
d_stack_insert_stack_range
  Inserts a shallow copy of a range of `_source` into `_destination` at an index.

Note:
  `_start` and `_end` are inclusive indices and support negative indexing.
*/
bool
d_stack_insert_stack_range
(
    struct d_stack*        _destination,
    const struct d_stack*  _source,
    d_index                _start,
    d_index                _end,
    d_index                _index
)
{
    struct d_linked_node* chain_top;
    size_t                chain_count;
    size_t                index;
    bool                  ok;

    if ( (!_destination) || (!_source) )
    {
        return false;
    }

    if (_source->count == 0)
    {
        return true;
    }

    index = 0;

    if (!d_internal_stack_normalize_insert_index(_destination->count, _index, &index))
    {
        return false;
    }

    chain_top   = NULL;
    chain_count = 0;
    ok          = false;

    ok = d_stack_common_init_copy_range(&chain_top,
                                        &chain_count,
                                        _source->top,
                                        _source->count,
                                        _start,
                                        _end);
    if (!ok)
    {
        return false;
    }

    ok = d_stack_common_insert_nodes(&_destination->top,
                                     &_destination->count,
                                     chain_top,
                                     NULL,
                                     chain_count,
                                     (d_index)index);
    if (!ok)
    {
        d_stack_common_free_nodes(chain_top);

        return false;
    }

    return true;
}

/*
d_stack_append
  Appends a value pointer to the bottom of the stack.
*/
bool
d_stack_append
(
    struct d_stack* _stack,
    void*           _value
)
{
    if (!_stack)
    {
        return false;
    }

    return d_stack_common_append_value(&_stack->top, &_stack->count, _value);
}

/*
d_stack_append_values
  Appends multiple values to the bottom of the stack (top-first order within
  the appended block).
*/
bool
d_stack_append_values
(
    struct d_stack*     _stack,
    const void* const*  _values,
    size_t              _value_count
)
{
    if (!_stack)
    {
        return false;
    }

    return d_stack_insert_values(_stack, _values, _value_count, (d_index)_stack->count);
}

/*
d_stack_append_stack
  Appends a shallow copy of `_source` to the bottom of `_destination`.
*/
bool
d_stack_append_stack
(
    struct d_stack*        _destination,
    const struct d_stack*  _source
)
{
    if ( (!_destination) || (!_source) )
    {
        return false;
    }

    if (_source->count == 0)
    {
        return true;
    }

    return d_stack_insert_stack_range(_destination,
                                      _source,
                                      0,
                                      (d_index)(_source->count - 1),
                                      (d_index)_destination->count);
}

/*
d_stack_append_stack_range
  Appends a shallow copy of a range of `_source` to the bottom of `_destination`.
*/
bool
d_stack_append_stack_range
(
    struct d_stack*        _destination,
    const struct d_stack*  _source,
    d_index                _start,
    d_index                _end
)
{
    if ( (!_destination) || (!_source) )
    {
        return false;
    }

    if (_source->count == 0)
    {
        return true;
    }

    return d_stack_insert_stack_range(_destination,
                                      _source,
                                      _start,
                                      _end,
                                      (d_index)_destination->count);
}
