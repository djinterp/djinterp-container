/******************************************************************************
* djinterp [container]                                         stack_common.c
*
* Common utilities and helper functions for stack implementations.
*
*
* path:      \src\container\stack\stack_common.c
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.12.21
******************************************************************************/

#include ".\stack_common.h"


/******************************************************************************
 * INTERNAL HELPERS
 *****************************************************************************/

static bool
d_internal_stack_common_normalize_range
(
    size_t   _count,
    d_index  _start,
    d_index  _end,
    size_t*  _out_start,
    size_t*  _out_end
)
{
    if ( (!_out_start) || (!_out_end) )
    {
        return false;
    }

    if (_count == 0)
    {
        *_out_start = 0;
        *_out_end   = 0;

        return true;
    }

    if (_start < 0)
    {
        _start = (d_index)_count + _start;
    }

    if (_end < 0)
    {
        _end = (d_index)_count + _end;
    }

    if ( (_start < 0)                 ||
         (_end < 0)                   ||
         (_start >= (d_index)_count)  ||
         (_end >= (d_index)_count)    ||
         (_start > _end) )
    {
        return false;
    }

    *_out_start = (size_t)_start;
    *_out_end   = (size_t)_end;

    return true;
}

static struct d_linked_node*
d_internal_stack_common_node_new
(
    void* _value
)
{
    return d_linked_node_new(_value, NULL);
}

static bool
d_internal_stack_common_value_matches
(
    const void*    _a,
    const void*    _b,
    fn_comparator  _comparator
)
{
    if (_comparator)
    {
        return (_comparator(_a, _b) == 0);
    }

    return (_a == _b);
}


/******************************************************************************
 * INITIALIZATION
 *****************************************************************************/

/*
d_stack_common_init_empty
  Initializes (top, count) to represent an empty stack.

Parameter(s):
  _top:   destination top pointer.
  _count: destination count.
Return:
  A boolean value corresponding to either:
  - true, if all output parameters were non-NULL, or
  - false, if any output parameter was NULL.
*/
bool
d_stack_common_init_empty
(
    struct d_linked_node** _top,
    size_t*                _count
)
{
    if ( (!_top) || (!_count) )
    {
        return false;
    }

    *_top   = NULL;
    *_count = 0;

    return true;
}

