#include "..\..\..\inc\container\table\table.h"


// ---------------------------------------------------------------------------
//  internal helpers
// ---------------------------------------------------------------------------

/*
d_internal_table_promote
  If the table does not own its row buffer, allocates a new heap buffer
of at least _min_capacity rows, copies the existing rows into it, and
sets the OWNS_DATA flag.  If the table already owns its data and
_min_capacity <= capacity, this is a no-op.

Parameter(s):
  _table:        the table to promote.
  _min_capacity: minimum row slots in the new buffer.
Return:
  true on success, false on allocation failure or invalid table.
*/
D_STATIC bool
d_internal_table_promote
(
    struct d_table* _table,
    size_t          _min_capacity
)
{
    void*  new_data;
    size_t cap;
    size_t total;

    if (!_table)
    {
        return false;
    }

    // already owned and big enough
    if ( (_table->flags & D_TABLE_FLAG_OWNS_DATA) &&
         (_table->capacity >= _min_capacity) )
    {
        return true;
    }

    // choose capacity
    cap = _min_capacity;
    if (cap < _table->row_count)
    {
        cap = _table->row_count;
    }
    if (cap < D_TABLE_DEFAULT_CAPACITY)
    {
        cap = D_TABLE_DEFAULT_CAPACITY;
    }

    // overflow check
    total = (cap * _table->struct_size);
    if ( (_table->struct_size > 0) && 
         ( (total / _table->struct_size) != cap) )
    {
        return false;
    }

    new_data = calloc(cap, _table->struct_size);

    // ensure that memory allocation was successful
    if (!new_data)
    {
        return false;
    }

    // copy existing rows
    if (_table->data && _table->row_count > 0)
    {
        d_memcpy(new_data,
                 _table->data,
                 _table->row_count * _table->struct_size);
    }

    // free old buffer only if previously owned
    if (_table->flags & D_TABLE_FLAG_OWNS_DATA)
    {
        free(_table->data);
    }

    _table->data     = new_data;
    _table->capacity = cap;
    _table->flags   |= D_TABLE_FLAG_OWNS_DATA;

    return true;
}


/*
d_internal_table_grow_capacity
  Compute the next capacity from the current capacity using the
configured growth factor (D_TABLE_GROWTH_FACTOR_NUM / DEN).  Returns
at least D_TABLE_DEFAULT_CAPACITY.

Parameter(s):
  _current: the current capacity.
Return:
  the new capacity, or 0 on overflow.
*/
D_STATIC size_t
d_internal_table_grow_capacity
(
    size_t _current
)
{
    size_t grown;

    if (_current == 0)
    {
        return D_TABLE_DEFAULT_CAPACITY;
    }

    grown = (_current * D_TABLE_GROWTH_FACTOR_NUM)
            / D_TABLE_GROWTH_FACTOR_DEN;

    // must grow by at least 1
    if (grown <= _current)
    {
        grown = _current + 1;
    }

    // overflow check
    if (grown < _current)
    {
        return 0;
    }

    return grown;
}


// ---------------------------------------------------------------------------
//  V.  constructor functions
// ---------------------------------------------------------------------------


/*
d_table_new
  Allocate a heap-owned d_table with the given column schema and an
initial row capacity.  The row buffer is zeroed.  The column descriptor
array is NOT copied; the caller must ensure it outlives the table.

Parameter(s):
  _struct_size:      size in bytes of one row struct.
  _column_descs:     column descriptor array.
  _column_count:     number of column descriptors.
  _initial_capacity: number of row slots to pre-allocate.
Return:
  pointer to a new d_table on success, NULL on failure.
*/
struct d_table*
d_table_new
(
    size_t                            _struct_size,
    const struct d_table_column_desc* _column_descs,
    size_t                            _column_count,
    size_t                            _initial_capacity
)
{
    struct d_table* table;
    void*           buffer;
    size_t          cap;
    size_t          total;

    if ( (_struct_size == 0)  ||
         (!_column_descs)     ||
         (_column_count == 0) )
    {
        return NULL;
    }

    // ensure minimum capacity
    cap = _initial_capacity;
    if (cap == 0)
    {
        cap = D_TABLE_DEFAULT_CAPACITY;
    }

    // overflow check
    total = cap * _struct_size;
    if (total / _struct_size != cap)
    {
        return NULL;
    }

    table = (struct d_table*)malloc(sizeof(struct d_table));
    if (!table)
    {
        return NULL;
    }

    buffer = calloc(cap, _struct_size);
    if (!buffer)
    {
        free(table);
        return NULL;
    }

    table->data         = buffer;
    table->column_descs = (struct d_table_column_desc*)_column_descs;
    table->struct_size  = _struct_size;
    table->row_count    = 0;
    table->column_count = _column_count;
    table->capacity     = cap;
    table->flags        = D_TABLE_FLAG_OWNS_DATA;

    return table;
}


