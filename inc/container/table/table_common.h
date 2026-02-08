/******************************************************************************
* djinterp [container]                                          table_common.h
*
*   This module provides common building blocks for table data structures.
* A table is a two-dimensional, structurally fixed container backed by a
* contiguous array of user-defined row structs. Each column maps to a field
* within the row struct via a byte offset, giving typed access to individual
* cells without additional indirection.
*   Functions in this module operate on raw pointers, descriptor arrays, and
* dimension values, enabling reuse across different table implementations
* such as `d_table` and `d_table_view`.
*   This module covers structurally immutable operations only: the number of
* rows and columns cannot change after initialization. Structural modifiers
* (insert/remove rows and columns) belong in separate extension modules.
*   Column descriptors carry only layout information (element size, byte
* offset, flags). Naming and other metadata belong in higher-level modules
* that depend on table_common.
*   For homogeneous flat buffers where every cell is the same size, use the
* `matrix` container instead.
*
*
* path:      \inc\container\table\table_common.h
* link(s):   TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2026.02.06
******************************************************************************/

/*
TABLE OF CONTENTS
=================
I.    CONFIGURATION
      -------------
      1.  D_TABLE_MAX_COLUMNS

II.   TYPE DEFINITIONS
      -----------------
      1.  Column descriptor flags
          a.  D_TABLE_COLUMN_FLAG_NONE
          b.  D_TABLE_COLUMN_FLAG_READ_ONLY
          c.  D_TABLE_COLUMN_FLAG_NULLABLE
          d.  D_TABLE_COLUMN_FLAG_KEY
          e.  D_TABLE_COLUMN_FLAG_HIDDEN
          f.  D_TABLE_COLUMN_FLAG_METADATA
      2.  Struct types
          a.  d_table_column_desc   (column descriptor)
          b.  d_table_cell_ref      (lightweight cell reference)

III.  COLUMN DESCRIPTOR MACROS
      ------------------------
      1.  Struct-field column declaration
          a.  D_TABLE_COLUMN_FIELD
          b.  D_TABLE_COLUMN_FIELD_FLAGS
          c.  D_TABLE_COLUMN_FULL
      2.  Column array helpers
          a.  D_TABLE_COLUMNS
          b.  D_TABLE_COLUMN_COUNT

IV.   ROW DECLARATION MACROS
      ----------------------
      1.  Typed row initializers
          a.  D_TABLE_ROW
          b.  D_TABLE_ROW_PTR
      2.  Row array initializers
          a.  D_TABLE_ROWS
          b.  D_TABLE_ROW_COUNT

V.    CELL VALUE MACROS
      -----------------
      1.  Typed compound-literal wrappers
          a.  D_TABLE_CELL_INT
          b.  D_TABLE_CELL_UINT
          c.  D_TABLE_CELL_FLOAT
          d.  D_TABLE_CELL_DOUBLE
          e.  D_TABLE_CELL_STR
          f.  D_TABLE_CELL_PTR
          g.  D_TABLE_CELL_BOOL
          h.  D_TABLE_CELL_NULL
          i.  D_TABLE_CELL_TYPED

VI.   STATIC INITIALIZATION MACROS
      ----------------------------
      1.  D_TABLE_INIT

VII.  ACCESSOR MACROS
      ---------------
      1.  Row access
          a.  D_TABLE_ROW_AT
          b.  D_TABLE_ROW_AT_T
      2.  Cell access
          a.  D_TABLE_CELL_AT
          b.  D_TABLE_CELL_AS
      3.  Column descriptor access
          a.  D_TABLE_CELL_SIZE
          b.  D_TABLE_CELL_OFFSET

VIII. QUERY MACROS
      -----------
      1.  Dimension queries
          a.  D_TABLE_IS_EMPTY
          b.  D_TABLE_TOTAL_SIZE
      2.  Bounds checking
          a.  D_TABLE_IS_VALID_ROW
          b.  D_TABLE_IS_VALID_COLUMN
          c.  D_TABLE_IS_VALID_CELL

IX.   FUNCTION DECLARATIONS
      ---------------------
      1.  Initialization functions
      2.  Search functions
      3.  Utility functions
      4.  Validation functions
      5.  Cleanup functions
*/