/*
d_stack_common_init_from_arr
  Initializes a stack from an array of value pointers.

Note:
  Values are treated as top-first order: `_values[0]` becomes the top element.

Parameter(s):
  _top:         destination top pointer.
  _count:       destination count.
  _values:      array of value pointers.
  _value_count: number of values.
Return:
  A boolean value corresponding to either:
  - true, if the stack was initialized successfully, or
  - false, if any parameter was invalid or allocation failed.
*/
bool
d_stack_common_init_from_arr
(
    struct d_linked_node** _top,
    size_t*                _count,
    const void* const*     _values,
    size_t                 _value_count
)
{
    struct d_linked_node* top;
    struct d_linked_node* node;
    struct d_linked_node* tail;
    size_t                i;

    if ( (!_top) ||
         (!_count) ||
         ( (_value_count > 0) && (!_values) ) )
    {
        return false;
    }

    top  = NULL;
    node = NULL;
    tail = NULL;
    i    = 0;

    for (i = 0; i < _value_count; ++i)
    {
        node = d_internal_stack_common_node_new((void*)_values[i]);
        if (!node)
        {
            d_stack_common_free_nodes(top);

            return false;
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

    *_top   = top;
    *_count = _value_count;

    return true;
}

/*
d_stack_common_init_from_args
  Initializes a stack from a va_list of value pointers.

Note:
  Values are treated as top-first order.

Parameter(s):
  _top:       destination top pointer.
  _count:     destination count.
  _arg_count: number of arguments in `_args`.
  _args:      argument list containing `void*` values.
Return:
  A boolean value corresponding to either:
  - true, if the stack was initialized successfully, or
  - false, if any parameter was invalid or allocation failed.
*/
bool
d_stack_common_init_from_args
(
    struct d_linked_node** _top,
    size_t*                _count,
    size_t                 _arg_count,
    va_list                _args
)
{
    struct d_linked_node* top;
    struct d_linked_node* tail;
    struct d_linked_node* node;
    size_t                i;
    void*                 value;

    if ( (!_top) || (!_count) )
    {
        return false;
    }

    top   = NULL;
    tail  = NULL;
    node  = NULL;
    i     = 0;
    value = NULL;

    for (i = 0; i < _arg_count; ++i)
    {
        value = va_arg(_args, void*);

        node = d_internal_stack_common_node_new(value);
        if (!node)
        {
            d_stack_common_free_nodes(top);

            return false;
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

    *_top   = top;
    *_count = _arg_count;

    return true;
}

/*
d_stack_common_init_copy
  Initializes a stack as a shallow copy of another node chain.

Parameter(s):
  _dst_top:   destination top pointer.
  _dst_count: destination count.
  _src_top:   source top pointer.
  _src_count: source count.
Return:
  A boolean value corresponding to either:
  - true, if the stack was copied successfully, or
  - false, if parameters were invalid or allocation failed.
*/
bool
d_stack_common_init_copy
(
    struct d_linked_node**      _dst_top,
    size_t*                     _dst_count,
    const struct d_linked_node* _src_top,
    size_t                      _src_count
)
{
    struct d_linked_node* top;
    struct d_linked_node* tail;
    struct d_linked_node* node;
    const struct d_linked_node* cur;
    size_t                i;

    if ( (!_dst_top) ||
         (!_dst_count) ||
         ( (_src_count > 0) && (!_src_top) ) )
    {
        return false;
    }

    top  = NULL;
    tail = NULL;
    node = NULL;
    cur  = NULL;
    i    = 0;

    cur = _src_top;

    for (i = 0; i < _src_count; ++i)
    {
        if (!cur)
        {
            d_stack_common_free_nodes(top);

            return false;
        }

        node = d_internal_stack_common_node_new(cur->value);
        if (!node)
        {
            d_stack_common_free_nodes(top);

            return false;
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

        cur = cur->next;
    }

    *_dst_top   = top;
    *_dst_count = _src_count;

    return true;
}

/*
d_stack_common_init_copy_range
  Initializes a stack as a shallow copy of a range of another node chain.

Note:
  `_start` and `_end` are treated as inclusive indices. Negative indices count
  backward from the end (e.g. -1 is the last element).

Parameter(s):
  _dst_top:   destination top pointer.
  _dst_count: destination count.
  _src_top:   source top pointer.
  _src_count: source count.
  _start:     first index to include.
  _end:       last index to include.
Return:
  A boolean value corresponding to either:
  - true, if the stack was copied successfully, or
  - false, if parameters were invalid or allocation failed.
*/
bool
d_stack_common_init_copy_range
(
    struct d_linked_node**      _dst_top,
    size_t*                     _dst_count,
    const struct d_linked_node* _src_top,
    size_t                      _src_count,
    d_index                     _start,
    d_index                     _end
)
{
    struct d_linked_node* top;
    struct d_linked_node* tail;
    struct d_linked_node* node;
    const struct d_linked_node* cur;
    size_t                i;
    size_t                start;
    size_t                end;
    size_t                copied;

    if ( (!_dst_top) ||
         (!_dst_count) ||
         ( (_src_count > 0) && (!_src_top) ) )
    {
        return false;
    }

    top    = NULL;
    tail   = NULL;
    node   = NULL;
    cur    = NULL;
    i      = 0;
    start  = 0;
    end    = 0;
    copied = 0;

    if (_src_count == 0)
    {
        *_dst_top   = NULL;
        *_dst_count = 0;

        return true;
    }

    if (!d_internal_stack_common_normalize_range(_src_count,
                                                 _start,
                                                 _end,
                                                 &start,
                                                 &end))
    {
        return false;
    }

    cur = _src_top;

    for (i = 0; i < _src_count; ++i)
    {
        if (!cur)
        {
            d_stack_common_free_nodes(top);

            return false;
        }

        if (i < start)
        {
            cur = cur->next;
            continue;
        }

        if (i > end)
        {
            break;
        }

        node = d_internal_stack_common_node_new(cur->value);
        if (!node)
        {
            d_stack_common_free_nodes(top);

            return false;
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

        ++copied;

        cur = cur->next;
    }

    *_dst_top   = top;
    *_dst_count = copied;

    return true;
}


/******************************************************************************
 * STACK CORE OPERATIONS
 *****************************************************************************/

/*
d_stack_common_push_value
  Pushes a value onto the stack (LIFO).

Parameter(s):
  _top:   pointer to the top pointer.
  _count: pointer to element count.
  _value: value pointer to push.
Return:
  true if the push succeeded, otherwise false.
*/
bool
d_stack_common_push_value
(
    struct d_linked_node** _top,
    size_t*                _count,
    void*                  _value
)
{
    struct d_linked_node* node;

    if ( (!_top) || (!_count) )
    {
        return false;
    }

    node = d_internal_stack_common_node_new(_value);
    if (!node)
    {
        return false;
    }

    node->next = *_top;
    *_top      = node;
    ++(*_count);

    return true;
}

/*
d_stack_common_peek_value
  Returns the value at the top of the stack, or NULL if empty.

Parameter(s):
  _top: top node pointer.
Return:
  value pointer, or NULL.
*/
void*
d_stack_common_peek_value
(
    const struct d_linked_node* _top
)
{
    if (!_top)
    {
        return NULL;
    }

    return _top->value;
}

/*
d_stack_common_pop_value
  Pops the top value from the stack.

Parameter(s):
  _top:   pointer to the top pointer.
  _count: pointer to element count.
Return:
  value pointer, or NULL if the stack was empty or parameters were invalid.
*/
void*
d_stack_common_pop_value
(
    struct d_linked_node** _top,
    size_t*                _count
)
{
    struct d_linked_node* node;
    void*                 value;

    if ( (!_top) || (!_count) )
    {
        return NULL;
    }

    node = *_top;
    if (!node)
    {
        return NULL;
    }

    *_top = node->next;
    value = node->value;

    d_linked_node_free(node);

    if (*_count > 0)
    {
        --(*_count);
    }

    return value;
}


/******************************************************************************
 * ACCESS
 *****************************************************************************/

/*
d_stack_common_node_at
  Returns the node at an index.

*/
struct d_linked_node*
d_stack_common_node_at
(
    struct d_linked_node* _top,
    d_index               _index
)
{
    struct d_linked_node* cur;
    d_index               i;

    cur = _top;
    i   = 0;

    if (_index < 0)
    {
        return NULL;
    }

    while (cur)
    {
        if (i == _index)
        {
            return cur;
        }

        cur = cur->next;
        ++i;
    }

    return NULL;
}

/*
d_stack_common_node_prev_at
  Returns the node immediately preceding the node at an index.

Note:
  If `_index` is 0, there is no previous node and NULL is returned.
*/
struct d_linked_node*
d_stack_common_node_prev_at
(
    struct d_linked_node* _top,
    d_index               _index
)
{
    struct d_linked_node* cur;
    d_index               i;

    if (_index <= 0)
    {
        return NULL;
    }

    cur = _top;
    i   = 0;

    while (cur)
    {
        if (i == (_index - 1))
        {
            return cur;
        }

        cur = cur->next;
        ++i;
    }

    return NULL;
}

/*
d_stack_common_get_value
  Returns the value pointer at an index, or NULL if out of range.
*/
void*
d_stack_common_get_value
(
    const struct d_linked_node* _top,
    d_index                     _index
)
{
    const struct d_linked_node* cur;
    d_index                     i;

    if (_index < 0)
    {
        return NULL;
    }

    cur = _top;
    i   = 0;

    while (cur)
    {
        if (i == _index)
        {
            return cur->value;
        }

        cur = cur->next;
        ++i;
    }

    return NULL;
}

/*
d_stack_common_set_value
  Sets the value pointer at an index.

Return:
  true if the index exists, otherwise false.
*/
bool
d_stack_common_set_value
(
    struct d_linked_node* _top,
    d_index               _index,
    void*                 _value
)
{
    struct d_linked_node* node;

    node = d_stack_common_node_at(_top, _index);
    if (!node)
    {
        return false;
    }

    node->value = _value;

    return true;
}


/******************************************************************************
 * REMOVAL
 *****************************************************************************/

/*
d_stack_common_detach_node_at
  Detaches (removes) a node at an index without freeing it.

Return:
  The detached node, or NULL on failure/out-of-range.
*/
struct d_linked_node*
d_stack_common_detach_node_at
(
    struct d_linked_node** _top,
    size_t*                _count,
    d_index                _index
)
{
    struct d_linked_node* prev;
    struct d_linked_node* node;

    if ( (!_top) || (!_count) )
    {
        return NULL;
    }

    if (_index < 0)
    {
        return NULL;
    }

    if (_index == 0)
    {
        node = *_top;
        if (!node)
        {
            return NULL;
        }

        *_top = node->next;
        node->next = NULL;

        if (*_count > 0)
        {
            --(*_count);
        }

        return node;
    }

    prev = d_stack_common_node_prev_at(*_top, _index);
    if (!prev)
    {
        return NULL;
    }

    node = prev->next;
    if (!node)
    {
        return NULL;
    }

    prev->next = node->next;
    node->next = NULL;

    if (*_count > 0)
    {
        --(*_count);
    }

    return node;
}

/*
d_stack_common_remove_value_at
  Removes a node at an index, frees it, and returns its stored value.
*/
void*
d_stack_common_remove_value_at
(
    struct d_linked_node** _top,
    size_t*                _count,
    d_index                _index
)
{
    struct d_linked_node* node;
    void*                 value;

    node  = d_stack_common_detach_node_at(_top, _count, _index);
    value = NULL;

    if (!node)
    {
        return NULL;
    }

    value = node->value;

    d_linked_node_free(node);

    return value;
}

/*
d_stack_common_remove_first_match
  Removes the first matching value and returns it through `_removed_value`.

Return:
  true if a value was removed, otherwise false.
*/
bool
d_stack_common_remove_first_match
(
    struct d_linked_node** _top,
    size_t*                _count,
    const void*            _value,
    fn_comparator          _comparator,
    void**                 _removed_value
)
{
    struct d_linked_node* cur;
    struct d_linked_node* prev;

    if ( (!_top) || (!_count) )
    {
        return false;
    }

    cur  = *_top;
    prev = NULL;

    while (cur)
    {
        if (d_internal_stack_common_value_matches(cur->value,
                                                  _value,
                                                  _comparator))
        {
            if (!prev)
            {
                *_top = cur->next;
            }
            else
            {
                prev->next = cur->next;
            }

            if (*_count > 0)
            {
                --(*_count);
            }

            if (_removed_value)
            {
                *_removed_value = cur->value;
            }

            d_linked_node_free(cur);

            return true;
        }

        prev = cur;
        cur  = cur->next;
    }

    return false;
}


/******************************************************************************
 * SEARCH
 *****************************************************************************/

/*
d_stack_common_contains
  Returns true if the stack contains the value.

Comparator behavior:
  If `_comparator` is NULL, pointer equality is used.
*/
bool
d_stack_common_contains
(
    const struct d_linked_node* _top,
    size_t                      _count,
    const void*                 _value,
    fn_comparator               _comparator
)
{
    return (d_stack_common_find(_top, _count, _value, _comparator) >= 0);
}

/*
d_stack_common_find
  Returns the index of the first match, or -1 if not found.

Comparator behavior:
  If `_comparator` is NULL, pointer equality is used.
*/
ssize_t
d_stack_common_find
(
    const struct d_linked_node* _top,
    size_t                      _count,
    const void*                 _value,
    fn_comparator               _comparator
)
{
    const struct d_linked_node* cur;
    size_t                      i;

    cur = _top;
    i   = 0;

    while (cur && (i < _count))
    {
        if (d_internal_stack_common_value_matches(cur->value,
                                                  _value,
                                                  _comparator))
        {
            return (ssize_t)i;
        }

        cur = cur->next;
        ++i;
    }

    return (ssize_t)-1;
}

/*
d_stack_common_find_node
  Returns the first matching node, or NULL if not found.

Comparator behavior:
  If `_comparator` is NULL, pointer equality is used.
*/
struct d_linked_node*
d_stack_common_find_node
(
    const struct d_linked_node* _top,
    const void*                 _value,
    fn_comparator               _comparator
)
{
    const struct d_linked_node* cur;

    cur = _top;

    while (cur)
    {
        if (d_internal_stack_common_value_matches(cur->value,
                                                  _value,
                                                  _comparator))
        {
            return (struct d_linked_node*)cur;
        }

        cur = cur->next;
    }

    return NULL;
}


/******************************************************************************
 * OTHER
 *****************************************************************************/

/*
d_stack_common_reverse
  Reverses the stack node chain in place.

Return:
  true if parameters were valid, otherwise false.
*/
bool
d_stack_common_reverse
(
    struct d_linked_node** _top,
    size_t                 _count
)
{
    struct d_linked_node* prev;
    struct d_linked_node* cur;
    struct d_linked_node* next;
    size_t                i;

    if (!_top)
    {
        return false;
    }

    prev = NULL;
    cur  = *_top;
    next = NULL;
    i    = 0;

    while (cur && (i < _count))
    {
        next = cur->next;
        cur->next = prev;

        prev = cur;
        cur  = next;

        ++i;
    }

    *_top = prev;

    return true;
}

/*
d_stack_common_validate_params
  Validates basic parameter invariants for a stack.

Return:
  true if valid, otherwise false.
*/
bool
d_stack_common_validate_params
(
    struct d_linked_node** _top,
    const size_t*          _count
)
{
    const struct d_linked_node* cur;
    size_t                      i;

    if ( (!_top) || (!_count) )
    {
        return false;
    }

    if ( (*_count == 0) && (*_top != NULL) )
    {
        return false;
    }

    if ( (*_count > 0) && (*_top == NULL) )
    {
        return false;
    }

    cur = *_top;
    i   = 0;

    while (cur && (i < *_count))
    {
        cur = cur->next;
        ++i;
    }

    if (i != *_count)
    {
        return false;
    }

    if (cur != NULL)
    {
        // chain longer than declared count
        return false;
    }

    return true;
}


/******************************************************************************
 * FREEING
 *****************************************************************************/

/*
d_stack_common_free_nodes
  Frees every node in a stack (shallow; does not free node values).

Parameter(s):
  _top: the top node, or NULL.
*/
void
d_stack_common_free_nodes
(
    struct d_linked_node* _top
)
{
    struct d_linked_node* cur;
    struct d_linked_node* next;

    cur  = _top;
    next = NULL;

    while (cur)
    {
        next = cur->next;

        d_linked_node_free(cur);

        cur = next;
    }

    return;
}

/*
d_stack_common_free_nodes_deep
  Frees every node in a stack and also frees each node value using `_free_fn`.

Parameter(s):
  _top:     the top node, or NULL.
  _free_fn: function pointer used to free each node value.
*/
void
d_stack_common_free_nodes_deep
(
    struct d_linked_node* _top,
    fn_free               _free_fn
)
{
    struct d_linked_node* cur;
    struct d_linked_node* next;

    if (!_free_fn)
    {
        d_stack_common_free_nodes(_top);

        return;
    }

    cur  = _top;
    next = NULL;

    while (cur)
    {
        next = cur->next;

        _free_fn(cur->value);
        d_linked_node_free(cur);

        cur = next;
    }

    return;
}


/******************************************************************************
 * INSERTION / SPLICING
 *****************************************************************************/

static struct d_linked_node*
d_internal_stack_common_tail_of
(
    struct d_linked_node* _top,
    size_t                _count
)
{
    struct d_linked_node* cur;
    size_t                i;

    cur = _top;
    i   = 0;

    if ((!cur) || (_count == 0))
    {
        return NULL;
    }

    while (cur && (i + 1 < _count))
    {
        cur = cur->next;
        ++i;
    }

    return cur;
}

/*
d_stack_common_insert_nodes
  Inserts an existing node chain into a stack at an index.

Parameter(s):
  _top:          pointer to stack top pointer.
  _count:        pointer to element count.
  _insert_top:   top of node chain to insert (takes ownership).
  _insert_tail:  tail of node chain to insert (may be NULL if unknown).
  _insert_count: number of nodes in the chain.
  _index:        insertion index from the top (0 inserts at the top).
Return:
  true if insertion succeeded, otherwise false.

Note:
  `_index` must be in range [0, *_count]. Negative indices are not supported
  by this common helper.
*/
bool
d_stack_common_insert_nodes
(
    struct d_linked_node** _top,
    size_t*                _count,
    struct d_linked_node*  _insert_top,
    struct d_linked_node*  _insert_tail,
    size_t                 _insert_count,
    d_index                _index
)
{
    struct d_linked_node* prev;
    struct d_linked_node* tail;

    if ( (!_top) || (!_count) )
    {
        return false;
    }

    if (_insert_count == 0)
    {
        return true;
    }

    if (!_insert_top)
    {
        return false;
    }

    if ( (_index < 0) || (_index > (d_index)(*_count)) )
    {
        return false;
    }

    tail = _insert_tail;
    if (!tail)
    {
        tail = d_internal_stack_common_tail_of(_insert_top, _insert_count);
        if (!tail)
        {
            return false;
        }
    }

    // insert at top
    if (_index == 0)
    {
        tail->next = *_top;
        *_top      = _insert_top;

        *_count += _insert_count;

        return true;
    }

    // insert after the previous node (index - 1)
    prev = d_stack_common_node_prev_at(*_top, _index);
    if (!prev)
    {
        return false;
    }

    tail->next = prev->next;
    prev->next = _insert_top;

    *_count += _insert_count;

    return true;
}

/*
d_stack_common_append_nodes
  Appends an existing node chain to the bottom of a stack.
*/
bool
d_stack_common_append_nodes
(
    struct d_linked_node** _top,
    size_t*                _count,
    struct d_linked_node*  _insert_top,
    struct d_linked_node*  _insert_tail,
    size_t                 _insert_count
)
{
    if ( (!_top) || (!_count) )
    {
        return false;
    }

    return d_stack_common_insert_nodes(_top,
                                       _count,
                                       _insert_top,
                                       _insert_tail,
                                       _insert_count,
                                       (d_index)(*_count));
}

/*
d_stack_common_insert_value
  Allocates a new node containing `_value` and inserts it at an index.
*/
bool
d_stack_common_insert_value
(
    struct d_linked_node** _top,
    size_t*                _count,
    void*                  _value,
    d_index                _index
)
{
    struct d_linked_node* node;
    bool                  ok;

    if ( (!_top) || (!_count) )
    {
        return false;
    }

    if ( (_index < 0) || (_index > (d_index)(*_count)) )
    {
        return false;
    }

    node = d_internal_stack_common_node_new(_value);
    if (!node)
    {
        return false;
    }

    ok = d_stack_common_insert_nodes(_top,
                                     _count,
                                     node,
                                     node,
                                     1,
                                     _index);
    if (!ok)
    {
        d_linked_node_free(node);

        return false;
    }

    return true;
}

/*
d_stack_common_append_value
  Allocates a new node containing `_value` and appends it to the bottom
  of a stack.
*/
bool
d_stack_common_append_value
(
    struct d_linked_node** _top,
    size_t*                _count,
    void*                  _value
)
{
    if ( (!_top) || (!_count) )
    {
        return false;
    }

    return d_stack_common_insert_value(_top, _count, _value, (d_index)(*_count));
}
