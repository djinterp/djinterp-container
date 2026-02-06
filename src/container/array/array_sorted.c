#include "..\..\..\inc\container\array\array_sorted.h"


// =============================================================================
// internal helper functions
// =============================================================================

/*
d_array_sorted_internal_find_insert_pos
  Finds the position where a value should be inserted to maintain sorted order.
  Uses binary search for O(log n) performance.

Parameter(s):
  _array_sorted: the sorted array to search
  _value:       pointer to the value to find position for
Return:
  The index where the value should be inserted.
*/
static size_t
d_array_sorted_internal_find_insert_pos
(
    const struct d_array_sorted* _array_sorted,
    const void*                  _value
)
{
    size_t      low;
    size_t      high;
    size_t      mid;
    int         cmp;
    const char* elem_ptr;

    if ( (!_array_sorted) ||
         (_array_sorted->count == 0) )
    {
        return 0;
    }

    low  = 0;
    high = _array_sorted->count;

    while (low < high)
    {
        mid = low + (high - low) / 2;
        elem_ptr = (const char*)_array_sorted->elements + 
                   (mid * _array_sorted->element_size);

        cmp = _array_sorted->comparator(elem_ptr, _value);

        if (cmp < 0)
        {
            low = mid + 1;
        }
        else
        {
            high = mid;
        }
    }

    return low;
}

/*
d_array_sorted_internal_ensure_capacity
  Ensures the sorted array has enough capacity for at least one more element.

Parameter(s):
  _array_sorted: the sorted array to check/grow
Return:
  A boolean value corresponding to either:
  - true, if capacity is sufficient or was successfully increased, or
  - false, if memory allocation failed.
*/
static bool
d_array_sorted_internal_ensure_capacity
(
    struct d_array_sorted* _array_sorted
)
{
    size_t new_capacity;

    if (_array_sorted->count < _array_sorted->capacity)
    {
        return D_SUCCESS;
    }

    new_capacity = (_array_sorted->capacity == 0)
        ? D_SORTED_LIST_DEFAULT_CAPACITY
        : (_array_sorted->capacity * D_SORTED_LIST_GROWTH_FACTOR);

    return d_array_sorted_reserve(_array_sorted, new_capacity);
}

/*
d_array_sorted_internal_shift_right
  Shifts elements from the given index to the right by one position.

Parameter(s):
  _array_sorted: the sorted array to modify
  _index:       starting index for the shift
Return:
  none
*/
static void
d_array_sorted_internal_shift_right
(
    struct d_array_sorted* _array_sorted,
    size_t                 _index
)
{
    char* dest;
    char* src;
    size_t move_count;

    if (_index >= _array_sorted->count)
    {
        return;
    }

    move_count = _array_sorted->count - _index;
    src  = (char*)_array_sorted->elements + (_index * _array_sorted->element_size);
    dest = src + _array_sorted->element_size;

    memmove(dest, src, move_count * _array_sorted->element_size);

    return;
}

/*
d_array_sorted_internal_shift_left
  Shifts elements from the given index to the left by one position.

Parameter(s):
  _array_sorted: the sorted array to modify
  _index:       index of the element to remove (elements after shift left)
Return:
  none
*/
static void
d_array_sorted_internal_shift_left
(
    struct d_array_sorted* _array_sorted,
    size_t                _index
)
{
    char*  dest;
    char*  src;
    size_t move_count;

    if (_index >= _array_sorted->count - 1)
    {
        return;
    }

    move_count = _array_sorted->count - _index - 1;
    dest = (char*)_array_sorted->elements + (_index * _array_sorted->element_size);
    src  = dest + _array_sorted->element_size;

    memmove(dest, src, move_count * _array_sorted->element_size);

    return;
}


// =============================================================================
// constructor functions
// =============================================================================

/*
d_array_sorted_new
  Allocates and initializes a new empty d_array_sorted with default capacity.

Parameter(s):
  _element_size: size in bytes of each element to be stored
  _comparator:   function to compare two elements for ordering
Return:
  Either a pointer to a new, empty d_array_sorted, or NULL if allocation failed,
  element_size is 0, or comparator is NULL.
*/
struct d_array_sorted*
d_array_sorted_new
(
    size_t        _element_size,
    fn_comparator _comparator
)
{
    return d_array_sorted_new_with_capacity(_element_size,
                                           _comparator,
                                           D_SORTED_LIST_DEFAULT_CAPACITY);
}

/*
d_array_sorted_new_with_capacity
  Allocates and initializes a new d_array_sorted with specified initial capacity.

Parameter(s):
  _element_size:     size in bytes of each element to be stored
  _comparator:       function to compare two elements for ordering
  _initial_capacity: initial capacity in number of elements
Return:
  Either a pointer to a new d_array_sorted, or NULL if allocation failed,
  element_size is 0, or comparator is NULL.
*/
struct d_array_sorted*
d_array_sorted_new_with_capacity
(
    size_t        _element_size,
    fn_comparator _comparator,
    size_t        _initial_capacity
)
{
    struct d_array_sorted* new_array_sorted;

    if ( (_element_size == 0) ||
         (!_comparator) )
    {
        return NULL;
    }

    new_array_sorted = malloc(sizeof(struct d_array_sorted));

    if (!new_array_sorted)
    {
        return NULL;
    }

    new_array_sorted->count        = 0;
    new_array_sorted->capacity     = _initial_capacity;
    new_array_sorted->element_size = _element_size;
    new_array_sorted->comparator   = _comparator;

    if (_initial_capacity > 0)
    {
        new_array_sorted->elements = calloc(_initial_capacity, _element_size);

        if (!new_array_sorted->elements)
        {
            free(new_array_sorted);

            return NULL;
        }
    }
    else
    {
        new_array_sorted->elements = NULL;
    }

    return new_array_sorted;
}