#ifndef DJINTERP_C_CONTAINER_TABLE_COMMON_
#define DJINTERP_C_CONTAINER_TABLE_COMMON_ 1

#include <stddef.h>
#include <stdlib.h>
#include "..\..\djinterp.h"
#include "..\..\dmemory.h"
#include "..\container.h"


///////////////////////////////////////////////////////////////////////////////
///             II.   TYPE DEFINITIONS                                      ///
///////////////////////////////////////////////////////////////////////////////

// --- II.1  Column descriptor flags ---

// D_TABLE_COLUMN_FLAG_NONE
//   constant: no special flags applied to this column.
#define D_TABLE_COLUMN_FLAG_NONE      0x00u

// D_TABLE_COLUMN_FLAG_READ_ONLY
//   constant: column data should not be modified through table accessors.
#define D_TABLE_COLUMN_FLAG_READ_ONLY 0x01u

// D_TABLE_COLUMN_FLAG_NULLABLE
//   constant: column permits null / sentinel values.
#define D_TABLE_COLUMN_FLAG_NULLABLE  0x02u

// D_TABLE_COLUMN_FLAG_KEY
//   constant: column is a key column used for lookups and uniqueness.
#define D_TABLE_COLUMN_FLAG_KEY       0x04u

// D_TABLE_COLUMN_FLAG_HIDDEN
//   constant: column is excluded from default iteration and printing.
#define D_TABLE_COLUMN_FLAG_HIDDEN    0x08u

// D_TABLE_COLUMN_FLAG_METADATA
//   constant: column contains metadata rather than regular data.
#define D_TABLE_COLUMN_FLAG_METADATA  0x10u


// --- II.2  Struct types ---

// d_table_column_desc
//   struct: describes the layout of a single column within a row struct.
// Carries the element size, the byte offset of the field within the row
// struct, and optional flags. Naming and other metadata belong in
// higher-level modules.
struct d_table_column_desc
{
    size_t   element_size;      // size in bytes of one cell in this column
    size_t   offset;            // byte offset of the field in the row struct
    uint32_t flags;             // bitwise OR of D_TABLE_COLUMN_FLAG_* values
};

// d_table_cell_ref
//   struct: lightweight reference to a single cell inside a table. Stores
// enough context to read or write the cell value without re-deriving its
// location.
struct d_table_cell_ref
{
    void*  data;                // pointer to the cell data
    size_t element_size;        // size of the cell value in bytes
    size_t row;                 // row index of the referenced cell
    size_t column;              // column index of the referenced cell
};


///////////////////////////////////////////////////////////////////////////////
///             III.  COLUMN DESCRIPTOR MACROS                              ///
///////////////////////////////////////////////////////////////////////////////

// D_TABLE_COLUMN_FIELD
//   macro: declare a column descriptor that maps to a field inside a row
// struct. The element size and byte offset are derived automatically from
// the struct type and field name. Flags default to
// D_TABLE_COLUMN_FLAG_NONE.
#define D_TABLE_COLUMN_FIELD(struct_type,                                \
                             field)                                      \
    {                                                                    \
        .element_size = sizeof(((struct_type*)0)->field),                \
        .offset       = offsetof(struct_type, field),                    \
        .flags        = D_TABLE_COLUMN_FLAG_NONE                         \
    }

// D_TABLE_COLUMN_FIELD_FLAGS
//   macro: declare a struct-field column descriptor with explicit flags.
#define D_TABLE_COLUMN_FIELD_FLAGS(struct_type,                          \
                                   field,                                \
                                   column_flags)                         \
    {                                                                    \
        .element_size = sizeof(((struct_type*)0)->field),                \
        .offset       = offsetof(struct_type, field),                    \
        .flags        = (column_flags)                                   \
    }

