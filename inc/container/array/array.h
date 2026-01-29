/******************************************************************************
* djinterp [container]                                                 array.h
*
*
*
*
* path:      \inc\container\array\array.h 
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.01.30
******************************************************************************/

#ifndef DJINTERP_C_CONTAINER_ARRAY_
#define DJINTERP_C_CONTAINER_ARRAY_ 1

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include "..\..\djinterp.h"
#include "..\..\dmemory.h"
#include "..\container.h"
#include ".\array_common.h"


// D_ARRAY_INIT
//   macro: macro-based initializer; assigns a `d_array` with the elements
// specified.
// Example:
//   struct d_array arr = D_ARRAY_INIT(int, 1, 2, 3, 4, 5);
#define D_ARRAY_INIT(element_type, ...)                              \
    {                                                                \
        .count    = D_ARRAY_COUNT_T(element_type,                    \
                                    __VA_ARGS__),                    \
        .elements = (element_type[]){ __VA_ARGS__ }                  \
    }

// D_ARRAY_S_INIT
//   macro: macro-based initializer; assigns a `d_array_s` with the elements
// specified.
// Example:
//   struct d_array arr = D_ARRAY_S_INIT(int, 1, 2, 3, 4, 5);
#define D_ARRAY_S_INIT(element_type, ...)                            \
    {                                                                \
        .count        = D_ARRAY_COUNT_T(element_type,                \
                                        __VA_ARGS__),                \
        .elements     = (element_type[]){ __VA_ARGS__ },             \
        .element_size = sizeof(element_type)                         \
    }


// d_array
//   struct: a safe array type.
struct d_array
{
	size_t count;
	void*  elements;
};

// d_array_s
//   struct: corresponds to a "sized" safe array, with the size of each
// individual element stored for added safety.
struct d_array_s
{
	size_t count;
	void*  elements;
	size_t element_size;
};


// `d_array` creation functions
struct d_array*   d_array_new(size_t _element_size, size_t _initial_size);
struct d_array*   d_array_new_default_size(size_t _element_size);
struct d_array*   d_array_new_from_arr(size_t _element_size, const void* _source, size_t _source_count);
struct d_array*   d_array_new_from_args(size_t _element_size, size_t _arg_count , ...);
struct d_array*   d_array_new_copy(const struct d_array* _source, size_t _element_size);
struct d_array*   d_array_new_copy_reverse(const struct d_array* _source, size_t _element_size, d_index _start, d_index _end);
struct d_array*   d_array_new_copy_range(const struct d_array* _source, size_t _element_size, d_index _start, d_index _end);
struct d_array*   d_array_new_copy_range_reverse(const struct d_array* _source, size_t _element_size, d_index _start, d_index _end);
struct d_array*   d_array_new_fill(size_t _element_size, size_t _initial_size, const void* _value);
struct d_array*   d_array_new_merge(size_t _element_size, size_t _count, ...);
struct d_array*   d_array_new_slice(const struct d_array* _source, size_t _element_size, d_index _start);
struct d_array*   d_array_new_slice_reverse(const struct d_array* _source, size_t _element_size, d_index _start);
struct d_array*   d_array_new_slice_range(const struct d_array* _source, size_t _element_size, d_index _start, d_index _end);
struct d_array*   d_array_new_slice_range_reverse(const struct d_array* _source, size_t _element_size, d_index _start, d_index _end);

// `d_array_s` creation functions
struct d_array_s* d_array_s_new(size_t _element_size, size_t _initial_size);
struct d_array_s* d_array_s_new_default_size(size_t _element_size);
struct d_array_s* d_array_s_new_from_arr(size_t _element_size, const void* _source, size_t _source_count);
struct d_array_s* d_array_s_new_from_args(size_t _element_size, size_t _arg_count, ...);
struct d_array_s* d_array_s_new_copy(const struct d_array* _source, size_t _element_size);
struct d_array_s* d_array_s_new_copy_reverse(const struct d_array* _source, size_t _element_size, d_index _start, d_index _end);
struct d_array_s* d_array_s_new_copy_range(const struct d_array* _source, size_t _element_size, d_index _start, d_index _end);
struct d_array_s* d_array_s_new_copy_range_reverse(const struct d_array* _source, size_t _element_size, d_index _start, d_index _end);
struct d_array_s* d_array_s_new_fill(size_t _element_size, size_t _initial_size, const void* _value);
struct d_array_s* d_array_s_new_merge(size_t _element_size, size_t _count, ...);
struct d_array_s* d_array_s_new_slice(const struct d_array* _source, size_t _element_size, d_index _start);
struct d_array_s* d_array_s_new_slice_reverse(const struct d_array* _source, size_t _element_size, d_index _start);
struct d_array_s* d_array_s_new_slice_range(const struct d_array* _source, size_t _element_size, d_index _start, d_index _end);
struct d_array_s* d_array_s_new_slice_range_reverse(const struct d_array* _source, size_t _element_size, d_index _start, d_index _end);

// `d_array` manipulation functions
bool    d_array_append_element(struct d_array* _array, size_t _element_size, const void* _element);
bool    d_array_append_elements(struct d_array* _array, size_t _element_size, const void* _elements, size_t _count);
bool    d_array_append_array(struct d_array* _array, size_t _element_size, const void* _elements, size_t _count, d_index _index);
bool    d_array_contains(const struct d_array* _array, size_t _element_size, const void* _value);
bool    d_array_fill(struct d_array* _array, size_t _element_size, const void* _fill_element);
ssize_t d_array_find(struct d_array* _array, size_t _element_size, const void* _value);
bool    d_array_insert_element(struct d_array* _array, size_t _element_size, const void* _element, d_index _index);
bool    d_array_insert_elements(struct d_array* _array, size_t _element_size, const void* _elements, size_t _count, d_index _index);
bool    d_array_insert_array(struct d_array* _destination, size_t _element_size, const struct d_array* _source, d_index _index);
bool    d_array_is_empty(const struct d_array* _array);
bool    d_array_prepend_element(struct d_array* _array, size_t _element_size, const void* _element);
bool    d_array_prepend_elements(struct d_array* _array, size_t _element_size, const void* _elements, size_t _count);
bool    d_array_prepend_array(struct d_array* _destination, size_t _element_size, const struct d_array* _source);
bool    d_array_resize_amount(struct d_array* _array, size_t _element_size, ssize_t _amount);
bool    d_array_resize_factor(struct d_array* _array, size_t _element_size, double _factor);
bool    d_array_reverse(struct d_array* _array, size_t _element_size);
bool    d_array_shift_left(struct d_array* _array, size_t _element_size, size_t _amount);
bool    d_array_shift_left_circular(struct d_array* _array, size_t _element_size, size_t _amount);
bool    d_array_shift_right(struct d_array* _array, size_t _element_size, size_t _amount);
bool    d_array_shift_right_circular(struct d_array* _array, size_t _element_size, size_t _amount);
void*   d_array_slice(void* _source, size_t _length, d_index index, size_t _element_size);
void*   d_array_slice_range(void* _source, size_t _length, d_index _start, d_index _end, size_t _element_size);
void    d_array_sort(struct d_array* _array, size_t _element_size, fn_comparator _comparator);

// memory management
void  d_array_free(struct d_array* _array);
void  d_array_s_free(struct d_array_s* _array);


#endif	// DJINTERP_C_CONTAINER_ARRAY_