/*
d_array_sorted_new_from_arr
  Allocates and initializes a new d_array_sorted by inserting elements from an
  existing array in sorted order.

Parameter(s):
  _element_size: size in bytes of each element
  _comparator:   function to compare two elements for ordering
  _source:       pointer to the source array data
  _count:        number of elements in the source array
Return:
  Either a pointer to a new d_array_sorted containing the sorted elements, or
  NULL if allocation failed.
*/
struct d_array_sorted*
d_array_sorted_new_from_arr
(
    size_t        _element_size,
    fn_comparator _comparator,
    const void*   _source,
    size_t        _count
)
{
    struct d_array_sorted* new_array_sorted;
    size_t                i;
    const char*           src_ptr;

    if ( (_element_size == 0) ||
         (!_comparator) )
    {
        return NULL;
    }

    if ( (!_source) &&
         (_count > 0) )
    {
        return NULL;
    }

    new_array_sorted = d_array_sorted_new_with_capacity(_element_size,
                                                      _comparator,
                                                      _count);

    if (!new_array_sorted)
    {
        return NULL;
    }

    // insert each element (maintains sorted order)
    src_ptr = (const char*)_source;

    for (i = 0; i < _count; i++)
    {
        if (!d_array_sorted_insert(new_array_sorted,
                                  src_ptr + (i * _element_size)))
        {
            d_array_sorted_free(new_array_sorted);

            return NULL;
        }
    }

    return new_array_sorted;
}

/*
d_array_sorted_new_from_args
  Allocates and initializes a new d_array_sorted from variadic arguments.

Parameter(s):
  _element_size: size in bytes of each element
  _comparator:   function to compare two elements for ordering
  _arg_count:    number of variadic arguments to process
  ...:           variadic arguments containing the element data
Return:
  Either a pointer to a new d_array_sorted, or NULL if allocation failed.
*/
struct d_array_sorted*
d_array_sorted_new_from_args
(
    size_t        _element_size,
    fn_comparator _comparator,
    size_t        _arg_count,
    ...
)
{
    struct d_array_sorted* new_array_sorted;
    va_array               args;
    size_t                i;
    void*                 value_ptr;
    int                   int_value;

    if ( (_element_size == 0) ||
         (!_comparator) )
    {
        return NULL;
    }

    new_array_sorted = d_array_sorted_new_with_capacity(_element_size,
                                                      _comparator,
                                                      _arg_count);

    if (!new_array_sorted)
    {
        return NULL;
    }

    va_start(args, _arg_count);

    for (i = 0; i < _arg_count; i++)
    {
        if (_element_size <= sizeof(int))
        {
            int_value = va_arg(args, int);

            if (!d_array_sorted_insert(new_array_sorted, &int_value))
            {
                va_end(args);
                d_array_sorted_free(new_array_sorted);

                return NULL;
            }
        }
        else
        {
            value_ptr = va_arg(args, void*);

            if (!d_array_sorted_insert(new_array_sorted, value_ptr))
            {
                va_end(args);
                d_array_sorted_free(new_array_sorted);

                return NULL;
            }
        }
    }

    va_end(args);

    return new_array_sorted;
}

/*
d_array_sorted_new_copy
  Allocates and initializes a new d_array_sorted as a copy of another.

Parameter(s):
  _other: pointer to the d_array_sorted to copy from
Return:
  Either a pointer to a new d_array_sorted containing copied elements, or NULL
  if allocation failed or _other is NULL.
*/
struct d_array_sorted*
d_array_sorted_new_copy
(
    const struct d_array_sorted* _other
)
{
    struct d_array_sorted* new_array_sorted;

    if (!_other)
    {
        return NULL;
    }

    new_array_sorted = d_array_sorted_new_with_capacity(_other->element_size,
                                                      _other->comparator,
                                                      _other->capacity);

    if (!new_array_sorted)
    {
        return NULL;
    }

    // directly copy elements since source is already sorted
    if ( (_other->elements) &&
         (_other->count > 0) )
    {
        d_memcpy(new_array_sorted->elements,
                 _other->elements,
                 _other->count * _other->element_size);

        new_array_sorted->count = _other->count;
    }

    return new_array_sorted;
}