/*
d_table_new_from_rows
  Allocate a heap-owned d_table and copy _row_count rows from _source.
The resulting table has capacity == _row_count.

Parameter(s):
  _struct_size:  size in bytes of one row struct.
  _source:       pointer to the source row array.
  _row_count:    number of rows to copy.
  _column_descs: column descriptor array.
  _column_count: number of column descriptors.
Return:
  pointer to a new d_table on success, NULL on failure.
*/
struct d_table*
d_table_new_from_rows
(
    size_t                            _struct_size,
    const void*                       _source,
    size_t                            _row_count,
    const struct d_table_column_desc* _column_descs,
    size_t                            _column_count
)
{
    struct d_table* table;
    void*           buffer;
    size_t          total;

    if ( (_struct_size == 0)  ||
         (!_source)           ||
         (!_column_descs)     ||
         (_column_count == 0) )
    {
        return NULL;
    }

    table = (struct d_table*)malloc(sizeof(struct d_table));
    if (!table)
    {
        return NULL;
    }

    if (_row_count == 0)
    {
        table->data         = NULL;
        table->column_descs = (struct d_table_column_desc*)_column_descs;
        table->struct_size  = _struct_size;
        table->row_count    = 0;
        table->column_count = _column_count;
        table->capacity     = 0;
        table->flags        = D_TABLE_FLAG_OWNS_DATA;
        return table;
    }

    // overflow check
    total = _row_count * _struct_size;
    if (total / _struct_size != _row_count)
    {
        free(table);
        return NULL;
    }

    buffer = malloc(total);
    if (!buffer)
    {
        free(table);
        return NULL;
    }

    d_memcpy(buffer, _source, total);

    table->data         = buffer;
    table->column_descs = (struct d_table_column_desc*)_column_descs;
    table->struct_size  = _struct_size;
    table->row_count    = _row_count;
    table->column_count = _column_count;
    table->capacity     = _row_count;
    table->flags        = D_TABLE_FLAG_OWNS_DATA;

    return table;
}


/*
d_table_new_copy
  Deep-copy another table.  Both the row buffer and the column
descriptor array are duplicated.

Parameter(s):
  _other: the table to copy.
Return:
  pointer to a new d_table on success, NULL on failure.
*/
struct d_table*
d_table_new_copy
(
    const struct d_table* _other
)
{
    struct d_table*          table;
    void*                    buffer;
    struct d_table_column_desc* descs;
    size_t                   data_total;
    size_t                   descs_total;

    if (!_other)
    {
        return NULL;
    }

    if ( (_other->struct_size == 0)  ||
         (!_other->column_descs)     ||
         (_other->column_count == 0) )
    {
        return NULL;
    }

    table = (struct d_table*)malloc(sizeof(struct d_table));
    if (!table)
    {
        return NULL;
    }

    // copy column descriptors
    descs_total = _other->column_count
                  * sizeof(struct d_table_column_desc);
    descs = (struct d_table_column_desc*)malloc(descs_total);
    if (!descs)
    {
        free(table);
        return NULL;
    }
    d_memcpy(descs, _other->column_descs, descs_total);

    // copy row data
    buffer = NULL;
    if (_other->data && _other->row_count > 0)
    {
        data_total = _other->row_count * _other->struct_size;
        buffer = malloc(data_total);
        if (!buffer)
        {
            free(descs);
            free(table);
            return NULL;
        }
        d_memcpy(buffer, _other->data, data_total);
    }

    table->data         = buffer;
    table->column_descs = descs;
    table->struct_size  = _other->struct_size;
    table->row_count    = _other->row_count;
    table->column_count = _other->column_count;
    table->capacity     = _other->row_count;
    table->flags        = D_TABLE_FLAG_OWNS_DATA
                          | D_TABLE_FLAG_OWNS_DESCS;

    return table;
}


