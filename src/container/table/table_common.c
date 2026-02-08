#include "..\..\..\inc\container\table\table_common.h"


/*
d_table_common_init
  Allocates a zeroed row array large enough to hold _num_rows rows, each
_struct_size bytes. On success, *_data points to the new buffer and
*_row_count is set to _num_rows.

Parameter(s):
  _data:        output pointer that receives the allocated row buffer.
  _row_count:   output pointer that receives the row count.
  _struct_size: size in bytes of one row struct.
  _num_rows:    number of rows to allocate.
Return:
  true if allocation succeeded and all parameters were valid, false
otherwise. On failure *_data is set to NULL and *_row_count to 0.
*/
bool
d_table_common_init
(
    void**  _data,
    size_t* _row_count,
    size_t  _struct_size,
    size_t  _num_rows
)
{
    void*  buffer;
    size_t total;

    // validate output pointers and struct size
    if ( (!_data)       ||
         (!_row_count)  ||
         (_struct_size == 0) )
    {
        return false;
    }

    // initialize outputs
    *_data      = NULL;
    *_row_count = 0;

    // a table with zero rows is valid but needs no allocation
    if (_num_rows == 0)
    {
        return true;
    }

    // check for overflow
    total = _num_rows * _struct_size;
    if (total / _struct_size != _num_rows)
    {
        return false;
    }

    // allocate zeroed memory
    buffer = calloc(_num_rows, _struct_size);
    if (!buffer)
    {
        return false;
    }

    *_data      = buffer;
    *_row_count = _num_rows;

    return true;
}


/*
d_table_common_init_copy
  Allocates a row array and copies _num_rows rows from _source into it.
Each row is _struct_size bytes. On success, *_data points to the new buffer
and *_row_count is set to _num_rows.

Parameter(s):
  _data:        output pointer that receives the allocated row buffer.
  _row_count:   output pointer that receives the row count.
  _struct_size: size in bytes of one row struct.
  _source:      pointer to the source row array to copy from.
  _num_rows:    number of rows in the source array.
Return:
  true if allocation and copy succeeded, false otherwise. On failure *_data
is set to NULL and *_row_count to 0.
*/
bool
d_table_common_init_copy
(
    void**      _data,
    size_t*     _row_count,
    size_t      _struct_size,
    const void* _source,
    size_t      _num_rows
)
{
    void*  buffer;
    size_t total;

    // validate parameters
    if ( (!_data)             ||
         (!_row_count)        ||
         (_struct_size == 0)  ||
         (!_source) )
    {
        return false;
    }

    // initialize outputs
    *_data      = NULL;
    *_row_count = 0;

    // zero rows is valid but needs no allocation or copy
    if (_num_rows == 0)
    {
        return true;
    }

    // check for overflow
    total = _num_rows * _struct_size;
    if (total / _struct_size != _num_rows)
    {
        return false;
    }

    // allocate and copy
    buffer = malloc(total);
    if (!buffer)
    {
        return false;
    }

    d_memcpy(buffer, _source, total);

    *_data      = buffer;
    *_row_count = _num_rows;

    return true;
}


/*
d_table_common_find_row
  Performs a forward linear search over the row array, comparing the field
described by _search_column in each row against _value using _comparator.
Returns the index of the first matching row, or -1 if no match is found.

Parameter(s):
  _data:          pointer to the contiguous row array.
  _struct_size:   size in bytes of one row struct.
  _row_count:     number of rows in the array.
  _search_column: column descriptor identifying the field to search.
  _value:         pointer to the value to compare against.
  _comparator:    comparison function; returns 0 on match.
Return:
  The index of the first matching row as a non-negative value, or -1 if
no match was found or any parameter was invalid.
*/
ssize_t
d_table_common_find_row
(
    const void*                    _data,
    size_t                         _struct_size,
    size_t                         _row_count,
    const struct d_table_column_desc* _search_column,
    const void*                    _value,
    fn_comparator                  _comparator
)
{
    const char* row_ptr;
    const void* cell_ptr;
    size_t      i;

    // validate parameters
    if ( (!_data)           ||
         (_struct_size == 0)  ||
         (_row_count == 0)  ||
         (!_search_column)  ||
         (!_value)          ||
         (!_comparator) )
    {
        return -1;
    }

    // linear scan forward
    row_ptr = (const char*)_data;
    for (i = 0; i < _row_count; i++)
    {
        cell_ptr = row_ptr + _search_column->offset;
        if (_comparator(cell_ptr, _value) == 0)
        {
            return (ssize_t)i;
        }

        row_ptr += _struct_size;
    }

    return -1;
}