/*
d_array_sorted_new_merge
  Creates a new sorted array by merging multiple sorted arrays.

Parameter(s):
  _count: number of d_array_sorted arguments that follow
  ...:    variadic d_array_sorted* arguments to merge together
Return:
  Either a pointer to a new d_array_sorted containing all elements from all
  input arrays in sorted order, or NULL if allocation failed or arrays have
  incompatible element sizes.
*/
struct d_array_sorted*
d_array_sorted_new_merge
(
    size_t _count,
    ...
)
{
    struct d_array_sorted* result;
    struct d_array_sorted* current;
    va_array               args;
    size_t                total_count;
    size_t                element_size;
    fn_comparator         comparator;
    size_t                i;
    size_t                j;
    const char*           src_ptr;

    if (_count == 0)
    {
        return NULL;
    }

    // first pass: get element size and comparator from first non-NULL array
    // and calculate total size
    total_count  = 0;
    element_size = 0;
    comparator   = NULL;

    va_start(args, _count);

    for (i = 0; i < _count; i++)
    {
        current = va_arg(args, struct d_array_sorted*);

        if (current)
        {
            if (element_size == 0)
            {
                element_size = current->element_size;
                comparator   = current->comparator;
            }
            else if (current->element_size != element_size)
            {
                va_end(args);

                return NULL;  // incompatible element sizes
            }

            total_count += current->count;
        }
    }

    va_end(args);

    if ( (element_size == 0) ||
         (!comparator) )
    {
        return NULL;
    }

    result = d_array_sorted_new_with_capacity(element_size,
                                             comparator,
                                             total_count);

    if (!result)
    {
        return NULL;
    }

    // second pass: insert all elements
    va_start(args, _count);

    for (i = 0; i < _count; i++)
    {
        current = va_arg(args, struct d_array_sorted*);

        if ( (current)              &&
             (current->count > 0)   &&
             (current->elements) )
        {
            src_ptr = (const char*)current->elements;

            for (j = 0; j < current->count; j++)
            {
                if (!d_array_sorted_insert(result,
                                          src_ptr + (j * element_size)))
                {
                    va_end(args);
                    d_array_sorted_free(result);

                    return NULL;
                }
            }
        }
    }

    va_end(args);

    return result;
}


// =============================================================================
// insertion functions
// =============================================================================

/*
d_array_sorted_insert
  Inserts an element into the sorted array while maintaining sorted order.
  Uses binary search to find the correct position.

Parameter(s):
  _array_sorted: the sorted array to modify
  _value:       pointer to the value to insert
Return:
  A boolean value corresponding to either:
  - true, if the element was successfully inserted, or
  - false, if the array is NULL, value is NULL, or memory allocation failed.
*/
bool
d_array_sorted_insert
(
    struct d_array_sorted* _array_sorted,
    const void*           _value
)
{
    size_t insert_pos;
    char*  dest;

    if ( (!_array_sorted) ||
         (!_value) )
    {
        return D_FAILURE;
    }

    // ensure we have capacity
    if (!d_array_sorted_internal_ensure_capacity(_array_sorted))
    {
        return D_FAILURE;
    }

    // find correct position using binary search
    insert_pos = d_array_sorted_internal_find_insert_pos(_array_sorted, _value);

    // shift elements to make room
    d_array_sorted_internal_shift_right(_array_sorted, insert_pos);

    // insert the element
    dest = (char*)_array_sorted->elements + 
           (insert_pos * _array_sorted->element_size);
    d_memcpy(dest, _value, _array_sorted->element_size);
    _array_sorted->count++;

    return D_SUCCESS;
}

/*
d_array_sorted_insert_unique
  Inserts an element only if it doesn't already exist in the array.

Parameter(s):
  _array_sorted: the sorted array to modify
  _value:       pointer to the value to insert
Return:
  A boolean value corresponding to either:
  - true, if the element was inserted (was unique), or
  - false, if the element already exists, array is NULL, or allocation failed.
*/
bool
d_array_sorted_insert_unique
(
    struct d_array_sorted* _array_sorted,
    const void*           _value
)
{
    if ( (!_array_sorted) ||
         (!_value) )
    {
        return D_FAILURE;
    }

    // check if value already exists
    if (d_array_sorted_contains(_array_sorted, _value))
    {
        return D_FAILURE;
    }

    return d_array_sorted_insert(_array_sorted, _value);
}

/*
d_array_sorted_insert_all
  Inserts multiple elements into the sorted array.

Parameter(s):
  _array_sorted: the sorted array to modify
  _values:      pointer to the array of values to insert
  _count:       number of elements to insert
Return:
  The number of elements successfully inserted.
*/
size_t
d_array_sorted_insert_all
(
    struct d_array_sorted* _array_sorted,
    const void*           _values,
    size_t                _count
)
{
    size_t      inserted;
    size_t      i;
    const char* src_ptr;

    if ( (!_array_sorted) ||
         (!_values)      ||
         (_count == 0) )
    {
        return 0;
    }

    inserted = 0;
    src_ptr  = (const char*)_values;

    for (i = 0; i < _count; i++)
    {
        if (d_array_sorted_insert(_array_sorted,
                                 src_ptr + (i * _array_sorted->element_size)))
        {
            inserted++;
        }
    }

    return inserted;
}