/*
d_table_new_fill
  Allocate a table with _row_count rows, each initialized to the bytes
at _fill_value (which must point to _struct_size bytes).

Parameter(s):
  _struct_size:  size in bytes of one row struct.
  _row_count:    number of rows to create.
  _column_descs: column descriptor array.
  _column_count: number of column descriptors.
  _fill_value:   pointer to the fill row; must be _struct_size bytes.
Return:
  pointer to a new d_table on success, NULL on failure.
*/
struct d_table*
d_table_new_fill
(
    size_t                            _struct_size,
    size_t                            _row_count,
    const struct d_table_column_desc* _column_descs,
    size_t                            _column_count,
    const void*                       _fill_value
)
{
    struct d_table* table;
    void*           buffer;
    size_t          total;
    size_t          i;
    char*           dst;

    if ( (_struct_size == 0)  ||
         (!_column_descs)     ||
         (_column_count == 0) ||
         (!_fill_value) )
    {
        return NULL;
    }

    table = (struct d_table*)malloc(sizeof(struct d_table));
    if (!table)
    {
        return NULL;
    }

    if (_row_count == 0)
    {
        table->data         = NULL;
        table->column_descs = (struct d_table_column_desc*)_column_descs;
        table->struct_size  = _struct_size;
        table->row_count    = 0;
        table->column_count = _column_count;
        table->capacity     = 0;
        table->flags        = D_TABLE_FLAG_OWNS_DATA;
        return table;
    }

    // overflow check
    total = _row_count * _struct_size;
    if (total / _struct_size != _row_count)
    {
        free(table);
        return NULL;
    }

    buffer = malloc(total);
    if (!buffer)
    {
        free(table);
        return NULL;
    }

    // fill each row
    dst = (char*)buffer;
    for (i = 0; i < _row_count; i++)
    {
        d_memcpy(dst, _fill_value, _struct_size);
        dst += _struct_size;
    }

    table->data         = buffer;
    table->column_descs = (struct d_table_column_desc*)_column_descs;
    table->struct_size  = _struct_size;
    table->row_count    = _row_count;
    table->column_count = _column_count;
    table->capacity     = _row_count;
    table->flags        = D_TABLE_FLAG_OWNS_DATA;

    return table;
}


// ---------------------------------------------------------------------------
//  VI.  capacity functions
// ---------------------------------------------------------------------------


/*
d_table_reserve
  Ensure the table can hold at least _new_capacity rows without further
allocation.  If the table does not own its data, promotes it to an
owned heap copy first.

Parameter(s):
  _table:        the table.
  _new_capacity: minimum row slots required.
Return:
  true on success, false on failure.
*/
bool
d_table_reserve
(
    struct d_table* _table,
    size_t          _new_capacity
)
{
    void*  new_data;
    size_t total;

    if ( (!_table) ||
         (_table->struct_size == 0) )
    {
        return false;
    }

    // if not owned, promote first
    if (!(_table->flags & D_TABLE_FLAG_OWNS_DATA))
    {
        return d_internal_table_promote(_table, _new_capacity);
    }

    // already big enough
    if (_table->capacity >= _new_capacity)
    {
        return true;
    }

    // overflow check
    total = _new_capacity * _table->struct_size;
    if (total / _table->struct_size != _new_capacity)
    {
        return false;
    }

    new_data = realloc(_table->data, total);
    
    // ensure that memory reallocation was successful
    if (!new_data)
    {
        return false;
    }

    // zero newly available region
    memset((char*)new_data + _table->row_count * _table->struct_size,
           0,
           (_new_capacity - _table->row_count) * _table->struct_size);

    _table->data     = new_data;
    _table->capacity = _new_capacity;

    return true;
}


/*
d_table_shrink_to_fit
  Reallocate the row buffer to exactly fit row_count rows.

Parameter(s):
  _table: the table.
Return:
  true on success, false on failure.
*/
bool
d_table_shrink_to_fit
(
    struct d_table* _table
)
{
    void*  new_data;
    size_t total;

    if (!_table)
    {
        return false;
    }

    if (!(_table->flags & D_TABLE_FLAG_OWNS_DATA))
    {
        return false;
    }

    if (_table->capacity == _table->row_count)
    {
        return true;
    }

    if (_table->row_count == 0)
    {
        free(_table->data);
        _table->data     = NULL;
        _table->capacity = 0;
        return true;
    }

    total    = _table->row_count * _table->struct_size;
    new_data = realloc(_table->data, total);
    if (!new_data)
    {
        return false;
    }

    _table->data     = new_data;
    _table->capacity = _table->row_count;

    return true;
}


/*
d_table_ensure_capacity
  Ensure room for at least _required total rows, growing by the
configured growth factor if necessary.

Parameter(s):
  _table:    the table.
  _required: minimum total row slots needed.
Return:
  true on success, false on failure.
*/
bool
d_table_ensure_capacity
(
    struct d_table* _table,
    size_t          _required
)
{
    size_t new_cap;

    if (!_table)
    {
        return false;
    }

    if (_table->capacity >= _required &&
        (_table->flags & D_TABLE_FLAG_OWNS_DATA))
    {
        return true;
    }

    // grow until we meet the requirement
    new_cap = _table->capacity;
    if (new_cap == 0)
    {
        new_cap = D_TABLE_DEFAULT_CAPACITY;
    }
    while (new_cap < _required)
    {
        new_cap = d_internal_table_grow_capacity(new_cap);
        if (new_cap == 0)
        {
            return false;
        }
    }

    return d_table_reserve(_table, new_cap);
}


/*
d_table_grow
  Grow the row buffer by the configured growth factor.

Parameter(s):
  _table: the table.
Return:
  true on success, false on failure.
*/
bool
d_table_grow
(
    struct d_table* _table
)
{
    size_t new_cap;

    if (!_table)
    {
        return false;
    }

    new_cap = d_internal_table_grow_capacity(_table->capacity);
    if (new_cap == 0)
    {
        return false;
    }

    return d_table_reserve(_table, new_cap);
}


