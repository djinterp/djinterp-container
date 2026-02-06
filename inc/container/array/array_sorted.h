/******************************************************************************
* djinterp [container]                                          array_sorted.h
*
*   A sorted array is a dynamically-sized array that maintains its elements in
* sorted order according to a user-provided comparator function.
*   This structure supports efficient binary search for lookups and insertions,
* making it ideal for scenarios requiring frequent searches on ordered data.
*
*
* path:      \inc\container\array\array_sorted.h
* link(s):   TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.01.30
******************************************************************************/

#ifndef DJINTERP_C_CONTAINER_ARRAY_SORTED_
#define DJINTERP_C_CONTAINER_ARRAY_SORTED_ 1

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "..\..\djinterp.h"
#include "..\..\dmemory.h"
#include "..\container.h"
#include ".\array_common.h"


#ifndef D_ARRAY_SORTED_DEFAULT_CAPACITY
    // D_ARRAY_SORTED_DEFAULT_CAPACITY
    //   constant: the default capacity, in number of elements, that a new
    // `d_array_sorted` has by default.
    #define D_ARRAY_SORTED_DEFAULT_CAPACITY 32
#endif  // D_ARRAY_SORTED_DEFAULT_CAPACITY

#ifndef D_ARRAY_SORTED_GROWTH_FACTOR
    // D_ARRAY_SORTED_GROWTH_FACTOR
    //   constant: the factor by which capacity grows when reallocation is
    // needed.
    #define D_ARRAY_SORTED_GROWTH_FACTOR 2
#endif  // D_ARRAY_SORTED_GROWTH_FACTOR


// d_array_sorted
//   struct: a dynamically-sized array that maintains elements in sorted order
// according to a user-provided comparator function. Supports efficient binary
// search operations.
struct d_array_sorted
{
    size_t        count;
    size_t        capacity;
    void*         elements;
    size_t        element_size;
    fn_comparator comparator;
};


// constructor functions
struct d_array_sorted* d_array_sorted_new(size_t _element_size, fn_comparator _comparator);
struct d_array_sorted* d_array_sorted_new_with_capacity(size_t _element_size, fn_comparator _comparator, size_t _initial_capacity);
struct d_array_sorted* d_array_sorted_new_from_arr(size_t _element_size, fn_comparator _comparator, const void* _source, size_t _count);
struct d_array_sorted* d_array_sorted_new_from_args(size_t _element_size, fn_comparator _comparator, size_t _arg_count, ...);
struct d_array_sorted* d_array_sorted_new_copy(const struct d_array_sorted* _other);
struct d_array_sorted* d_array_sorted_new_merge(size_t _count, ...);

// insertion functions (maintain sorted order)
bool   d_array_sorted_insert(struct d_array_sorted* _array_sorted, const void* _value);
bool   d_array_sorted_insert_unique(struct d_array_sorted* _array_sorted, const void* _value);
size_t d_array_sorted_insert_all(struct d_array_sorted* _array_sorted, const void* _values, size_t _count);
size_t d_array_sorted_insert_all_unique(struct d_array_sorted* _array_sorted, const void* _values, size_t _count);

// search functions (binary search)
bool    d_array_sorted_contains(const struct d_array_sorted* _array_sorted, const void* _value);
ssize_t d_array_sorted_find(const struct d_array_sorted* _array_sorted, const void* _value);
ssize_t d_array_sorted_find_first(const struct d_array_sorted* _array_sorted, const void* _value);
ssize_t d_array_sorted_find_last(const struct d_array_sorted* _array_sorted, const void* _value);
ssize_t d_array_sorted_lower_bound(const struct d_array_sorted* _array_sorted, const void* _value);
ssize_t d_array_sorted_upper_bound(const struct d_array_sorted* _array_sorted, const void* _value);
size_t  d_array_sorted_count_value(const struct d_array_sorted* _array_sorted, const void* _value);

// element access functions
void*  d_array_sorted_get(const struct d_array_sorted* _array_sorted, d_index _index);
void*  d_array_sorted_first(const struct d_array_sorted* _array_sorted);
void*  d_array_sorted_last(const struct d_array_sorted* _array_sorted);
void*  d_array_sorted_median(const struct d_array_sorted* _array_sorted);

// removal functions
bool   d_array_sorted_remove(struct d_array_sorted* _array_sorted, const void* _value);
bool   d_array_sorted_remove_all(struct d_array_sorted* _array_sorted, const void* _value);
bool   d_array_sorted_remove_at(struct d_array_sorted* _array_sorted, d_index _index);
bool   d_array_sorted_remove_first(struct d_array_sorted* _array_sorted);
bool   d_array_sorted_remove_last(struct d_array_sorted* _array_sorted);
bool   d_array_sorted_remove_range(struct d_array_sorted* _array_sorted, d_index _start, d_index _end);
bool   d_array_sorted_clear(struct d_array_sorted* _array_sorted);

// query functions
bool   d_array_sorted_is_empty(const struct d_array_sorted* _array_sorted);
size_t d_array_sorted_count(const struct d_array_sorted* _array_sorted);
size_t d_array_sorted_capacity(const struct d_array_sorted* _array_sorted);
bool   d_array_sorted_equals(const struct d_array_sorted* _a, const struct d_array_sorted* _b);

// set operations (returns new sorted array)
struct d_array_sorted* d_array_sorted_union(const struct d_array_sorted* _a, const struct d_array_sorted* _b);
struct d_array_sorted* d_array_sorted_intersection(const struct d_array_sorted* _a, const struct d_array_sorted* _b);
struct d_array_sorted* d_array_sorted_difference(const struct d_array_sorted* _a, const struct d_array_sorted* _b);

// utility functions
bool   d_array_sorted_copy_to(const struct d_array_sorted* _array_sorted, void* _destination, size_t _dest_capacity);
void** d_array_sorted_to_array(const struct d_array_sorted* _array_sorted);

// memory management
bool   d_array_sorted_reserve(struct d_array_sorted* _array_sorted, size_t _new_capacity);
bool   d_array_sorted_shrink_to_fit(struct d_array_sorted* _array_sorted);
void   d_array_sorted_free(struct d_array_sorted* _array_sorted);


#endif  // DJINTERP_C_CONTAINER_ARRAY_SORTED_