/*
d_array_sorted_insert_all_unique
  Inserts multiple elements, skipping duplicates.

Parameter(s):
  _array_sorted: the sorted array to modify
  _values:      pointer to the array of values to insert
  _count:       number of elements to insert
Return:
  The number of unique elements successfully inserted.
*/
size_t
d_array_sorted_insert_all_unique
(
    struct d_array_sorted* _array_sorted,
    const void*           _values,
    size_t                _count
)
{
    size_t      inserted;
    size_t      i;
    const char* src_ptr;

    if ( (!_array_sorted) ||
         (!_values)      ||
         (_count == 0) )
    {
        return 0;
    }

    inserted = 0;
    src_ptr  = (const char*)_values;

    for (i = 0; i < _count; i++)
    {
        if (d_array_sorted_insert_unique(_array_sorted,
                                        src_ptr + (i * _array_sorted->element_size)))
        {
            inserted++;
        }
    }

    return inserted;
}


// =============================================================================
// search functions
// =============================================================================

/*
d_array_sorted_contains
  Checks if the sorted array contains the specified value.
  Uses binary search for O(log n) performance.

Parameter(s):
  _array_sorted: the sorted array to search
  _value:       pointer to the value to find
Return:
  A boolean value corresponding to either:
  - true, if the value is found in the array, or
  - false, if not found or parameters are invalid.
*/
bool
d_array_sorted_contains
(
    const struct d_array_sorted* _array_sorted,
    const void*                 _value
)
{
    return (d_array_sorted_find(_array_sorted, _value) >= 0);
}

/*
d_array_sorted_find
  Finds the index of a value in the sorted array using binary search.

Parameter(s):
  _array_sorted: the sorted array to search
  _value:       pointer to the value to find
Return:
  The index of the value if found, or -1 if not found or parameters are
  invalid.
*/
ssize_t
d_array_sorted_find
(
    const struct d_array_sorted* _array_sorted,
    const void*                 _value
)
{
    size_t      low;
    size_t      high;
    size_t      mid;
    int         cmp;
    const char* elem_ptr;

    if ( (!_array_sorted)            ||
         (!_value)                  ||
         (!_array_sorted->elements)  ||
         (_array_sorted->count == 0) )
    {
        return -1;
    }

    low  = 0;
    high = _array_sorted->count;

    while (low < high)
    {
        mid = low + (high - low) / 2;
        elem_ptr = (const char*)_array_sorted->elements + 
                   (mid * _array_sorted->element_size);

        cmp = _array_sorted->comparator(elem_ptr, _value);

        if (cmp == 0)
        {
            return (ssize_t)mid;
        }
        else if (cmp < 0)
        {
            low = mid + 1;
        }
        else
        {
            high = mid;
        }
    }

    return -1;
}

/*
d_array_sorted_find_first
  Finds the index of the first occurrence of a value (for duplicates).

Parameter(s):
  _array_sorted: the sorted array to search
  _value:       pointer to the value to find
Return:
  The index of the first occurrence if found, or -1 if not found.
*/
ssize_t
d_array_sorted_find_first
(
    const struct d_array_sorted* _array_sorted,
    const void*                 _value
)
{
    ssize_t     pos;
    const char* elem_ptr;

    pos = d_array_sorted_lower_bound(_array_sorted, _value);

    if (pos < 0 || (size_t)pos >= _array_sorted->count)
    {
        return -1;
    }

    elem_ptr = (const char*)_array_sorted->elements + 
               (pos * _array_sorted->element_size);

    if (_array_sorted->comparator(elem_ptr, _value) == 0)
    {
        return pos;
    }

    return -1;
}

/*
d_array_sorted_find_last
  Finds the index of the last occurrence of a value (for duplicates).

Parameter(s):
  _array_sorted: the sorted array to search
  _value:       pointer to the value to find
Return:
  The index of the last occurrence if found, or -1 if not found.
*/
ssize_t
d_array_sorted_find_last
(
    const struct d_array_sorted* _array_sorted,
    const void*                 _value
)
{
    ssize_t     pos;
    const char* elem_ptr;

    pos = d_array_sorted_upper_bound(_array_sorted, _value);

    if (pos <= 0)
    {
        return -1;
    }

    pos--;
    elem_ptr = (const char*)_array_sorted->elements + 
               (pos * _array_sorted->element_size);

    if (_array_sorted->comparator(elem_ptr, _value) == 0)
    {
        return pos;
    }

    return -1;
}

/*
d_array_sorted_lower_bound
  Finds the index of the first element not less than the given value.

Parameter(s):
  _array_sorted: the sorted array to search
  _value:       pointer to the value to compare against
Return:
  The index of the lower bound, or the count if all elements are less than
  the value.
*/
ssize_t
d_array_sorted_lower_bound
(
    const struct d_array_sorted* _array_sorted,
    const void*                 _value
)
{
    size_t      low;
    size_t      high;
    size_t      mid;
    int         cmp;
    const char* elem_ptr;

    if ( (!_array_sorted)            ||
         (!_value)                  ||
         (_array_sorted->count == 0) )
    {
        return 0;
    }

    low  = 0;
    high = _array_sorted->count;

    while (low < high)
    {
        mid = low + (high - low) / 2;
        elem_ptr = (const char*)_array_sorted->elements + 
                   (mid * _array_sorted->element_size);

        cmp = _array_sorted->comparator(elem_ptr, _value);

        if (cmp < 0)
        {
            low = mid + 1;
        }
        else
        {
            high = mid;
        }
    }

    return (ssize_t)low;
}