// D_TABLE_COLUMN_FULL
//   macro: declare a column descriptor with all fields specified
// explicitly. Useful when the offset and size are known ahead of time
// or computed manually.
#define D_TABLE_COLUMN_FULL(byte_size,                                   \
                            byte_offset,                                 \
                            column_flags)                                \
    {                                                                    \
        .element_size = (byte_size),                                     \
        .offset       = (byte_offset),                                   \
        .flags        = (column_flags)                                   \
    }

// D_TABLE_COLUMNS
//   macro: declare a brace-enclosed list of column descriptors,
// suitable for initializing a static array.
// Usage:
//   static const struct d_table_column_desc cols[] =
//       D_TABLE_COLUMNS(
//           D_TABLE_COLUMN_FIELD(struct my_row, id),
//           D_TABLE_COLUMN_FIELD(struct my_row, name)
//       );
#define D_TABLE_COLUMNS(...)                                             \
    {                                                                    \
        __VA_ARGS__                                                      \
    }

// D_TABLE_COLUMN_COUNT
//   macro: compute the number of column descriptors in a statically
// sized array.
#define D_TABLE_COLUMN_COUNT(column_array)                               \
    ( sizeof(column_array) / sizeof((column_array)[0]) )


///////////////////////////////////////////////////////////////////////////////
///             IV.   ROW DECLARATION MACROS                                ///
///////////////////////////////////////////////////////////////////////////////

// D_TABLE_ROW
//   macro: declare a compound-literal row. The caller supplies the row
// struct type and the field values.
// Usage:
//   D_TABLE_ROW(struct my_row, .id = 1, .name = "hello")
#define D_TABLE_ROW(row_type,                                            \
                    ...)                                                 \
    (row_type)                                                           \
    {                                                                    \
        __VA_ARGS__                                                      \
    }

// D_TABLE_ROW_PTR
//   macro: declare a compound-literal row and yield a pointer to it.
// Useful for populating arrays of row pointers or passing to functions
// that accept const void*.
#define D_TABLE_ROW_PTR(row_type,                                        \
                        ...)                                             \
    &(row_type)                                                          \
    {                                                                    \
        __VA_ARGS__                                                      \
    }

// D_TABLE_ROWS
//   macro: declare a brace-enclosed list of row initializers, suitable
// for initializing a static array of row structs.
// Usage:
//   struct my_row rows[] = D_TABLE_ROWS(
//       { .id = 1, .name = "a" },
//       { .id = 2, .name = "b" }
//   );
#define D_TABLE_ROWS(...)                                                \
    {                                                                    \
        __VA_ARGS__                                                      \
    }

// D_TABLE_ROW_COUNT
//   macro: compute the number of rows in a statically sized row array.
// The row_type parameter is required because the array element type
// may include padding.
#define D_TABLE_ROW_COUNT(row_type,                                      \
                          row_array)                                     \
    ( sizeof(row_array) / sizeof(row_type) )


///////////////////////////////////////////////////////////////////////////////
///             V.    CELL VALUE MACROS                                     ///
///////////////////////////////////////////////////////////////////////////////
//
// These macros wrap a value in a compound literal so it can be passed
// as a const void* to cell-set functions.

// D_TABLE_CELL_INT
//   macro: wrap an int value for cell assignment.
#define D_TABLE_CELL_INT(val)          (&(int){(val)})

// D_TABLE_CELL_UINT
//   macro: wrap an unsigned int value for cell assignment.
#define D_TABLE_CELL_UINT(val)         (&(unsigned int){(val)})

// D_TABLE_CELL_FLOAT
//   macro: wrap a float value for cell assignment.
#define D_TABLE_CELL_FLOAT(val)        (&(float){(val)})

// D_TABLE_CELL_DOUBLE
//   macro: wrap a double value for cell assignment.
#define D_TABLE_CELL_DOUBLE(val)       (&(double){(val)})

// D_TABLE_CELL_STR
//   macro: wrap a string pointer for cell assignment.
#define D_TABLE_CELL_STR(val)          (&(const char*){(val)})

// D_TABLE_CELL_PTR
//   macro: wrap an arbitrary pointer for cell assignment.
#define D_TABLE_CELL_PTR(val)          (&(void*){(val)})

