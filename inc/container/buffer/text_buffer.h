/******************************************************************************
* djinterp [container]                                           text_buffer.h
*
* Text buffer implementation optimized for string operations with automatic
* null-termination and capacity management for temporary text processing.
*
*   Supports two write modes:
*
*   RESIZE mode (default):
*     The backing char array is reallocated in place via the growth
*     factor. Best for buffers whose final size is roughly known or that
*     are modified in a read-modify-write pattern.
*
*   APPEND mode:
*     Additional text is stored in linked overflow chunks instead of
*     reallocating. Ideal for streaming or incremental construction where
*     the final size is unknown. Use `consolidate` to flatten all chunks
*     into one contiguous, null-terminated buffer.
*
*   Both modes may be active on the same buffer simultaneously.
*
*
* path:      \inc\container\buffer\text_buffer.h
* link(s):   TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.11.13
******************************************************************************/

#ifndef DJINTERP_C_CONTAINER_BUFFER_TEXT_
#define DJINTERP_C_CONTAINER_BUFFER_TEXT_ 1

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "..\..\djinterp.h"
#include "..\..\dmemory.h"
#include "..\..\dstring.h"
#include "..\..\filter.h"
#include "..\container.h"
#include ".\buffer_common.h"


// d_text_buffer
//   struct: a capacity-aware text buffer optimized for string operations
// with automatic null-termination management. Optionally supports
// overflow chunks via a d_buffer_chunk_list for append-mode writes.
struct d_text_buffer
{
    size_t                     count;    // byte length (excl. null)
    size_t                     capacity; // allocated bytes (incl. null)
    char*                      data;     // primary contiguous store
    struct d_buffer_chunk_list chunks;   // overflow chunks (append mode)
};


// I.    creation
struct d_text_buffer* d_text_buffer_new(size_t _initial_capacity);
struct d_text_buffer* d_text_buffer_new_default_capacity(void);
struct d_text_buffer* d_text_buffer_new_from_string(const char* _string);
struct d_text_buffer* d_text_buffer_new_from_string_n(const char* _string, size_t _length);
struct d_text_buffer* d_text_buffer_new_from_strings(size_t _count, ...);
struct d_text_buffer* d_text_buffer_new_from_buffer(const char* _buffer, size_t _length);
struct d_text_buffer* d_text_buffer_new_copy(const struct d_text_buffer* _other);
struct d_text_buffer* d_text_buffer_new_copy_range(const struct d_text_buffer* _other, d_index _start, d_index _end);
struct d_text_buffer* d_text_buffer_new_fill(size_t _length, char _fill_char);
struct d_text_buffer* d_text_buffer_new_formatted(const char* _format, ...);

// II.   capacity management
bool d_text_buffer_ensure_capacity(struct d_text_buffer* _buffer, size_t _required_capacity);
bool d_text_buffer_resize_to_fit(struct d_text_buffer* _buffer);
bool d_text_buffer_reserve(struct d_text_buffer* _buffer, size_t _additional_capacity);

// III.  string operations (resize mode)
bool d_text_buffer_append_string(struct d_text_buffer* _buffer, const char* _string);
bool d_text_buffer_append_string_n(struct d_text_buffer* _buffer, const char* _string, size_t _length);
bool d_text_buffer_append_buffer(struct d_text_buffer* _buffer, const char* _data, size_t _length);
bool d_text_buffer_append_char(struct d_text_buffer* _buffer, char _character);
bool d_text_buffer_append_chars(struct d_text_buffer* _buffer, char _character, size_t _count);
bool d_text_buffer_append_formatted(struct d_text_buffer* _buffer, const char* _format, ...);
bool d_text_buffer_append_buffer_obj(struct d_text_buffer* _destination, const struct d_text_buffer* _source);
bool d_text_buffer_prepend_string(struct d_text_buffer* _buffer, const char* _string);
bool d_text_buffer_prepend_buffer(struct d_text_buffer* _buffer, const char* _data, size_t _length);
bool d_text_buffer_prepend_char(struct d_text_buffer* _buffer, char _character);
bool d_text_buffer_insert_string(struct d_text_buffer* _buffer, d_index _index, const char* _string);
bool d_text_buffer_insert_buffer(struct d_text_buffer* _buffer, d_index _index, const char* _data, size_t _length);
bool d_text_buffer_insert_char(struct d_text_buffer* _buffer, d_index _index, char _character);

// IV.   string operations (append mode)
bool   d_text_buffer_append_string_chunked(struct d_text_buffer* _buffer, const char* _string, size_t _chunk_capacity);
bool   d_text_buffer_append_buffer_chunked(struct d_text_buffer* _buffer, const char* _data, size_t _length, size_t _chunk_capacity);
bool   d_text_buffer_append_char_chunked(struct d_text_buffer* _buffer, char _character, size_t _chunk_capacity);
bool   d_text_buffer_append_formatted_chunked(struct d_text_buffer* _buffer, size_t _chunk_capacity, const char* _format, ...);
bool   d_text_buffer_consolidate(struct d_text_buffer* _buffer);
size_t d_text_buffer_total_length(const struct d_text_buffer* _buffer);
bool   d_text_buffer_has_chunks(const struct d_text_buffer* _buffer);