/*
d_array_sorted_upper_bound
  Finds the index of the first element greater than the given value.

Parameter(s):
  _array_sorted: the sorted array to search
  _value:       pointer to the value to compare against
Return:
  The index of the upper bound, or the count if no elements are greater than
  the value.
*/
ssize_t
d_array_sorted_upper_bound
(
    const struct d_array_sorted* _array_sorted,
    const void*                 _value
)
{
    size_t      low;
    size_t      high;
    size_t      mid;
    int         cmp;
    const char* elem_ptr;

    if ( (!_array_sorted)            ||
         (!_value)                  ||
         (_array_sorted->count == 0) )
    {
        return 0;
    }

    low  = 0;
    high = _array_sorted->count;

    while (low < high)
    {
        mid = low + (high - low) / 2;
        elem_ptr = (const char*)_array_sorted->elements + 
                   (mid * _array_sorted->element_size);

        cmp = _array_sorted->comparator(elem_ptr, _value);

        if (cmp <= 0)
        {
            low = mid + 1;
        }
        else
        {
            high = mid;
        }
    }

    return (ssize_t)low;
}

/*
d_array_sorted_count_value
  Counts the number of occurrences of a value in the array.

Parameter(s):
  _array_sorted: the sorted array to search
  _value:       pointer to the value to count
Return:
  The number of times the value appears in the array.
*/
size_t
d_array_sorted_count_value
(
    const struct d_array_sorted* _array_sorted,
    const void*                 _value
)
{
    ssize_t lower;
    ssize_t upper;

    if ( (!_array_sorted) ||
         (!_value) )
    {
        return 0;
    }

    lower = d_array_sorted_lower_bound(_array_sorted, _value);
    upper = d_array_sorted_upper_bound(_array_sorted, _value);

    return (size_t)(upper - lower);
}


// =============================================================================
// element access functions
// =============================================================================

/*
d_array_sorted_get
  Returns a pointer to the element at the specified index.
  Supports negative indexing (e.g., -1 for last element).

Parameter(s):
  _array_sorted: the sorted array to access
  _index:       index of the element (supports negative indexing)
Return:
  A pointer to the element at the specified index, or NULL if the array is
  NULL, empty, or the index is out of bounds.
*/
void*
d_array_sorted_get
(
    const struct d_array_sorted* _array_sorted,
    d_index                     _index
)
{
    size_t actual_index;

    if ( (!_array_sorted)            ||
         (!_array_sorted->elements)  ||
         (_array_sorted->count == 0) )
    {
        return NULL;
    }

    if (!d_index_convert_safe(_index, _array_sorted->count, &actual_index))
    {
        return NULL;
    }

    return (char*)_array_sorted->elements + 
           (actual_index * _array_sorted->element_size);
}

/*
d_array_sorted_first
  Returns a pointer to the first (smallest) element in the array.

Parameter(s):
  _array_sorted: the sorted array to access
Return:
  A pointer to the first element, or NULL if the array is NULL or empty.
*/
void*
d_array_sorted_first
(
    const struct d_array_sorted* _array_sorted
)
{
    if ( (!_array_sorted)            ||
         (!_array_sorted->elements)  ||
         (_array_sorted->count == 0) )
    {
        return NULL;
    }

    return _array_sorted->elements;
}

/*
d_array_sorted_last
  Returns a pointer to the last (largest) element in the array.

Parameter(s):
  _array_sorted: the sorted array to access
Return:
  A pointer to the last element, or NULL if the array is NULL or empty.
*/
void*
d_array_sorted_last
(
    const struct d_array_sorted* _array_sorted
)
{
    if ( (!_array_sorted)            ||
         (!_array_sorted->elements)  ||
         (_array_sorted->count == 0) )
    {
        return NULL;
    }

    return (char*)_array_sorted->elements + 
           ((_array_sorted->count - 1) * _array_sorted->element_size);
}

/*
d_array_sorted_median
  Returns a pointer to the median element in the array.

Parameter(s):
  _array_sorted: the sorted array to access
Return:
  A pointer to the median element, or NULL if the array is NULL or empty.
*/
void*
d_array_sorted_median
(
    const struct d_array_sorted* _array_sorted
)
{
    size_t median_index;

    if ( (!_array_sorted)            ||
         (!_array_sorted->elements)  ||
         (_array_sorted->count == 0) )
    {
        return NULL;
    }

    median_index = _array_sorted->count / 2;

    return (char*)_array_sorted->elements + 
           (median_index * _array_sorted->element_size);
}


// =============================================================================
// removal functions
// =============================================================================

/*
d_array_sorted_remove
  Removes the first occurrence of a value from the array.

Parameter(s):
  _array_sorted: the sorted array to modify
  _value:       pointer to the value to remove
Return:
  A boolean value corresponding to either:
  - true, if the value was found and removed, or
  - false, if the value was not found or parameters are invalid.
*/
bool
d_array_sorted_remove
(
    struct d_array_sorted* _array_sorted,
    const void*           _value
)
{
    ssize_t pos;

    if ( (!_array_sorted) ||
         (!_value) )
    {
        return D_FAILURE;
    }

    pos = d_array_sorted_find(_array_sorted, _value);

    if (pos < 0)
    {
        return D_FAILURE;
    }

    return d_array_sorted_remove_at(_array_sorted, pos);
}