// D_TABLE_CELL_BOOL
//   macro: wrap a bool value for cell assignment.
#define D_TABLE_CELL_BOOL(val)         (&(bool){(val)})

// D_TABLE_CELL_NULL
//   macro: a null pointer suitable for nullable cell assignment.
#define D_TABLE_CELL_NULL              (NULL)

// D_TABLE_CELL_TYPED
//   macro: wrap a value of an explicit type for cell assignment.
// Usage: D_TABLE_CELL_TYPED(size_t, 42)
#define D_TABLE_CELL_TYPED(type, val)  (&(type){(val)})


///////////////////////////////////////////////////////////////////////////////
///             VI.   STATIC INITIALIZATION MACROS                         ///
///////////////////////////////////////////////////////////////////////////////

// D_TABLE_INIT
//   macro: static initializer for a table backed by a contiguous array
// of row structs and an array of column descriptors.
// Usage:
//   struct my_row rows[] = { ... };
//   static const struct d_table_column_desc cols[] = { ... };
//   struct d_table t = D_TABLE_INIT(
//       rows,
//       sizeof(struct my_row),
//       D_TABLE_ROW_COUNT(struct my_row, rows),
//       cols,
//       D_TABLE_COLUMN_COUNT(cols));
#define D_TABLE_INIT(row_array,                                          \
                     struct_size,                                        \
                     num_rows,                                           \
                     column_desc_array,                                  \
                     num_columns)                                        \
    {                                                                    \
        .data          = (void*)(row_array),                             \
        .column_descs  = (col_desc_array),                               \
        .struct_size   = (struct_size),                                  \
        .row_count     = (num_rows),                                     \
        .column_count  = (num_columns)                                   \
    }


///////////////////////////////////////////////////////////////////////////////
///             VII.  ACCESSOR MACROS                                       ///
///////////////////////////////////////////////////////////////////////////////
//
// All accessor macros take raw size_t indices. For d_index (negative
// index) support, use d_index_convert_safe or d_index_convert_fast
// before calling these macros, or use the function-based accessors in
// the d_table wrapper module.

// D_TABLE_ROW_AT
//   macro: return a void* to the start of row `row` in the row array
// `data`, where each row is `struct_size` bytes.
#define D_TABLE_ROW_AT(data,                                             \
                       struct_size,                                      \
                       row)                                              \
    ( (void*)((char*)(data) + (size_t)(row) * (size_t)(struct_size)) )

// D_TABLE_ROW_AT_T
//   macro: return a typed pointer to row `row`. Equivalent to casting
// D_TABLE_ROW_AT to the given pointer type.
// Usage:
//   struct my_row* r =
//       D_TABLE_ROW_AT_T(struct my_row*, rows,
//                        sizeof(struct my_row), 3);
#define D_TABLE_ROW_AT_T(type,                                           \
                         data,                                           \
                         struct_size,                                    \
                         row)                                            \
    ( (type)D_TABLE_ROW_AT( (data),                                      \
                            (struct_size),                               \
                            (row) ) )

// D_TABLE_CELL_AT
//   macro: return a void* to the cell at (`row`, `column`) in the row
// array `data`. The column's byte offset is read from `column_descs`.
#define D_TABLE_CELL_AT(data,                                            \
                        struct_size,                                     \
                        column_descs,                                    \
                        row,                                             \
                        column)                                          \
    ((void*)( (char*)(data)                                              \
                   + (size_t)(row) * (size_t)(struct_size)               \
                   + (column_descs)[(column)].offset ))

// D_TABLE_CELL_AS
//   macro: read the cell at (`row`, `column`) as a value of the given
// C type. The caller is responsible for ensuring the type matches the
// actual field type.
// Usage:
//   int id = D_TABLE_CELL_AS(int, rows,
//                            sizeof(struct my_row),
//                            cols, 0, 0);
#define D_TABLE_CELL_AS(type,                                            \
                        data,                                            \
                        struct_size,                                     \
                        column_descs,                                    \
                        row,                                             \
                        column)                                          \
    ( *(type*)D_TABLE_CELL_AT( (data),                                   \
                               (struct_size),                            \
                               (column_descs),                           \
                               (row),                                    \
                               (column)) )