// V.    modification operations
bool d_text_buffer_set_string(struct d_text_buffer* _buffer, const char* _string);
bool d_text_buffer_set_buffer(struct d_text_buffer* _buffer, const char* _data, size_t _length);
bool d_text_buffer_set_formatted(struct d_text_buffer* _buffer, const char* _format, ...);
bool d_text_buffer_replace_char(struct d_text_buffer* _buffer, char _old_char, char _new_char);
bool d_text_buffer_replace_string(struct d_text_buffer* _buffer, const char* _old_string, const char* _new_string);
bool d_text_buffer_replace_range(struct d_text_buffer* _buffer, d_index _start, d_index _end, const char* _replacement);
bool d_text_buffer_remove_char(struct d_text_buffer* _buffer, d_index _index);
bool d_text_buffer_remove_range(struct d_text_buffer* _buffer, d_index _start, d_index _end);
bool d_text_buffer_consume_front(struct d_text_buffer* _buffer, size_t _amount);
bool d_text_buffer_consume_back(struct d_text_buffer* _buffer, size_t _amount);

// VI.   access operations
char  d_text_buffer_get_char(const struct d_text_buffer* _buffer, d_index _index);
bool  d_text_buffer_set_char(struct d_text_buffer* _buffer, d_index _index, char _character);
char* d_text_buffer_get_string(const struct d_text_buffer* _buffer);
char* d_text_buffer_get_range_string(const struct d_text_buffer* _buffer, d_index _start, d_index _end);

// VII.  search operations
ssize_t d_text_buffer_find_char(const struct d_text_buffer* _buffer, char _character);
ssize_t d_text_buffer_find_char_from(const struct d_text_buffer* _buffer, char _character, d_index _start);
ssize_t d_text_buffer_find_string(const struct d_text_buffer* _buffer, const char* _string);
ssize_t d_text_buffer_find_string_from(const struct d_text_buffer* _buffer, const char* _string, d_index _start);
ssize_t d_text_buffer_find_last_char(const struct d_text_buffer* _buffer, char _character);
ssize_t d_text_buffer_find_last_string(const struct d_text_buffer* _buffer, const char* _string);
bool    d_text_buffer_contains_char(const struct d_text_buffer* _buffer, char _character);
bool    d_text_buffer_contains_string(const struct d_text_buffer* _buffer, const char* _string);
bool    d_text_buffer_starts_with(const struct d_text_buffer* _buffer, const char* _prefix);
bool    d_text_buffer_ends_with(const struct d_text_buffer* _buffer, const char* _suffix);
size_t  d_text_buffer_count_char(const struct d_text_buffer* _buffer, char _character);
size_t  d_text_buffer_count_string(const struct d_text_buffer* _buffer, const char* _string);

// VIII. comparison operations
int  d_text_buffer_compare(const struct d_text_buffer* _buffer1, const struct d_text_buffer* _buffer2);
int  d_text_buffer_compare_string(const struct d_text_buffer* _buffer, const char* _string);
int  d_text_buffer_compare_n(const struct d_text_buffer* _buffer1, const struct d_text_buffer* _buffer2, size_t _n);
bool d_text_buffer_equals(const struct d_text_buffer* _buffer1, const struct d_text_buffer* _buffer2);
bool d_text_buffer_equals_string(const struct d_text_buffer* _buffer, const char* _string);

// IX.   text processing
bool d_text_buffer_trim_whitespace(struct d_text_buffer* _buffer);
bool d_text_buffer_trim_front(struct d_text_buffer* _buffer);
bool d_text_buffer_trim_back(struct d_text_buffer* _buffer);
bool d_text_buffer_trim_chars(struct d_text_buffer* _buffer, const char* _chars);
bool d_text_buffer_to_upper(struct d_text_buffer* _buffer);
bool d_text_buffer_to_lower(struct d_text_buffer* _buffer);
bool d_text_buffer_reverse(struct d_text_buffer* _buffer);
bool d_text_buffer_pad_left(struct d_text_buffer* _buffer, size_t _width, char _pad_char);
bool d_text_buffer_pad_right(struct d_text_buffer* _buffer, size_t _width, char _pad_char);

// X.    filter
struct d_text_buffer* d_text_buffer_filter(const struct d_text_buffer* _buffer, const struct d_filter_expr* _expression);
bool                  d_text_buffer_filter_in_place(struct d_text_buffer* _buffer, const struct d_filter_expr* _expression);
bool                  d_text_buffer_filter_indices(const struct d_text_buffer* _buffer, const struct d_filter_expr* _expression, d_index** _out_indices, size_t* _out_count);
size_t                d_text_buffer_count_matching(const struct d_text_buffer* _buffer, const struct d_filter_expr* _expression);
struct d_text_buffer* d_text_buffer_filter_chunked(const struct d_text_buffer* _buffer, const struct d_filter_expr* _expression);

// XI.   utility
void   d_text_buffer_clear(struct d_text_buffer* _buffer);
bool   d_text_buffer_is_empty(const struct d_text_buffer* _buffer);
size_t d_text_buffer_length(const struct d_text_buffer* _buffer);
size_t d_text_buffer_capacity(const struct d_text_buffer* _buffer);
double d_text_buffer_utilization(const struct d_text_buffer* _buffer);
size_t d_text_buffer_hash(const struct d_text_buffer* _buffer);

// XII.  conversion
char*            d_text_buffer_to_cstring(const struct d_text_buffer* _buffer);
bool             d_text_buffer_copy_to_buffer(const struct d_text_buffer* _source, char* _destination, size_t _destination_size);
size_t           d_text_buffer_copy_to_buffer_n(const struct d_text_buffer* _source, char* _destination, size_t _destination_size, size_t _max_chars);
struct d_string* d_text_buffer_to_d_string(const struct d_text_buffer* _buffer);

// XIII. memory management
void d_text_buffer_free(struct d_text_buffer* _buffer);


#endif  // DJINTERP_C_CONTAINER_BUFFER_TEXT_