/*
d_array_sorted_remove_all
  Removes all occurrences of a value from the array.

Parameter(s):
  _array_sorted: the sorted array to modify
  _value:       pointer to the value to remove
Return:
  A boolean value corresponding to either:
  - true, if at least one value was removed, or
  - false, if no values were found or parameters are invalid.
*/
bool
d_array_sorted_remove_all
(
    struct d_array_sorted* _array_sorted,
    const void*           _value
)
{
    ssize_t lower;
    ssize_t upper;
    size_t  remove_count;
    size_t  remaining;
    char*   dest;
    char*   src;

    if ( (!_array_sorted) ||
         (!_value) )
    {
        return D_FAILURE;
    }

    lower = d_array_sorted_lower_bound(_array_sorted, _value);
    upper = d_array_sorted_upper_bound(_array_sorted, _value);

    remove_count = (size_t)(upper - lower);

    if (remove_count == 0)
    {
        return D_FAILURE;
    }

    // shift remaining elements
    remaining = _array_sorted->count - (size_t)upper;

    if (remaining > 0)
    {
        dest = (char*)_array_sorted->elements + 
               ((size_t)lower * _array_sorted->element_size);
        src  = (char*)_array_sorted->elements + 
               ((size_t)upper * _array_sorted->element_size);

        memmove(dest, src, remaining * _array_sorted->element_size);
    }

    _array_sorted->count -= remove_count;

    return D_SUCCESS;
}

/*
d_array_sorted_remove_at
  Removes the element at the specified index.
  Supports negative indexing (e.g., -1 for last element).

Parameter(s):
  _array_sorted: the sorted array to modify
  _index:       index of the element to remove (supports negative indexing)
Return:
  A boolean value corresponding to either:
  - true, if the element was removed, or
  - false, if the index is out of bounds or parameters are invalid.
*/
bool
d_array_sorted_remove_at
(
    struct d_array_sorted* _array_sorted,
    d_index               _index
)
{
    size_t actual_index;

    if ( (!_array_sorted)            ||
         (_array_sorted->count == 0) )
    {
        return D_FAILURE;
    }

    if (!d_index_convert_safe(_index, _array_sorted->count, &actual_index))
    {
        return D_FAILURE;
    }

    d_array_sorted_internal_shift_left(_array_sorted, actual_index);
    _array_sorted->count--;

    return D_SUCCESS;
}

/*
d_array_sorted_remove_first
  Removes the first (smallest) element from the array.

Parameter(s):
  _array_sorted: the sorted array to modify
Return:
  A boolean value corresponding to either:
  - true, if the element was removed, or
  - false, if the array is empty or NULL.
*/
bool
d_array_sorted_remove_first
(
    struct d_array_sorted* _array_sorted
)
{
    return d_array_sorted_remove_at(_array_sorted, 0);
}

/*
d_array_sorted_remove_last
  Removes the last (largest) element from the array.

Parameter(s):
  _array_sorted: the sorted array to modify
Return:
  A boolean value corresponding to either:
  - true, if the element was removed, or
  - false, if the array is empty or NULL.
*/
bool
d_array_sorted_remove_last
(
    struct d_array_sorted* _array_sorted
)
{
    if ( (!_array_sorted)            ||
         (_array_sorted->count == 0) )
    {
        return D_FAILURE;
    }

    _array_sorted->count--;

    return D_SUCCESS;
}

/*
d_array_sorted_remove_range
  Removes elements in the specified range [_start, _end).
  Supports negative indexing.

Parameter(s):
  _array_sorted: the sorted array to modify
  _start:       starting index (inclusive, supports negative indexing)
  _end:         ending index (exclusive, supports negative indexing)
Return:
  A boolean value corresponding to either:
  - true, if elements were removed, or
  - false, if parameters are invalid or range is empty.
*/
bool
d_array_sorted_remove_range
(
    struct d_array_sorted* _array_sorted,
    d_index                _start,
    d_index                _end
)
{
    size_t start_idx;
    size_t end_idx;
    size_t remove_count;
    size_t remaining;
    char*  dest;
    char*  src;

    if ( (!_array_sorted)            ||
         (_array_sorted->count == 0) )
    {
        return D_FAILURE;
    }

    if ( (!d_index_convert_safe(_start, _array_sorted->count, &start_idx)) ||
         (!d_index_convert_safe(_end, _array_sorted->count, &end_idx)) )
    {
        return D_FAILURE;
    }

    if (start_idx >= end_idx)
    {
        return D_FAILURE;
    }

    remove_count = end_idx - start_idx;
    remaining = _array_sorted->count - end_idx;

    if (remaining > 0)
    {
        dest = (char*)_array_sorted->elements + 
               (start_idx * _array_sorted->element_size);
        src  = (char*)_array_sorted->elements + 
               (end_idx * _array_sorted->element_size);

        memmove(dest, src, remaining * _array_sorted->element_size);
    }

    _array_sorted->count -= remove_count;

    return D_SUCCESS;
}

/*
d_array_sorted_clear
  Removes all elements from the array without deallocating memory.

Parameter(s):
  _array_sorted: the sorted array to clear
Return:
  A boolean value corresponding to either:
  - true, if the array was successfully cleared, or
  - false, if the array is NULL.
*/
bool
d_array_sorted_clear
(
    struct d_array_sorted* _array_sorted
)
{
    if (!_array_sorted)
    {
        return D_FAILURE;
    }

    _array_sorted->count = 0;

    return D_SUCCESS;
}