/*
d_table_available
  Return the number of row slots available before the next reallocation.

Parameter(s):
  _table: the table.
Return:
  number of available row slots, or 0 if the table is NULL or a view.
*/
size_t
d_table_available
(
    const struct d_table* _table
)
{
    if (!_table)
    {
        return 0;
    }

    if (_table->capacity <= _table->row_count)
    {
        return 0;
    }

    return _table->capacity - _table->row_count;
}


// ---------------------------------------------------------------------------
//  VII.  row mutation functions
// ---------------------------------------------------------------------------


/*
d_table_push_row
  Append a single row at the end of the table.

Parameter(s):
  _table:    the table.
  _row_data: pointer to the row to append (struct_size bytes).
Return:
  true on success, false on failure.
*/
bool
d_table_push_row
(
    struct d_table* _table,
    const void*     _row_data
)
{
    char* dest;

    if ( (!_table)    ||
         (!_row_data) )
    {
        return false;
    }

    if (_table->struct_size == 0)
    {
        return false;
    }

    if (!d_table_ensure_capacity(_table, _table->row_count + 1))
    {
        return false;
    }

    dest = (char*)_table->data
           + _table->row_count * _table->struct_size;
    d_memcpy(dest, _row_data, _table->struct_size);
    _table->row_count++;

    return true;
}


/*
d_table_push_row_empty
  Append a zeroed row at the end of the table.

Parameter(s):
  _table: the table.
Return:
  true on success, false on failure.
*/
bool
d_table_push_row_empty
(
    struct d_table* _table
)
{
    char* dest;

    if (!_table)
    {
        return false;
    }

    if (_table->struct_size == 0)
    {
        return false;
    }

    if (!d_table_ensure_capacity(_table, _table->row_count + 1))
    {
        return false;
    }

    dest = (char*)_table->data
           + _table->row_count * _table->struct_size;
    memset(dest, 0, _table->struct_size);
    _table->row_count++;

    return true;
}


/*
d_table_insert_row
  Insert a single row at _index, shifting subsequent rows down.

Parameter(s):
  _table:    the table.
  _index:    insertion position (supports negative indexing).
  _row_data: pointer to the row to insert (struct_size bytes).
Return:
  true on success, false on failure or out-of-bounds index.
*/
bool
d_table_insert_row
(
    struct d_table* _table,
    d_index         _index,
    const void*     _row_data
)
{
    size_t idx;
    char*  slot;
    size_t bytes_to_move;

    if ( (!_table)    ||
         (!_row_data) )
    {
        return false;
    }

    if (_table->struct_size == 0)
    {
        return false;
    }

    // allow inserting at row_count (append)
    if (_index >= 0 && (size_t)_index == _table->row_count)
    {
        return d_table_push_row(_table, _row_data);
    }

    if (!d_index_convert_safe(_index, _table->row_count, &idx))
    {
        return false;
    }

    if (!d_table_ensure_capacity(_table, _table->row_count + 1))
    {
        return false;
    }

    slot = (char*)_table->data + idx * _table->struct_size;
    bytes_to_move = (_table->row_count - idx) * _table->struct_size;

    memmove(slot + _table->struct_size, slot, bytes_to_move);
    d_memcpy(slot, _row_data, _table->struct_size);
    _table->row_count++;

    return true;
}


/*
d_table_insert_rows
  Insert _count contiguous rows starting at _index.

Parameter(s):
  _table:  the table.
  _index:  insertion position (supports negative indexing).
  _source: pointer to the row array to insert.
  _count:  number of rows to insert.
Return:
  true on success, false on failure.
*/
bool
d_table_insert_rows
(
    struct d_table* _table,
    d_index         _index,
    const void*     _source,
    size_t          _count
)
{
    size_t idx;
    char*  slot;
    size_t bytes_to_move;
    size_t insert_bytes;

    if ( (!_table)  ||
         (!_source) )
    {
        return false;
    }

    if (_table->struct_size == 0)
    {
        return false;
    }

    if (_count == 0)
    {
        return true;
    }

    // allow inserting at row_count (append)
    if (_index >= 0 && (size_t)_index == _table->row_count)
    {
        idx = _table->row_count;
    }
    else
    {
        if (!d_index_convert_safe(_index, _table->row_count, &idx))
        {
            return false;
        }
    }

    if (!d_table_ensure_capacity(_table, _table->row_count + _count))
    {
        return false;
    }

    slot          = (char*)_table->data + idx * _table->struct_size;
    bytes_to_move = (_table->row_count - idx) * _table->struct_size;
    insert_bytes  = _count * _table->struct_size;

    memmove(slot + insert_bytes, slot, bytes_to_move);
    d_memcpy(slot, _source, insert_bytes);
    _table->row_count += _count;

    return true;
}