/*
d_table_common_find_row_last
  Performs a reverse linear search over the row array, comparing the field
described by _search_column in each row against _value using _comparator.
Returns the index of the last matching row, or -1 if no match is found.

Parameter(s):
  _data:          pointer to the contiguous row array.
  _struct_size:   size in bytes of one row struct.
  _row_count:     number of rows in the array.
  _search_column: column descriptor identifying the field to search.
  _value:         pointer to the value to compare against.
  _comparator:    comparison function; returns 0 on match.
Return:
  The index of the last matching row as a non-negative value, or -1 if
no match was found or any parameter was invalid.
*/
ssize_t
d_table_common_find_row_last
(
    const void*                    _data,
    size_t                         _struct_size,
    size_t                         _row_count,
    const struct d_table_column_desc* _search_column,
    const void*                    _value,
    fn_comparator                  _comparator
)
{
    const char* row_ptr;
    const void* cell_ptr;
    size_t      i;

    // validate parameters
    if ( (!_data)             ||
         (_struct_size == 0)  ||
         (_row_count == 0)    ||
         (!_search_column)    ||
         (!_value)            ||
         (!_comparator) )
    {
        return -1;
    }

    // linear scan backward
    i = _row_count;
    while (i > 0)
    {
        i--;
        row_ptr  = (const char*)_data + (i * _struct_size);
        cell_ptr = row_ptr + _search_column->offset;
        if (_comparator(cell_ptr, _value) == 0)
        {
            return (ssize_t)i;
        }
    }

    return -1;
}


/*
d_table_common_swap_rows
  Swaps the contents of two rows in the row array. Both indices support
negative indexing via d_index.

Parameter(s):
  _data:        pointer to the contiguous row array.
  _struct_size: size in bytes of one row struct.
  _row_count:   number of rows in the array.
  _row_a:       index of the first row (supports negative indexing).
  _row_b:       index of the second row (supports negative indexing).
Return:
  true if the swap succeeded, false if any parameter was invalid or an
index was out of bounds.
*/
bool
d_table_common_swap_rows
(
    void*   _data,
    size_t  _struct_size,
    size_t  _row_count,
    d_index _row_a,
    d_index _row_b
)
{
    size_t idx_a;
    size_t idx_b;
    char*  ptr_a;
    char*  ptr_b;
    char*  temp;

    // validate parameters
    if ( (!_data)             ||
         (_struct_size == 0)  ||
         (_row_count == 0) )
    {
        return false;
    }

    // convert and validate indices
    if ( (!d_index_convert_safe(_row_a, _row_count, &idx_a)) ||
         (!d_index_convert_safe(_row_b, _row_count, &idx_b)) )
    {
        return false;
    }

    // swapping a row with itself is a no-op
    if (idx_a == idx_b)
    {
        return true;
    }

    // allocate temporary buffer for the swap
    temp = (char*)malloc(_struct_size);
    if (!temp)
    {
        return false;
    }

    ptr_a = (char*)_data + (idx_a * _struct_size);
    ptr_b = (char*)_data + (idx_b * _struct_size);

    d_memcpy(temp,  ptr_a, _struct_size);
    d_memcpy(ptr_a, ptr_b, _struct_size);
    d_memcpy(ptr_b, temp,  _struct_size);

    free(temp);

    return true;
}


/*
d_table_common_sort_by_column
  Sorts the row array in place by the field described by _sort_column,
using _comparator to order elements. The comparator receives pointers
to the column field within each row, not to the rows themselves.

Parameter(s):
  _data:        pointer to the contiguous row array.
  _struct_size: size in bytes of one row struct.
  _row_count:   number of rows in the array.
  _sort_column: column descriptor identifying the sort key field.
  _comparator:  comparison function for the column values.
Return:
  true if the sort completed successfully, false if any parameter was
invalid.
*/

// D_INTERNAL_TABLE_SORT_CTX
//   struct (internal): context passed through to the qsort comparator
// wrapper so it can locate the correct field within each row.
struct D_INTERNAL_TABLE_SORT_CTX
{
    size_t        offset;
    fn_comparator comparator;
};