// =============================================================================
// query functions
// =============================================================================

/*
d_array_sorted_is_empty
  Checks if the array contains no elements.

Parameter(s):
  _array_sorted: the sorted array to check
Return:
  A boolean value corresponding to either:
  - true, if the array is NULL or contains no elements, or
  - false, if the array contains one or more elements.
*/
bool
d_array_sorted_is_empty
(
    const struct d_array_sorted* _array_sorted
)
{
    return ( (!_array_sorted) ||
             (_array_sorted->count == 0) );
}

/*
d_array_sorted_count
  Returns the number of elements currently in the array.

Parameter(s):
  _array_sorted: the sorted array to query
Return:
  The number of elements in the array, or 0 if the array is NULL.
*/
size_t
d_array_sorted_count
(
    const struct d_array_sorted* _array_sorted
)
{
    return (_array_sorted) ? _array_sorted->count : 0;
}

/*
d_array_sorted_capacity
  Returns the current capacity of the array.

Parameter(s):
  _array_sorted: the sorted array to query
Return:
  The capacity of the array in number of elements, or 0 if the array is NULL.
*/
size_t
d_array_sorted_capacity
(
    const struct d_array_sorted* _array_sorted
)
{
    return (_array_sorted) ? _array_sorted->capacity : 0;
}

/*
d_array_sorted_equals
  Checks if two sorted arrays contain the same elements in the same order.

Parameter(s):
  _a: first sorted array to compare
  _b: second sorted array to compare
Return:
  A boolean value corresponding to either:
  - true, if both arrays have the same elements in the same order, or
  - false, if they differ or either is NULL.
*/
bool
d_array_sorted_equals
(
    const struct d_array_sorted* _a,
    const struct d_array_sorted* _b
)
{
    size_t      i;
    const char* ptr_a;
    const char* ptr_b;

    if ( (!_a) ||
         (!_b) )
    {
        return ((!_a) && (!_b));  // both NULL is equal
    }

    if ( (_a->count != _b->count)              ||
         (_a->element_size != _b->element_size) )
    {
        return false;
    }

    if (_a->count == 0)
    {
        return true;  // both empty
    }

    // compare elements using the comparator
    ptr_a = (const char*)_a->elements;
    ptr_b = (const char*)_b->elements;

    for (i = 0; i < _a->count; i++)
    {
        if (_a->comparator(ptr_a + (i * _a->element_size),
                           ptr_b + (i * _a->element_size)) != 0)
        {
            return false;
        }
    }

    return true;
}


// =============================================================================
// set operations
// =============================================================================

/*
d_array_sorted_union
  Creates a new sorted array containing all unique elements from both arrays.

Parameter(s):
  _a: first sorted array
  _b: second sorted array
Return:
  A new sorted array containing the union, or NULL if allocation failed or
  arrays are incompatible.
*/
struct d_array_sorted*
d_array_sorted_union
(
    const struct d_array_sorted* _a,
    const struct d_array_sorted* _b
)
{
    struct d_array_sorted* result;
    size_t                i;
    const char*           ptr;

    if ( (!_a) ||
         (!_b) ||
         (_a->element_size != _b->element_size) )
    {
        return NULL;
    }

    result = d_array_sorted_new_copy(_a);

    if (!result)
    {
        return NULL;
    }

    // insert all unique elements from _b
    ptr = (const char*)_b->elements;

    for (i = 0; i < _b->count; i++)
    {
        d_array_sorted_insert_unique(result, ptr + (i * _b->element_size));
    }

    return result;
}

/*
d_array_sorted_intersection
  Creates a new sorted array containing only elements present in both arrays.

Parameter(s):
  _a: first sorted array
  _b: second sorted array
Return:
  A new sorted array containing the intersection, or NULL if allocation failed
  or arrays are incompatible.
*/
struct d_array_sorted*
d_array_sorted_intersection
(
    const struct d_array_sorted* _a,
    const struct d_array_sorted* _b
)
{
    struct d_array_sorted* result;
    size_t                i;
    const char*           ptr;

    if ( (!_a) ||
         (!_b) ||
         (_a->element_size != _b->element_size) )
    {
        return NULL;
    }

    result = d_array_sorted_new(_a->element_size, _a->comparator);

    if (!result)
    {
        return NULL;
    }

    // insert elements from _a that exist in _b
    ptr = (const char*)_a->elements;

    for (i = 0; i < _a->count; i++)
    {
        if (d_array_sorted_contains(_b, ptr + (i * _a->element_size)))
        {
            d_array_sorted_insert_unique(result, ptr + (i * _a->element_size));
        }
    }

    return result;
}