/*
d_table_remove_row
  Remove the row at _index, shifting subsequent rows up.

Parameter(s):
  _table: the table.
  _index: row to remove (supports negative indexing).
Return:
  true on success, false on failure or out-of-bounds index.
*/
bool
d_table_remove_row
(
    struct d_table* _table,
    d_index         _index
)
{
    size_t idx;
    char*  slot;
    size_t bytes_to_move;

    if (!_table)
    {
        return false;
    }

    if (_table->row_count == 0)
    {
        return false;
    }

    if (!d_index_convert_safe(_index, _table->row_count, &idx))
    {
        return false;
    }

    // promote if needed (we're mutating data)
    if (!(_table->flags & D_TABLE_FLAG_OWNS_DATA))
    {
        if (!d_internal_table_promote(_table, _table->row_count))
        {
            return false;
        }
    }

    slot          = (char*)_table->data + idx * _table->struct_size;
    bytes_to_move = (_table->row_count - idx - 1) * _table->struct_size;

    if (bytes_to_move > 0)
    {
        memmove(slot, slot + _table->struct_size, bytes_to_move);
    }

    _table->row_count--;

    return true;
}


/*
d_table_remove_range
  Remove rows from _start to _end inclusive.

Parameter(s):
  _table: the table.
  _start: first row to remove (supports negative indexing).
  _end:   last row to remove (supports negative indexing).
Return:
  true on success, false on failure.
*/
bool
d_table_remove_range
(
    struct d_table* _table,
    d_index         _start,
    d_index         _end
)
{
    size_t idx_start;
    size_t idx_end;
    size_t remove_count;
    char*  slot;
    size_t bytes_to_move;

    if (!_table)
    {
        return false;
    }

    if (_table->row_count == 0)
    {
        return false;
    }

    if ( (!d_index_convert_safe(_start, _table->row_count, &idx_start)) ||
         (!d_index_convert_safe(_end,   _table->row_count, &idx_end)) )
    {
        return false;
    }

    if (idx_start > idx_end)
    {
        return false;
    }

    // promote if needed
    if (!(_table->flags & D_TABLE_FLAG_OWNS_DATA))
    {
        if (!d_internal_table_promote(_table, _table->row_count))
        {
            return false;
        }
    }

    remove_count  = idx_end - idx_start + 1;
    slot          = (char*)_table->data
                    + idx_start * _table->struct_size;
    bytes_to_move = (_table->row_count - idx_end - 1)
                    * _table->struct_size;

    if (bytes_to_move > 0)
    {
        memmove(slot,
                slot + remove_count * _table->struct_size,
                bytes_to_move);
    }

    _table->row_count -= remove_count;

    return true;
}


/*
d_table_pop_row
  Remove the last row.  If _out_row is non-NULL, the row data is copied
into it before removal.

Parameter(s):
  _table:   the table.
  _out_row: output buffer for the removed row, or NULL.
Return:
  true on success, false if the table is NULL or empty.
*/
bool
d_table_pop_row
(
    struct d_table* _table,
    void*           _out_row
)
{
    char* last;

    if (!_table)
    {
        return false;
    }

    if (_table->row_count == 0)
    {
        return false;
    }

    last = (char*)_table->data
           + (_table->row_count - 1) * _table->struct_size;

    if (_out_row)
    {
        d_memcpy(_out_row, last, _table->struct_size);
    }

    _table->row_count--;

    return true;
}


/*
d_table_clear
  Set row_count to 0 without freeing or reallocating.

Parameter(s):
  _table: the table.
Return:
  none.
*/
void
d_table_clear
(
    struct d_table* _table
)
{
    if (_table)
    {
        _table->row_count = 0;
    }

    return;
}


// ---------------------------------------------------------------------------
//  VIII.  cell access functions
// ---------------------------------------------------------------------------


/*
d_table_cell_ptr
  Return a void* to the cell at (_row, _col).  Both indices support
negative indexing.

Parameter(s):
  _table: the table.
  _row:   row index (supports negative indexing).
  _col:   column index (supports negative indexing).
Return:
  pointer to the cell, or NULL if out of bounds.
*/
void*
d_table_cell_ptr
(
    const struct d_table* _table,
    d_index               _row,
    d_index               _col
)
{
    size_t r;
    size_t c;

    if (!_table)
    {
        return NULL;
    }

    if (!_table->data)
    {
        return NULL;
    }

    if ( (!d_index_convert_safe(_row, _table->row_count,    &r)) ||
         (!d_index_convert_safe(_col, _table->column_count, &c)) )
    {
        return NULL;
    }

    return D_TABLE_CELL_AT(_table->data,
                           _table->struct_size,
                           _table->column_descs,
                           r, c);
}