// d_internal_table_sort_comparator
//   function (internal): qsort-compatible wrapper that extracts the
// column field from each row before delegating to the user comparator.
// The context is accessed via a file-scope pointer because qsort does
// not support a user-data parameter.
static struct D_INTERNAL_TABLE_SORT_CTX* g_table_sort_ctx = NULL;

static int
d_internal_table_sort_comparator
(
    const void* _a,
    const void* _b
)
{
    const void* field_a;
    const void* field_b;

    field_a = (const char*)_a + g_table_sort_ctx->offset;
    field_b = (const char*)_b + g_table_sort_ctx->offset;

    return g_table_sort_ctx->comparator(field_a, field_b);
}

bool
d_table_common_sort_by_column
(
    void*                          _data,
    size_t                         _struct_size,
    size_t                         _row_count,
    const struct d_table_column_desc* _sort_column,
    fn_comparator                  _comparator
)
{
    struct D_INTERNAL_TABLE_SORT_CTX ctx;

    // validate parameters
    if ( (!_data)             ||
         (_struct_size == 0)  ||
         (!_sort_column)      ||
         (!_comparator) )
    {
        return false;
    }

    // zero or one row is already sorted
    if (_row_count <= 1)
    {
        return true;
    }

    // set up the sort context and call qsort
    ctx.offset     = _sort_column->offset;
    ctx.comparator = _comparator;
    g_table_sort_ctx = &ctx;

    qsort(_data,
          _row_count,
          _struct_size,
          d_internal_table_sort_comparator);

    g_table_sort_ctx = NULL;

    return true;
}


/*
d_table_common_copy_to
  Copies the entire row array into a caller-supplied destination buffer.
The destination must be at least _row_count * _struct_size bytes.

Parameter(s):
  _source:        pointer to the source row array.
  _struct_size:   size in bytes of one row struct.
  _row_count:     number of rows in the source array.
  _destination:   pointer to the destination buffer.
  _dest_capacity: size in bytes of the destination buffer.
Return:
  true if the copy succeeded, false if any parameter was invalid or the
destination buffer is too small.
*/
bool
d_table_common_copy_to
(
    const void* _source,
    size_t      _struct_size,
    size_t      _row_count,
    void*       _destination,
    size_t      _dest_capacity
)
{
    size_t total;

    // validate parameters
    if ( (!_source)           ||
         (_struct_size == 0)  ||
         (!_destination) )
    {
        return false;
    }

    // zero rows is a valid no-op
    if (_row_count == 0)
    {
        return true;
    }

    // compute total size and check for overflow
    total = _row_count * _struct_size;
    if (total / _struct_size != _row_count)
    {
        return false;
    }

    // check destination capacity
    if (_dest_capacity < total)
    {
        return false;
    }

    d_memcpy(_destination, _source, total);

    return true;
}


/*
d_table_common_column_desc_validate
  Validates an array of column descriptors against a row struct of the
given size. Checks that every column's offset + element_size falls
within _struct_size, that no column has a zero element_size, and that
the column count does not exceed D_TABLE_MAX_COLUMNS.

Parameter(s):
  _column_descs: pointer to the column descriptor array.
  _column_count: number of descriptors in the array.
  _struct_size:  size in bytes of the row struct.
Return:
  true if all descriptors are valid, false otherwise.
*/
bool
d_table_common_column_desc_validate
(
    const struct d_table_column_desc* _column_descs,
    size_t                            _column_count,
    size_t                            _struct_size
)
{
    size_t i;
    size_t end;

    // validate parameters
    if ( (!_column_descs)     ||
         (_column_count == 0) ||
         (_struct_size == 0) )
    {
        return false;
    }

    // validate each descriptor
    for (i = 0; i < _column_count; i++)
    {
        // zero-sized column is invalid
        if (_column_descs[i].element_size == 0)
        {
            return false;
        }

        // check that the field fits within the struct
        end = _column_descs[i].offset + _column_descs[i].element_size;

        // check for overflow
        if (end < _column_descs[i].offset)
        {
            return false;
        }

        // check that the field does not extend past the struct
        if (end > _struct_size)
        {
            return false;
        }
    }

    return true;
}


/*
d_table_common_free_data
  Frees a heap-allocated row array. Safe to call with NULL.

Parameter(s):
  _data: pointer to the row array to free; may be NULL.
Return:
  none.
*/
void
d_table_common_free_data
(
    void* _data
)
{
    if (_data)
    {
        free(_data);
    }

    return;
}