/*
d_array_sorted_difference
  Creates a new sorted array containing elements in _a but not in _b.

Parameter(s):
  _a: first sorted array (elements to keep)
  _b: second sorted array (elements to exclude)
Return:
  A new sorted array containing the difference, or NULL if allocation failed
  or arrays are incompatible.
*/
struct d_array_sorted*
d_array_sorted_difference
(
    const struct d_array_sorted* _a,
    const struct d_array_sorted* _b
)
{
    struct d_array_sorted* result;
    size_t                i;
    const char*           ptr;

    if ( (!_a) ||
         (!_b) ||
         (_a->element_size != _b->element_size) )
    {
        return NULL;
    }

    result = d_array_sorted_new(_a->element_size, _a->comparator);

    if (!result)
    {
        return NULL;
    }

    // insert elements from _a that don't exist in _b
    ptr = (const char*)_a->elements;

    for (i = 0; i < _a->count; i++)
    {
        if (!d_array_sorted_contains(_b, ptr + (i * _a->element_size)))
        {
            d_array_sorted_insert(result, ptr + (i * _a->element_size));
        }
    }

    return result;
}


// =============================================================================
// utility functions
// =============================================================================

/*
d_array_sorted_copy_to
  Copies the array's elements to a destination buffer.

Parameter(s):
  _array_sorted:   pointer to the source sorted array
  _destination:   pointer to destination buffer
  _dest_capacity: capacity of destination buffer (in number of elements)
Return:
  A boolean value corresponding to either:
  - true, if copy was successful, or
  - false, if destination is too small or parameters are invalid.
*/
bool
d_array_sorted_copy_to
(
    const struct d_array_sorted* _array_sorted,
    void*                        _destination,
    size_t                       _dest_capacity
)
{
    if ( (!_array_sorted) ||
         (!_destination) )
    {
        return D_FAILURE;
    }

    if (_dest_capacity < _array_sorted->count)
    {
        return D_FAILURE;
    }

    if (_array_sorted->count > 0)
    {
        d_memcpy(_destination,
                 _array_sorted->elements,
                 _array_sorted->count * _array_sorted->element_size);
    }

    return D_SUCCESS;
}

/*
d_array_sorted_to_array
  Creates a new array containing copies of all elements.

Parameter(s):
  _array_sorted: the sorted array to convert
Return:
  A newly allocated array containing all elements, or NULL if allocation
  failed or the array is empty. Caller is responsible for freeing the returned
  array.
*/
void**
d_array_sorted_to_array
(
    const struct d_array_sorted* _array_sorted
)
{
    void* result;

    if ( (!_array_sorted)            ||
         (_array_sorted->count == 0) )
    {
        return NULL;
    }

    result = malloc(_array_sorted->count * _array_sorted->element_size);

    if (!result)
    {
        return NULL;
    }

    d_memcpy(result,
             _array_sorted->elements,
             _array_sorted->count * _array_sorted->element_size);

    return (void**)result;
}


// =============================================================================
// memory management functions
// =============================================================================

/*
d_array_sorted_reserve
  Ensures the array has at least the specified capacity.

Parameter(s):
  _array_sorted: the sorted array to modify
  _new_capacity: the minimum capacity to ensure
Return:
  A boolean value corresponding to either:
  - true, if the capacity is now at least _new_capacity, or
  - false, if the array is NULL or memory allocation failed.
*/
bool
d_array_sorted_reserve
(
    struct d_array_sorted* _array_sorted,
    size_t                 _new_capacity
)
{
    void* new_elements;

    if (!_array_sorted)
    {
        return D_FAILURE;
    }

    // already have sufficient capacity
    if (_new_capacity <= _array_sorted->capacity)
    {
        return D_SUCCESS;
    }

    new_elements = realloc(_array_sorted->elements,
                           _new_capacity * _array_sorted->element_size);

    if (!new_elements)
    {
        return D_FAILURE;
    }

    _array_sorted->elements = new_elements;
    _array_sorted->capacity = _new_capacity;

    return D_SUCCESS;
}

/*
d_array_sorted_shrink_to_fit
  Reduces the array's capacity to match its current count.

Parameter(s):
  _array_sorted: the sorted array to shrink
Return:
  A boolean value corresponding to either:
  - true, if the operation succeeded or no shrinking was needed, or
  - false, if the array is NULL or memory reallocation failed.
*/
bool
d_array_sorted_shrink_to_fit
(
    struct d_array_sorted* _array_sorted
)
{
    void* new_elements;

    if (!_array_sorted)
    {
        return D_FAILURE;
    }

    // already optimal size
    if (_array_sorted->capacity == _array_sorted->count)
    {
        return D_SUCCESS;
    }

    // handle empty array
    if (_array_sorted->count == 0)
    {
        if (_array_sorted->elements)
        {
            free(_array_sorted->elements);
            _array_sorted->elements = NULL;
        }

        _array_sorted->capacity = 0;

        return D_SUCCESS;
    }

    new_elements = realloc(_array_sorted->elements,
                           _array_sorted->count * _array_sorted->element_size);

    if (!new_elements)
    {
        return D_FAILURE;
    }

    _array_sorted->elements = new_elements;
    _array_sorted->capacity = _array_sorted->count;

    return D_SUCCESS;
}

/*
d_array_sorted_free
  Frees all memory associated with the sorted array.

Parameter(s):
  _array_sorted: the sorted array to free
Return:
  none
*/
void
d_array_sorted_free
(
    struct d_array_sorted* _array_sorted
)
{
    if (_array_sorted)
    {
        if (_array_sorted->elements)
        {
            free(_array_sorted->elements);
        }

        free(_array_sorted);
    }

    return;
}