/*
d_table_cell_get
  Copy the cell at (_row, _col) into _out.

Parameter(s):
  _table: the table.
  _row:   row index (supports negative indexing).
  _col:   column index (supports negative indexing).
  _out:   output buffer (must be >= element_size bytes).
Return:
  true on success, false on failure.
*/
bool
d_table_cell_get
(
    const struct d_table* _table,
    d_index               _row,
    d_index               _col,
    void*                 _out
)
{
    size_t r;
    size_t c;
    void*  cell;

    if ( (!_table) ||
         (!_out) )
    {
        return false;
    }

    if (!_table->data)
    {
        return false;
    }

    if ( (!d_index_convert_safe(_row, _table->row_count,    &r)) ||
         (!d_index_convert_safe(_col, _table->column_count, &c)) )
    {
        return false;
    }

    cell = D_TABLE_CELL_AT(_table->data,
                           _table->struct_size,
                           _table->column_descs,
                           r, c);
    d_memcpy(_out, cell, _table->column_descs[c].element_size);

    return true;
}


/*
d_table_cell_set
  Write _value into the cell at (_row, _col).

Parameter(s):
  _table: the table.
  _row:   row index (supports negative indexing).
  _col:   column index (supports negative indexing).
  _value: pointer to the value to write (element_size bytes).
Return:
  true on success, false on failure.
*/
bool
d_table_cell_set
(
    struct d_table* _table,
    d_index         _row,
    d_index         _col,
    const void*     _value
)
{
    size_t r;
    size_t c;
    void*  cell;

    if ( (!_table) ||
         (!_value) )
    {
        return false;
    }

    if (!_table->data)
    {
        return false;
    }

    if ( (!d_index_convert_safe(_row, _table->row_count,    &r)) ||
         (!d_index_convert_safe(_col, _table->column_count, &c)) )
    {
        return false;
    }

    cell = D_TABLE_CELL_AT(_table->data,
                           _table->struct_size,
                           _table->column_descs,
                           r, c);
    d_memcpy(cell, _value, _table->column_descs[c].element_size);

    return true;
}


// ---------------------------------------------------------------------------
//  IX.  row access functions
// ---------------------------------------------------------------------------


/*
d_table_row_ptr
  Return a void* to the start of row _row.

Parameter(s):
  _table: the table.
  _row:   row index (supports negative indexing).
Return:
  pointer to the row, or NULL if out of bounds.
*/
void*
d_table_row_ptr
(
    const struct d_table* _table,
    d_index               _row
)
{
    size_t r;

    if (!_table)
    {
        return NULL;
    }

    if (!_table->data)
    {
        return NULL;
    }

    if (!d_index_convert_safe(_row, _table->row_count, &r))
    {
        return NULL;
    }

    return D_TABLE_ROW_AT(_table->data, _table->struct_size, r);
}


/*
d_table_row_get
  Copy the entire row at _row into _out.

Parameter(s):
  _table: the table.
  _row:   row index (supports negative indexing).
  _out:   output buffer (must be >= struct_size bytes).
Return:
  true on success, false on failure.
*/
bool
d_table_row_get
(
    const struct d_table* _table,
    d_index               _row,
    void*                 _out
)
{
    size_t r;
    void*  src;

    if ( (!_table) ||
         (!_out) )
    {
        return false;
    }

    if (!_table->data)
    {
        return false;
    }

    if (!d_index_convert_safe(_row, _table->row_count, &r))
    {
        return false;
    }

    src = D_TABLE_ROW_AT(_table->data, _table->struct_size, r);
    d_memcpy(_out, src, _table->struct_size);

    return true;
}


/*
d_table_row_set
  Overwrite the row at _row with struct_size bytes from _source.

Parameter(s):
  _table:  the table.
  _row:    row index (supports negative indexing).
  _source: pointer to the replacement row.
Return:
  true on success, false on failure.
*/
bool
d_table_row_set
(
    struct d_table* _table,
    d_index         _row,
    const void*     _source
)
{
    size_t r;
    void*  dest;

    if ( (!_table)  ||
         (!_source) )
    {
        return false;
    }

    if (!_table->data)
    {
        return false;
    }

    if (!d_index_convert_safe(_row, _table->row_count, &r))
    {
        return false;
    }

    dest = D_TABLE_ROW_AT(_table->data, _table->struct_size, r);
    d_memcpy(dest, _source, _table->struct_size);

    return true;
}


// ---------------------------------------------------------------------------
//  X.  query functions
// ---------------------------------------------------------------------------


/*
d_table_row_count
  Return the current number of rows.

Parameter(s):
  _table: the table.
Return:
  the row count, or 0 if the table is NULL.
*/
size_t
d_table_row_count
(
    const struct d_table* _table
)
{
    if (!_table)
    {
        return 0;
    }

    return _table->row_count;
}


/*
d_table_column_count
  Return the number of columns.

Parameter(s):
  _table: the table.
Return:
  the column count, or 0 if the table is NULL.
*/
size_t
d_table_column_count
(
    const struct d_table* _table
)
{
    if (!_table)
    {
        return 0;
    }

    return _table->column_count;
}