// D_TABLE_CELL_SIZE
//   macro: return the element size of column `column` from a column
// descriptor array.
#define D_TABLE_CELL_SIZE(column_descs,                                  \
                          column)                                        \
    ( (column_descs)[(column)].element_size )

// D_TABLE_CELL_OFFSET
//   macro: return the byte offset of column `column` from a column
// descriptor array.
#define D_TABLE_CELL_OFFSET(column_descs,                                \
                            column)                                      \
    ( (column_descs)[(column)].offset )


///////////////////////////////////////////////////////////////////////////////
///             VIII. QUERY MACROS                                          ///
///////////////////////////////////////////////////////////////////////////////

// D_TABLE_IS_EMPTY
//   macro: evaluates to true if the table has zero rows.
#define D_TABLE_IS_EMPTY(row_count)                                      \
    ( (size_t)(row_count) == 0 )                                         
                                                                         
// D_TABLE_TOTAL_SIZE                                                    
//   macro: compute the total byte size of the row data.                 
#define D_TABLE_TOTAL_SIZE(row_count,                                    \
                           struct_size)                                  \
    ( (size_t)(row_count) * (size_t)(struct_size) )                      
                                                                         
// D_TABLE_IS_VALID_ROW                                                  
//   macro: evaluates to true if `row` is a valid d_index for a table    
// with `row_count` rows.                                                
#define D_TABLE_IS_VALID_ROW(row,                                        \
                             row_count)                                  \
    D_IS_VALID_INDEX( (row),                                             \
                      (row_count) )                                       
                                                                         
// D_TABLE_IS_VALID_COLUMN                                               
//   macro: evaluates to true if `column` is a valid d_index for a       
// table with `column_count` columns.                                    
#define D_TABLE_IS_VALID_COLUMN(column,                                  \
                                column_count)                            \
    D_IS_VALID_INDEX( (column), (column_count) )                           
                                                                         
// D_TABLE_IS_VALID_CELL                                                 
//   macro: evaluates to true if (`row`, `column`) is a valid cell       
// address.                                                              
#define D_TABLE_IS_VALID_CELL(row,                                       \
                              column,                                    \
                              row_count,                                 \
                              column_count)                              \
    ( D_TABLE_IS_VALID_ROW( (row), (row_count) ) &&                        \
      D_TABLE_IS_VALID_COLUMN( (column), (column_count)) )


// I.    initialization functions
bool    d_table_common_init(void** _data, size_t* _row_count, size_t _struct_size, size_t _num_rows);
bool    d_table_common_init_copy(void** _data, size_t* _row_count, size_t _struct_size, const void* _source, size_t _num_rows);

// II.   search functions
ssize_t d_table_common_find_row(const void* _data, size_t _struct_size, size_t _row_count, const struct d_table_column_desc* _search_column, const void* _value, fn_comparator _comparator);
ssize_t d_table_common_find_row_last(const void* _data, size_t _struct_size, size_t _row_count, const struct d_table_column_desc* _search_column, const void* _value, fn_comparator _comparator);

// III.  utility functions
bool    d_table_common_swap_rows(void* _data, size_t _struct_size, size_t _row_count, d_index _row_a, d_index _row_b);
bool    d_table_common_sort_by_column(void* _data, size_t _struct_size, size_t _row_count, const struct d_table_column_desc* _sort_column, fn_comparator _comparator);
bool    d_table_common_copy_to(const void* _source, size_t _struct_size, size_t _row_count, void* _destination, size_t _dest_capacity);

// IV.   validation functions
bool    d_table_common_column_desc_validate(const struct d_table_column_desc* _column_descs, size_t _column_count, size_t _struct_size);

// V.    cleanup functions
void    d_table_common_free_data(void* _data);


#endif  // DJINTERP_C_CONTAINER_TABLE_COMMON_