/*
d_table_capacity
  Return the current row capacity.

Parameter(s):
  _table: the table.
Return:
  the capacity, or 0 if the table is NULL.
*/
size_t
d_table_capacity
(
    const struct d_table* _table
)
{
    if (!_table)
    {
        return 0;
    }

    return _table->capacity;
}


/*
d_table_struct_size
  Return the size in bytes of one row.

Parameter(s):
  _table: the table.
Return:
  the struct size, or 0 if the table is NULL.
*/
size_t
d_table_struct_size
(
    const struct d_table* _table
)
{
    if (!_table)
    {
        return 0;
    }

    return _table->struct_size;
}


/*
d_table_is_empty
  Return true if the table has zero rows.

Parameter(s):
  _table: the table.
Return:
  true if empty or NULL, false otherwise.
*/
bool
d_table_is_empty
(
    const struct d_table* _table
)
{
    if (!_table)
    {
        return true;
    }

    return _table->row_count == 0;
}


/*
d_table_is_full
  Return true if row_count equals capacity.

Parameter(s):
  _table: the table.
Return:
  true if full, false otherwise.
*/
bool
d_table_is_full
(
    const struct d_table* _table
)
{
    if (!_table)
    {
        return true;
    }

    return _table->row_count >= _table->capacity;
}


/*
d_table_data
  Return a pointer to the raw row buffer.

Parameter(s):
  _table: the table.
Return:
  the data pointer, or NULL if the table is NULL.
*/
void*
d_table_data
(
    const struct d_table* _table
)
{
    if (!_table)
    {
        return NULL;
    }

    return _table->data;
}


// ---------------------------------------------------------------------------
//  XI.  search functions
// ---------------------------------------------------------------------------


/*
d_table_find_row
  Forward linear search for the first row whose cell in column _col
compares equal to _value.

Parameter(s):
  _table:      the table.
  _col:        column index to search.
  _value:      pointer to the value to compare against.
  _comparator: comparison function; returns 0 on match.
Return:
  the row index of the first match, or -1 if not found.
*/
ssize_t
d_table_find_row
(
    const struct d_table* _table,
    size_t                _col,
    const void*           _value,
    fn_comparator         _comparator
)
{
    if (!_table)
    {
        return -1;
    }

    if (_col >= _table->column_count)
    {
        return -1;
    }

    return d_table_common_find_row(
        _table->data,
        _table->struct_size,
        _table->row_count,
        &_table->column_descs[_col],
        _value,
        _comparator);
}


/*
d_table_find_row_last
  Reverse linear search for the last matching row.

Parameter(s):
  _table:      the table.
  _col:        column index to search.
  _value:      pointer to the value to compare against.
  _comparator: comparison function; returns 0 on match.
Return:
  the row index of the last match, or -1 if not found.
*/
ssize_t
d_table_find_row_last
(
    const struct d_table* _table,
    size_t                _col,
    const void*           _value,
    fn_comparator         _comparator
)
{
    if (!_table)
    {
        return -1;
    }

    if (_col >= _table->column_count)
    {
        return -1;
    }

    return d_table_common_find_row_last(
        _table->data,
        _table->struct_size,
        _table->row_count,
        &_table->column_descs[_col],
        _value,
        _comparator);
}


/*
d_table_contains
  Return true if any row's cell in column _col matches _value.

Parameter(s):
  _table:      the table.
  _col:        column index to search.
  _value:      pointer to the value to compare against.
  _comparator: comparison function; returns 0 on match.
Return:
  true if a match exists, false otherwise.
*/
bool
d_table_contains
(
    const struct d_table* _table,
    size_t                _col,
    const void*           _value,
    fn_comparator         _comparator
)
{
    return d_table_find_row(_table, _col, _value, _comparator) >= 0;
}


/*
d_table_count_value
  Count the number of rows whose cell in column _col matches _value.

Parameter(s):
  _table:      the table.
  _col:        column index to search.
  _value:      pointer to the value to compare against.
  _comparator: comparison function; returns 0 on match.
Return:
  the match count, or 0 if the table is NULL or _col is invalid.
*/
size_t
d_table_count_value
(
    const struct d_table* _table,
    size_t                _col,
    const void*           _value,
    fn_comparator         _comparator
)
{
    size_t      count;
    size_t      i;
    const char* row_ptr;
    const void* cell_ptr;

    if ( (!_table)           ||
         (!_table->data)     ||
         (!_value)           ||
         (!_comparator) )
    {
        return 0;
    }

    if (_col >= _table->column_count)
    {
        return 0;
    }

    count   = 0;
    row_ptr = (const char*)_table->data;

    for (i = 0; i < _table->row_count; i++)
    {
        cell_ptr = row_ptr + _table->column_descs[_col].offset;
        if (_comparator(cell_ptr, _value) == 0)
        {
            count++;
        }
        row_ptr += _table->struct_size;
    }

    return count;
}


// ---------------------------------------------------------------------------
//  XII.  utility functions
// ---------------------------------------------------------------------------


/*
d_table_swap_rows
  Swap the contents of two rows.

Parameter(s):
  _table: the table.
  _row_a: first row index (supports negative indexing).
  _row_b: second row index (supports negative indexing).
Return:
  true on success, false on failure.
*/
bool
d_table_swap_rows
(
    struct d_table* _table,
    d_index         _row_a,
    d_index         _row_b
)
{
    if (!_table)
    {
        return false;
    }

    return d_table_common_swap_rows(
        _table->data,
        _table->struct_size,
        _table->row_count,
        _row_a,
        _row_b);
}


/*
d_table_reverse
  Reverse the order of all rows.

Parameter(s):
  _table: the table.
Return:
  true on success, false on failure.
*/
bool
d_table_reverse
(
    struct d_table* _table
)
{
    size_t lo;
    size_t hi;

    if (!_table)
    {
        return false;
    }

    if (_table->row_count <= 1)
    {
        return true;
    }

    lo = 0;
    hi = _table->row_count - 1;

    while (lo < hi)
    {
        if (!d_table_common_swap_rows(
                _table->data,
                _table->struct_size,
                _table->row_count,
                (d_index)lo,
                (d_index)hi))
        {
            return false;
        }
        lo++;
        hi--;
    }

    return true;
}


/*
d_table_sort_by_column
  Sort all rows by the values in column _col.

Parameter(s):
  _table:      the table.
  _col:        column index to sort by.
  _comparator: comparison function for the column values.
Return:
  true on success, false on failure.
*/
bool
d_table_sort_by_column
(
    struct d_table* _table,
    size_t          _col,
    fn_comparator   _comparator
)
{
    if (!_table)
    {
        return false;
    }

    if (_col >= _table->column_count)
    {
        return false;
    }

    return d_table_common_sort_by_column(
        _table->data,
        _table->struct_size,
        _table->row_count,
        &_table->column_descs[_col],
        _comparator);
}


/*
d_table_copy_to
  Copy the row data into a caller-supplied buffer.

Parameter(s):
  _table:         the table.
  _destination:   output buffer.
  _dest_capacity: size in bytes of _destination.
Return:
  true on success, false on failure.
*/
bool
d_table_copy_to
(
    const struct d_table* _table,
    void*                 _destination,
    size_t                _dest_capacity
)
{
    if (!_table)
    {
        return false;
    }

    return d_table_common_copy_to(
        _table->data,
        _table->struct_size,
        _table->row_count,
        _destination,
        _dest_capacity);
}


/*
d_table_validate
  Validate the table's column descriptors against its struct_size.

Parameter(s):
  _table: the table.
Return:
  true if valid, false otherwise.
*/
bool
d_table_validate
(
    const struct d_table* _table
)
{
    if (!_table)
    {
        return false;
    }

    return d_table_common_column_desc_validate(
        _table->column_descs,
        _table->column_count,
        _table->struct_size);
}


// ---------------------------------------------------------------------------
//  XIII.  destructor functions
// ---------------------------------------------------------------------------


/*
d_table_free
  Free all owned resources according to the table's flags, then free
the table struct itself.  Safe to call with NULL.

Parameter(s):
  _table: the table to free; may be NULL.
Return:
  none.
*/
void
d_table_free
(
    struct d_table* _table
)
{
    if (!_table)
    {
        return;
    }

    if (_table->flags & D_TABLE_FLAG_OWNS_DATA)
    {
        free(_table->data);
    }

    if (_table->flags & D_TABLE_FLAG_OWNS_DESCS)
    {
        free(_table->column_descs);
    }

    free(_table);

    return;
}


/*
d_table_free_deep
  Like d_table_free, but first applies _free_fn to every cell in
columns that have pointer-like element sizes.  Useful for tables
whose cells hold heap-allocated strings or nested structures.

Parameter(s):
  _table:   the table to free; may be NULL.
  _free_fn: function to call on each pointer-sized cell.
Return:
  none.
*/
void
d_table_free_deep
(
    struct d_table* _table,
    fn_free         _free_fn
)
{
    size_t i;
    size_t c;
    char*  row_ptr;
    void*  cell_ptr;
    void*  ptr_val;

    if (!_table)
    {
        return;
    }

    if (_free_fn && _table->data)
    {
        for (i = 0; i < _table->row_count; i++)
        {
            row_ptr = (char*)_table->data
                      + i * _table->struct_size;

            for (c = 0; c < _table->column_count; c++)
            {
                // apply free_fn to pointer-sized columns
                if (_table->column_descs[c].element_size
                    == sizeof(void*))
                {
                    cell_ptr = row_ptr
                               + _table->column_descs[c].offset;
                    d_memcpy(&ptr_val, cell_ptr, sizeof(void*));
                    if (ptr_val)
                    {
                        _free_fn(ptr_val);
                    }
                }
            }
        }
    }

    d_table_free(_table);

    return;
}
